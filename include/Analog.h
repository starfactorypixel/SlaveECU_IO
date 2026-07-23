#pragma once
#include <AnalogRegular.h>
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
	
	
	inline void Setup()
	{
		RegularSetup();

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
