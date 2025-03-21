#pragma once
#include <inttypes.h>
#include "WS2812EffectInterface.h"
#include "PrimitiveDraw.h"

class WS2812EffectPrimitiveLights : public WS2812EffectInterface
{
	static constexpr uint8_t width = FrameBuffer::frame_width;
	static constexpr uint8_t height = FrameBuffer::frame_height;

	public:

		WS2812EffectPrimitiveLights(): _pd(width, height)
		{

		};

		virtual void Init() override
		{
			_pd.SetFrameBuffer(*_frame_buffer);
			h = Random(0, 15);
			
			return;
		}
		
		virtual void Tick(uint32_t time) override
		{
			return;
		}
		
		virtual void Render(uint32_t time) override
		{
			//memset_dma32(_frame_buffer->raw, 0x00000000, sizeof(_frame_buffer->raw));

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
			_frame_buffer->is_rendered = true;
			
			return;
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
