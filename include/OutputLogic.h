#pragma once
#include <PowerOutV2.h>
#include <CUtils.h>

namespace Outputs
{
	void OnControl(uint8_t id, uint8_t state);
	uint16_t OnCurrentGet(uint8_t id);
	void OnCurrentLimit(uint8_t port, uint16_t current);
	
	PowerOutV2<8> ports(HAL_GetTick, OnControl, OnCurrentGet);
	INACurrentCalc ina_calc(12, 3300, 2, 100);

	enum port_t : uint8_t
	{
		PORT_NONE,
		PORT_1,		// Выход 1
		PORT_2,		// Выход 2
		PORT_3,		// Выход 3
		PORT_4,		// Выход 4
		PORT_5,		// Выход 5
		PORT_6,		// Выход 6
		PORT_7,		// Выход 7
		PORT_Hi,	// Выход Hi
	};
	
	void OnControl(uint8_t id, uint8_t state)
	{
		bool new_state = (state == ports.STATE_ON) ? false : true;
		SPI::hc595.SetState(0, id, new_state);
	}
	
	uint16_t OnCurrentGet(uint8_t id)
	{
		uint16_t adc = Analog::adc_buf[id];
		return ina_calc.Get_mA(adc);
	}
	
	void OnCurrentLimit(uint8_t port, uint16_t current)
	{

	}


	can_result_t OnPowerOutputCtrl(uint8_t port, can_frame_t &can_frame, can_error_t &error)
	{
		can_frame.initialized = true;

		switch(can_frame.function_id)
		{
			case CAN_FUNC_SET_IN:
			{
				PowerOutBase::state_t state = (can_frame.data[0] > 0) ? PowerOutBase::STATE_ON : PowerOutBase::STATE_OFF;
				Outputs::ports.CtrlWrite(port, state);
				//can_frame.function_id = (result == true) ? CAN_FUNC_EVENT_OK : CAN_FUNC_EVENT_ERROR;
				can_frame.function_id = CAN_FUNC_EVENT_OK;
				can_frame.data[0] = can_frame.data[0];
				can_frame.raw_data_length = 2;
				
				break;
			}
			case CAN_FUNC_TOGGLE_IN:
			{
				Outputs::ports.CtrlToggle(port);
				can_frame.function_id = CAN_FUNC_EVENT_OK;
				uint8_t result = (Outputs::ports.GetState(port) == Outputs::ports.STATE_ON) ? 0xFF : 0x00;
				can_frame.data[0] = result;
				can_frame.raw_data_length = 2;
				
				break;
			}
			case CAN_FUNC_ACTION_IN:
			{
				Outputs::ports.CtrlOn(port, 250);
				//can_frame.function_id = (result == true) ? CAN_FUNC_EVENT_OK : CAN_FUNC_EVENT_ERROR;
				can_frame.function_id = CAN_FUNC_EVENT_OK;
				
				break;
			}
			case CAN_FUNC_REQUEST_IN:
			{
				uint8_t result = (Outputs::ports.GetState(port) == Outputs::ports.STATE_ON) ? 0xFF : 0x00;
				can_frame.function_id = CAN_FUNC_EVENT_OK;
				can_frame.data[0] = result;
				
				break;
			}
			default:
			{
				break;
			}
		}
		
		return CAN_RESULT_CAN_FRAME;
	}

	can_result_t OnPowerOutputCtrl_1(can_frame_t &can_frame, can_error_t &error)
	{
		return OnPowerOutputCtrl(1, can_frame, error);
	}

	can_result_t OnPowerOutputCtrl_2(can_frame_t &can_frame, can_error_t &error)
	{
		return OnPowerOutputCtrl(2, can_frame, error);
	}

	can_result_t OnPowerOutputCtrl_3(can_frame_t &can_frame, can_error_t &error)
	{
		return OnPowerOutputCtrl(3, can_frame, error);
	}

	can_result_t OnPowerOutputCtrl_4(can_frame_t &can_frame, can_error_t &error)
	{
		return OnPowerOutputCtrl(4, can_frame, error);
	}

	can_result_t OnPowerOutputCtrl_5(can_frame_t &can_frame, can_error_t &error)
	{
		return OnPowerOutputCtrl(5, can_frame, error);
	}

	can_result_t OnPowerOutputCtrl_6(can_frame_t &can_frame, can_error_t &error)
	{
		return OnPowerOutputCtrl(6, can_frame, error);
	}

	can_result_t OnPowerOutputCtrl_7(can_frame_t &can_frame, can_error_t &error)
	{
		return OnPowerOutputCtrl(7, can_frame, error);
	}

	can_result_t OnPowerOutputCtrl_8(can_frame_t &can_frame, can_error_t &error)
	{
		return OnPowerOutputCtrl(8, can_frame, error);
	}



	
	
	inline void Setup()
	{
		ports.SetPort(PORT_1, 0, 0, 5000);
		ports.SetPort(PORT_2, 1, 1, 5000);
		ports.SetPort(PORT_3, 2, 2, 5000);
		ports.SetPort(PORT_4, 3, 3, 5000);
		ports.SetPort(PORT_5, 4, 4, 5000);
		ports.SetPort(PORT_6, 5, 5, 5000);
		ports.SetPort(PORT_7, 6, 6, 5000);
		ports.SetPort(PORT_Hi, 7, 7, 15000);
		
		ports.Init();

		//ports.CtrlOn(4);
		//ports.CtrlOn(6);
		//ports.CtrlOff(1);
		ports.SetCallbackCurrentLimit(OnCurrentLimit);
		//ports.Current(1);

		//ports.CtrlOn(6, 250, 500);
		//ports.CtrlOn(5, 1000, 100);


		{
		CANLib::obj_out_1.RegisterFunctionSet(OnPowerOutputCtrl_1);
		CANLib::obj_out_1.RegisterFunctionToggle(OnPowerOutputCtrl_1);
		CANLib::obj_out_1.RegisterFunctionAction(OnPowerOutputCtrl_1);
		CANLib::obj_out_1.RegisterFunctionRequest(OnPowerOutputCtrl_1);

		CANLib::obj_out_2.RegisterFunctionSet(OnPowerOutputCtrl_2);
		CANLib::obj_out_2.RegisterFunctionToggle(OnPowerOutputCtrl_2);
		CANLib::obj_out_2.RegisterFunctionAction(OnPowerOutputCtrl_2);
		CANLib::obj_out_2.RegisterFunctionRequest(OnPowerOutputCtrl_2);

		CANLib::obj_out_3.RegisterFunctionSet(OnPowerOutputCtrl_3);
		CANLib::obj_out_3.RegisterFunctionToggle(OnPowerOutputCtrl_3);
		CANLib::obj_out_3.RegisterFunctionAction(OnPowerOutputCtrl_3);
		CANLib::obj_out_3.RegisterFunctionRequest(OnPowerOutputCtrl_3);

		CANLib::obj_out_4.RegisterFunctionSet(OnPowerOutputCtrl_4);
		CANLib::obj_out_4.RegisterFunctionToggle(OnPowerOutputCtrl_4);
		CANLib::obj_out_4.RegisterFunctionAction(OnPowerOutputCtrl_4);
		CANLib::obj_out_4.RegisterFunctionRequest(OnPowerOutputCtrl_4);

		CANLib::obj_out_5.RegisterFunctionSet(OnPowerOutputCtrl_5);
		CANLib::obj_out_5.RegisterFunctionToggle(OnPowerOutputCtrl_5);
		CANLib::obj_out_5.RegisterFunctionAction(OnPowerOutputCtrl_5);
		CANLib::obj_out_5.RegisterFunctionRequest(OnPowerOutputCtrl_5);

		CANLib::obj_out_6.RegisterFunctionSet(OnPowerOutputCtrl_6);
		CANLib::obj_out_6.RegisterFunctionToggle(OnPowerOutputCtrl_6);
		CANLib::obj_out_6.RegisterFunctionAction(OnPowerOutputCtrl_6);
		CANLib::obj_out_6.RegisterFunctionRequest(OnPowerOutputCtrl_6);

		CANLib::obj_out_7.RegisterFunctionSet(OnPowerOutputCtrl_7);
		CANLib::obj_out_7.RegisterFunctionToggle(OnPowerOutputCtrl_7);
		CANLib::obj_out_7.RegisterFunctionAction(OnPowerOutputCtrl_7);
		CANLib::obj_out_7.RegisterFunctionRequest(OnPowerOutputCtrl_7);

		CANLib::obj_out_8.RegisterFunctionSet(OnPowerOutputCtrl_8);
		CANLib::obj_out_8.RegisterFunctionToggle(OnPowerOutputCtrl_8);
		CANLib::obj_out_8.RegisterFunctionAction(OnPowerOutputCtrl_8);
		CANLib::obj_out_8.RegisterFunctionRequest(OnPowerOutputCtrl_8);
		}
		
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
