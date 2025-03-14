#pragma once
#include <inttypes.h>
#include "WS2812Manager.h"
#include "WS2812EffectInterface.h"

/*
	_manager - Объект менеджера
	_frame_buffer - Объект кадрового буфера с параметрами и флагами
	Init() - Вызывается при инициализации эффекта
	Tick() - Вызывается не менее чем каждые 1 мс
	Render() - Вызывается каждые 100 мс
	Для расчёта положения пикселя в кадровом буфере нужно воспользоваться такой конструкцией:
		uint16_t index = _frame_buffer->Convertor(index_2d, width, height);
		_frame_buffer->pixel[index] = color;
*/

class WS2812EffectTest : public WS2812EffectInterface
{
	public:
		
		virtual void Init() override
		{
			return;
		}
		
		virtual void Tick(uint32_t time) override
		{
			return;
		}
		
		virtual void Render(uint32_t time) override
		{
			updateEffect();
			_frame_buffer->is_rendered = true;
			
			return;
		}
		
	private:
		
		void updateEffect()
		{
			memset_dma32(_frame_buffer->raw, 0x00000000, sizeof(_frame_buffer->raw));
			
			static const uint8_t width = _frame_buffer->width;
			static const uint8_t height = _frame_buffer->height;

			for(uint8_t y = 0; y < height; ++y)
			{
				for(uint8_t x = 0; x < width; ++x)
				{
					uint16_t dst = x + (y * width);
					float perlin_val = PerlinNoiseFloat( (float)x, ((float)y + offset) );
					uint8_t perlin_val_fix = (perlin_val * 255) / 32;

					uint8_t value = Random(0, 1);
					uint8_t r = (value ? perlin_val_fix : 0);
					uint8_t g = (value ? perlin_val_fix : 0);
					uint8_t b = (value ? perlin_val_fix : 0);

					uint16_t index = _frame_buffer->Convertor(dst, width, height);
					_frame_buffer->pixel[index] = { g, r, b };
				}
				offset += 0.5;
			}
		}

		float offset = 0;
		
};
