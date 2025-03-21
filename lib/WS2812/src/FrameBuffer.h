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
	using FuncPtr = uint16_t (FrameBuffer::*)(uint16_t);

	public:
		
#if not DISPLAY_WIDTH > 0 or not DISPLAY_HEIGHT > 0
		#error You must specify 'DISPLAY_WIDTH' and 'DISPLAY_HEIGHT' before including 'FrameBuffer.h'
#endif	

#if DISPLAY_PIXEL_TYPE == 1
		using color_t = FrameBufferInterface::color_rgb_t
#elif DISPLAY_PIXEL_TYPE == 2
		using color_t = FrameBufferInterface::color_grb_t;
#elif DISPLAY_PIXEL_TYPE == 3
		using color_t = FrameBufferInterface::color_bgr_t;
#else
		#error You must specify 'DISPLAY_PIXEL_TYPE' before including 'FrameBuffer.h'
#endif

#if DISPLAY_TYPE == 1
		FuncPtr Mapper = &FrameBuffer::_Mapper0;
#elif DISPLAY_TYPE == 2
		FuncPtr Mapper = &FrameBuffer::_Mapper1;
#elif DISPLAY_TYPE == 3
		FuncPtr Mapper = &FrameBuffer::_Mapper2;
#else
		#error You must specify 'DISPLAY_TYPE' before including 'FrameBuffer.h'
#endif




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
		
		volatile bool is_sending = false;						// Флаг выполнения отправки данных на экран
		volatile bool is_rendered = false;						// Флаг готовности буфера к отправкe
		static constexpr uint8_t frame_width = DISPLAY_WIDTH;	// Ширина кадра
		static constexpr uint8_t frame_height = DISPLAY_HEIGHT;	// Высота кадра
		frame_buffer_t frame_buffer;							// Массив пикселей и байт




		inline void GetPixel(uint16_t idx, color_t &pixel, bool clear)
		{
			uint16_t index = (this->*Mapper)(idx);
			pixel = frame_buffer.pixel[index];
			if(clear == true)
				frame_buffer.pixel[index] = {0x00, 0x00, 0x00};
			
			return;
		}
		
		inline void SetPixel(uint16_t idx, color_t &pixel)
		{
			uint16_t index = (this->*Mapper)(idx);
			frame_buffer.pixel[index] = pixel;

			return;
		}
		
		inline void Clear()
		{
			memset_dma32(frame_buffer.raw, 0x00000000, sizeof(frame_buffer.raw));

			return;
		}



	private:
		
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
};
