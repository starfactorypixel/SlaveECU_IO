#pragma once
#include <inttypes.h>
#include "FrameEffectInterface.h"

class FrameEffectSphere : public FrameEffectInterface
{
	static constexpr uint8_t _count = 6;

	
	public:
		
		virtual void Init() override
		{
			for(uint8_t i = 0; i < _count; ++i)
			{
				balls[i].x = Random(0, 127);
				balls[i].y = Random(0, 15);
				
				// Исключение строго вертикального и горизонтального движения
				do
				{
					balls[i].vx = (Random(-2, 2));
					balls[i].vy = (Random(-2, 2));
				} while (balls[i].vx == 0 || balls[i].vy == 0);
				
				balls[i].radius = Random(1, 3);
				balls[i].r = Random(0, 16);
				balls[i].g = Random(0, 16);
				balls[i].b = Random(0, 16);
			}
			
			return;
		}
		
		virtual void Tick(uint32_t time) override
		{			
			return;
		}
		
		virtual bool FramePrepare(uint32_t time) override
		{
			updateEffect();
			
			return true;
		}

	private:
		
		void updateEffect()
		{
			Ball *ball = nullptr;
			for(uint8_t i = 0; i < _count; ++i)
			{
				ball = &balls[i];
				
				// Пропуск обновления позиции для неподвижных шаров
				if(ball->vx == 0 && ball->vy == 0)
					continue;
				
				ball->x += ball->vx;
				ball->y += ball->vy;
				
				if(ball->x - ball->radius < 0 || ball->x + ball->radius >= frame_width)
				{
					ball->vx *= -1;
					ball->x += ball->vx;
				}
				if(ball->y - ball->radius < 0 || ball->y + ball->radius >= frame_height)
				{
					ball->vy *= -1;
					ball->y += ball->vy;
				}
				
				if(ball->vx == 0)
					ball->vx = Random(-2, 2);
				
				if(ball->vy == 0)
					ball->vy = Random(-2, 2);
			}
			
			render();
		}
		
		void render()
		{
			//memset_dma32(frame_buffer->raw, 0x00000000, sizeof(frame_buffer->raw));
			frame_buffer->Clear();

			Ball *ball = nullptr;
			for(uint8_t i = 0; i < _count; ++i)
			{
				ball = &balls[i];
				
				for(int dx = -ball->radius; dx <= ball->radius; dx++)
				{
					for(int dy = -ball->radius; dy <= ball->radius; dy++)
					{
						int px = (int)(ball->x) + dx;
						int py = (int)(ball->y) + dy;
						if(px >= 0 && px < frame_width && py >= 0 && py < frame_height && dx * dx + dy * dy <= ball->radius * ball->radius)
						{
							//uint16_t index = frame_buffer->Convertor( (py * width + px), width, height);
							//frame_buffer->pixel[index] = {ball->g, ball->r, ball->b};
							color_t pixel = {ball->r, ball->g, ball->b};
							frame_buffer->SetPixel((py * frame_width + px), pixel);
						}
					}
				}
			}
		}
		
		struct Ball
		{
			uint8_t x, y;
			uint8_t vx, vy;
			uint8_t r, g, b;
			uint8_t radius;
		} balls[_count];
		
};
