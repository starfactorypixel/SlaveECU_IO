#pragma once
#include <inttypes.h>

class CanOutPort : public CANObjectBase
{
	struct __attribute__((packed)) set_t { uint8_t fId; uint8_t val; };
	struct __attribute__((packed)) toggle_t { uint8_t fId; };
	struct __attribute__((packed)) action_t { uint8_t fId; };
	struct __attribute__((packed)) request_t { uint8_t fId; };
	struct __attribute__((packed)) event_ok_t { uint8_t fId = CAN_FUNC_EVENT_OK; uint8_t val; };
	
	public:
		CanOutPort(can_object_id_t id, Outputs::port_t port) : CANObjectBase(id), _port(port)
		{

		};

		void SoftEvent(CANLib::backevent_type_t type, uint16_t val)
		{
			DEBUG_LOG_SIMPLE("type: %d, val: %d, port: %d\n", type, val, _port);
		}

		
	protected:
		void handlerSetFunction(set_t *obj)
		{
			PowerOutBase::state_t state = (obj->val > 0) ? PowerOutBase::STATE_ON : PowerOutBase::STATE_OFF;
			Outputs::ports.CtrlWrite(_port, state);
			_GetAndSendPortState();
			
			return;
		}
		
		void handlerToggleFunction(toggle_t *obj)
		{
			Outputs::ports.CtrlToggle(_port);
			_GetAndSendPortState();
			
			return;
		}
		
		void handlerActionFunction(action_t *obj)
		{
			Outputs::ports.CtrlOn(_port, 500);
			_GetAndSendPortState();
			
			return;
		}
		
		void handlerRequestFunction(request_t *obj)
		{
			_GetAndSendPortState();
			
			return;
		}
		
		virtual void OnProcessFrame(can_frame_t &can_frame) noexcept override
		{
			uint8_t fId = can_frame.raw_data[0];
			switch(fId)
			{
				case CAN_FUNC_SET_IN:
				{
					set_t *obj = (set_t *)can_frame.raw_data;
					handlerSetFunction(obj);
					break;
				}
				
				case CAN_FUNC_TOGGLE_IN:
				{
					toggle_t *obj = (toggle_t *)can_frame.raw_data;
					handlerToggleFunction(obj);
					break;
				}

				case CAN_FUNC_ACTION_IN:
				{
					action_t *obj = (action_t *)can_frame.raw_data;
					handlerActionFunction(obj);
					break;
				}

				case CAN_FUNC_REQUEST_IN:
				{
					request_t *obj = (request_t *)can_frame.raw_data;
					handlerRequestFunction(obj);
					break;
				}
			}

			return;
		}
		
	private:
		void _GetAndSendPortState()
		{
			event_ok_t answer = {};
			answer.val = ((Outputs::ports.GetState(_port) == PowerOutBase::STATE_ON) ? 0xFF : 0x00);
			this->sendFrame((uint8_t *)&answer, sizeof(answer));
		}
		
		Outputs::port_t _port;
};
