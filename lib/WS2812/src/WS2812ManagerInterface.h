#pragma once
#include <inttypes.h>

class WS2812ManagerInterface
{
	public:
		
		struct __attribute__((__packed__)) color_t
		{
			uint8_t G;
			uint8_t R;
			uint8_t B;
		};
		
		struct __attribute__((aligned(4))) frame_buffer_t
		{
			volatile bool is_sending = false;					// Флаг выполнения отправки данных на экран
			volatile bool is_rendered = false;					// Флаг готовности буфера к отправкe
			static constexpr uint8_t width = DISPLAY_WIDTH;		// Ширина кадра
			static constexpr uint8_t height = DISPLAY_HEIGHT;	// Высота кадра
			union __attribute__((aligned(4)))
			{
				// Массив байт кадрового буфера
				uint8_t raw[(DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(color_t))];

				// Массив пикселей кадрового буфера
				color_t pixel[(DISPLAY_WIDTH * DISPLAY_HEIGHT)];
			};
			
			uint16_t (*Convertor)(uint16_t input, uint8_t width, uint8_t height) = [](uint16_t input, uint8_t width, uint8_t height){ return input; };
		};
		
};
