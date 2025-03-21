#pragma once
#include <inttypes.h>
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

class WS2812EffectTemplate : public WS2812EffectInterface
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
		
		virtual void Render(uint32_t time) override
		{
			//updateEffect();
			_frame_buffer->is_rendered = true;
			
			return;
		}
		
	private:
		
};
