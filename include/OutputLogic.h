#pragma once
#include  <PowerOut.h>

extern ADC_HandleTypeDef hadc1;

namespace Outputs
{
	/* Настройки */
	static constexpr uint8_t CFG_PortCount = 8;			// Кол-во портов управления.
	static constexpr uint32_t CFG_RefVoltage = 3300000;	// Опорное напряжение, микровольты.
	static constexpr uint8_t CFG_INA180_Gain = 50;		// Усиление микросхемы INA180.
	static constexpr uint8_t CFG_ShuntResistance = 5;	// Сопротивление шунта, миллиомы.
	/* */
	
	PowerOut<CFG_PortCount> outObj(&hadc1, CFG_RefVoltage, CFG_INA180_Gain, CFG_ShuntResistance);
	


	void OnExternalControl(uint8_t external_id, GPIO_PinState state)
	{
		SPI::hc595.SetState(0, external_id, state);
	}
	
	void OnShortCircuit(uint8_t num, uint16_t current)
	{

	}


	
	
	inline void Setup()
	{
		outObj.AddPort( 0, {GPIOA, GPIO_PIN_1, ADC_CHANNEL_1}, 5000 );		// Выход 1
		outObj.AddPort( 1, {GPIOA, GPIO_PIN_2, ADC_CHANNEL_2}, 5000 );		// Выход 2
		outObj.AddPort( 2, {GPIOA, GPIO_PIN_3, ADC_CHANNEL_3}, 5000 );		// Выход 3
		outObj.AddPort( 3, {GPIOA, GPIO_PIN_4, ADC_CHANNEL_4}, 5000 );		// Выход 4
		outObj.AddPort( 4, {GPIOA, GPIO_PIN_5, ADC_CHANNEL_5}, 5000 );		// Выход 5
		outObj.AddPort( 5, {GPIOA, GPIO_PIN_6, ADC_CHANNEL_6}, 5000 );		// Выход 6
		outObj.AddPort( 6, {GPIOA, GPIO_PIN_7, ADC_CHANNEL_7}, 15000 );		// Выход 7
		outObj.AddPort( 7, {GPIOB, GPIO_PIN_0, ADC_CHANNEL_8}, 20000 );		// Выход HiPower-1
		
		outObj.Init();

		//outObj.On(4);
		//outObj.On(6);
		//outObj.Off(1);
		outObj.RegExternalControlEvent(OnExternalControl);
		outObj.RegShortCircuitEvent(OnShortCircuit);
		//outObj.Current(1);

		//outObj.SetOn(6, 250, 500);
		//outObj.SetOn(5, 1000, 100);
		
		return;
	}


	uint8_t test_iter = 1;
	
	inline void Loop(uint32_t &current_time)
	{
		outObj.Processing(current_time);
		
		static uint32_t last_time = 0;
		if(current_time - last_time > 250)
		{
			last_time = current_time;

/*
			outObj.SetOff(test_iter++);
			if(test_iter == 9) test_iter = 1;
			outObj.SetOn(test_iter);
*/			
			for(uint8_t i = 1; i < CFG_PortCount+1; ++i)
			{
				//Logger.PrintTopic("POUT").Printf("Port: %d, current: %5d;", i, outObj.GetCurrent(i)).PrintNewLine();
			}
		}
		
		current_time = HAL_GetTick();
		
		return;
	}
}
