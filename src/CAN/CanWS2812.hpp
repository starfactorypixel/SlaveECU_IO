#pragma once
#include <inttypes.h>

class CanWS2812 : public CANObjectBase
{
	struct __attribute__((packed)) request_t { uint8_t fId; };
	
	public:
		CanWS2812(can_object_id_t id) : CANObjectBase(id)
		{

		};
		
	protected:
		virtual void OnTick(uint32_t time) noexcept override
		{
			return;
		}
		
		virtual void OnProcessFrame(can_frame_t &can_frame) noexcept override
		{
			return;
		}
		virtual void OnTimer() noexcept override											
		{
			return;
		}
		
	private:
};
