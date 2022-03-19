#include <Arduino.h>
#include "MeasureProtocol.h"
#include <assert.h>

using namespace MeasureProtocol;

Protocol::Protocol(Filter* f):filter(f), protocolFormat() {
  init();
}

void Protocol::init() volatile {
  state = RESET;
  data_read = true;
}

bool Protocol::setData(uint32_t proto_data) volatile {
  if(hasData()) {
    state = ERROR_OCCURED;
    return false;  
  }
  data = proto_data;
  data_read = false;
  return true;     
}

void Protocol::beginTransaction() volatile {
  if(state==RESET) {
    setData(protocolFormat.transmissionBegin());
    state = BEGIN;
  } else {
    state = ERROR_OCCURED; 
  }
}

void Protocol::setInversion(bool v) volatile {
  if(state==BEGIN) {
    stepNumber = 0;
    dir = v;
    state=INVERSION_DETECTED;
  } else if(state==INVERSION_DETECTED) {
    if(dir==v) {
      state=ERROR_OCCURED;
      return;
    }
    steps = stepNumber;
    stepNumber = 0;    
    dir = v;
    state=STEPS_COUNTED;
    setData(protocolFormat.stepsFinalized(steps, dir));
  } else if(state==STEPS_COUNTED) {
    if(stepNumber!=steps || dir==v) {
      state=ERROR_OCCURED;
      return;
    }
    dir=v;
    setData(protocolFormat.inversion(steps, dir));
    stepNumber = 0;
  }
}

void Protocol::markStep() volatile {
  if(state==INVERSION_DETECTED) {
    stepNumber++;
  } else if(state==STEPS_COUNTED) {
    stepNumber++;
    if(filter->hasValue()) {
       setData(protocolFormat.pinReading(filter->getValue()));
    } else {
      setData(protocolFormat.pinReadingNotAvailable());  
    }
    if(stepNumber>steps) {
      state = ERROR_OCCURED;
    }
  }
}

bool Protocol::hasData() volatile {
  return !data_read;  
}

uint32_t Protocol::getData() volatile {
  assert(!data_read);
  uint32_t cp_data = data;
  data_read = true;
  return cp_data;
}

bool Protocol::hasErrorOcurred() volatile {
  return state == ERROR_OCCURED;  
}
void Protocol::reset() volatile {
  init();
}

void Protocol::onData(uint8_t* buff, uint16_t len) volatile {
  state=ERROR_OCCURED;
}
