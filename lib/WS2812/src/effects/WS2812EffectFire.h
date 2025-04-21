#pragma once
#include <inttypes.h>
#include "WS2812EffectInterface.h"

class WS2812EffectFire : public WS2812EffectInterface
{
	static constexpr uint8_t width = FrameBuffer::frame_width;
	static constexpr uint8_t height = FrameBuffer::frame_height;
	
	public:
		
		virtual void Init() override
		{
			return;
		}
		
		virtual void Tick(uint32_t time) override
		{
			return;
		}
		
		virtual bool FramePrepare(uint32_t time) override
		{
			updateFireEffect();
			
			return true;
		}

	private:
		
		// Функция для обновления эффекта огня
		void updateFireEffect()
		{
			static uint8_t firePixels[width * height] = {};
			
			uint16_t src, dst;
			uint8_t decay, intensity;
			color_t color;
			
			// Распространение огня вверх
			for(uint8_t y = 0; y < height - 1; ++y)
			{
				for(uint8_t x = 0; x < width; ++x)
				{
					src = x + ((y + 1) * width);
					dst = x + (y * width);
					decay = rand() % 48;

					if(x > 0 && x < width && firePixels[src] < 48)	// firePixels[src] < 48
						dst += Random(-1, 1);
					
					firePixels[dst] = (firePixels[src] > decay) ? firePixels[src] - decay : firePixels[src] / 2; // : 0
				}
			}
			
			// Генерация шума в нижней части буфера
			for(uint8_t x = 0; x < width; ++x)
			{
				dst = x + ((height - 1) * width);
				
				firePixels[dst] = (uint8_t)(ValueNoiseFloat(x * 1.1, RandomFloat() * 10) * 255);	// x * 0.1
				//if(firePixels[dst] < 32) firePixels[dst] = 0;
				
				//firePixels[dst] = (uint8_t)(PerlinNoiseInt(x * 1, Random(0, 255) * 100) * 1);
				//if(firePixels[dst] < 128) firePixels[dst] = 0;
			}
			
			// Преобразование в RGB буфер
			for(uint8_t y = 0; y < height; ++y)
			{
				for(uint8_t x = 0; x < width; ++x)
				{
					dst = x + (y * width);
					intensity = firePixels[dst] / 16;

					color.G = intensity / 5;/*(intensity > 128) ? (255 - intensity) * 2 : intensity / 2;*/
					color.R = intensity;
					color.B = 0;
					
					//uint16_t index = _frame_buffer->Convertor(dst, width, height);
					//_frame_buffer->pixel[index] = color;
					_frame_buffer->SetPixel(dst, color);
				}
			}
		}
};
