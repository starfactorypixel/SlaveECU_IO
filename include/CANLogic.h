#pragma once
#include <DrakePinD.hpp>
#include <CANLibrary.h>
#include <CAN/CanBlockInfo.hpp>
#include <CAN/CanOutPort.hpp>
#include <CAN/CanInPort.hpp>
#include <CAN/CanStreamObj.hpp>
#include <CAN/CanWS2812.hpp>

extern CAN_HandleTypeDef hcan;
extern bool HAL_CAN_Send(uint16_t id, uint8_t *data_raw, uint8_t length_raw);

namespace CANLib
{
	static constexpr uint8_t CFG_CANObjectsCount = 22;
	static constexpr uint8_t CFG_CANFrameBufferSize = 16;
	static constexpr uint16_t CFG_CANFirstId = 0x0160;
	
	DrakePinD can_rs({GPIOA, GPIO_PIN_15}, DrakePin::OutputOpenDrain, DrakePin::High);








		



































	void OnInterruptCtrl(bool enable)
	{
		if(enable)
			__HAL_CAN_ENABLE_IT(&hcan, CAN_IT_TX_MAILBOX_EMPTY);
		else
			__HAL_CAN_DISABLE_IT(&hcan, CAN_IT_TX_MAILBOX_EMPTY);
	}

	void OnStaticInfoReq(CanBlockInfo::block_info_static_t &data)
	{
		data.hw_ver = About::board_ver;
		data.hw_type = About::board_type;
		data.can_ver = About::can_ver;
		data.sw_ver = About::soft_ver;
		memcpy(data.sn, About::sn, sizeof(data.sn));
		memcpy(data.features, (const uint8_t *)&About::features, sizeof(data.features));

		return;
	}

	void OnDynamicInfoReq(CanBlockInfo::block_info_dynamic_t &data)
	{
		data.uptime = HAL_GetTick();
		data.voltage = Analog::VoltCalc.GetmV( Analog::GetMuxValue(Analog::PORT_VIN) );
		data.current = Outputs::ports.GetCurrentAll();
		data.temperature = INT8_MIN;

		return;
	}


	CANManager<22> can_manager(&HAL_CAN_Send, &HAL_GetTick, &OnInterruptCtrl);

	CanBlockInfo obj_block_info(0x0160, OnStaticInfoReq, OnDynamicInfoReq);
	
	CanOutPort obj_out_1(0x0164, Outputs::PORT_1);
	CanOutPort obj_out_2(0x0165, Outputs::PORT_2);
	CanOutPort obj_out_3(0x0166, Outputs::PORT_3);
	CanOutPort obj_out_4(0x0167, Outputs::PORT_4);
	CanOutPort obj_out_5(0x0168, Outputs::PORT_5);
	CanOutPort obj_out_6(0x0169, Outputs::PORT_6);
	CanOutPort obj_out_7(0x016A, Outputs::PORT_7);
	CanOutPort obj_out_8(0x016B, Outputs::PORT_Hi);
	
	CanInPort obj_in_1(0x016C, Analog::PORT_IN1, 100);
	CanInPort obj_in_2(0x016D, Analog::PORT_IN2, 100);
	CanInPort obj_in_3(0x016E, Analog::PORT_IN3, 100);
	CanInPort obj_in_4(0x016F, Analog::PORT_IN4, 100);
	CanInPort obj_in_5(0x0170, Analog::PORT_IN5, 100);
	CanInPort obj_in_6(0x0171, Analog::PORT_IN6, 100);
	CanInPort obj_in_7(0x0172, Analog::PORT_IN7, 100);
	CanInPort obj_in_8(0x0173, Analog::PORT_IN8, 100);
	
	CanStreamObj<int8_t> obj_temperature(0x0174, OneWire::temp, sizeofarray(OneWire::temp));
	
	CanWS2812 obj_led_ctrl(0x0175);
	
	
	void SoftEventOutputs(backevent_type_t type, uint8_t port, uint16_t val)
	{
		switch(port)
		{
			case Outputs::PORT_1:  obj_out_1.SoftEvent(type, val); break;
			case Outputs::PORT_2:  obj_out_2.SoftEvent(type, val); break;
			case Outputs::PORT_3:  obj_out_3.SoftEvent(type, val); break;
			case Outputs::PORT_4:  obj_out_4.SoftEvent(type, val); break;
			case Outputs::PORT_5:  obj_out_5.SoftEvent(type, val); break;
			case Outputs::PORT_6:  obj_out_6.SoftEvent(type, val); break;
			case Outputs::PORT_7:  obj_out_7.SoftEvent(type, val); break;
			case Outputs::PORT_Hi: obj_out_8.SoftEvent(type, val); break;
		}
	}

	
	
	void CAN_Enable()
	{
		HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_ERROR | CAN_IT_BUSOFF | CAN_IT_LAST_ERROR_CODE | CAN_IT_TX_MAILBOX_EMPTY);
		HAL_CAN_Start(&hcan);
		
		can_rs.Off();
		
		return;
	}
	
	void CAN_Disable()
	{
		HAL_CAN_DeactivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_ERROR | CAN_IT_BUSOFF | CAN_IT_LAST_ERROR_CODE | CAN_IT_TX_MAILBOX_EMPTY);
		HAL_CAN_Stop(&hcan);
		
		can_rs.On();
		
		return;
	}
	
	inline void Setup()
	{
		can_rs.Init();
		
		//obj_block_info.SetSN(About::sn);
		// Необходимо чтобы ДО этого вызова структура возможностей уже была заполенна
		//obj_block_info.SetFeatures(About::features);
		
		can_manager.AddObject(obj_block_info);
		can_manager.AddObject(obj_out_1);
		can_manager.AddObject(obj_out_2);
		can_manager.AddObject(obj_out_3);
		can_manager.AddObject(obj_out_4);
		can_manager.AddObject(obj_out_5);
		can_manager.AddObject(obj_out_6);
		can_manager.AddObject(obj_out_7);
		can_manager.AddObject(obj_out_8);
		can_manager.AddObject(obj_in_1);
		can_manager.AddObject(obj_in_2);
		can_manager.AddObject(obj_in_3);
		can_manager.AddObject(obj_in_4);
		can_manager.AddObject(obj_in_5);
		can_manager.AddObject(obj_in_6);
		can_manager.AddObject(obj_in_7);
		can_manager.AddObject(obj_in_8);
		can_manager.AddObject(obj_temperature);
		can_manager.AddObject(obj_led_ctrl);
		
		CAN_Enable();
		
		return;
	}

	inline void Loop(uint32_t &current_time)
	{
		can_manager.Processing(/*current_time*/);

		// При выходе обновляем время
		current_time = HAL_GetTick();
		
		return;
	}
}

IBlockInfoSender &BlockInfoSender = CANLib::obj_block_info;
