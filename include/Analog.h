#pragma once
#include <AnalogMux.h>
#include <DrakePinA.hpp>
#include <DrakePinD.hpp>
#include <CUtils.h>

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

namespace Analog
{
	uint16_t OnMuxRequest(uint8_t address);
	void OnMuxResponse(uint8_t address, uint16_t value);
	
	DrakePinA adc_pin({&hadc2, GPIOB, GPIO_PIN_1, ADC_CHANNEL_9}, ADC_SAMPLETIME_7CYCLES_5);
	DividerVoltageCalc VoltCalc(12, 3300, 69000, 10000);

	DrakePinD InPwrEn({GPIOB, GPIO_PIN_8}, DrakePin::Output, DrakePin::Low);
	
	AnalogMux<4> mux( OnMuxRequest, OnMuxResponse, 
		DrakePin::PinD_t{GPIOB, GPIO_PIN_4}, 
		DrakePin::PinD_t{GPIOB, GPIO_PIN_5}, 
		DrakePin::PinD_t{GPIOB, GPIO_PIN_6}, 
		DrakePin::PinD_t{GPIOB, GPIO_PIN_7}
	);



	struct regular_channel_t
	{
		GPIO_TypeDef *port;
		uint32_t pin;
		uint32_t channel;
		uint32_t rank;
	};
	
	static constexpr regular_channel_t channels[] = 
	{
		{GPIOA, GPIO_PIN_1, ADC_CHANNEL_1, ADC_REGULAR_RANK_1},
		{GPIOA, GPIO_PIN_2, ADC_CHANNEL_2, ADC_REGULAR_RANK_2},
		{GPIOA, GPIO_PIN_3, ADC_CHANNEL_3, ADC_REGULAR_RANK_3},
		{GPIOA, GPIO_PIN_4, ADC_CHANNEL_4, ADC_REGULAR_RANK_4},
		{GPIOA, GPIO_PIN_5, ADC_CHANNEL_5, ADC_REGULAR_RANK_5},
		{GPIOA, GPIO_PIN_6, ADC_CHANNEL_6, ADC_REGULAR_RANK_6},
		{GPIOA, GPIO_PIN_7, ADC_CHANNEL_7, ADC_REGULAR_RANK_7},
		{GPIOB, GPIO_PIN_0, ADC_CHANNEL_8, ADC_REGULAR_RANK_8}
	};
	static constexpr uint8_t regular_channel_count = sizeofarray(channels);
	volatile uint16_t adc_buf[regular_channel_count];

	
	
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
				uint16_t vin = VoltCalc.GetmV(value);
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
		
		if(address == 15)
		{
			DEBUG_LOG_TOPIC("MUX", "%04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d", mux.adc_value[0], mux.adc_value[1], mux.adc_value[2], 
			mux.adc_value[3], mux.adc_value[4], mux.adc_value[5], mux.adc_value[6], mux.adc_value[7], mux.adc_value[8], mux.adc_value[9], mux.adc_value[10], mux.adc_value[11], 
			mux.adc_value[12], mux.adc_value[13], mux.adc_value[14], mux.adc_value[15]);

			DEBUG_LOG_TOPIC("DNA", "    %04d %04d %04d %04d %04d %04d %04d %04d\n", 
				adc_buf[0], adc_buf[1], adc_buf[2], adc_buf[3], adc_buf[4], adc_buf[5], adc_buf[6],adc_buf[7]);
		}
		
		return;
	}



	static void RegularConfig()
	{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		ADC_ChannelConfTypeDef sConfig = {0};

		for(auto &channel : channels)
		{
			GPIO_InitStruct.Pin = channel.pin;
			GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
			HAL_GPIO_Init(channel.port, &GPIO_InitStruct);

			sConfig.Channel = channel.channel;
			sConfig.Rank = channel.rank;
			sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
			if(HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
			{
				Error_Handler();
			}
		}
	}







	
	inline void Setup()
	{
		HAL_ADCEx_Calibration_Start(&hadc1);
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buf, 8);

		mux.Init();
		adc_pin.Init();
		InPwrEn.Init();
		InPwrEn.On();
		// Реализовать управление InPwrEn
		
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
