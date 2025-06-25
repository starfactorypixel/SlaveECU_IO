#pragma once
#include <inttypes.h>
#include "PXLReaderInterface.h"

class PXLReaderArray : public PXLReaderInterface
{
	static constexpr uint8_t _map_size = 10;
	
	public:
		
		struct file_map_t
		{
			const char *filename;
			const uint8_t *array_ptr;
			uint32_t array_length;
		};
		
		void PutFileMap(file_map_t map)
		{
			if(_map_idx == sizeofarray(_map)) return;

			_map[_map_idx++] = map;

			return;
		}
		
		
		virtual int8_t Open(const char *filename) override
		{
			for(file_map_t &file : _map)
			{
				if( strcmp(file.filename, filename) == 0)
				{
					_active_file = &file;

					break;
				}
			}

			return 0;
		}
		
		virtual int8_t Close() override
		{
			_active_file = nullptr;

			return 0;
		}
		
		virtual uint16_t Read(const uint32_t offset, uint16_t length) override
		{
			if (_active_file == nullptr || offset >= _active_file->array_length) 
				return 0;
		
			// Вычисляем доступное количество байт для чтения
			uint16_t readable_length = _active_file->array_length - offset;
			uint16_t bytes_to_read = (length > readable_length) ? readable_length : length;
		
			// Устанавливаем указатель на начало чтения
			_active_file_ptr = &_active_file->array_ptr[offset];
		
			return bytes_to_read;
		}
		
		virtual const uint8_t *GetBufferPtr() override
		{
			return _active_file_ptr;
		}

	private:
		
		file_map_t _map[_map_size];
		uint8_t _map_idx = 0;

		file_map_t *_active_file = nullptr;
		const uint8_t *_active_file_ptr;
};
