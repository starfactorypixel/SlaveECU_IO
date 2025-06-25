#pragma once
#include <inttypes.h>
#include "FrameEffectInterface.h"
#include "CUtils_Perlin2DInt.h"

class FrameEffectFire : public FrameEffectInterface
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
		
		virtual bool FramePrepare(uint32_t time) override
		{
			updateFireEffect();
			
			return true;
		}

	private:
		
		// Функция для обновления эффекта огня
		void updateFireEffect()
		{
			static uint8_t firePixels[frame_width * frame_height] = {};
			
			uint16_t src, dst;
			uint8_t decay, intensity;
			color_t color;
			
			// Распространение огня вверх
			for(uint8_t y = 0; y < frame_height - 1; ++y)
			{
				for(uint8_t x = 0; x < frame_width; ++x)
				{
					src = x + ((y + 1) * frame_width);
					dst = x + (y * frame_width);
					decay = rand() % 48;

					if(x > 0 && x < frame_width && firePixels[src] < 48)	// firePixels[src] < 48
						dst += Random(-1, 1);
					
					firePixels[dst] = (firePixels[src] > decay) ? firePixels[src] - decay : firePixels[src] / 2; // : 0
				}
			}

			static int32_t y_offset = 0;
			
			// Генерация шума в нижней части буфера
			for(uint8_t x = 0; x < frame_width; ++x)
			{
				dst = x + ((frame_height - 1) * frame_width);
				
				int16_t value = Perlin2DInt::noise2d(x * 64, y_offset/*Random(-255, 255)*/);
				firePixels[dst] = (value + 1024) & 0xFF;
				
				//firePixels[dst] = (uint8_t)(ValueNoiseFloat(x * 1.1, RandomFloat() * 10) * 255);	// x * 0.1
				//if(firePixels[dst] < 32) firePixels[dst] = 0;
				
				//firePixels[dst] = (uint8_t)(PerlinNoiseInt(x * 1, Random(0, 255) * 100) * 1);
				//if(firePixels[dst] < 128) firePixels[dst] = 0;
			}

			y_offset += 32;
			
			// Преобразование в RGB буфер
			for(uint8_t y = 0; y < frame_height; ++y)
			{
				for(uint8_t x = 0; x < frame_width; ++x)
				{
					dst = x + (y * frame_width);
					intensity = firePixels[dst];

					color.R = intensity;
					color.G = intensity / 8;/*(intensity > 128) ? (255 - intensity) * 2 : intensity / 2;*/
					color.B = 0;
					
					//uint16_t index = frame_buffer->Convertor(dst, width, height);
					//frame_buffer->pixel[index] = color;
					frame_buffer->SetPixel(dst, color);
				}
			}
		}
};
