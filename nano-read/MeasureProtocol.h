#pragma once

namespace MeasureProtocol {
  const uint8_t MAX_FILTER_SIZE=5;
  
  enum ProtocolState {
      RESET,
      BEGIN,
      INVERSION_DETECTED,
      STEPS_COUNTED,
      ERROR_OCCURED
  };

  class Filter {
    
    uint16_t filter[MAX_FILTER_SIZE];
    uint8_t filter_size;
    uint8_t filter_index;
    uint32_t filter_sum;    
    bool filter_full;
        
    public:
      Filter(uint8_t);
      uint16_t getValue();
      void setNextValue(uint16_t);
      bool hasValue();
      void reset();
  };

  class ProtocolFormat {
    uint8_t transaction_id;
    enum ControlChar {
      TRANSACTION_BEGIN,
      STEPS_FINALIZED,
      STEP_READING,
      STEP_READING_NOT_AVAILABLE,
      INVERSION
    };
    public:
      uint32_t pinReading(uint16_t reading);
      uint32_t pinReadingNotAvailable();
      uint32_t inversion(int16_t steps, bool dir);
      uint32_t transmissionBegin();
      uint32_t stepsFinalized(int16_t steps, bool dir);
  };
  
  class Protocol {
    
    ProtocolState state;
    bool data_read;
    uint32_t data;

    Filter* filter;
    ProtocolFormat protocolFormat;
    int16_t steps;
    bool dir;
    int16_t stepNumber;
    
    void init() volatile;
    bool setData(uint32_t) volatile;
    
    public:
      
      Protocol(Filter*);
      void beginTransaction()volatile;    
      void setInversion(bool)volatile;
      void markStep()volatile;
      void onData(uint8_t*, uint16_t)volatile;
      bool hasData()volatile;
      uint32_t getData()volatile;
      bool hasErrorOcurred()volatile;
      void reset()volatile;
  };
}
