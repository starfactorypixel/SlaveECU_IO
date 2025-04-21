#pragma once
#include <inttypes.h>
#include "FastString.h"

class FrameBufferInterface
{
	public:
		
		struct __attribute__((__packed__)) color_rgb_t
		{
			uint8_t R;
			uint8_t G;
			uint8_t B;
		};

		struct __attribute__((__packed__)) color_grb_t
		{
			uint8_t G;
			uint8_t R;
			uint8_t B;
		};

		struct __attribute__((__packed__)) color_bgr_t
		{
			uint8_t B;
			uint8_t G;
			uint8_t R;
		};
};

class FrameBuffer
{
	public:
		
#if not DISPLAY_WIDTH > 0 or not DISPLAY_HEIGHT > 0
		#error You must specify 'DISPLAY_WIDTH' and 'DISPLAY_HEIGHT' before including 'FrameBuffer.h'
#endif
		
		using color_t = FrameBufferInterface::color_rgb_t;
		
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
		
		
		
		FrameBuffer() : _Mapper(&FrameBuffer::_Mapper0), _brightness(255)
		{}
		
		void SetMapper(uint8_t id)
		{
			switch(id)
			{
				case 0:  { _Mapper = &FrameBuffer::_Mapper0; break; }
				case 1:  { _Mapper = &FrameBuffer::_Mapper1; break; }
				case 2:  { _Mapper = &FrameBuffer::_Mapper2; break; }
				default: { _Mapper = &FrameBuffer::_Mapper0; break; }
			}
			
			return;
		}
		
		void SetBrightness(uint8_t brightness)
		{
			_brightness = brightness;

			return;
		}
		
		void Prepare()
		{
			if(_brightness < 255)
			{
				for(color_t &pixel : frame_buffer.pixel)
				{
					AdjustBrightness(pixel, _brightness);
				}
			}

			return;
		}
		
		inline void GetPixel(uint16_t idx, color_t &pixel, bool clear)
		{
			if(idx >= sizeofarray(frame_buffer.pixel)) return;
			
			//uint16_t index = (this->*_Mapper)(idx);
			uint16_t index = idx;
			pixel = frame_buffer.pixel[index];
			if(clear == true)
				frame_buffer.pixel[index] = {0x00, 0x00, 0x00};
			
			return;
		}
		
		inline void GetPixel(uint8_t x, uint8_t y, color_t &pixel, bool clear)
		{
			if(x >= frame_width || y >= frame_height) return;
			
			uint16_t idx = x + (y * frame_width);
			//uint16_t index = (this->*_Mapper)(idx);
			uint16_t index = idx;
			pixel = frame_buffer.pixel[index];
			if(clear == true)
				frame_buffer.pixel[index] = {0x00, 0x00, 0x00};
			
			return;
		}
		
		inline void SetPixel(uint16_t idx, const color_t &pixel)
		{
			if(idx >= sizeofarray(frame_buffer.pixel)) return;
			
			//uint16_t index = (this->*_Mapper)(idx);
			uint16_t index = idx;
			frame_buffer.pixel[index] = pixel;
			
			return;
		}
		
		inline void SetPixel(uint8_t x, uint8_t y, const color_t &pixel)
		{
			if(x >= frame_width || y >= frame_height) return;
			
			uint16_t idx = x + (y * frame_width);
			//uint16_t index = (this->*_Mapper)(idx);
			uint16_t index = idx;
			frame_buffer.pixel[index] = pixel;
			
			return;
		}
		
		inline void Clear()
		{
			memset_dma32(frame_buffer.raw, 0x00000000, sizeof(frame_buffer.raw));

			return;
		}
		
		void AdjustBrightness(color_t &color, uint8_t brightness)
		{
			color.R = (color.R * brightness) / 255;
			color.G = (color.G * brightness) / 255;
			color.B = (color.B * brightness) / 255;
			
			return;
		}



	private:
		
		/*
			Конвертор - заглушка, оставляет индекс не тронутым
		*/
		uint16_t _Mapper0(uint16_t input)
		{
			return input;
		}

		/*
			Конвертор индексов 2D кадрового буфера в вертикальный зиг-заг, сверху-вниз, слево-направо (светодиодне панели)
		*/
		uint16_t _Mapper1(uint16_t input)
		{
			uint8_t row = input / frame_width;
			uint8_t col = input % frame_width;
			uint16_t index = col * frame_height + (col % 2 == 0 ? row : (frame_height - row - 1));
			
			return index;
		}

		/*
			Конвертор индексов 2D кадрового буфера в горизонтальный зиг-заг, слево-направо, сверху-вниз (светодиодне ленты)
		*/
		uint16_t _Mapper2(uint16_t input)
		{
			uint8_t row = input / frame_width;
			uint8_t col = input % frame_width;
			uint16_t index = row * frame_width + (row % 2 == 0 ? col : (frame_width - col - 1));
			
			return index;
		}
		
		
		uint16_t (FrameBuffer::*_Mapper)(uint16_t);
		uint8_t _brightness;
		
};
