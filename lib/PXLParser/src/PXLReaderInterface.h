#pragma once
#include <inttypes.h>
#include <CUtils.h>

class PXLReaderInterface
{
	public:
		
		virtual int8_t Open(const char *filename) = 0;
		virtual int8_t Close() = 0;
		virtual uint16_t Read(const uint32_t offset, uint16_t length = __UINT16_MAX__) = 0;
		virtual const uint8_t *GetBufferPtr() = 0;

		virtual void Init(){}
		virtual void Deinit(){}
		virtual void Tick(){}
};
