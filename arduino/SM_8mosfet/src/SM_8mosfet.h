#ifndef SM_8mosfet_h
#define SM_8mosfet_h
#include "Arduino.h"
#include <inttypes.h>

class SM_8mosfet
{
  public:
    SM_8mosfet();
    uint8_t begin(uint8_t);
    uint8_t begin();
    uint8_t set(uint8_t, uint8_t);
    uint8_t set(int, int);
    uint8_t get(uint8_t);
    uint8_t setAll(uint8_t);
    uint8_t getAll();
     
    inline uint8_t get(int channel) { return get((uint8_t)channel); }
    inline uint8_t setAll(int val) { return setAll((uint8_t)val); }
  private:
    uint8_t _hwAdd;
    uint8_t rawRead(uint8_t *val);
    uint8_t rawWrite(uint8_t val);
};

#endif