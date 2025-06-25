#pragma once
#include <inttypes.h>
#include "FrameEffectInterface.h"

#include <PXLParser.h>
#include <readers/PXLReaderArray.h>
#include <readers/PXLReaderSPI.h>
#include <readers/PXLReaderUART.h>

class FrameEffectReader : public FrameEffectInterface
{
	//PXLReaderArray reader;
	//PXLReaderSPI reader;
	PXLReaderUART reader;
	
	PXLParser pxl;
	
	public:
		
		FrameEffectReader() : pxl(frame_width, frame_height)
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
			frame_buffer->Clear();
			
			uint32_t lasttime = HAL_GetTick();
			Leds::obj.SetOn(Leds::LED_WHITE);
			
			pxl.GetAutoFrame(time, [&](const pxl_pixel_t &pxl_pixel)
			{
				if(pxl_pixel.color[3] < 255)
					return;
				
				color_t &pixel = *(color_t *)pxl_pixel.color;
				
				if(pxl_pixel.repeat > 1)
					frame_buffer->SetPixel(pxl_pixel.index, pixel, pxl_pixel.repeat);
				else
					frame_buffer->SetPixel(pxl_pixel.index, pixel);
			});
			
			Leds::obj.SetOff(Leds::LED_WHITE);
			DEBUG_LOG_TOPIC("PXLDraw", "time: %d\n", (HAL_GetTick() - lasttime));
			
			return true;
		}
		
	private:
		
};
