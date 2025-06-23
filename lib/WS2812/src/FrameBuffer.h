#pragma once
#include <inttypes.h>
#include "FastString.h"

class FrameBuffer
{

#if not DISPLAY_WIDTH > 0 or not DISPLAY_HEIGHT > 0
	#error You must specify 'DISPLAY_WIDTH' and 'DISPLAY_HEIGHT' before including 'FrameBuffer.h'
#endif
	
	public:
		
		struct __attribute__((__packed__)) color_t
		{
			uint8_t R;
			uint8_t G;
			uint8_t B;
		};
		
		struct __attribute__((aligned(4))) frame_buffer_t
		{
			union __attribute__((aligned(4)))
			{
				// Массив байт кадрового буфера
				uint8_t raw[(DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(color_t))];

				// Массив пикселей кадрового буфера
				color_t pixel[(DISPLAY_WIDTH * DISPLAY_HEIGHT)];
			};
		};
		
		volatile bool is_sending = false;						// Флаг активной отправки данных на экран
		volatile bool is_ready_sending = false;					// Флаг готовности отправки данных на экран
		volatile bool is_rendered = false;						// Флаг готовности кадра
		static constexpr uint8_t frame_width = DISPLAY_WIDTH;	// Ширина кадра
		static constexpr uint8_t frame_height = DISPLAY_HEIGHT;	// Высота кадра
		frame_buffer_t frame_buffer;							// Массив пикселей и байт
		
		static constexpr color_t color_black = {0x00, 0x00, 0x00};
		
		
		FrameBuffer() : _brightness(127), _corr_red(255), _corr_greed(255), _corr_blue(255)
		{}
		
		
		// Возвращает true когда кадр готов к отрисовки
		bool DrawIsReady()
		{
			return (is_sending == false && is_ready_sending == true);
		}
		
		// Подготавливает кадр и блокирует работу на момент отрисовки
		void DrawBegin()
		{
			is_sending = true;
			is_ready_sending = false;
			Prepare();
			
			return;
		}
		
		// Снимает блокировку на момент отрисовки
		void DrawEnding()
		{
			is_sending = false;

			return;
		}

		void SetBrightness(uint8_t brightness)
		{
			_brightness = brightness;

			return;
		}
		
		void SetColorCorrection(uint8_t red, uint8_t green, uint8_t blue)
		{
			_corr_red = red;
			_corr_greed = green;
			_corr_blue = blue;
			
			return;
		}
		
		void Prepare()
		{
			uint8_t scale_r = (_corr_red * _brightness) / 255;
			uint8_t scale_g = (_corr_greed * _brightness) / 255;
			uint8_t scale_b = (_corr_blue * _brightness) / 255;
			
			for(color_t &pixel : frame_buffer.pixel)
			{
				AdjustBrightness(pixel, scale_r, scale_g, scale_b);
			}
			
			return;
		}
		
		void GetPixel(uint16_t idx, color_t &pixel, bool clear)
		{
			if(idx >= sizeofarray(frame_buffer.pixel)) return;

			pixel = frame_buffer.pixel[idx];
			if(clear == true)
				frame_buffer.pixel[idx] = color_black;
			
			return;
		}
		
		void GetPixel(uint8_t x, uint8_t y, color_t &pixel, bool clear)
		{
			if(x >= frame_width || y >= frame_height) return;

			uint16_t idx = x + (y * frame_width);
			pixel = frame_buffer.pixel[idx];
			if(clear == true)
				frame_buffer.pixel[idx] = color_black;
			
			return;
		}

		void SetPixel(uint16_t idx, const color_t &pixel)
		{
			if(idx >= sizeofarray(frame_buffer.pixel)) return;

			frame_buffer.pixel[idx] = pixel;

			return;
		}
		
		void SetPixel(uint8_t x, uint8_t y, const color_t &pixel)
		{
			if(x >= frame_width || y >= frame_height) return;
			
			uint16_t idx = x + (y * frame_width);
			frame_buffer.pixel[idx] = pixel;
			
			return;
		}
		
		void SetPixel(uint16_t idx, const color_t &pixel, uint16_t count)
		{
			if(idx + count >= sizeofarray(frame_buffer.pixel)) return;
			
			memcpy_repeat_fast( (uint8_t *)&frame_buffer.pixel[idx], (uint8_t *)&pixel, sizeof(pixel), count );
			
			return;
		}
		
		void SetPixel(uint8_t x, uint8_t y, const color_t &pixel, uint16_t count)
		{
			if(x >= frame_width || y >= frame_height) return;
			
			uint16_t idx = x + (y * frame_width);
			return SetPixel(idx, pixel, count);
		}
		
		void Clear()
		{
			memset_dma32(frame_buffer.raw, 0x00000000, sizeof(frame_buffer.raw));

			return;
		}
		
		void AdjustBrightness(color_t &color, uint8_t scale_r, uint8_t scale_g, uint8_t scale_b)
		{
			if( (*(uint32_t*)&color & 0x00FFFFFF) == 0 ) return;
			
			color.R = (color.R * scale_r) / 255;
			color.G = (color.G * scale_g) / 255;
			color.B = (color.B * scale_b) / 255;
			
			return;
		}
		
	private:
		
		uint8_t _brightness;
		uint8_t _corr_red;
		uint8_t _corr_greed;
		uint8_t _corr_blue;

};
