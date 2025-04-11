#pragma once

#include <inttypes.h>
#include <string.h>
#include <functional>
//#include "ReaderWrapper.h"
#include "PXLFormat_v2.h"
#include <PXLReaderInterface.h>

//template <uint8_t _width, uint8_t _height> 
class PXLParser
{
	static const uint8_t _file_version = 2;
	
	static constexpr uint8_t _file_header_size_bytes = sizeof(file_header_t);
	static constexpr uint8_t _frame_header_size_bytes = sizeof(file_frame_t);
	//static constexpr uint8_t _frame_pixel_size_bytes = sizeof(file_pixel_t);
	//static constexpr uint16_t _frame_chank_size_bytes = 504;
	//static constexpr uint8_t _frame_chank_size_pixels = 84;

	uint8_t _width;
	uint8_t _height;

	public:
		
		PXLParser(uint8_t width, uint8_t height) : _width(width), _height(height)
		{

		}










	//typedef std::function<void(file_pixel_t &pixel_data, uint8_t x, uint8_t y)> pixel_callback_t;
	typedef std::function<void(uint16_t index, uint8_t data[4])> pixel_callback_t;

	enum error_t : uint8_t
	{
		ERROR_OK,
		ERROR_INVALID_FILE,
		ERROR_VERSION,
		ERROR_SIZE,
		ERROR_FORMAT,
		ERROR_NOFRAMES,
		ERROR_OPEN_FILE,
		ERROR_READ_ZERO
	};



/*
		PXLParser(PXLReaderInterface &reader) : _reader(&reader)
		{}
*/

		PXLReaderInterface *_reader = nullptr;

		void SetReader(PXLReaderInterface &reader)
		{
			_reader = &reader;
		}

/*
		error_t Open(const char *filename)
		{
			_Init();
			
			if(_reader->Open(filename) != 0)
				_SetError(ERROR_OPEN_FILE);
			
			return _HeaderRead();
		}


		
		
		
		
		// Сброс всех настроек класса в начальное состояние для чтение нового файла
		void _Init()
		{
			return;
		}
		
		// Чтение файла
		uint16_t Read(const uint32_t offset, const uint16_t length)
		{
			return 0;
		}
		
		// Чтение и разбор заголовка файла
		error_t _HeaderRead()
		{
			if( Read(0, _file_header_size_bytes) == _file_header_size_bytes )
			{}

			//_HeaderParse
			return _error;
		}
*/






	error_t OpenFile(const char *filename)
	{
		ReInit();
		
		if( _reader->Open(filename) == 0 )
		{
			if( _reader->Read(_file_offset, _file_header_size_bytes) == _file_header_size_bytes )
			{
				_file_offset = _file_header_size_bytes;
				
				_ParseFileHeader( _reader->GetBufferPtr() );

				#warning Добавить проверку, что если файл не открыть то нельзя получать кадлы в GetAutoFrame
				// Есть запрос +PXLS=3,10,512 когда не получен ответ от +PXLS=3,0,10


			}
		}
		else
		{
			_SetError(ERROR_OPEN_FILE);
		}
		
		return _error;
	}
	
	void CloseFile()
	{
		_reader->Close();
		
		return;
	}
	
	/*

	*/
	bool GetAutoFrame(uint32_t current_time, pixel_callback_t callback)
	{
		if (_error != ERROR_OK)
			return false;

		// Если пришло время рисовать следующий кадр.
		if (_layer_params.has_animation && current_time - _frame_last_draw_time > _header_frame.frame_timeout)
		{
			_frame_last_draw_time = current_time;	// сохраняем время начала отрисовки кадра
			_file_offset = _file_offset_next_frame; // устанавливаем текущее смещение в файле на следующий кадр
			++_frame_current;						// увеличиваем номер кадра
		}
		// Если рано рисовать следующий кадр, то рисовать будем текущий кадр.
		else
		{
			_file_offset = _file_offset_current_frame; // текущее смещение в файле равно текущему кадру
													   // номер кадра не увеличиваем
		}

		// если текущий фрейм — это последний в анимации, то сохраняем на будущее его смещение в файле
		if (_header_file.img_frames == _frame_current)
		{
			_file_offset_last_frame = _file_offset;
		}

		// если достигли последнего кадра в анимации...
		if (_header_file.img_frames + 1 == _frame_current)
		{
			// если анимация должна быть проиграна конечное количество раз
			// и последняя итерация еще не достигнута, то...
			if (_layer_params.animation_finite_repeatable && _img_repeats_count < _header_file.img_repeats)
			{
				// увеличиваем счетчик повторов анимации
				++_img_repeats_count;
			}

			// если в анимации достигли заданного количества повторов,
			// то можно останавливаться
			if (_img_repeats_count == _header_file.img_repeats)
			{
				--_frame_current;						// уменьшаем счетчик текущего кадра, потому что до этого его зря увеличили, видимо....
				_file_offset = _file_offset_last_frame; // текущее смещение в файле устанавливаем на последний кадр
			}
			// если же повторяем дальше,
			// то стартовать надо с первого кадра
			else
			{
				_frame_current = 1;						// счетчик кадров на начало
				_file_offset = _file_header_size_bytes; // смещение в файле на позицию сразу после хедера
			}
		}

		// сохраняем смещение в файле как текущий кадр
		_file_offset_current_frame = _file_offset;

		
		
		
		
		
		
		
		
		


		//uint16_t frame_pixels;

		const uint8_t *buffer = nullptr;
		//file_frame_t *frame_header = nullptr;
		
		//uint16_t bytes_need = (_frame_chank_size_bytes + _frame_header_size_bytes);

		// читаем из файла данные, начиная со смещения _file_offset
		// количество данных не более _frame_chank_size_bytes + _frame_header_size_bytes
		uint16_t read = _reader->Read(_file_offset);

		// не прочитали, уходим
		if(read == 0)
		{
			_SetError(ERROR_READ_ZERO);
			//ReOpen();
			
			return false;
		}

		// если что-то прочитали, то получаем указатель на буфер
		buffer = _reader->GetBufferPtr();

// Заменить на копирование в новый объект.
		//frame_header = (file_frame_t *)buffer;
		//frame_pixels = frame_header->frame_pixels;
		//_frame_delay_time = frame_header->frame_timeout;

		memcpy(&_header_frame, buffer, _frame_header_size_bytes);
		//_frame_delay_time = _header_frame.frame_timeout;

		_file_offset += _frame_header_size_bytes;


		//-------------------------------------------------------------------------------------------------------------------------
		
		// Размер пикселя в байтах, Максимально возможный
		uint8_t _frame_pixel_size_bytes = _layer_params.pixel_type.size_pixel;
		
		// Размер пикселя в байтах, Упакованный
		uint8_t _frame_pixel_pack_size_bytes = _layer_params.pixel_type.size_pack;
		
		// Кол-во байт одного кадра, включая заголовок, Максимально возможное
		uint32_t frame_total_bytes = (_header_frame.frame_pixels * _frame_pixel_size_bytes) + _frame_header_size_bytes;
		
		// Кол-во прочитанных байт
		uint32_t frame_loaded_bytes_raw = read;
		
		// Кол-во обработанных байт
		uint32_t bytes_processed = _frame_header_size_bytes;
		
		// Кол-во байт кадра, которое осталось загрузить, Максимально возможное
		//uint32_t frame_bytes_left = (_header_frame.frame_pixels * _frame_pixel_size_bytes) - read;
		uint32_t frame_bytes_left = frame_total_bytes - frame_loaded_bytes_raw;




		//file_pixel_raw_t pixel_raw = {};

		//file_pixel_t pixel_data = {0};
		//file_pixel_t *pixel_data_pointer = (file_pixel_t *)&buffer[_frame_header_size_bytes];
		uint8_t *pixel_data_pointer = (uint8_t *)&buffer[_frame_header_size_bytes];



		uint8_t pack_alg = _layer_params.pixel_type.pack_alg;

		pxl_pixel_t curr_pixel;

		for(uint16_t pixel_idx = 0; pixel_idx < _header_frame.frame_pixels; ++pixel_idx)
		{
			if(bytes_processed + _frame_pixel_size_bytes > frame_loaded_bytes_raw)
			{
				read = _reader->Read(_file_offset, frame_bytes_left);
				if(read == 0)
				{
					_SetError(ERROR_READ_ZERO);
					//ReOpen();
					
					return false;
				}
				
				frame_loaded_bytes_raw = bytes_processed + read;
				
				frame_bytes_left = ((_header_frame.frame_pixels - pixel_idx)) * _frame_pixel_size_bytes;
				
				pixel_data_pointer = (uint8_t *)_reader->GetBufferPtr();
			}
			

			uint8_t add_offset = _layer_params.pixel_type.Parser( pixel_data_pointer, curr_pixel );

			

			callback(curr_pixel.index, curr_pixel.color);



			pixel_data_pointer += add_offset;
			_file_offset += add_offset;
			bytes_processed += add_offset;

		}

		// сохранение текущего смещения в файле как смещение следующего кадра
		_file_offset_next_frame = _file_offset;

		return true;
	}

	void ReInit()
	{
		//_file_offset = _file_header_size_bytes;
		_file_offset = 0;
		_frame_current = 0;
		_img_repeats_count = 0;

		_file_offset_current_frame = _file_header_size_bytes;
		_file_offset_next_frame = _file_header_size_bytes;
		_file_offset_last_frame = _file_header_size_bytes;

		_frame_last_draw_time = 0;
		//_frame_delay_time = 0;

		memset(&_header_frame, 0x00, _frame_header_size_bytes);

		return;
	}

private:
	void _ParseFileHeader(const uint8_t *data)
	{
		if (memcmp(data, "PXL", 3) != 0)
			return _SetError(ERROR_INVALID_FILE);

		memcpy(&_header_file, data, _file_header_size_bytes);
			
		_header_file.img_repeats = (_header_file.img_repeats == 0) ? 1 : _header_file.img_repeats;

		_AnalysisFile();
		
		return;
	}

	void _AnalysisFile()
	{
		if (_header_file.version != _file_version)
			return _SetError(ERROR_VERSION);

		if (_header_file.size_x != _width || _header_file.size_y != _height)
			return _SetError(ERROR_SIZE);

		//if (_header_file.format_strip != 2 || _header_file.format_color != 2)
		//	return _SetError(ERROR_FORMAT);

		if (_header_file.img_frames == 0)
			return _SetError(ERROR_NOFRAMES);

		_layer_params.pixel_type = file_pixel_type[ _header_file.format_color ];

		if (_layer_params.pixel_type.size_pixel == 0)
			return _SetError(ERROR_FORMAT);

		_layer_params.animation_finite_repeatable = _header_file.img_repeats > 0 && _header_file.img_repeats < 255;
		_layer_params.has_animation = _header_file.img_frames > 1 && _header_file.img_repeats != 0;
		
		
		
		return _SetError(ERROR_OK);
	}

	error_t _error;
	void _SetError(error_t error)
	{
		_error = error;
	}


	uint32_t _file_offset;				 // Смещение чтения файла, байт.
	uint32_t _file_offset_last_frame;	 // Смещение от начала файла до последнего кадра (для отрисовки последнего кадра в не бесконечной анимации).
	uint32_t _file_offset_current_frame; // Смещение текущего кадра в файле
	uint32_t _file_offset_next_frame;	 // Смещение следующего кадра в файле

	uint16_t _frame_current;

	uint16_t _img_repeats_count;

	uint32_t _frame_last_draw_time; // Время рисование последнего кадра.
	//uint16_t _frame_delay_time;		// задержка между кадрами анимации

	file_header_t _header_file;

	file_frame_t _header_frame = {};

	struct /*__attribute__((__packed__))*/ layer_params_t
	{
		bool animation_finite_repeatable;
		bool has_animation;

		file_pixel_type_t pixel_type;
	} _layer_params;
};