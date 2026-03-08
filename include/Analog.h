#pragma once
#include <AnalogMux.h>
#include <DrakePinA.hpp>
#include <DrakePinD.hpp>
#include <CUtils.h>

extern ADC_HandleTypeDef hadc1;

namespace Analog
{
	uint16_t OnMuxRequest(uint8_t address);
	void OnMuxResponse(uint8_t address, uint16_t value);
	
	DrakePinA adc_pin({&hadc1, GPIOB, GPIO_PIN_1, ADC_CHANNEL_9}, ADC_SAMPLETIME_7CYCLES_5);
	volt_calc_t VoltCalcParams = {((1 << 12) - 1), 3324, 82000, 10000, 17};
	
	AnalogMux<4> mux( OnMuxRequest, OnMuxResponse, 
		DrakePin::PinD_t{GPIOB, GPIO_PIN_4}, 
		DrakePin::PinD_t{GPIOB, GPIO_PIN_5}, 
		DrakePin::PinD_t{GPIOB, GPIO_PIN_6}, 
		DrakePin::PinD_t{GPIOB, GPIO_PIN_7}
	);
	
	
	uint16_t OnMuxRequest(uint8_t address)
	{
		return adc_pin.ReadRaw();
	}
	
	void OnMuxResponse(uint8_t address, uint16_t value)
	{
		switch(address)
		{
			case 0:
			{
				CANLib::obj_in_1.SetValue(0, value, CAN_TIMER_TYPE_NORMAL);
				break;
			}
			case 1:
			{
				CANLib::obj_in_2.SetValue(0, value, CAN_TIMER_TYPE_NORMAL);
				break;
			}
			case 2:
			{
				CANLib::obj_in_3.SetValue(0, value, CAN_TIMER_TYPE_NORMAL);
				break;
			}
			case 3:
			{
				CANLib::obj_in_4.SetValue(0, value, CAN_TIMER_TYPE_NORMAL);
				break;
			}
			case 4:
			{
				CANLib::obj_in_5.SetValue(0, value, CAN_TIMER_TYPE_NORMAL);
				break;
			}
			case 5:
			{
				CANLib::obj_in_6.SetValue(0, value, CAN_TIMER_TYPE_NORMAL);
				break;
			}
			case 6:
			{
				CANLib::obj_in_7.SetValue(0, value, CAN_TIMER_TYPE_NORMAL);
				break;
			}
			case 7:
			{
				CANLib::obj_in_8.SetValue(0, value, CAN_TIMER_TYPE_NORMAL);
				break;
			}
			case 14:
			{
				uint16_t vin = VoltageCalculate(value, VoltCalcParams);
				uint8_t *vin_bytes = (uint8_t *)&vin;

				CANLib::obj_block_health.SetValue(0, vin_bytes[0]);
				CANLib::obj_block_health.SetValue(1, vin_bytes[1]);

				break;
			}
			case 15:
			{
				break;
			}
			default:
			{
				break;
			}
		}
		
		if(address == 150)
		{
			DEBUG_LOG_TOPIC("MUX", "%04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d\n", mux.adc_value[0], mux.adc_value[1], mux.adc_value[2], 
			mux.adc_value[3], mux.adc_value[4], mux.adc_value[5], mux.adc_value[6], mux.adc_value[7], mux.adc_value[8], mux.adc_value[9], mux.adc_value[10], mux.adc_value[11], 
			mux.adc_value[12], mux.adc_value[13], mux.adc_value[14], mux.adc_value[15]);
		}
		
		return;
	}
	
	inline void Setup()
	{
		mux.Init();
		adc_pin.Init();
		
		return;
	}
	
	inline void Loop(uint32_t &current_time)
	{
		mux.Processing(current_time);
		
		static uint32_t tick1000 = 0;
		if(current_time - tick1000 > 1000)
		{
			tick1000 = current_time;
			
			// Раз в минуту запускаем калибровку ADC
			static uint8_t adc_calibration = 0;
			if(++adc_calibration >= 60)
			{
				adc_calibration = 0;

				adc_pin.Calibration();
			}
		}
		
		// При выходе обновляем время
		current_time = HAL_GetTick();
		
		return;
	}
};
