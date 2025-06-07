#pragma once
#include <inttypes.h>
#include "WS2812EffectInterface.h"

#include <PXLParser.h>
#include <PXLReaderArray.h>
#include <PXLReaderSPI.h>
#include <PXLReaderUART.h>

class WS2812EffectReader : public WS2812EffectInterface
{
	static constexpr uint8_t width = FrameBuffer::frame_width;
	static constexpr uint8_t height = FrameBuffer::frame_height;
	
	//PXLReaderArray reader;
	//PXLReaderSPI reader;
	PXLReaderUART reader;
	
	PXLParser pxl;
	
	public:
		
		WS2812EffectReader() : pxl(width, height)
		{
			return;
		};
		
		virtual void Init() override
		{

/*
			// PXLReaderArray
			reader.PutFileMap( {"dragon.pxl", dragon, sizeof(dragon)} );
			reader.PutFileMap( {"test.pxl", test, sizeof(test)} );
*/
/*
			// PXLReaderSPI
			reader.PutFileMap( {"dragon1.pxl", 0x001000, 40754UL} );
			reader.PutFileMap( {"dragon2.pxl", 0x00B000, 42975UL} );
			reader.PutFileMap( {"dragon3.pxl", 0x020000, 40754UL} );
			reader.PutFileMap( {"dragon4.pxl", 0x030000, 38444UL} );
			reader.PutFileMap( {"dragon5.pxl", 0x03A000, 27714UL} );
*/
			
			pxl.SetIgnoreFrameTimeout(true);
			pxl.SetAutoReopen(true);
			pxl.SetReader(reader);
			
			//pxl.OpenFile("dragon.pxl");
			pxl.OpenFile("dragon5.pxl");
			
			return;
		}
		
		virtual void Tick(uint32_t time) override
		{
			pxl.Tick(time);
			
			return;
		}
		
		virtual bool FramePrepare(uint32_t time) override
		{
			_frame_buffer->Clear();
			
			uint32_t lasttime = HAL_GetTick();
			Leds::obj.SetOn(Leds::LED_WHITE);
			
			pxl.GetAutoFrame(time, [&](uint16_t index, uint8_t data[4])
			{
				if(data[3] < 255)
					return;
				
				color_t &pixel = *(color_t *)data;
				_frame_buffer->SetPixel(index, pixel);
			});
			
			Leds::obj.SetOff(Leds::LED_WHITE);
			DEBUG_LOG_TOPIC("PXLDraw", "time: %d\n", (HAL_GetTick() - lasttime));
			
			return true;
		}
		
	private:
		
};
