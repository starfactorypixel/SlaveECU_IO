#pragma once
#include <PowerOutV2.h>
#include <CUtils.h>

extern IBlockInfoSender &BlockInfoSender;

namespace Outputs
{
	void OnControl(uint8_t port, uint8_t id, uint8_t state);
	uint16_t OnCurrentGet(uint8_t port, uint8_t id);
	void OnCurrentLimit(uint8_t port, uint16_t current);
	
	PowerOutV2<8> ports(HAL_GetTick, OnControl, OnCurrentGet);
	INACurrentCalc ina_calc(12, 3300, 2, 100);

	enum port_t : uint8_t
	{
		PORT_NONE, 
		PORT_1, PORT_2, PORT_3, PORT_4, 
		PORT_5, PORT_6, PORT_7, PORT_Hi
	};






	void OnControl(uint8_t port, uint8_t id, uint8_t state)
	{
		bool new_state = (state == PowerOutBase::STATE_ON) ? false : true;
		SPI::hc595.SetState(0, id, new_state);
	}
	
	uint16_t OnCurrentGet(uint8_t port, uint8_t id)
	{
		uint16_t adc = Analog::GetRegularValue(id);
		if(port == PORT_Hi) adc *= 3;			// Добавить в INACurrentCalc чтобы принимать шунт в микроомах
		return ina_calc.Get_mA(adc);
	}
	
	void OnCurrentLimit(uint8_t port, uint16_t current)
	{
		CANLib::SoftEventOutputs(CANLib::EVENT_CURR_LIMIT, port, current);
		BlockInfoSender.SendErrorMsg(port, 10, current);
	}


	

	
	
	inline void Setup()
	{
		ports.SetPort(PORT_1, 0, Analog::PORT_REG1, 1000);
		ports.SetPort(PORT_2, 1, Analog::PORT_REG2, 1000);
		ports.SetPort(PORT_3, 2, Analog::PORT_REG3, 1000);
		ports.SetPort(PORT_4, 3, Analog::PORT_REG4, 1000);
		ports.SetPort(PORT_5, 4, Analog::PORT_REG5, 1000);
		ports.SetPort(PORT_6, 5, Analog::PORT_REG6, 1000);
		ports.SetPort(PORT_7, 6, Analog::PORT_REG7, 1000);
		ports.SetPort(PORT_Hi, 7, Analog::PORT_REG8, 1000);
		
		ports.Init();

		//ports.CtrlOn(4);
		//ports.CtrlOn(6);
		//ports.CtrlOff(1);
		ports.SetCallbackCurrentLimit(OnCurrentLimit);
		//ports.Current(1);

		//ports.CtrlOn(6, 250, 500);
		//ports.CtrlOn(5, 1000, 100);
		
		return;
	}


	uint8_t test_iter = 1;
	
	inline void Loop(uint32_t &current_time)
	{
		ports.Processing(current_time);
		
		static uint32_t last_time = 0;
		if(current_time - last_time > 250)
		{
			last_time = current_time;

/*
			outObj.SetOff(test_iter++);
			if(test_iter == 9) test_iter = 1;
			outObj.SetOn(test_iter);
*/			
			for(uint8_t i = 1; i <= ports.GetPortCount(); ++i)
			{
				//Logger.PrintTopic("POUT").Printf("Port: %d, current: %5d;", i, outObj.GetCurrent(i)).PrintNewLine();
			}
		}
		
		current_time = HAL_GetTick();
		return;
	}
}
