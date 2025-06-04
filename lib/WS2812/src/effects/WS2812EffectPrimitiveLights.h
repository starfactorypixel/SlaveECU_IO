#pragma once
#include <inttypes.h>
#include "WS2812EffectInterface.h"
#include "PrimitiveDraw.h"
#include "image.h"

#include <PXLParser.h>
#include <PXLReaderArray.h>
#include <PXLReaderSPI.h>
#include <PXLReaderUART.h>

class WS2812EffectPrimitiveLights : public WS2812EffectInterface
{
	static constexpr uint8_t width = FrameBuffer::frame_width;
	static constexpr uint8_t height = FrameBuffer::frame_height;
	
	//PXLReaderArray reader;
	//PXLReaderSPI reader;
	PXLReaderUART reader;
	//PXLParser pxl(width, height);
	PXLParser pxl;
	

	public:

		WS2812EffectPrimitiveLights() : pxl(width, height), _pd(width, height)
		{

		};

		virtual void Init() override
		{

			
			//reader.PutFileMap( {"dragon.pxl", dragon, sizeof(dragon)} );
			//reader.PutFileMap( {"test.pxl", test, sizeof(test)} );
/*
			reader.PutFileMap( {"dragon1.pxl", 0x001000, 40754UL} );	// 4096UL
			reader.PutFileMap( {"dragon2.pxl", 0x00B000, 42975UL} );	// 4096UL
			reader.PutFileMap( {"dragon3.pxl", 0x020000, 40754UL} );	// 4096UL
			reader.PutFileMap( {"dragon4.pxl", 0x030000, 38444UL} );	// 4096UL
			reader.PutFileMap( {"dragon5.pxl", 0x03A000, 27714UL} );	// 4096UL
*/
			pxl.SetIgnoreFrameTimeout(true);
			pxl.SetAutoReopen(true);
			pxl.SetReader(reader);
			pxl.OpenFile("dragon5.pxl");
			//pxl.OpenFile("test.pxl");





			_pd.SetFrameBuffer(*_frame_buffer);
			h = Random(0, 15);
			
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


			//static int16_t x_offset = 0;

			uint32_t lasttime = HAL_GetTick();

			Leds::obj.SetOn(Leds::LED_WHITE);

			//pxl.GetAutoFrame(time, [&](file_pixel_t &pixel_data, uint8_t x, uint8_t y)
			pxl.GetAutoFrame(time, [&](uint16_t index, uint8_t data[4])
			{
				if(data[3] < 255)
					return;

				//memcpy(_frame_buff + pixel_data.index, &pixel_data.color1, 3);
				color_t &pixel = *(color_t *)data;
				_frame_buffer->SetPixel(index, pixel);
				//_frame_buffer->SetPixel(x + x_offset, y, pixel);
			});

			Leds::obj.SetOff(Leds::LED_WHITE);

			DEBUG_LOG_TOPIC("PXLDraw", "time: %d\n", (HAL_GetTick() - lasttime));

			//if( (x_offset += 4) >= width ) x_offset = -36;











// Дракон gif
/*
			static int16_t x_offset = 0;
			static uint8_t frame_idx = 0;

			FrameBuffer::color_t color;
			//uint16_t index_2d;
			for (uint8_t x = 0; x < 36; x++)
			{
				for (uint8_t y = 0; y < 16; y++)
				{
					//index_2d = x + (y * width) + x_offset;
					const uint8_t *tmp = image_data[frame_idx][y][x];
					color.G = tmp[0]; color.R = tmp[1]; color.B = tmp[2];
					
					_frame_buffer->SetPixel(x + x_offset, y, color);
					
				}
				
			}
			if( ++frame_idx == (sizeof(image_data) / sizeof(image_data[0])) ) frame_idx = 0;
			if( (x_offset += 4) >= width ) x_offset = -36;
*/			


/*
			uint16_t src, dst;
			uint8_t decay, intensity;
			color_t color;
			for(uint8_t y = 0; y < height; ++y)
			{
				for(uint8_t x = 0; x < width - 1; ++x)
				{
					src = x + 1 + (y * width);
					dst = x + (y * width);

					_frame_buffer->GetPixel(src, color, true);
					_frame_buffer->SetPixel(dst, color);
				}
			}

			
			h += Random(-1, 1);
			h = clamp<int8_t>(h, 0, 15);
			PrimitiveDraw::line_params_t params = {};
			params.x0 = 127;
			params.y0 = 15;
			params.x1 = 127;
			params.y1 = h;
			//params.color = {(uint8_t)Random(0, 31), (uint8_t)Random(0, 31), (uint8_t)Random(0, 31)};
			params.color = {0x00, 0x06, 0x00};
			params.thickness = 1;
			_pd.draw_line(params);
			uint16_t idx = 127 + (h * width);
			color = {0x00, 0x64, 0x00};
			_frame_buffer->SetPixel(idx, color);
*/

/*
			for(uint8_t y = 0; y < height; ++y)
			{
				for(uint8_t x = 0; x < width; ++x)
				{
					uint16_t dst = x + (y * width);
					color_t &pixel = _frame_buffer->pixel[dst];
					if( pixel.R > 0 ) pixel.R--;
					if( pixel.G > 0 ) pixel.G--;
					if( pixel.B > 0 ) pixel.B--;
				}
			}
*/
/*
			PrimitiveDraw::line_params_t params = {};
			params.x0 = Random(0, 127);
			params.y0 = Random(0, 15);
			params.x1 = Random(0, 127);
			params.y1 = Random(0, 15);
			params.color = {(uint8_t)Random(0, 31), (uint8_t)Random(0, 31), (uint8_t)Random(0, 31)};
			params.thickness = 1;
			_pd.draw_line(params);
*/

/*
			static uint8_t h = 0;
			for(uint8_t x = 0; x < width; ++x)
			{
				h += Random(-1, 1);
				h = clamp<uint8_t>(h, 0, 15);
				PrimitiveDraw::line_params_t params = {};
				params.x0 = x;
				params.y0 = 15;
				params.x1 = x;
				params.y1 = h;
				params.color = {(uint8_t)Random(0, 31), (uint8_t)Random(0, 31), (uint8_t)Random(0, 31)};
				params.thickness = 1;
				_pd.draw_line(params);
			}
*/


			//updateEffect();
			
			return true;
		}

		void DrawStop()
		{
			PrimitiveDraw::line_params_t params = {};
			params.x0 = 3;
			params.y0 = 4;
			params.x1 = 110;
			params.y1 = 15;
			params.color = {0x05, 0x05, 0x00};
			params.thickness = 3;
			_pd.draw_line(params);
		}

	private:
		
		PrimitiveDraw _pd;

		int8_t h;



	
		
};
