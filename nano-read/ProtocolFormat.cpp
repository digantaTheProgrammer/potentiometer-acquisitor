#include <Arduino.h>
#include "MeasureProtocol.h"
#include <assert.h>

using namespace MeasureProtocol;
#define htonl(x) __builtin_bswap32((uint32_t) (x))

uint32_t generate_data(uint8_t &transaction_id, uint8_t control_char, uint16_t proto_data) {
  uint32_t data = 0;
  data = (static_cast<uint32_t>(transaction_id&0xFF)<<24) + (static_cast<uint32_t>(control_char&0xFF)<<16) + proto_data;
  transaction_id++;
  return htonl(data);  
}

uint16_t get_steps(int16_t steps, bool dir) {
  assert(steps>=0 && steps<=INT16_MAX);
  return (dir<<15) | steps;
}
uint32_t ProtocolFormat::transmissionBegin() {
  transaction_id = random(UINT8_MAX);
  return generate_data(transaction_id, TRANSACTION_BEGIN, 0);
}

uint32_t ProtocolFormat::stepsFinalized(int16_t steps, bool dir) {
  return generate_data(transaction_id, STEPS_FINALIZED, get_steps(steps, dir));  
}
uint32_t ProtocolFormat::pinReading(uint16_t reading) {
  return generate_data(transaction_id, STEP_READING, reading);
}

uint32_t ProtocolFormat::pinReadingNotAvailable() {
  return generate_data(transaction_id, STEP_READING_NOT_AVAILABLE, 0);
}

uint32_t ProtocolFormat::inversion(int16_t steps, bool dir) {
  return generate_data(transaction_id, INVERSION, get_steps(steps, dir));
}
