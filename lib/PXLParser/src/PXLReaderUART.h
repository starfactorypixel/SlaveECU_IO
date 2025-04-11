#pragma once
#include <inttypes.h>
#include "PXLReaderInterface.h"
#include <stm32f1xx_hal_uart.h>

extern UART_HandleTypeDef hDebugUart;

class PXLReaderUART : public PXLReaderInterface
{
	static constexpr uint16_t _max_data_request = 512;
	
	typedef struct
	{
		char id;
		uint32_t offset;
		uint32_t length;
		const uint8_t *data;
	} PXLS_Packet;
	
	public:
		
		virtual int8_t Open(const char *filename) override
		{
			_SendRequest('1', 0, 0);
			//uint16_t length = generate_pxls_packet('1', 0, 0, _buffer_tx);
			//_HW_Print(_buffer_tx, length);

			//uint16_t rx_len;
			//HAL_UARTEx_ReceiveToIdle(&hDebugUart, _buffer_rx, sizeof(_buffer_rx), &rx_len, 64);
			bool status = _WaitResponse('2');
			if(status == true)
			{
				return 0;
			}
			
			return -1;
		}
		
		virtual int8_t Close() override
		{
			_SendRequest('5', 0, 0);
			//uint16_t length = generate_pxls_packet('5', 0, 0, _buffer_tx);
			//_HW_Print(_buffer_tx, length);

			//uint16_t rx_len;
			//HAL_UARTEx_ReceiveToIdle(&hDebugUart, _buffer_rx, sizeof(_buffer_rx), &rx_len, 64);
			bool status = _WaitResponse('6');
			if(status == true)
			{
				return 0;
			}
			
			return -1;
		}

		uint32_t time[3];
		
		virtual uint16_t Read(const uint32_t offset, uint16_t length) override
		{
			if(length > _max_data_request)
				length = _max_data_request;

			time[0] = HAL_GetTick();
			
			_SendRequest('3', offset, length);
			//uint16_t _buffer_tx_length = generate_pxls_packet('3', offset, length, _buffer_tx);
			//_HW_Print(_buffer_tx, _buffer_tx_length);

			time[1] = HAL_GetTick();

			//uint16_t rx_len = 0;
			//PXLS_Packet data = {};
			//volatile HAL_StatusTypeDef wqe = HAL_UARTEx_ReceiveToIdle(&hDebugUart, _buffer_rx, sizeof(_buffer_rx), &rx_len, 64);
			//bool status = parse_pxls_packet(_buffer_rx, rx_len, &data);
			bool status = _WaitResponse('4');
			if(status == true)
			{
				time[2] = HAL_GetTick();
				DEBUG_LOG_TOPIC("UARTTX", "time: %d\n", (time[1] - time[0]));
				DEBUG_LOG_TOPIC("UARTRX", "time: %d\n", (time[2] - time[1]));
				
				_buffer_rx_data_ptr = &_parsed_data.data[0];
					
				return _parsed_data.length;
			}
			
			return 0;
		}
		
		virtual const uint8_t *GetBufferPtr() override
		{
			return _buffer_rx_data_ptr;
		}

	private:



		void _HW_ReInit()
		{
			HAL_UART_DeInit(&hDebugUart);
			hDebugUart.Init.BaudRate = 1000000UL;
			if(HAL_UART_Init(&hDebugUart) != HAL_OK)
			{
				//Error_Handler();
			}

			return;
		}
		
		bool _HW_Print(const void *pData, uint16_t Size)
		{
			HAL_StatusTypeDef result = HAL_UART_Transmit(&hDebugUart, (uint8_t *)pData, Size, 64);
			if(result != HAL_OK)
			{
				HAL_UART_AbortTransmit(&hDebugUart);
			}
			
			return (result == HAL_OK);
		}


		void _SendRequest(char id, uint32_t offset, uint32_t length)
		{
			uint16_t buffer_tx_length = generate_pxls_packet(id, offset, length, _buffer_tx);
			
			_HW_Print(_buffer_tx, buffer_tx_length);
		}

		bool _WaitResponse(char id)
		{
			uint16_t rx_len;
			HAL_StatusTypeDef result = HAL_UARTEx_ReceiveToIdle(&hDebugUart, _buffer_rx, sizeof(_buffer_rx), &rx_len, 64);
			if(result == HAL_OK)
			{
				bool status = parse_pxls_packet(_buffer_rx, rx_len, &_parsed_data);
				if(status == true && _parsed_data.id == id)
				{
					return true;
				}
			}

			return false;
		}

// Функция для конвертации uint32_t в ASCII без sprintf
uint8_t *u32_to_ascii(uint32_t value, uint8_t *buf) {
    char tmp[11]; // макс 10 цифр + \0
    int i = 0;

    if (value == 0) {
        *buf++ = '0';
        *buf = '\0';
        return buf;
    }

    while (value) {
        tmp[i++] = '0' + (value % 10);
        value /= 10;
    }

    // обратная запись
    for (int j = i - 1; j >= 0; j--) {
        *buf++ = tmp[j];
    }
    *buf = '\0';
    return buf;
}

// Простой парсер uint32 из ASCII. Возвращает новое положение указателя.
uint8_t *ascii_to_u32(uint8_t *str, uint32_t* out) {
    uint32_t result = 0;
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    *out = result;
    return str;
}

		uint16_t generate_pxls_packet(char id, uint32_t offset, uint32_t length, uint8_t *out_buf)
		{
			uint8_t *ptr = out_buf;
			
			*ptr++ = '+';
			memcpy(ptr, "PXLS=", 5);
			ptr += 5;
			
			*ptr++ = id;
			*ptr++ = ',';
			
			ptr = u32_to_ascii(offset, ptr);
			*ptr++ = ',';
			
			ptr = u32_to_ascii(length, ptr);
			*ptr++ = '\n';
			*ptr++ = '\n';
			*ptr = '\0';
			
			return (uint16_t)(ptr - out_buf);
		}




		bool parse_pxls_packet(uint8_t *input, uint16_t len, PXLS_Packet *out)
		{
			if(len < 10 || input[0] != '+' || strncmp((const char *)input, "+PXLS=", 6) != 0)
				return false;
			
			uint8_t *ptr = input + 6;
			
			out->id = *ptr++;
			if(*ptr++ != ',')
				return false;
			
			ptr = ascii_to_u32(ptr, &out->offset);
			if(*ptr++ != ',')
				return false;
			
			ptr = ascii_to_u32(ptr, &out->length);
			if(*ptr++ != '\n')
				return false;
			
			// Проверка длины и извлечение data
			uint32_t expected_len = out->length;
			uint16_t remaining = len - (ptr - input);
			if(remaining < expected_len + 1) // +1 — это завершающий \n
				return false;
			
			out->data = (const uint8_t*)ptr;
			if(ptr[expected_len] != '\n')
				return false;
			
			return true;
		}








		
		uint8_t _buffer_rx[_max_data_request + 64];
		uint8_t _buffer_tx[64];

		const uint8_t *_buffer_rx_data_ptr;

		PXLS_Packet _parsed_data = {};
};
