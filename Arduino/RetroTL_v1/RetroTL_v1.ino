#include <FadeLed.h>
#include <ClickEncoder.h>
#include <TimerOne.h>

const byte SsrPin = A3;
const byte DimPin = 3;
const byte LedPin = 2;

//rotary pinnne
const byte RotaryA = A2;
const byte RotaryB = A1;
const byte RotaryBtn = A0;

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
byte tlBrightness = 0;

//Encoder variables
ClickEncoder *encoder;
bool pressed = 0;

void timerIsr() {
  encoder->service();
}

void setup() {
  //setup ssr pin
  pinMode(SsrPin, OUTPUT);
  
  //Set PWM of timer2 (pin 3 & 11) to +-31kHz
  TCCR2B = (TCCR2B & 0b11111000) | 0x01;
  
  //set the fade time of the TL
  tlOut.setTime(FadeTime);
  
  //setup encoder, 4th parameter = notches per step
  encoder = new ClickEncoder(RotaryA, RotaryB, RotaryBtn, 4);
}

void loop() {
  //update FadeLed to make the fading happen
  FadeLed::update();
  
  //Check to see if the SSR needs to be on
  updateSsr();
  
  //check rotary
  checkButton();
}

void inline updateSsr(){
  //turn on the SSR when the brightness is >0
  if(tlOut.getCurrent()){
    digitalWrite(SsrPin, HIGH);
  }
  else{
    digitalWrite(SsrPin, LOW);
  }
}

void checkButton(){
  //read button
  ClickEncoder::Button b = encoder->getButton();
  
  if(mode == M_OFF){
    if(b == ClickEncoder::Pressed){
      mode = M_TL;
    }
    /*
    else if( b == ClickEncoder::DoubleClicked){
      mode = M_LED;
    }
    */
  }
  else{
    if(b == ClickEncoder::Pressed){
      mode = M_OFF;
    }
    /*
    else if( b == ClickEncoder::DoubleClicked){
      if(mode == M_LED){
        mode = M_TL;
      }
      else{
        mode++;
      }
    }
    */
  }
}

void checkRotary(){
  int delta = encoder->getValue();
  
  if(tlBrightness)
}

void updateMode(){
  static Mode prevMode = M_OFF;
  
  if(mode != prevMode){
    if(mode == M_TL){
      tlOut.set(DefaultBrightness);
    }
    else if(mode == M_OFF){
      tlOut.off();
    }
  
    prevMode = mode;
  }
}