#pragma once
#include <inttypes.h>
#include "FrameBuffer.h"
#include "WS2812EffectInterface.h"

class WS2812Manager
{
	public:
		
		WS2812Manager(FrameBuffer &frame_buffer) : frame_buffer(&frame_buffer)
		{
			return;
		}
		
		void SelectEffect(WS2812EffectInterface &effect, uint32_t frame_rate = 100)
		{
			_effect = &effect;
			_frame_rate = frame_rate;
			effect.PrepareInit(*frame_buffer);
			effect.Init();
		
			return;
		}
		
		void Tick(uint32_t time)
		{
			if(_effect == nullptr) return;

			if(time - last_tick >= 5)
			{
				last_tick = time;
				
				_effect->Tick(time);
			}
			
			if(frame_buffer->is_sending == false && time - last_render >= _frame_rate)
			{
				last_render = time;
				
				_effect->Render(time);
			}
			
			return;
		}
		
		FrameBuffer *frame_buffer;
		
	private:
		
		WS2812EffectInterface *_effect = nullptr;
		uint32_t _frame_rate;
		uint32_t last_tick = 0;
		uint32_t last_render = 0;
		
};
