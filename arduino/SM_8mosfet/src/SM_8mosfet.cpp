extern "C"
{
#include <inttypes.h>
}

#include "Arduino.h"
#include "Wire.h"
#include "SM_8mosfet.h"

#define MOSFET8_INPORT_REG_ADD	0x00
#define MOSFET8_OUTPORT_REG_ADD	0x01
#define MOSFET8_POLINV_REG_ADD	0x02
#define MOSFET8_CFG_REG_ADD		0x03
#define MOSFET8_HW_I2C_BASE_ADD	0x38

const uint8_t mosfetsRemap[8] =
{
	0x20,
	0x40,
	0x08,
	0x04,
	0x02,
	0x01,
	0x80,
	0x10};

SM_8mosfet::SM_8mosfet()
{
}

uint8_t SM_8mosfet::begin(uint8_t stack)
{
	if (stack < 0)
	{
		stack = 0;
	}
	if (stack > 7)
	{
		stack = 7;
	}

	_hwAdd = MOSFET8_HW_I2C_BASE_ADD + stack ^ 0x07;
	
	Wire.begin();
	Wire.beginTransmission(_hwAdd);
	Wire.write(MOSFET8_OUTPORT_REG_ADD);
	Wire.write(0xff); //make all High
	if(0 !=  Wire.endTransmission())
	{
		return 1;
	}

	Wire.begin();
	Wire.beginTransmission(_hwAdd);
	Wire.write(MOSFET8_CFG_REG_ADD);
	Wire.write(0x00); //make all output
	return Wire.endTransmission();
}

uint8_t SM_8mosfet::begin()
{
	return begin(0);
}
uint8_t SM_8mosfet::rawRead(uint8_t *val)
{
	Wire.beginTransmission(_hwAdd);
	Wire.write(MOSFET8_OUTPORT_REG_ADD);
	if (Wire.endTransmission() != 0)
	{
		return 1;
	}

	Wire.requestFrom(_hwAdd, (uint8_t)1);
	if (1 <= Wire.available())
	{
		*val = Wire.read();
	}
	return 0;
}

uint8_t SM_8mosfet::rawWrite(uint8_t val)
{

	Wire.beginTransmission(_hwAdd);
	Wire.write(MOSFET8_OUTPORT_REG_ADD);
	Wire.write(val);
	return Wire.endTransmission();
}

uint8_t SM_8mosfet::set(uint8_t channel, uint8_t state)
{
	uint8_t val = 0;

	if (channel < 1)
	{
		channel = 1;
	}
	if (channel > 8)
	{
		channel = 8;
	}

	if (rawRead(&val) != 0)
	{
		return 1;
	}

	if (state == 0)//reverse polarity
	{
		val |= mosfetsRemap[channel - 1];
	}
	else
	{
		val &= ~mosfetsRemap[channel - 1];
	}

	return rawWrite(val);
}

uint8_t SM_8mosfet::set(int channel, int state)
{
	return set((uint8_t)channel, (uint8_t)state);
}

uint8_t SM_8mosfet::get(uint8_t channel)
{
	uint8_t val = 0;

	if (channel < 1)
	{
		channel = 1;
	}
	if (channel > 8)
	{
		channel = 8;
	}

	if (rawRead(&val) != 0)
	{
		return 2;
	}
	if ( (val & mosfetsRemap[channel - 1]) == 0)
	{
		return 1;
	}
	return 0;
}

uint8_t SM_8mosfet::setAll(uint8_t val)
{
	uint8_t outVal = 0;
	uint8_t i = 0;

	for (i = 0; i < 8; i++)
	{
		if ( ( (1 << i) & val) == 0)
		{
			outVal += mosfetsRemap[i];
		}
	}
	return rawWrite(outVal);
}

uint8_t SM_8mosfet::getAll()
{
	uint8_t val = 0;
	uint8_t outVal = 0;
	uint8_t i = 0;
	if (rawRead(&val) != 0)
	{
		return 0xff; // todo: error passing
	}
	for (i = 0; i < 8; i++)
	{
		if ( (val & mosfetsRemap[i]) == 0)
		{
			outVal += 1 << i;
		}
	}
	return outVal;

}
