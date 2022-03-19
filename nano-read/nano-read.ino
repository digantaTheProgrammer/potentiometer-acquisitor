#include "MeasureProtocol.h"

const int clock_pin = 3; //should not change (interrupt compatible pin required)
const int rate_pin = 2; //should not change (interrupt compatible pin required)
const int unconnected_pin = A0; //any pin which is not connected and not near to any voltage line

const int clock_mask = (1<<clock_pin);
const int rate_mask = (1<<rate_pin);

const int MAX_BUFF_SIZE = 4;
uint8_t buff[MAX_BUFF_SIZE];

MeasureProtocol::Filter filter(2);
volatile MeasureProtocol::Protocol protocol(&filter);

void setup(){

  pinMode(unconnected_pin, INPUT);
  randomSeed(analogRead(unconnected_pin));
  
  ADMUX &= B11011111;
  ADMUX |= B01000000;
  ADMUX &= B11110000;
  ADMUX |= 4;
  ADCSRA |= B10000000;
  ADCSRA |= B00100000;
  ADCSRB &= B11111000;
  ADCSRA |= B00000111;
  ADCSRA |= B00001000;
  sei();
  ADCSRA |=B01000000;

  pinMode(rate_pin, INPUT_PULLUP);
  pinMode(clock_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(clock_pin), clock_rise, RISING);
  attachInterrupt(digitalPinToInterrupt(rate_pin), rate_change, CHANGE);
  protocol.beginTransaction();
  Serial.begin(115200);
}


void loop(){
  uint16_t available_data_len = min(Serial.available(), MAX_BUFF_SIZE);
  if(available_data_len) {
    available_data_len = Serial.readBytes(buff, available_data_len);
    protocol.onData(buff, available_data_len);
  } 
  if(protocol.hasData()) {
    uint32_t protocol_data = protocol.getData();
    Serial.write(reinterpret_cast<uint8_t*>(&protocol_data), sizeof(protocol_data));
    Serial.flush();
  } else if(protocol.hasErrorOcurred()){
    Serial.println("Error");
    Serial.flush();
    delay(1000);
  }
}

ISR(ADC_vect){
  uint16_t analogVal = ADCL | (ADCH << 8);
  filter.setNextValue(analogVal);
}

void clock_rise() {
  protocol.markStep();
}

void rate_change() {
  protocol.setInversion(PIND&rate_mask); 
}
