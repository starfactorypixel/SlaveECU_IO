#pragma once
#include <inttypes.h>
#include <CanObjectBase.h>

class CanInPort : public CANObjectBase
{
	struct __attribute__((packed)) request_t { uint8_t fId; };
	struct __attribute__((packed)) timer_t { uint8_t fId; uint16_t adc; uint16_t mv; };
	struct __attribute__((packed)) event_ok_t { uint8_t fId = CAN_FUNC_EVENT_OK; uint16_t adc; uint16_t mv; };
	struct __attribute__((packed)) event_er_t { uint8_t fId = CAN_FUNC_EVENT_ERROR; uint8_t val1; uint8_t val2; };
	
	public:
		CanInPort(can_object_id_t id, Analog::port_mux_t port, uint16_t timer) : CANObjectBase(id), _port(port)
		{
			this->SetTimerPeriod(timer);

			return;
		};
		
	protected:
		void handlerRequestFunction(request_t *obj)
		{
			uint16_t adc = Analog::GetMuxValue(_port);
			
			event_ok_t answer = {};
			answer.adc = adc;
			answer.mv = Analog::VoltCalcIn.GetmV(adc);
			this->SendFrame((uint8_t *)&answer, sizeof(answer));

			return;
		}
		
		virtual void OnTick(uint32_t time) noexcept override
		{
			return;
		}

		virtual void OnProcessFrame(can_frame_t &can_frame) noexcept override
		{
			uint8_t fId = can_frame.raw_data[0];
			switch(fId)
			{
				case CAN_FUNC_REQUEST_IN:
				{
					request_t *obj = (request_t *)can_frame.raw_data;
					handlerRequestFunction(obj);
					break;
				}
			}

			return;
		}

		virtual void OnTimer() noexcept override
		{
			uint16_t adc = Analog::GetMuxValue(_port);
			
			timer_t answer = {};
			answer.fId = CAN_FUNC_TIMER_NORMAL;
			answer.adc = adc;
			answer.mv = Analog::VoltCalcIn.GetmV(adc);
			this->SendFrame((uint8_t *)&answer, sizeof(answer));
		}
		
	private:
		Analog::port_mux_t _port;
};
