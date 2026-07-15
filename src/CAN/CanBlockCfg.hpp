#pragma once
#include <inttypes.h>
#include <CanObjectBase.h>

class CanBlockCfg : public CANObjectBase
{
	static constexpr uint8_t fId = 0x37;
	static constexpr uint8_t header_size = 3;
	struct __attribute__((packed)) packet_t { uint8_t fId; uint8_t cmd; uint8_t id; uint8_t data[5]; };
	
	using save_reset_func_t = void (*)();
	
	enum cmd_t : uint8_t { CMD_NONE, CMD_GET, CMD_SET, CMD_SAVE_RESET, CMD_UNKNOWN_ID = 0xE0 };
	
	public:
		struct config_item_t
		{
			uint8_t id;
			uint8_t size;
			void *value_ptr;
		};
		
		CanBlockCfg(can_object_id_t id, save_reset_func_t save_reset, const config_item_t *items, const uint8_t count) : CANObjectBase(id), _save_reset_func(save_reset), _items(items), _items_count(count)
		{
			return;
		};
		
	protected:
		virtual void OnTick(uint32_t time) noexcept override
		{
			return;
		}
		
		virtual void OnProcessFrame(can_frame_t &can_frame) noexcept override
		{
			packet_t *rx_obj = (packet_t *)can_frame.raw_data;
			
			if(rx_obj->fId != fId)
				return;
				
			switch(rx_obj->cmd)
			{
				case CMD_GET:
				case CMD_SET:
				{
					packet_t answer = *rx_obj;

					const config_item_t *item = _Search(rx_obj->id);
					if(item != nullptr)
					{
						if(rx_obj->cmd == CMD_SET)
							_Set(item, rx_obj->data);
						_Get(item, answer.data);
					}
					else
					{
						answer.cmd = CMD_UNKNOWN_ID;
					}
					this->SendFrame((uint8_t *)&answer, header_size + item->size);
					
					break;
				}
				case CMD_SAVE_RESET:
				{
					packet_t answer = *rx_obj;
					this->SendFrame((uint8_t *)&answer, header_size);
					
					_save_reset_func();

					break;
				}
			}
			
			return;
		}
		
	private:
		void _Set(const config_item_t *item, const uint8_t *data)
		{
			memcpy(item->value_ptr, data, item->size);
			return;
		}
		
		void _Get(const config_item_t *item, uint8_t *data)
		{
			memcpy(data, item->value_ptr, item->size);
			return;
		}
		
		inline const config_item_t *_Search(uint8_t id)
		{
			for(uint8_t i = 0; i < _items_count; ++i)
			{
				if(_items[i].id != id) continue;

				return &_items[i];
			}
			
			return nullptr;
		}
		
		save_reset_func_t _save_reset_func;
		const config_item_t *_items;
		const uint8_t _items_count;
};
