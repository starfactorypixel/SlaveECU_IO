#pragma once
#include <inttypes.h>
#include <string.h>
#include "FastString.h"

class WS2812Manager;

class WS2812EffectInterface
{
	public:

		void PrepareInit(WS2812Manager *manager, WS2812ManagerInterface::frame_buffer_t &frame_buffer)
		{
			_manager = manager;
			_frame_buffer = &frame_buffer;
			
			return;
		};

		virtual void Init() = 0;
		virtual void Tick(uint32_t time) = 0;
		virtual void Render(uint32_t time) = 0;
		
	protected:
		
		int32_t Random() { return rand(); }
		int32_t Random(int32_t min, int32_t max) { return (rand() % (max - min + 1)) + min; }
		float RandomFloat(){ return (float)rand() / (float)RAND_MAX; }
		
		// Простая 2D версия шума Перлина (упрощенная для МК)
		uint8_t PerlinNoiseInt(int32_t x, int32_t y)
		{
			int32_t n = x + y * 57;
			n = (n << 13) ^ n;
			return (uint8_t)(0 + (((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) >> 23));
		}
		
		// Простая 2D версия шума Перлина (упрощенная для МК)
		float PerlinNoiseFloat(float x, float y)
		{
			uint32_t n = (int)x + ((int)y * 57);
			n = (n << 13) ^ n;
			return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
		}
		
		WS2812Manager *_manager = nullptr;
		WS2812ManagerInterface::frame_buffer_t *_frame_buffer = nullptr;
		
};
