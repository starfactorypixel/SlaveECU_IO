#pragma once
#include <DrakePinD.hpp>
#include <CANLibrary.h>
#include <CanObj/CanBlockInfo.hpp>
#include <CanObj/CanBlockCfg.hpp>
#include <CanObj/CanOutPort.hpp>
#include <CanObj/CanInPort.hpp>
#include <CanObj/CanStreamObj.hpp>
#include <CanObj/CanWS2812.hpp>

extern CAN_HandleTypeDef hcan;
extern bool HAL_CAN_Send(uint16_t id, uint8_t *data_raw, uint8_t length_raw);

namespace CANLib
{
	static constexpr uint8_t CFG_CANObjectsCount = 22;
	static constexpr uint8_t CFG_CANFrameBufferSize = 16;
	static constexpr uint16_t CAN_BASE_ID = 0x0160;
	
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

	
	
	static const CanBlockCfg::config_item_t block_cfg_table[] =
	{
		{1, sizeof(uint16_t), &Config::obj.body.in1.interval_ms},
		{2, sizeof(uint16_t), &Config::obj.body.in2.interval_ms},
		{3, sizeof(uint16_t), &Config::obj.body.in3.interval_ms},
		{4, sizeof(uint16_t), &Config::obj.body.in4.interval_ms},
		{5, sizeof(uint16_t), &Config::obj.body.in5.interval_ms},
		{6, sizeof(uint16_t), &Config::obj.body.in6.interval_ms},
	};
	static constexpr uint8_t block_cfg_table_count = sizeofarray(block_cfg_table);
	
	void OnCfgSaveReset()
	{
		return;
	}


	CANManager<22> can_manager(&HAL_CAN_Send, &HAL_GetTick, &OnInterruptCtrl);

	CanBlockInfo obj_block_info(CAN_BASE_ID+0, OnStaticInfoReq, OnDynamicInfoReq);
	CanBlockCfg obj_block_cfg(CAN_BASE_ID+1, &OnCfgSaveReset, block_cfg_table, block_cfg_table_count);
	CanOutPort obj_out_1(CAN_BASE_ID+4, Outputs::PORT_1);
	CanOutPort obj_out_2(CAN_BASE_ID+5, Outputs::PORT_2);
	CanOutPort obj_out_3(CAN_BASE_ID+6, Outputs::PORT_3);
	CanOutPort obj_out_4(CAN_BASE_ID+7, Outputs::PORT_4);
	CanOutPort obj_out_5(CAN_BASE_ID+8, Outputs::PORT_5);
	CanOutPort obj_out_6(CAN_BASE_ID+9, Outputs::PORT_6);
	CanOutPort obj_out_7(CAN_BASE_ID+10, Outputs::PORT_7);
	CanOutPort obj_out_8(CAN_BASE_ID+11, Outputs::PORT_Hi);
	CanInPort obj_in_1(CAN_BASE_ID+12, Analog::PORT_IN1, 100);
	CanInPort obj_in_2(CAN_BASE_ID+13, Analog::PORT_IN2, 100);
	CanInPort obj_in_3(CAN_BASE_ID+14, Analog::PORT_IN3, 100);
	CanInPort obj_in_4(CAN_BASE_ID+15, Analog::PORT_IN4, 100);
	CanInPort obj_in_5(CAN_BASE_ID+16, Analog::PORT_IN5, 100);
	CanInPort obj_in_6(CAN_BASE_ID+17, Analog::PORT_IN6, 100);
	CanInPort obj_in_7(CAN_BASE_ID+18, Analog::PORT_IN7, 100);
	CanInPort obj_in_8(CAN_BASE_ID+19, Analog::PORT_IN8, 100);
	CanStreamObj<int8_t> obj_temperature(CAN_BASE_ID+20, OneWire::temp, sizeofarray(OneWire::temp));
	CanWS2812 obj_led_ctrl(CAN_BASE_ID+21);
	
	
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
		can_manager.AddObject(obj_block_cfg);
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
