#pragma once
#include <inttypes.h>
#include "FrameEffectInterface.h"

/*
	frame_buffer - Объект кадрового буфера с параметрами и флагами
	frame_width - Ширина кадра
	frame_height - Высота кадра
	Init() - Вызывается при инициализации эффекта
	Tick() - Вызывается не менее чем каждые 1 мс
	Render() - Вызывается каждые 100 мс
*/

class FrameEffectTemplate : public FrameEffectInterface
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
			//updateEffect();
			
			return true;
		}
		
	private:
		
};
