#pragma once
#include <inttypes.h>
#include <string.h>
#include <math.h>

//#include <stm32f1xx_hal_def.h>
#include <CUtils.h>
#include "WS2812HW.h"




#define DISPLAY_WIDTH		64		// переименовать в FRAME_ OR NOT?
#define DISPLAY_HEIGHT		48
#include <FrameBuffer.h>
#include <FrameManager.h>
#include <effects/FrameEffectFire.h>
#include <effects/FrameEffectSphere.h>
#include <effects/FrameEffectGameOfLife.h>
#include <effects/FrameEffectPrimitiveLights.h>
#include <effects/FrameEffectReader.h>

//extern TIM_HandleTypeDef htim2;
//extern DMA_HandleTypeDef hdma_tim2_ch1;







namespace WS2812Logic
{

	/* Настройки */
	static constexpr uint8_t CFG_Layers = 8;		// Кол-во слоёв анимации.
	static constexpr uint8_t CFG_Width = 128;		// Ширина экрана.
	static constexpr uint8_t CFG_Height = 16;		// Высота экрана.
	static constexpr uint16_t CFG_Delay = 200;		// Интервал обновления экрана.
	static constexpr uint8_t CFG_Brightness = 10;	// Яркость матрицы.
	#define ROOT_DIRECTORY ("/pxl_r")				// Папка с файлами pxl.
	/* */
	
	//MatrixLed<CFG_Layers, CFG_Width, CFG_Height> matrixObj(CFG_Delay);



	
	uint8_t *frame_buffer_ptr;
	uint16_t frame_buffer_len;
	volatile uint16_t frame_buffer_idx = 0;
	volatile uint8_t dma_buffer[ (8 * 3 * 6) ];		// 8 бит * 3 цвета * 6 пикселя.
	


	

	FrameBuffer buffer;

	FrameManager manager(buffer);
	FrameEffectFire effect_fire;
	FrameEffectSphere effect_sphere;
	FrameEffectGameOfLife effect_game;
	FrameEffectPrimitiveLights effect_primitive;
	FrameEffectReader effect_reader;


	// Маппер без маппинга
	static inline uint16_t mapper_0(uint16_t input)
	{
		return input;
	}
	
	// Маппер последовательной панели, Линейное подлючение, Один ряд
	// +----+----+----+----+----+----+
	// | 01 | 02 | 03 | 04 | 05 | XX |
	// +----+----+----+----+----+----+	
	static inline uint16_t mapper_1(uint16_t input)
	{
		static uint16_t width = DISPLAY_WIDTH;
		static uint16_t height = DISPLAY_HEIGHT;
		static uint8_t color_map[] = {1, 0, 2, 0};

		uint16_t pixelIndex = input / 3;
		uint16_t row = pixelIndex % height;		// Номер пикселя в зиг-заге
		uint16_t col = pixelIndex / height;		// Номер столбца зиг-зага

		uint16_t rowTransformed = (col & 1) ? (height - row - 1) : row;
		uint16_t index = rowTransformed * width + col;

		return (index * 3) + color_map[(input - pixelIndex * 3)];
	}

	// Маппер последовательной панели, Построчного подключения, Несколько рядов
	// +----+----+----+----+
	// | 01 | 02 | 03 | 04 |
	// +----+----+----+----+
	// | 05 | 06 | 07 | 08 |
	// +----+----+----+----+
	// | 09 | 10 | 11 | 12 |
	// +----+----+----+----+
	static inline uint16_t mapper_2(uint16_t input)
	{
		static const uint16_t width = DISPLAY_WIDTH;
		static const uint16_t height = DISPLAY_HEIGHT;
		static const uint16_t block_height = 16;
		static const uint8_t color_map[] = {1, 0, 2, 0};
		
		// Номер пикселя
		uint16_t pixelIndex = input / 3;

		// Идём по «столбцам»
		uint16_t row = pixelIndex % block_height;				// 0..15
		uint16_t col = (pixelIndex / block_height) % width;		// 0..63
		uint16_t block = pixelIndex / (width * block_height);	// 0..2

		// Зигзаг по строкам в столбце
		uint16_t rowTransformed = (col & 1) 
			? (block_height - 1 - row) 
			: row;
		
		// Номер строки на экране (0..47)
		uint16_t transformed_row = (block * block_height) + rowTransformed;

		// Линейный индекс пикселя на физическом экране
		uint16_t index = transformed_row * width + col;

		// Смещение по компоненте
		return (index * 3) + color_map[(input - (pixelIndex * 3))];
	}

	
	
	
	


	typedef uint16_t (*idx_mapper_ptr)(uint16_t idx);
	idx_mapper_ptr mapper_func = mapper_0;
	







/*
58us	29us	Итератор тут
60		30
49us	25us	без иторетор , указатели
52us	26us	без итератора, по массиву
*/

void CreateDMABuffer(uint8_t mode)
{
	//Leds::obj.SetOn(Leds::LED_WHITE);
	
	static uint16_t buff_copy_logic[3][2] = 
	{
		{0, sizeof(dma_buffer)}, 
		{0, (sizeof(dma_buffer) / 2)}, 
		{(sizeof(dma_buffer) / 2), sizeof(dma_buffer)}
	};	
	uint16_t start = buff_copy_logic[mode][0];
	uint16_t end = buff_copy_logic[mode][1];
	
	//uint8_t *frame_ptr = &frame_buffer_ptr[frame_buffer_idx];
	uint8_t byte, mask;
	uint16_t index;
	
	for(uint16_t i = start; i < end; i += 8)
	{
		//byte = *frame_ptr++;
		index = mapper_func(frame_buffer_idx++);
		byte = frame_buffer_ptr[index];
		mask = 0x80;
		
		for(uint8_t b = 0; b < 8; ++b)
		{
			dma_buffer[i + b] = (byte & mask) ? PWM_HI : PWM_LO;
			mask >>= 1;
		}
	}
	//frame_buffer_idx += (end - start) / 8;
	
	//Leds::obj.SetOff(Leds::LED_WHITE);
}



static void DMA_FullCpltCallback(DMA_HandleTypeDef *hdma);
static void DMA_HalfCpltCallback(DMA_HandleTypeDef *hdma);

void DMA_Start()
{
	if(frame_buffer_idx != 0) return;
	if(DMA_HANDLE.State != HAL_DMA_STATE_READY) return;
	
	CreateDMABuffer(0);
	
	HAL_StatusTypeDef DMA_Send_Stat = HAL_ERROR;
	do
	{
		if(TIM_CHANNEL_STATE_GET(&TIM_HANDLE, TIM_CH) == HAL_TIM_CHANNEL_STATE_BUSY)
		{
			DMA_Send_Stat = HAL_BUSY;
		}
		else if(TIM_CHANNEL_STATE_GET(&TIM_HANDLE, TIM_CH) == HAL_TIM_CHANNEL_STATE_READY)
		{
			TIM_CHANNEL_STATE_SET(&TIM_HANDLE, TIM_CH, HAL_TIM_CHANNEL_STATE_BUSY);
		}
		else
		{
			DMA_Send_Stat = HAL_ERROR;
			break;
		}
		
		TIM_HANDLE.hdma[ARGB_TIM_DMA_ID]->XferCpltCallback = DMA_FullCpltCallback;
		TIM_HANDLE.hdma[ARGB_TIM_DMA_ID]->XferHalfCpltCallback = DMA_HalfCpltCallback;
		TIM_HANDLE.hdma[ARGB_TIM_DMA_ID]->XferErrorCallback = TIM_DMAError;
		DMA_Send_Stat = HAL_DMA_Start_IT(TIM_HANDLE.hdma[ARGB_TIM_DMA_ID], (uint32_t)dma_buffer, (uint32_t) &TIM_HANDLE.Instance->ARGB_TIM_CCR, sizeof(dma_buffer));
		
		if(DMA_Send_Stat == HAL_OK)
		{
			__HAL_TIM_ENABLE_DMA(&TIM_HANDLE, ARGB_TIM_DMA_CC);
			
			if(IS_TIM_BREAK_INSTANCE(TIM_HANDLE.Instance) != RESET)
				__HAL_TIM_MOE_ENABLE(&TIM_HANDLE);
			
			if(IS_TIM_SLAVE_INSTANCE(TIM_HANDLE.Instance) != RESET && IS_TIM_SLAVEMODE_TRIGGER_ENABLED(TIM_HANDLE.Instance->SMCR & TIM_SMCR_SMS) == RESET)
				__HAL_TIM_ENABLE(&TIM_HANDLE);
			else
				__HAL_TIM_ENABLE(&TIM_HANDLE);
		}
	} while(DMA_Send_Stat != HAL_OK);
	
	return;
}

static void DMA_HalfCpltCallback(DMA_HandleTypeDef *hdma)
{
	if(hdma != &DMA_HANDLE || hdma->Parent != &TIM_HANDLE) return;
	if(frame_buffer_idx == 0) return;
	
	if(frame_buffer_idx < frame_buffer_len)
	{
		CreateDMABuffer(1);
	}
	else if(frame_buffer_idx < frame_buffer_len + (sizeof(dma_buffer) / 8))
	{
		memset((uint8_t *) &dma_buffer[0], 0x00, (sizeof(dma_buffer) / 2));
		frame_buffer_idx += (sizeof(dma_buffer) / 2 / 8);
	}

	return;
}

static void DMA_FullCpltCallback(DMA_HandleTypeDef *hdma)
{
	if(hdma != &DMA_HANDLE || hdma->Parent != &TIM_HANDLE) return;
	if(frame_buffer_idx == 0) return;
	
	//TIM_HANDLE.Channel = (HAL_TIM_ActiveChannel)TIM_CH;
	TIM_CHANNEL_STATE_SET(&TIM_HANDLE, TIM_CH, HAL_TIM_CHANNEL_STATE_READY);
	
	if(frame_buffer_idx < frame_buffer_len)
	{
		CreateDMABuffer(2);
	}
	else if(frame_buffer_idx < frame_buffer_len + (sizeof(dma_buffer) / 8))
	{
		memset((uint8_t *) &dma_buffer[sizeof(dma_buffer) / 2], 0x00, (sizeof(dma_buffer) / 2));
		frame_buffer_idx += (sizeof(dma_buffer) / 2 / 8);
	}
	else
	{
		frame_buffer_idx = 0;
		
		__HAL_TIM_DISABLE_DMA(&TIM_HANDLE, ARGB_TIM_DMA_CC);
		(void) HAL_DMA_Abort_IT(TIM_HANDLE.hdma[ARGB_TIM_DMA_ID]);
		
		if(IS_TIM_BREAK_INSTANCE(TIM_HANDLE.Instance) != RESET)
			__HAL_TIM_MOE_DISABLE(&TIM_HANDLE);
		
		__HAL_TIM_DISABLE(&TIM_HANDLE);
		TIM_CHANNEL_STATE_SET(&TIM_HANDLE, TIM_CH, HAL_TIM_CHANNEL_STATE_READY);
		
		buffer.DrawEnding();
	}
	//TIM_HANDLE.Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
	
	return;
}















inline void Setup()
{
	srand( Analog::mux.Get(10) * 10 );
	
	mapper_func = mapper_2;

	//manager.SelectEffect(effect_reader, 100);
	manager.SelectEffect(effect_game, 100);

	//effect_primitive.Control(FrameEffectPrimitiveLights::SIGNAL_EMERGENCY, 255);

	buffer.SetBrightness(64);
	buffer.SetColorCorrection(255, 211, 167);
	frame_buffer_ptr = buffer.frame_buffer.raw;
	frame_buffer_len = sizeof(buffer.frame_buffer.raw);


	
	SetupHW();


	
	return;
}

uint32_t timer1, timer2, timer3, timer12, timer23;

inline void Loop(uint32_t &current_time)
{
/*
	static uint8_t idx = 0;
	static uint32_t tick = 0;
	static uint32_t tick_time = 0;
	if(current_time - tick > tick_time)
	{
		tick = current_time;

		if(idx == 0)
		{
			manager.SelectEffect(effect_fire);
			tick_time = 60000;
			idx = 1;
		}
		
		else if(idx == 1)
		{
			manager.SelectEffect(effect_sphere);
			tick_time = 30000;
			idx = 2;
		}

		else if(idx == 2)
		{
			manager.SelectEffect(effect_game);
			tick_time = 60000;
			idx = 3;
		}

		else if(idx == 3)
		{
			manager.SelectEffect(effect_primitive);
			tick_time = 15000;
			idx = 0;
		}

		
	}
*/



	manager.Tick(current_time);

/*
	static uint32_t tick = 0;
	if(current_time - tick > 1000)
	{
		tick = current_time ;
		
		srand(Analog::mux.adc_value[11]);
	}
*/

	static uint32_t lasttime = 0;
	if(buffer.DrawIsReady() == true)
	{
		buffer.DrawBegin();
		DMA_Start();

		//Logger.Print("+PXL=128,16,6144,");
		//Logger.Print(frame_buffer_ptr, frame_buffer_len, LOG_OUT_TYPE_BYTES);
		//Logger.Print("\n");

		//DEBUG_LOG_TOPIC("DMADraw", "time: %d\n", (HAL_GetTick() - lasttime));
		lasttime = HAL_GetTick();
	}








	/*
	timer1 = HAL_GetTick();
	matrixObj.Processing(current_time);
	timer2 = HAL_GetTick();
	
	if(matrixObj.IsBufferReady() == true)
	{
		timer12 = timer2 - timer1;
		timer23 = HAL_GetTick();
		
		matrixObj.SetFrameDrawStart();
		
		DMA_Start();
		
		//Serial::Print("+PXL=128,16,2\r\n");
		//Serial::Print(frame_buffer_ptr, frame_buffer_len);
	}
	
	if( matrixObj.GetFrameIsDraw() == true && frame_buffer_idx == 0 )
	{
		matrixObj.SetFrameDrawEnd();
		
		timer23 = HAL_GetTick() - timer23;
		//DEBUG_LOG_TOPIC("PXLTime", "render: %d ms, draw: %d ms, total: %d ms\n", timer12, timer23, (timer23 + timer12));
		Logger.PrintTopic("PXLTime").Printf("render: %d ms, draw: %d ms, total: %d ms;\n", timer12, timer23, (timer23 + timer12));
	}
	*/
	
	current_time = HAL_GetTick();
	
	return;
}
};
