#pragma once
#include <inttypes.h>
#include <string.h>
#include "FrameBuffer.h"
#include "FastString.h"

class FrameEffectInterface
{
	public:
		
		void PrepareInit(FrameBuffer &frame_buffer)
		{
			this->frame_buffer = &frame_buffer;
			
			return;
		};

		// Вызывается один раз в момент инициализации объекта
		virtual void Init() = 0;

		// Вызывается каждые 5 мс (по мере возможности)
		// uint32_t time - Текущее время
		virtual void Tick(uint32_t time) = 0;

		// Вызывается 1 и более раз, когда нужно начинать готовить следующий кадр
		// uint32_t time - Расчитанное время следующего кадра
		// return - true если кадр готов, false если не готов.
		virtual bool FramePrepare(uint32_t time) = 0;
		
	protected:
		
		int32_t Random() { return rand(); }
		int32_t Random(int32_t min, int32_t max) { return (rand() % (max - min + 1)) + min; }
		float RandomFloat(){ return (float)rand() / (float)RAND_MAX; }
		
		// Простая 2D версия шума (упрощенная для МК)
		uint8_t ValueNoiseInt(int32_t x, int32_t y)
		{
			int32_t n = x + y * 57;
			n = (n << 13) ^ n;
			return (uint8_t)(0 + (((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) >> 23));
		}
		
		// Простая 2D версия шума (упрощенная для МК)
		float ValueNoiseFloat(float x, float y)
		{
			uint32_t n = (int)x + ((int)y * 57);
			n = (n << 13) ^ n;
			return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
		}
		
		
		FrameBuffer *frame_buffer = nullptr;
		using color_t = FrameBuffer::color_t;
		static constexpr uint8_t frame_width = FrameBuffer::frame_width;
		static constexpr uint8_t frame_height = FrameBuffer::frame_height;
		
};
