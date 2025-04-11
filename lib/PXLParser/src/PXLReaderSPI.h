#pragma once
#include <inttypes.h>
#include "PXLReaderInterface.h"

class PXLReaderSPI : public PXLReaderInterface
{
	public:

		struct file_map_t
		{
			const char *filename;
			uint32_t page_start;
			uint32_t length;
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
			if (_active_file == nullptr || offset >= _active_file->length) 
				return 0;
			
			if(length > sizeof(_buffer))
				length = sizeof(_buffer);
		
			// Вычисляем доступное количество байт для чтения
			uint16_t readable_length = _active_file->length - offset;
			uint16_t bytes_to_read = (length > readable_length) ? readable_length : length;

			SPI::flash.ReadBytes( (_active_file->page_start + offset), bytes_to_read, _buffer );
		
			return bytes_to_read;
		}
		

		virtual const uint8_t *GetBufferPtr() override
		{
			return _buffer;
		}

	private:
		
		file_map_t _map[6];
		uint8_t _map_idx = 0;

		file_map_t *_active_file = nullptr;
		const uint8_t *_active_file_ptr;

		uint8_t _buffer[174];
};
