#pragma once
#include <inttypes.h>
#include "WS2812EffectInterface.h"
#include "PrimitiveDraw.h"

class WS2812EffectPrimitiveLights : public WS2812EffectInterface
{
	using draw_func = void (*)(const uint32_t time);
	
	public:

		enum type_t : uint8_t
		{
			SIGNAL_NONE = 0,				// Выключить всё
			SIGNAL_SIDE = (1 << 0),			// Габаритный огонь
			SIGNAL_REVERSE = (1 << 1),		// Задний ход
			SIGNAL_LEFT = (1 << 2),			// Левый поворотник
			SIGNAL_RIGHT = (1 << 3),		// Правый поворотник
			SIGNAL_STOP = (1 << 4),			// Стоп сигнал
			SIGNAL_EMERGENCY = (1 << 5),	// Аварийный сигнал
		};

		struct data_t
		{
			uint16_t delay;
			draw_func func;
		};

		data_t _dataa[1] = 
		{
			//{0, _Draw}
		};


		
		WS2812EffectPrimitiveLights() : _pd(frame_width, frame_height)
		{
			return;
		};
		
		virtual void Init() override
		{
			_pd.SetFrameBuffer(*frame_buffer);
			
			return;
		}
		
		virtual void Tick(uint32_t time) override
		{
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
		
		void DrawStop()
		{
			PrimitiveDraw::line_params_t params = {};
			params.x0 = 3;
			params.y0 = 4;
			params.x1 = 110;
			params.y1 = 15;
			params.color = {0x05, 0x05, 0x00};
			params.thickness = 3;
			_pd.draw_line(params);
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


typedef struct {
    int16_t x;
    int16_t y;
} Point;
void drawRectangle(Point p1, Point p2, color_t color, bool fill) {
    int16_t x0 = (p1.x < p2.x) ? p1.x : p2.x;
    int16_t x1 = (p1.x > p2.x) ? p1.x : p2.x;
    int16_t y0 = (p1.y < p2.y) ? p1.y : p2.y;
    int16_t y1 = (p1.y > p2.y) ? p1.y : p2.y;

    if (fill) {
        for (int16_t y = y0; y <= y1; y++) {
            for (int16_t x = x0; x <= x1; x++) {
				frame_buffer->SetPixel(x, y, color);
            }
        }
    } else {
        for (int16_t x = x0; x <= x1; x++) {
            frame_buffer->SetPixel(x, y0, color);
            frame_buffer->SetPixel(x, y1, color);
        }
        for (int16_t y = y0; y <= y1; y++) {
            frame_buffer->SetPixel(x0, y, color);
            frame_buffer->SetPixel(x1, y, color);
        }
    }
}




		void _DrawSideLights()
		{
			return;
		}
		
		void _DrawReverseLights()
		{
			return;
		}

		void _DrawLeftBlink()
		{
			return;
		}

		void _DrawRightBlink()
		{
			return;
		}

		void _DrawStopLights()
		{
			return;
		}

		void _DrawEmergencyBlink()
		{
			return;
		}
		
		PrimitiveDraw _pd;
		struct
		{
			uint8_t mode;
			uint8_t state;
		} _data;
		
};
