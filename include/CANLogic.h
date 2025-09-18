#pragma once
#include <EasyPinD.h>
#include <CANLibrary.h>

extern CAN_HandleTypeDef hcan;
extern void HAL_CAN_Send(uint16_t id, uint8_t *data_raw, uint8_t length_raw);

namespace CANLib
{
	static constexpr uint8_t CFG_CANObjectsCount = 22;
	static constexpr uint8_t CFG_CANFrameBufferSize = 16;
	static constexpr uint16_t CFG_CANFirstId = 0x0160;
	
	EasyPinD can_rs(GPIOA, {GPIO_PIN_15, GPIO_MODE_OUTPUT_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW});
	
	CANManager<CFG_CANObjectsCount, CFG_CANFrameBufferSize> can_manager(&HAL_CAN_Send);
	
	CANObject<uint8_t,  7> obj_block_info(CFG_CANFirstId + 0);
	CANObject<uint8_t,  7> obj_block_health(CFG_CANFirstId + 1);
	CANObject<uint8_t,  7> obj_block_features(CFG_CANFirstId + 2);
	CANObject<uint8_t,  7> obj_block_error(CFG_CANFirstId + 3);

	CANObject<uint8_t,  1> obj_out_1(CFG_CANFirstId + 4);
	CANObject<uint8_t,  1> obj_out_2(CFG_CANFirstId + 5);
	CANObject<uint8_t,  1> obj_out_3(CFG_CANFirstId + 6);
	CANObject<uint8_t,  1> obj_out_4(CFG_CANFirstId + 7);
	CANObject<uint8_t,  1> obj_out_5(CFG_CANFirstId + 8);
	CANObject<uint8_t,  1> obj_out_6(CFG_CANFirstId + 9);
	CANObject<uint8_t,  1> obj_out_7(CFG_CANFirstId + 10);
	CANObject<uint8_t,  1> obj_out_8(CFG_CANFirstId + 11);
	CANObject<uint16_t, 1> obj_in_1(CFG_CANFirstId + 12, 100);
	CANObject<uint16_t, 1> obj_in_2(CFG_CANFirstId + 13, 100);
	CANObject<uint16_t, 1> obj_in_3(CFG_CANFirstId + 14, 100);
	CANObject<uint16_t, 1> obj_in_4(CFG_CANFirstId + 15, 100);
	CANObject<uint16_t, 1> obj_in_5(CFG_CANFirstId + 16, 100);
	CANObject<uint16_t, 1> obj_in_6(CFG_CANFirstId + 17, 100);
	CANObject<uint16_t, 1> obj_in_7(CFG_CANFirstId + 18, 100);
	CANObject<uint16_t, 1> obj_in_8(CFG_CANFirstId + 19, 100);
	CANObject<int8_t,   7> obj_temperature(CFG_CANFirstId + 20);
	CANObject<uint8_t,  1> obj_led_ctrl(CFG_CANFirstId + 21);




	can_result_t OnPowerOutputCtrl(uint8_t port, can_frame_t &can_frame, can_error_t &error)
	{
		can_frame.initialized = true;

		switch(can_frame.function_id)
		{
			case CAN_FUNC_SET_IN:
			{
				bool result = Outputs::outObj.SetWrite( port, can_frame.data[0] );
				can_frame.function_id = (result == true) ? CAN_FUNC_EVENT_OK : CAN_FUNC_EVENT_ERROR;
				
				break;
			}
			case CAN_FUNC_TOGGLE_IN:
			{
				Outputs::outObj.SetToggle(port);
				can_frame.function_id = CAN_FUNC_EVENT_OK;
				
				break;
			}
			case CAN_FUNC_ACTION_IN:
			{
				bool result = Outputs::outObj.SetOn(port, 250);
				can_frame.function_id = (result == true) ? CAN_FUNC_EVENT_OK : CAN_FUNC_EVENT_ERROR;
				
				break;
			}
			case CAN_FUNC_REQUEST_IN:
			{
				uint8_t result = (Outputs::outObj.GetState(port) > 0) ? 0xFF : 0x00;
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
	
	
	void CAN_Enable()
	{
		HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_ERROR | CAN_IT_BUSOFF | CAN_IT_LAST_ERROR_CODE);
		HAL_CAN_Start(&hcan);
		
		can_rs.On();
		
		return;
	}
	
	void CAN_Disable()
	{
		HAL_CAN_DeactivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_ERROR | CAN_IT_BUSOFF | CAN_IT_LAST_ERROR_CODE);
		HAL_CAN_Stop(&hcan);
		
		can_rs.Off();
		
		return;
	}
	
	inline void Setup()
	{
		can_rs.Init();
		
		set_block_info_params(obj_block_info);
		set_block_health_params(obj_block_health);
		set_block_features_params(obj_block_features);
		set_block_error_params(obj_block_error);
		
		can_manager.RegisterObject(obj_block_info);
		can_manager.RegisterObject(obj_block_health);
		can_manager.RegisterObject(obj_block_features);
		can_manager.RegisterObject(obj_block_error);

		can_manager.RegisterObject(obj_out_1);
		can_manager.RegisterObject(obj_out_2);
		can_manager.RegisterObject(obj_out_3);
		can_manager.RegisterObject(obj_out_4);
		can_manager.RegisterObject(obj_out_5);
		can_manager.RegisterObject(obj_out_6);
		can_manager.RegisterObject(obj_out_7);
		can_manager.RegisterObject(obj_out_8);
		can_manager.RegisterObject(obj_in_1);
		can_manager.RegisterObject(obj_in_2);
		can_manager.RegisterObject(obj_in_3);
		can_manager.RegisterObject(obj_in_4);
		can_manager.RegisterObject(obj_in_5);
		can_manager.RegisterObject(obj_in_6);
		can_manager.RegisterObject(obj_in_7);
		can_manager.RegisterObject(obj_in_8);
		can_manager.RegisterObject(obj_temperature);
		can_manager.RegisterObject(obj_led_ctrl);

		obj_out_1.RegisterFunctionSet(OnPowerOutputCtrl_1);
		obj_out_1.RegisterFunctionToggle(OnPowerOutputCtrl_1);
		obj_out_1.RegisterFunctionAction(OnPowerOutputCtrl_1);
		obj_out_1.RegisterFunctionRequest(OnPowerOutputCtrl_1);

		obj_out_2.RegisterFunctionSet(OnPowerOutputCtrl_2);
		obj_out_2.RegisterFunctionToggle(OnPowerOutputCtrl_2);
		obj_out_2.RegisterFunctionAction(OnPowerOutputCtrl_2);
		obj_out_2.RegisterFunctionRequest(OnPowerOutputCtrl_2);

		obj_out_3.RegisterFunctionSet(OnPowerOutputCtrl_3);
		obj_out_3.RegisterFunctionToggle(OnPowerOutputCtrl_3);
		obj_out_3.RegisterFunctionAction(OnPowerOutputCtrl_3);
		obj_out_3.RegisterFunctionRequest(OnPowerOutputCtrl_3);

		obj_out_4.RegisterFunctionSet(OnPowerOutputCtrl_4);
		obj_out_4.RegisterFunctionToggle(OnPowerOutputCtrl_4);
		obj_out_4.RegisterFunctionAction(OnPowerOutputCtrl_4);
		obj_out_4.RegisterFunctionRequest(OnPowerOutputCtrl_4);

		obj_out_5.RegisterFunctionSet(OnPowerOutputCtrl_5);
		obj_out_5.RegisterFunctionToggle(OnPowerOutputCtrl_5);
		obj_out_5.RegisterFunctionAction(OnPowerOutputCtrl_5);
		obj_out_5.RegisterFunctionRequest(OnPowerOutputCtrl_5);

		obj_out_6.RegisterFunctionSet(OnPowerOutputCtrl_6);
		obj_out_6.RegisterFunctionToggle(OnPowerOutputCtrl_6);
		obj_out_6.RegisterFunctionAction(OnPowerOutputCtrl_6);
		obj_out_6.RegisterFunctionRequest(OnPowerOutputCtrl_6);

		obj_out_7.RegisterFunctionSet(OnPowerOutputCtrl_7);
		obj_out_7.RegisterFunctionToggle(OnPowerOutputCtrl_7);
		obj_out_7.RegisterFunctionAction(OnPowerOutputCtrl_7);
		obj_out_7.RegisterFunctionRequest(OnPowerOutputCtrl_7);

		obj_out_8.RegisterFunctionSet(OnPowerOutputCtrl_8);
		obj_out_8.RegisterFunctionToggle(OnPowerOutputCtrl_8);
		obj_out_8.RegisterFunctionAction(OnPowerOutputCtrl_8);
		obj_out_8.RegisterFunctionRequest(OnPowerOutputCtrl_8);



		Config::eeprom_body_t &cfg = Config::obj.body;
		
		if(cfg.in1.enable == true)
		{
			cfg.in1.interval_ms;
			if(cfg.in1.realtime == true)
			{
				//obj_in_1.
			}
			else
			{
				//obj_in_1.
			}
		}

		
		


/*		
		obj_trunk_control
			.RegisterFunctionSet([](can_frame_t &can_frame, can_error_t &error) -> can_result_t
			{
				TrunkHood::LogicSet(TrunkHood::driver2, TrunkHood::actuator_data[1], can_frame.data[0]);
				obj_trunk_control.SetValue(0, can_frame.data[0], CAN_TIMER_TYPE_NONE, CAN_EVENT_TYPE_NORMAL);

				return CAN_RESULT_IGNORE;
			})
			.RegisterFunctionToggle([](can_frame_t &can_frame, can_error_t &error) -> can_result_t
			{
				TrunkHood::LogicToggle(TrunkHood::driver2, TrunkHood::actuator_data[1]);
				obj_trunk_control.SetValue(0, TrunkHood::actuator_data[1].state, CAN_TIMER_TYPE_NONE, CAN_EVENT_TYPE_NORMAL);

				return CAN_RESULT_IGNORE;
			});
		
		
		obj_hood_control
			.RegisterFunctionSet([](can_frame_t &can_frame, can_error_t &error) -> can_result_t
			{
				TrunkHood::LogicSet(TrunkHood::driver1, TrunkHood::actuator_data[0], can_frame.data[0]);
				obj_hood_control.SetValue(0, can_frame.data[0], CAN_TIMER_TYPE_NONE, CAN_EVENT_TYPE_NORMAL);

				return CAN_RESULT_IGNORE;
			})
			.RegisterFunctionToggle([](can_frame_t &can_frame, can_error_t &error) -> can_result_t
			{
				TrunkHood::LogicToggle(TrunkHood::driver1, TrunkHood::actuator_data[0]);
				obj_hood_control.SetValue(0, TrunkHood::actuator_data[0].state, CAN_TIMER_TYPE_NONE, CAN_EVENT_TYPE_NORMAL);

				return CAN_RESULT_IGNORE;
			});	
		
		obj_secelec_control
			.RegisterFunctionSet([](can_frame_t &can_frame, can_error_t &error) -> can_result_t
			{
				bool result = true;
				if(can_frame.data[0] == 0)
				{
					Outputs::outObj.SetOff(1);
				}
				else
				{
					result = Outputs::outObj.SetOn(1);
				}

				if (result)
				{
					obj_secelec_control.SetValue(0, on_off_validator(can_frame.data[0]), CAN_TIMER_TYPE_NONE, CAN_EVENT_TYPE_NORMAL);
					return CAN_RESULT_IGNORE;
				}
				
				error.error_section = ERROR_SECTION_HARDWARE;
				error.error_code = ERROR_CODE_HW_SECONDARY_ELECTRONICS_ERROR;
				return CAN_RESULT_ERROR;
			})
			.RegisterFunctionToggle([](can_frame_t &can_frame, can_error_t &error) -> can_result_t
			{
				Outputs::outObj.SetToggle(1);
				obj_secelec_control.SetValue(0, Outputs::outObj.GetState(1), CAN_TIMER_TYPE_NONE, CAN_EVENT_TYPE_NORMAL);
				
				return CAN_RESULT_IGNORE;
			});
		
		
		obj_leftdoor_control.RegisterFunctionAction([](can_frame_t &can_frame, can_error_t &error) -> can_result_t
		{
			Outputs::outObj.SetOn(2, 1000);
			
			can_frame.initialized = true;
			can_frame.function_id = CAN_FUNC_EVENT_OK;
			
			return CAN_RESULT_CAN_FRAME;
		});
		
		
		obj_rightdoor_control.RegisterFunctionAction([](can_frame_t &can_frame, can_error_t &error) -> can_result_t
		{
			Outputs::outObj.SetOn(3, 1000);
			
			can_frame.initialized = true;
			can_frame.function_id = CAN_FUNC_EVENT_OK;
			
			return CAN_RESULT_CAN_FRAME;
		});
		
		
		obj_cabinlight_control
			.RegisterFunctionSet([](can_frame_t &can_frame, can_error_t &error) -> can_result_t
			{
				bool result = true;
				if(can_frame.data[0] == 0)
				{
					Outputs::outObj.SetOff(4);
				}
				else
				{
					result = Outputs::outObj.SetOn(4);
				}

				if (result)
				{
					obj_cabinlight_control.SetValue(0, on_off_validator(can_frame.data[0]), CAN_TIMER_TYPE_NONE, CAN_EVENT_TYPE_NORMAL);
					return CAN_RESULT_IGNORE;
				}
				
				error.error_section = ERROR_SECTION_HARDWARE;
				error.error_code = ERROR_CODE_HW_CABIN_LIGHT_ERROR;
				return CAN_RESULT_ERROR;
			})
			.RegisterFunctionToggle([](can_frame_t &can_frame, can_error_t &error) -> can_result_t
			{
				Outputs::outObj.SetToggle(4);
				obj_cabinlight_control.SetValue(0, Outputs::outObj.GetState(4), CAN_TIMER_TYPE_NONE, CAN_EVENT_TYPE_NORMAL);
				
				return CAN_RESULT_IGNORE;
			});
		
		obj_rearcamera_control
			.RegisterFunctionSet([](can_frame_t &can_frame, can_error_t &error) -> can_result_t
			{
				bool result = true;
				if(can_frame.data[0] == 0)
				{
					Outputs::outObj.SetOff(5);
				}
				else
				{
					result = Outputs::outObj.SetOn(5);
				}

				if (result)
				{
					obj_rearcamera_control.SetValue(0, on_off_validator(can_frame.data[0]), CAN_TIMER_TYPE_NONE, CAN_EVENT_TYPE_NORMAL);
					return CAN_RESULT_IGNORE;
				}
				
				error.error_section = ERROR_SECTION_HARDWARE;
				error.error_code = ERROR_CODE_HW_REAR_CAMERA_ERROR;
				return CAN_RESULT_ERROR;
			})
			.RegisterFunctionToggle([](can_frame_t &can_frame, can_error_t &error) -> can_result_t
			{
				Outputs::outObj.SetToggle(5);
				obj_rearcamera_control.SetValue(0, Outputs::outObj.GetState(5), CAN_TIMER_TYPE_NONE, CAN_EVENT_TYPE_NORMAL);
				
				return CAN_RESULT_IGNORE;
			});
		
		obj_horn_control
			.RegisterFunctionSet([](can_frame_t &can_frame, can_error_t &error) -> can_result_t
			{
				bool result = true;
				if(can_frame.data[0] == 0)
				{
					Outputs::outObj.SetOff(6);
				}
				else
				{
					result = Outputs::outObj.SetOn(6);
				}

				if (result)
				{
					obj_horn_control.SetValue(0, on_off_validator(can_frame.data[0]), CAN_TIMER_TYPE_NONE, CAN_EVENT_TYPE_NORMAL);
					return CAN_RESULT_IGNORE;
				}
				
				error.error_section = ERROR_SECTION_HARDWARE;
				error.error_code = ERROR_CODE_HW_HORN_ERROR;
				return CAN_RESULT_ERROR;
			})
			.RegisterFunctionToggle([](can_frame_t &can_frame, can_error_t &error) -> can_result_t
			{
				Outputs::outObj.SetToggle(6);
				obj_horn_control.SetValue(0, Outputs::outObj.GetState(6), CAN_TIMER_TYPE_NONE, CAN_EVENT_TYPE_NORMAL);
				
				return CAN_RESULT_IGNORE;
			});
*/


		// Передача версий и типов в объект block_info
		obj_block_info.SetValue(0, (About::board_type << 3 | About::board_ver), CAN_TIMER_TYPE_NORMAL);
		obj_block_info.SetValue(1, (About::soft_ver << 2 | About::can_ver), CAN_TIMER_TYPE_NORMAL);

		CAN_Enable();
		
		return;
	}

	inline void Loop(uint32_t &current_time)
	{
		can_manager.Process(current_time);

		// Передача UpTime блока в объект block_info
		static uint32_t iter1000 = 0;
		if(current_time - iter1000 > 1000)
		{
			iter1000 = current_time;
			
			uint8_t *data = (uint8_t *)&current_time;
			obj_block_info.SetValue(2, data[0], CAN_TIMER_TYPE_NORMAL);
			obj_block_info.SetValue(3, data[1], CAN_TIMER_TYPE_NORMAL);
			obj_block_info.SetValue(4, data[2], CAN_TIMER_TYPE_NORMAL);
			obj_block_info.SetValue(5, data[3], CAN_TIMER_TYPE_NORMAL);
		}
		
		// При выходе обновляем время
		current_time = HAL_GetTick();
		
		return;
	}
}
