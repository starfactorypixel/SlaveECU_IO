#pragma once
#include <inttypes.h>
#include <string.h>
#include <math.h>

//#include <stm32f1xx_hal_def.h>
#include <CUtils.h>
#include "WS2812HW.h"




#define DISPLAY_WIDTH		128
#define DISPLAY_HEIGHT		16
#define DISPLAY_FRAME_RATE	100
#include <WS2812Manager.h>
#include <effects/WS2812EffectFire.h>
#include <effects/WS2812EffectSphere.h>
#include <effects/WS2812EffectTest.h>
#include <effects/WS2812EffectGameOfLife.h>


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
	volatile uint8_t dma_buffer[ (8 * 3 * 4) ];		// 8 бит * 3 цвета * 4 пикселя.
	






	WS2812Manager manager;
	WS2812EffectFire effect_fire;
	WS2812EffectSphere effect_sphere;
	WS2812EffectTest effect_test;
	WS2812EffectGameOfLife effect_game;








static void RGB_TIM_DMADelayPulseCplt(DMA_HandleTypeDef *hdma);
static void RGB_TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma);








void DMADraw()
{

	//frame_buffer_idx = 0;

    //ARGB_LOC_ST = ARGB_BUSY;
    if (frame_buffer_idx != 0 || DMA_HANDLE.State != HAL_DMA_STATE_READY) {
        return;
    } 
		else {
        for (volatile uint8_t i = 0; i < 8; i++) {
            // set first transfer from first values
            dma_buffer[i] = (((frame_buffer_ptr[0] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 8] = (((frame_buffer_ptr[1] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 16] = (((frame_buffer_ptr[2] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 24] = (((frame_buffer_ptr[3] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 32] = (((frame_buffer_ptr[4] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 40] = (((frame_buffer_ptr[5] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
					
			dma_buffer[i + 48] = (((frame_buffer_ptr[6] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 56] = (((frame_buffer_ptr[7] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 64] = (((frame_buffer_ptr[8] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
			dma_buffer[i + 72] = (((frame_buffer_ptr[9] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 80] = (((frame_buffer_ptr[10] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 88] = (((frame_buffer_ptr[11] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
        }
        HAL_StatusTypeDef DMA_Send_Stat = HAL_ERROR;
        while (DMA_Send_Stat != HAL_OK) {
            if (TIM_CHANNEL_STATE_GET(&TIM_HANDLE, TIM_CH) == HAL_TIM_CHANNEL_STATE_BUSY) {
                DMA_Send_Stat = HAL_BUSY;
                continue;
            } else if (TIM_CHANNEL_STATE_GET(&TIM_HANDLE, TIM_CH) == HAL_TIM_CHANNEL_STATE_READY) {
                TIM_CHANNEL_STATE_SET(&TIM_HANDLE, TIM_CH, HAL_TIM_CHANNEL_STATE_BUSY);
            } else {
                DMA_Send_Stat = HAL_ERROR;
                continue;
            }
						// Callback
            TIM_HANDLE.hdma[ARGB_TIM_DMA_ID]->XferCpltCallback = RGB_TIM_DMADelayPulseCplt;
            TIM_HANDLE.hdma[ARGB_TIM_DMA_ID]->XferHalfCpltCallback = RGB_TIM_DMADelayPulseHalfCplt;
            TIM_HANDLE.hdma[ARGB_TIM_DMA_ID]->XferErrorCallback = TIM_DMAError;
						// DMA init
            if (HAL_DMA_Start_IT(TIM_HANDLE.hdma[ARGB_TIM_DMA_ID], (uint32_t) dma_buffer,
                                 (uint32_t) &TIM_HANDLE.Instance->ARGB_TIM_CCR,
                                 (uint32_t) sizeof(dma_buffer)) != HAL_OK) {
                DMA_Send_Stat = HAL_ERROR;
                continue;
            }
            __HAL_TIM_ENABLE_DMA(&TIM_HANDLE, ARGB_TIM_DMA_CC);
            if (IS_TIM_BREAK_INSTANCE(TIM_HANDLE.Instance) != RESET)
                __HAL_TIM_MOE_ENABLE(&TIM_HANDLE);
            if (IS_TIM_SLAVE_INSTANCE(TIM_HANDLE.Instance)) {
                uint32_t tmpsmcr = TIM_HANDLE.Instance->SMCR & TIM_SMCR_SMS;
                if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr))
                    __HAL_TIM_ENABLE(&TIM_HANDLE);
            } else
                __HAL_TIM_ENABLE(&TIM_HANDLE);
            DMA_Send_Stat = HAL_OK;
        }
        frame_buffer_idx = 12;
        return;
    }
}

static void RGB_TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma) {

    TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *) ((DMA_HandleTypeDef *) hdma)->Parent;
    // if wrong handlers
    if (hdma != &DMA_HANDLE || htim != &TIM_HANDLE) return;
    if (frame_buffer_idx == 0) return; // if no data to transmit - return
    // if data transfer
    if (frame_buffer_idx < frame_buffer_len) {
        // fill first part of buffer
			

        for (volatile uint8_t i = 0; i < 8; i++) {
					
            dma_buffer[i] = (((frame_buffer_ptr[frame_buffer_idx + 0] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 8] = (((frame_buffer_ptr[frame_buffer_idx + 1] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 16] = (((frame_buffer_ptr[frame_buffer_idx + 2] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
					
            dma_buffer[i + 24] = (((frame_buffer_ptr[frame_buffer_idx + 3] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 32] = (((frame_buffer_ptr[frame_buffer_idx + 4] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 40] = (((frame_buffer_ptr[frame_buffer_idx + 5] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;		
					
        }
        frame_buffer_idx += 6;			// old set ++
    } else if (frame_buffer_idx < frame_buffer_len + 12) { // if RET transfer
        memset((uint8_t *) &dma_buffer[0], 0, (sizeof(dma_buffer) / 2)); // first part
        frame_buffer_idx += 6;			// old set ++
    }
}

static void RGB_TIM_DMADelayPulseCplt(DMA_HandleTypeDef *hdma) {
	
    TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *) ((DMA_HandleTypeDef *) hdma)->Parent;
    // if wrong handlers
    if (hdma != &DMA_HANDLE || htim != &TIM_HANDLE) return;
    if (frame_buffer_idx == 0) return; // if no data to transmit - return
	

    if (hdma == htim->hdma[TIM_DMA_ID_CC1]) {
        htim->Channel = HAL_TIM_ACTIVE_CHANNEL_1;
        if (hdma->Init.Mode == DMA_NORMAL) {
            TIM_CHANNEL_STATE_SET(htim, TIM_CHANNEL_1, HAL_TIM_CHANNEL_STATE_READY);
        }
    } else if (hdma == htim->hdma[TIM_DMA_ID_CC2]) {
        htim->Channel = HAL_TIM_ACTIVE_CHANNEL_2;
        if (hdma->Init.Mode == DMA_NORMAL) {
            TIM_CHANNEL_STATE_SET(htim, TIM_CHANNEL_2, HAL_TIM_CHANNEL_STATE_READY);
        }
    } else if (hdma == htim->hdma[TIM_DMA_ID_CC3]) {
        htim->Channel = HAL_TIM_ACTIVE_CHANNEL_3;
        if (hdma->Init.Mode == DMA_NORMAL) {
            TIM_CHANNEL_STATE_SET(htim, TIM_CHANNEL_3, HAL_TIM_CHANNEL_STATE_READY);
        }
    } else if (hdma == htim->hdma[TIM_DMA_ID_CC4]) {
        htim->Channel = HAL_TIM_ACTIVE_CHANNEL_4;
        if (hdma->Init.Mode == DMA_NORMAL) {
            TIM_CHANNEL_STATE_SET(htim, TIM_CHANNEL_4, HAL_TIM_CHANNEL_STATE_READY);
        }
    } else {
        //nothing to do 
    }


// if data transfer
    if (frame_buffer_idx < frame_buffer_len) {
        // fill second part of buffer
			

        for (volatile uint8_t i = 0; i < 8; i++) {
					
            dma_buffer[i + 48] = (((frame_buffer_ptr[frame_buffer_idx + 0] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 56] = (((frame_buffer_ptr[frame_buffer_idx + 1] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 64] = (((frame_buffer_ptr[frame_buffer_idx + 2] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
					
            dma_buffer[i + 72] = (((frame_buffer_ptr[frame_buffer_idx + 3] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 80] = (((frame_buffer_ptr[frame_buffer_idx + 4] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 88] = (((frame_buffer_ptr[frame_buffer_idx + 5] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
					
					
        }
        frame_buffer_idx += 6;				// old set ++
    } else if (frame_buffer_idx < frame_buffer_len + 12) { // if RET transfer
        memset((uint8_t *) &dma_buffer[sizeof(dma_buffer) / 2], 0, (sizeof(dma_buffer) / 2)); // second part
        frame_buffer_idx += 6;				// old set ++
    } else { // if END of transfer
			
        frame_buffer_idx = 0;
        // STOP DMA:


        __HAL_TIM_DISABLE_DMA(htim, ARGB_TIM_DMA_CC);
        (void) HAL_DMA_Abort_IT(htim->hdma[ARGB_TIM_DMA_ID]);


        if (IS_TIM_BREAK_INSTANCE(htim->Instance) != RESET) {
            /* Disable the Main Output */
            __HAL_TIM_MOE_DISABLE(htim);
        }
        /* Disable the Peripheral */
        __HAL_TIM_DISABLE(htim);
        /* Set the TIM channel state */
        TIM_CHANNEL_STATE_SET(htim, TIM_CH, HAL_TIM_CHANNEL_STATE_READY);
        //ARGB_LOC_ST = ARGB_READY;

		manager.frame_buffer.is_sending = false;
		manager.frame_buffer.is_rendered = false;
				
    }
    htim->Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
}












/*
	Конвертор индексов 2D кадрового буфера в вертикальный зиг-заг, сверху-вниз, слево-направо (светодиодне панели)
*/
uint16_t iterator1(uint16_t input, uint8_t width = 128, uint8_t height = 16)
{
	uint8_t row = input / width;
	uint8_t col = input % width;
	uint16_t index = col * height + (col % 2 == 0 ? row : (height - row - 1));
	
	return index;
}

/*
	Конвертор индексов 2D кадрового буфера в горизонтальный зиг-заг, слево-направо, сверху-вниз (светодиодне ленты)
*/
uint16_t iterator2(uint16_t input, uint8_t width = 128, uint8_t height = 16)
{
	uint8_t row = input / width;
	uint8_t col = input % width;
	uint16_t index = row * width + (row % 2 == 0 ? col : (width - col - 1));
	
	return index;
}





inline void Setup()
{
	srand( Analog::mux.Get(10) * 10 );

	manager.frame_buffer.Convertor = iterator1;
	manager.SelectEffect(effect_game);



		frame_buffer_ptr = manager.frame_buffer.raw;
		frame_buffer_len = sizeof(manager.frame_buffer.raw);


	
		SetupHW();


	
	return;
}

uint32_t timer1, timer2, timer3, timer12, timer23;

inline void Loop(uint32_t &current_time)
{

	static uint8_t idx = 0;
	static uint32_t tick = 0;
	if(current_time - tick > (30 * 1000))
	{
		tick = current_time;

		if(idx == 0)
		{
			//manager.SelectEffect(effect_fire);
			idx = 1;
		}
		
		else if(idx == 1)
		{
			//manager.SelectEffect(effect_sphere);
			idx = 0;
		}

		
	}




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
	if(manager.frame_buffer.is_sending == false && manager.frame_buffer.is_rendered == true)
	{
		manager.frame_buffer.is_sending = true;

		DMADraw();

		DEBUG_LOG_TOPIC("DMADraw", "time: %d\n", (HAL_GetTick() - lasttime));
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
		
		DMADraw();
		
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
