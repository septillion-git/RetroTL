#include <FadeLed.h>
#include <ClickEncoder.h>
#include <TimerOne.h>

#define SKETCH_DEBUG

const byte SsrPin = A3;
const byte DimPin = 3;
const byte LedPin = 2;

//rotary pinnne
const byte RotaryA    = A2;
const byte RotaryB    = A1;
const byte RotaryBtn  = A0;

//mode variable
typedef enum Mode_e {
  M_OFF,
  M_TL,
  M_TLLED,
  M_LED
} Mode;
Mode mode = M_OFF;

byte state = 0;

//TL setings
const byte DefaultBrightness = 150;
const unsigned int FadeTime = 5000;

//TL variables
FadeLed tlOut(DimPin);

//Encoder variables
ClickEncoder *encoder;
bool pressed = 0;

void inline timerIsr() {
  encoder->service();
}

void setup() {
  #if defined(SKETCH_DEBUG)
  Serial.begin(115200);
  Serial.println(F("RetroTL started!"));
  #endif
  
  //setup ssr pin
  pinMode(SsrPin, OUTPUT);
  
  //Set PWM of timer2 (pin 3 & 11) to +-31kHz
  TCCR2B = (TCCR2B & 0b11111000) | 0x01;
  
  //set the fade time of the TL
  tlOut.setTime(FadeTime);
  
  //setup encoder, 4th parameter = notches per step
  encoder = new ClickEncoder(RotaryA, RotaryB, RotaryBtn, 4);
  
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr); 
}

void loop() {
  //update FadeLed to make the fading happen
  FadeLed::update();
  
  //Check to see if the SSR needs to be on
  updateSsr();
  
  //check rotary
  checkButton();
  checkRotary();
}

void inline updateSsr(){
  //turn on the SSR when the brightness is >0
  digitalWrite(SsrPin, !!tlOut.getCurrent());
}

void checkButton(){
  //read button
  ClickEncoder::Button b = encoder->getButton();
  
  #if defined(SKETCH_DEBUG)
  if(b != ClickEncoder::Open){
    Serial.println(b);
  }
  #endif
  
  if(b == ClickEncoder::Clicked){
    modeOnOff();
    Serial.println("go On/Off!");
    
  }
}



void checkRotary(){
  int delta = encoder->getValue();
  
  if(mode != M_OFF && delta != 0){
    int temp = tlOut.getCurrent() + delta;
    
    if(temp > 255){
      temp = 255;
    }
    else if(temp < 1){
      temp = 1;
    }
    
    tlOut.begin(temp);
  }
}

void modeOnOff(){
  if(mode == M_OFF){
    mode = M_TL;
    tlOut.set(DefaultBrightness);
  }
  else{
    mode = M_OFF;
    tlOut.off();
  }
}
