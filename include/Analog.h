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
	DividerVoltageCalc VoltCalcIn(12, 3300, 10400, 10000);
	DividerVoltageCalc VoltCalc(12, 3300, 69000, 10000);

	DrakePinD InPwrEn({GPIOB, GPIO_PIN_8}, DrakePin::Output, DrakePin::Low);
	
	AnalogMux<4> mux( OnMuxRequest, OnMuxResponse, 
		DrakePin::PinD_t{GPIOB, GPIO_PIN_4}, 
		DrakePin::PinD_t{GPIOB, GPIO_PIN_5}, 
		DrakePin::PinD_t{GPIOB, GPIO_PIN_6}, 
		DrakePin::PinD_t{GPIOB, GPIO_PIN_7}
	);


	// Входные АЦП порты, обрабатываемые мультиплексором
	enum port_mux_t : uint8_t
	{
		PORT_IN_NONE,
		PORT_IN1,     PORT_IN2,     PORT_IN3,      PORT_IN4,
		PORT_IN5,     PORT_IN6,     PORT_IN7,      PORT_IN8,
		PORT_IN9_NC,  PORT_IN10_NC, PORT_IN11_NC,  PORT_IN12_NC,
		PORT_IN13_NC, PORT_IN14_NC, PORT_VIN,      PORT_NTC
	};
	
	// Входные АЦП порты, обрабатываемые регулярной группой
	enum port_regular_t : uint8_t
	{
		PORT_REG_NONE,
		PORT_REG1, PORT_REG2, PORT_REG3, PORT_REG4, 
		PORT_REG5, PORT_REG6, PORT_REG7, PORT_REG8
	};



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
	volatile uint16_t regular_buf[regular_channel_count];

	const uint16_t GetRegularValue(/*port_regular_t*/ uint8_t num)
	{
		if(--num >= regular_channel_count) return 0;

		return regular_buf[num];
	}

	const uint16_t GetMuxValue(port_mux_t port)
	{
		return mux.Get(port);
	}

	
	
	uint16_t OnMuxRequest(uint8_t address)
	{
		return adc_pin.ReadRaw();
	}
	
	void OnMuxResponse(uint8_t address, uint16_t value)
	{
		switch(address)
		{
			case 1:
			{
				break;
			}
			case 2:
			{
				break;
			}
			case 3:
			{
				break;
			}
			case 4:
			{
				break;
			}
			case 5:
			{
				break;
			}
			case 6:
			{
				break;
			}
			case 7:
			{
				break;
			}
			case 14:
			{
				break;
			}
			case 15:
			{
				break;
			}
			case 16:
			{
				break;
			}
			default:
			{
				break;
			}
		}
		
		if(address == 16)
		{
			/*
			DEBUG_LOG_TOPIC("MUX", "%04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d", 
				mux.Get(1), mux.Get(2),  mux.Get(3),  mux.Get(4),  mux.Get(5),  mux.Get(6),  mux.Get(7),  mux.Get(8), 
				mux.Get(9), mux.Get(10), mux.Get(11), mux.Get(12), mux.Get(13), mux.Get(14), mux.Get(15), mux.Get(16)
			);

			DEBUG_LOG_TOPIC("DNA", "    %04d %04d %04d %04d %04d %04d %04d %04d\n", 
				regular_buf[0], regular_buf[1], regular_buf[2], regular_buf[3], regular_buf[4], regular_buf[5], regular_buf[6],regular_buf[7]);
			*/
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
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)regular_buf, regular_channel_count);

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
