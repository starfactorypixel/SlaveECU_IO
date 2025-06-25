#pragma once
#include <inttypes.h>
#include "FrameEffectInterface.h"

class FrameEffectPrimitiveLights : public FrameEffectInterface
{
	using draw_func = void (*)(const uint32_t time);

	public:

		enum type_t : uint8_t
		{
			SIGNAL_NONE = 0x00,				// Выключить всё
			SIGNAL_SIDE = (1 << 0),			// Габаритный огонь
			SIGNAL_REVERSE = (1 << 1),		// Задний ход
			SIGNAL_LEFT = (1 << 2),			// Левый поворотник
			SIGNAL_RIGHT = (1 << 3),		// Правый поворотник
			SIGNAL_STOP = (1 << 4),			// Стоп сигнал
			SIGNAL_EMERGENCY = (1 << 5),	// Аварийный сигнал
			SIGNAL_ALL = 0xFF				// Включить все сигналы
		};
		
		
		FrameEffectPrimitiveLights() : _data{0, SIGNAL_NONE}
		{
			return;
		};
		
		virtual void Init() override
		{
			return;
		}
		
		virtual void Tick(uint32_t time) override
		{
			_data.now_time = time;

			return;
		}
		
		virtual bool FramePrepare(uint32_t time) override
		{
			frame_buffer->Clear();
			
			uint32_t lasttime = HAL_GetTick();
			Leds::obj.SetOn(Leds::LED_WHITE);
			
			_RenderSignals();
			
			Leds::obj.SetOff(Leds::LED_WHITE);
			DEBUG_LOG_TOPIC("PXLDraw", "time: %d\n", (HAL_GetTick() - lasttime));
			
			return true;
		}
		
		
		void TurnOn(type_t type)
		{
			_data.mode |= type;

			return;
		}
		
		void TurnOff(type_t type)
		{
			_data.mode &= ~type;

			return;
		}
		
		void Control(type_t type, uint8_t mode)
		{
			return (mode == 0) ? TurnOff(type) : TurnOn(type);
		}
		
	private:
		
		void _RenderSignals()
		{
			for(uint8_t i = 0; i < 8; ++i)
			{
				if( (_data.mode >> i) & 0x01 )
				{
					switch((1 << i))
					{
						case SIGNAL_SIDE:
							_DrawSideLights();
							break;
						case SIGNAL_REVERSE:
							_DrawReverseLights();
							break;
						case SIGNAL_LEFT:
							_DrawLeftBlink();
							break;
						case SIGNAL_RIGHT:
							_DrawRightBlink();
							break;
						case SIGNAL_STOP:
							_DrawStopLights();
							break;
						case SIGNAL_EMERGENCY:
							_DrawEmergencyBlink();
							break;
						default:
							break;
					}
				}
			}

			return;
		}

		static constexpr uint16_t max_x = frame_width - 1;
		static constexpr uint16_t max_y = frame_height - 1;
		
		void _DrawSideLights()
		{
			static color_t color = {0x80, 0x00, 0x00};

			frame_buffer->SetPixel(0, 0, color);
			frame_buffer->SetPixel(0, max_y, color);
			_DrawRectangle( {1, 0}, {2, max_y}, color, false );

			frame_buffer->SetPixel(max_x, 0, color);
			frame_buffer->SetPixel(max_x, max_y, color);
			_DrawRectangle( {(uint16_t)(max_x - 2), 0}, {(uint16_t)(max_x - 1), max_y}, color, false );
			
			return;
		}
		
		void _DrawReverseLights()
		{
			static color_t color = {0xFF, 0xFF, 0xFF};

			_DrawRectangle( {1, 5}, {15, (uint16_t)(max_y - 5)}, color, true );
			_DrawRectangle( {(uint16_t)(max_x - 15), 5}, {(uint16_t)(max_x - 1), (uint16_t)(max_y - 5)}, color, true );

			return;
		}

		void _DrawLeftBlink()
		{
			static color_t color = {0xFF, 0xA5, 0x00};

			if((_data.now_time / 500) % 2 == 0) return;

			_DrawRectangle( {1, 5}, {15, (uint16_t)(max_y - 5)}, color, true );
			
			return;
		}

		void _DrawRightBlink()
		{
			static color_t color = {0xFF, 0xA5, 0x00};

			if((_data.now_time / 500) % 2 == 0) return;

			_DrawRectangle( {(uint16_t)(max_x - 15), 5}, {(uint16_t)(max_x - 1), (uint16_t)(max_y - 5)}, color, true );

			return;
		}

		void _DrawStopLights()
		{
			static color_t color = {0xFF, 0x00, 0x00};

			_DrawRectangle( {1, 3}, {15, (uint16_t)(max_y - 3)}, color, true );
			_DrawRectangle( {(uint16_t)(max_x - 15), 3}, {(uint16_t)(max_x - 1), (uint16_t)(max_y - 3)}, color, true );
			
			return;
		}

		void _DrawEmergencyBlink()
		{
			_DrawLeftBlink();
			_DrawRightBlink();
			
			return;
		}

		
		typedef struct
		{
			uint16_t x;
			uint16_t y;
		} Point;
		
		void _DrawRectangle(Point p1, Point p2, color_t color, bool fill)
		{
			uint16_t x0 = (p1.x < p2.x) ? p1.x : p2.x;
			uint16_t x1 = (p1.x > p2.x) ? p1.x : p2.x;
			uint16_t y0 = (p1.y < p2.y) ? p1.y : p2.y;
			uint16_t y1 = (p1.y > p2.y) ? p1.y : p2.y;
			
			if(fill)
			{
				for(uint16_t y = y0; y <= y1; y++)
				{
					for(uint16_t x = x0; x <= x1; x++)
					{
						frame_buffer->SetPixel(x, y, color);
					}
				}
			}
			else
			{
				for(uint16_t x = x0; x <= x1; x++)
				{
					frame_buffer->SetPixel(x, y0, color);
					frame_buffer->SetPixel(x, y1, color);
				}
				for(uint16_t y = y0; y <= y1; y++)
				{
					frame_buffer->SetPixel(x0, y, color);
					frame_buffer->SetPixel(x1, y, color);
				}
			}
			
			return;
		}
		
		struct
		{
			uint32_t now_time;
			uint8_t mode;
		} _data;
		
};
