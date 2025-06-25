#pragma once
#include <inttypes.h>
#include "stm32f1xx_hal.h"

DMA_Channel_TypeDef *DMA_Channel = DMA1_Channel7;

void memset_dma8(uint8_t *dst, uint8_t c, size_t size)
{
	// Включаем тактирование DMA1
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	
	// Сбрасываем настройки DMA
	DMA1->IFCR = DMA_IFCR_CGIF7;
	DMA_Channel->CCR = 0;
	
	// Устанавливаем адреса источника и приемника
	static uint8_t value;
	value = c;
	DMA_Channel->CPAR = (uint32_t)&value;	// Источник
	DMA_Channel->CMAR = (uint32_t)dst;		// Приемник
	DMA_Channel->CNDTR = size;				// Количество пересылок в байтах (8 бит)
	
	// Конфигурация DMA: Memory-to-Memory, 8 бита, инкремент адресов
	DMA_Channel->CCR = DMA_CCR_MINC			// Инкремент адреса памяти
						| DMA_CCR_MEM2MEM	// Memory-to-Memory режим
						| DMA_CCR_MSIZE_0	// Размер данных 8 бит
						| DMA_CCR_PSIZE_0	// Размер периферийных данных 8 бит
						| DMA_CCR_PL_1;		// Высокий приоритет
	
	// Запуск DMA
	DMA_Channel->CCR |= DMA_CCR_EN;
	
	// Ожидаем завершения передачи
	while(DMA_Channel->CNDTR > 0);
	
	// Останавливаем DMA
	DMA_Channel->CCR &= ~DMA_CCR_EN;
	
	return;
}

void memset_dma32(uint8_t *dst, uint32_t c, size_t size)
{
	if(size % 4 != 0)
		size += 4 - (size % 4);
	
	// Включаем тактирование DMA1
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	
	// Сбрасываем настройки DMA
	DMA1->IFCR = DMA_IFCR_CGIF7;
	DMA_Channel->CCR = 0;
	
	// Устанавливаем адреса источника и приемника
	static uint32_t value;
	value = c;
	DMA_Channel->CPAR = (uint32_t)&value;	// Источник
	DMA_Channel->CMAR = (uint32_t)dst;		// Приемник
	DMA_Channel->CNDTR = size / 4;			// Количество пересылок в словах (32 бита)
	
	// Конфигурация DMA: Memory-to-Memory, 32 бита, инкремент адресов
	DMA_Channel->CCR = DMA_CCR_MINC			// Инкремент адреса памяти
						| DMA_CCR_MEM2MEM	// Memory-to-Memory режим
						| DMA_CCR_MSIZE_1	// Размер данных 32 бита
						| DMA_CCR_PSIZE_1	// Размер периферийных данных 32 бита
						| DMA_CCR_PL_1;		// Высокий приоритет
	
	// Запуск DMA
	DMA_Channel->CCR |= DMA_CCR_EN;
	
	// Ожидаем завершения передачи
	while(DMA_Channel->CNDTR > 0);
	
	// Останавливаем DMA
	DMA_Channel->CCR &= ~DMA_CCR_EN;
	
	return;
}

void memcpy_dma8(uint8_t *dst, const uint8_t *src, size_t size)
{
	// Включаем тактирование DMA1
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	
	// Сбрасываем настройки DMA
	DMA1->IFCR = DMA_IFCR_CGIF7;
	DMA_Channel->CCR = 0;
	
	// Устанавливаем адреса источника и приемника
	DMA_Channel->CPAR = (uint32_t)src;		// Источник
	DMA_Channel->CMAR = (uint32_t)dst;		// Приемник
	DMA_Channel->CNDTR = size;				// Количество пересылок в байтах (8 бит)
	
	// Конфигурация DMA: Memory-to-Memory, 8 бит, инкремент адресов
	DMA_Channel->CCR = DMA_CCR_MINC			// Инкремент адреса памяти
						| DMA_CCR_PINC		// Инкремент адреса источника
						| DMA_CCR_MEM2MEM	// Memory-to-Memory режим
						| DMA_CCR_MSIZE_0	// Размер данных 8 бит
						| DMA_CCR_PSIZE_0	// Размер периферийных данных 8 бит
						| DMA_CCR_PL_1;		// Высокий приоритет
	
	// Запуск DMA
	DMA_Channel->CCR |= DMA_CCR_EN;
	
	// Ожидаем завершения передачи
	while(DMA_Channel->CNDTR > 0);
	
	// Останавливаем DMA
	DMA_Channel->CCR &= ~DMA_CCR_EN;
	
	return;
}

void memcpy_dma32(uint8_t *dst, const uint8_t *src, size_t size)
{
	if(size % 4 != 0)
		size += 4 - (size % 4);
	
	// Включаем тактирование DMA1
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	
	// Сбрасываем настройки DMA
	DMA1->IFCR = DMA_IFCR_CGIF7;
	DMA_Channel->CCR = 0;
	
	// Устанавливаем адреса источника и приемника
	DMA_Channel->CPAR = (uint32_t)src;		// Источник
	DMA_Channel->CMAR = (uint32_t)dst;		// Приемник
	DMA_Channel->CNDTR = size / 4;			// Количество пересылок в словах (32 бита)
	
	// Конфигурация DMA: Memory-to-Memory, 32 бита, инкремент адресов
	DMA_Channel->CCR = DMA_CCR_MINC			// Инкремент адреса памяти
						| DMA_CCR_PINC		// Инкремент адреса источника
						| DMA_CCR_MEM2MEM	// Memory-to-Memory режим
						| DMA_CCR_MSIZE_1	// Размер данных 32 бита
						| DMA_CCR_PSIZE_1	// Размер периферийных данных 32 бита
						| DMA_CCR_PL_1;		// Высокий приоритет
	
	// Запуск DMA
	DMA_Channel->CCR |= DMA_CCR_EN;
	
	// Ожидаем завершения передачи
	while(DMA_Channel->CNDTR > 0);
	
	// Останавливаем DMA
	DMA_Channel->CCR &= ~DMA_CCR_EN;
	
	return;
}

void memcpy_repeat_fast(uint8_t *dest, const uint8_t *src, size_t elem_size, size_t count)
{
	memcpy(dest, src, elem_size);

	size_t total_copied = 1;
	while(total_copied < count)
	{
		size_t copy_now = (count - total_copied) < total_copied ? (count - total_copied) : total_copied;
		memcpy(dest + total_copied * elem_size, dest, copy_now * elem_size);
		total_copied += copy_now;
	}

	return;
}
