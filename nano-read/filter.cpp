#include <Arduino.h>
#include "MeasureProtocol.h"
#include <assert.h>

using namespace MeasureProtocol;

Filter::Filter(uint8_t filter_size): filter_size(filter_size), filter_full(false), filter_index(0), filter_sum(0) {
  assert(filter_size<=MAX_FILTER_SIZE);
}

bool Filter::hasValue(){
  return filter_full;
}

void Filter::setNextValue(uint16_t value) {
  filter_sum+=value;
  if(hasValue()) {
    filter_sum-=filter[filter_index];
  }
  filter[filter_index++] = value;
  if(filter_index==filter_size) {
    filter_full = true;
    filter_index = 0;
  }
}

uint16_t Filter::getValue() {
  assert(hasValue());
  return filter_sum/filter_size;
}

void Filter::reset() {
  filter_full = false;
  filter_index = 0;
}
