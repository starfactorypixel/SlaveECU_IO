#pragma once
#include <inttypes.h>

class CanBlockInfo : public CANObjectBase, public IBlockInfoSender
{
	struct __attribute__((packed)) request_t { uint8_t fId; };
	struct __attribute__((packed)) block_wakeup_t { uint8_t fId; uint8_t reason; };
	struct __attribute__((packed)) block_info_t
	{
		//uint8_t fId;				// Т.к. пакет отправляется чанками, то fId не указывается, - только данные
		uint8_t hw_ver:3;			// Версия платы, 3 бита
		uint8_t hw_type:5;			// Тип платы, 5 бит
		uint8_t can_ver:2;			// Версия протокола CAN, 2 бита
		uint8_t sw_ver:6;			// Версия программы, 6 бит
		uint8_t error_flags;		// Флаги налчичия ошибок блока
		uint32_t uptime;			// Uptime блока, мс.
		uint16_t voltage;			// Напряжение питание блока
		uint16_t current;			// Общий потребляемый ток блока
		int8_t temperature;			// Температура блока, если есть
		uint8_t sn[8];				// Серийный номер блока
		uint8_t features[7];		// Возможности блока
	};

	struct __attribute__((packed)) heartbeat_t
	{
		uint8_t fId = 0x6A;
		uint8_t counter;			// Счётчик
	};
	
	struct __attribute__((packed)) error_t
	{
		uint8_t fId;
		uint8_t group;				// Группа ошибки
		uint8_t code;				// Код ошибки
		uint16_t subcode;			// Доп. код ошибки
	};
	
	public:
		CanBlockInfo(can_object_id_t id) : CANObjectBase(id)
		{
			_block_info.hw_ver = About::board_ver;
			_block_info.hw_type = About::board_type;
			_block_info.can_ver = About::can_ver;
			_block_info.sw_ver = About::soft_ver;
			_block_info.temperature = INT8_MIN;
			//GetSerialNumber(_block_info.sn);
			
			return;
		};
		
		virtual void SetErrorFlag(error_flag_t flag, bool state) override
		{
			if(state)
				_block_info.error_flags |= flag;
			else
				_block_info.error_flags &= ~flag;
			
			return;
		}
		
		virtual bool GetErrorFlag(error_flag_t flag) override
		{
			return (_block_info.error_flags & flag) != 0;
		}
		
		virtual void SendWakeupMsg(uint8_t reason) override
		{
			_SendWakeup(reason);

			return;
		}
		
		virtual void SendErrorMsg(uint8_t group, uint8_t code, uint16_t subcode) override
		{
			_SendError(group, code, subcode);
			
			return;
		}
		
		template<typename T> 
		void SetSN(const T &data)
		{
			static_assert(sizeof(T) == sizeof(_block_info.sn), "SN has invalid size");
			
			memcpy(_block_info.sn, (const uint8_t *)&data, sizeof(_block_info.sn));
			
			return;
		}
		
		template<typename T> 
		void SetFeatures(const T &data)
		{
			static_assert(sizeof(T) == sizeof(_block_info.features), "Features has invalid size");
			
			memcpy(_block_info.features, (const uint8_t *)&data, sizeof(_block_info.features));
			
			return;
		}
		
	protected:
		void handlerRequestFunction(request_t *obj)
		{
			_SendBlockInfo();
			
			return;
		}
		
		virtual void OnTick(uint32_t time) noexcept override
		{
			if(time - _last_heartbeat >= 5000)
			{
				_last_heartbeat = time;

				_SendHeartbeat();
			}
			
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
			return;
		}
		
	private:
		void _PrepareBlockInfo()
		{
			_block_info.uptime = this->GetParent()->GetTime();
			_block_info.voltage = Analog::VoltCalc.GetmV( Analog::mux.Get(Analog::PORT_VIN) );
			_block_info.current = Outputs::ports.GetCurrentAll();

			return;
		}
		
		void _SendWakeup(uint8_t reason)
		{
			block_wakeup_t wakeup = {};
			wakeup.reason = reason;
			this->sendFrame((uint8_t *)&wakeup, sizeof(wakeup));
			
			return;
		}
		
		void _SendBlockInfo()
		{
			const uint8_t header_size = 2;
			const uint8_t frame_size = 8;
			const uint8_t payload_size = frame_size - header_size;
			const uint8_t data_size = sizeof(_block_info);
			const uint8_t *src = (const uint8_t *)&_block_info;
			
			_PrepareBlockInfo();
			
			uint8_t offset = 0;
			uint8_t page = 0;
			while(offset < data_size)
			{
				uint8_t data[8] = {0x6B, ++page};
				uint8_t data_len = data_size - offset;
				if(data_len > payload_size)
					data_len = payload_size;
				memcpy(&data[2], src + offset, data_len);
				offset += payload_size;
				
				this->sendFrame(data, data_len + header_size);
			}
			
			return;
		}
		
		void _SendHeartbeat()
		{
			_heartbeat.counter += 1;
			this->sendFrame((uint8_t *)&_heartbeat, sizeof(_heartbeat));

			return;
		}
		
		void _SendError(uint8_t group, uint8_t code, uint16_t subcode)
		{
			error_t error = {};
			error.group = group;
			error.code = code;
			error.subcode = subcode;
			this->sendFrame((uint8_t *)&error, sizeof(error));
			
			return;
		}
		
		block_info_t _block_info = {};
		heartbeat_t _heartbeat = {};
		uint32_t _last_heartbeat = 0;
};
