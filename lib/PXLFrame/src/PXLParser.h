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
	
	typedef std::function<void(const pxl_pixel_t &pxl_pixel)> pixel_callback_t;

	public:
		
		PXLParser(uint8_t width, uint8_t height) : _cfg{width, height, false, false}
		{

		}











	

	enum error_t : int8_t
	{
		ERROR_OK = 0,
		ERROR_INVALID_FILE = -1,
		ERROR_VERSION = -2,
		ERROR_SIZE = -3,
		ERROR_FORMAT = -4,
		ERROR_NOFRAMES = -5,
		ERROR_OPEN_FILE = -6,
		ERROR_READ1_ZERO = 1,
		ERROR_READ2_ZERO = 2,
		ERROR_IDX_OVERFLOW = 3,
	};



/*
		PXLParser(PXLReaderInterface &reader) : _reader(&reader)
		{}
*/

		

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


	/*
		В случае ошибки в процессе чтения файла, если true то файл будет переоткрыт и начнёться с начала.
		Если false, то фалй будет просто закрыт
	*/
	void SetAutoReopen(bool state)
	{
		_cfg.reopen = state;
		
		return;
	}
	
	void SetIgnoreFrameTimeout(bool state)
	{
		_cfg.ignore_frame_timeout = state;
		
		return;
	}
	

	error_t OpenFile(const char *filename)
	{
		_file.filename = filename;
		
		ReInit();
		if(_reader->Open(filename) == 0)
		{
			if(_reader->Read(_file.offset, _file_header_size_bytes) == _file_header_size_bytes)
			{
				_file.offset = _file_header_size_bytes;
				_file.buffer_ptr = _reader->GetBufferPtr();
				_file.isopen = true;
				
				_ParseFile();
			}
		}
		else
		{
			_SetError(ERROR_OPEN_FILE);
		}
		
		return _error;
	}

	error_t ReOpenFile()
	{
		CloseFile();
		OpenFile(_file.filename);

		return _error;
	}
	
		void CloseFile()
		{
			_reader->Close();
			_file.isopen = false;
			
			return;
		}



		void Tick(uint32_t time)
		{
			if(_file.isopen == false)
			{
				if(_cfg.reopen == true)
				{
					ReOpenFile();
				}
			}
			
			return;
		}
	
	/*

	*/
	bool GetAutoFrame(uint32_t current_time, pixel_callback_t callback)
	{
		if(_file.isopen == false)
			return false;
		
		if(_error < ERROR_OK)
			return false;

		// Если пришло время рисовать следующий кадр.
		if (_pxl.has_animation && (_cfg.ignore_frame_timeout || current_time - _frame_last_draw_time > _pxl.header_frame.frame_timeout))
		{
			_frame_last_draw_time = current_time;	// сохраняем время начала отрисовки кадра
			_file.offset = _file.offset_next_frame; // устанавливаем текущее смещение в файле на следующий кадр
			++_pxl.frame_current;						// увеличиваем номер кадра
		}
		// Если рано рисовать следующий кадр, то рисовать будем текущий кадр.
		else
		{
			_file.offset = _file.offset_current_frame; // текущее смещение в файле равно текущему кадру
													   // номер кадра не увеличиваем
		}

		// если текущий фрейм — это последний в анимации, то сохраняем на будущее его смещение в файле
		if (_pxl.header_file.img_frames == _pxl.frame_current)
		{
			_file.offset_last_frame = _file.offset;
		}

		// если достигли последнего кадра в анимации...
		if (_pxl.header_file.img_frames + 1 == _pxl.frame_current)
		{
			// если анимация должна быть проиграна конечное количество раз
			// и последняя итерация еще не достигнута, то...
			if (_pxl.animation_finite_repeatable && _pxl.img_repeats_count < _pxl.header_file.img_repeats)
			{
				// увеличиваем счетчик повторов анимации
				++_pxl.img_repeats_count;
			}

			// если в анимации достигли заданного количества повторов,
			// то можно останавливаться
			if (_pxl.img_repeats_count == _pxl.header_file.img_repeats)
			{
				--_pxl.frame_current;						// уменьшаем счетчик текущего кадра, потому что до этого его зря увеличили, видимо....
				_file.offset = _file.offset_last_frame; // текущее смещение в файле устанавливаем на последний кадр
			}
			// если же повторяем дальше,
			// то стартовать надо с первого кадра
			else
			{
				_pxl.frame_current = 1;						// счетчик кадров на начало
				_file.offset = _file_header_size_bytes; // смещение в файле на позицию сразу после хедера
			}
		}

		// сохраняем смещение в файле как текущий кадр
		_file.offset_current_frame = _file.offset;

		
		
		
		
		
		
		
		
		


		//uint16_t frame_pixels;

		//const uint8_t *buffer = nullptr;
		//file_frame_t *frame_header = nullptr;
		
		//uint16_t bytes_need = (_frame_chank_size_bytes + _frame_header_size_bytes);

		// читаем из файла данные, начиная со смещения _file_offset
		// количество данных не более _frame_chank_size_bytes + _frame_header_size_bytes
		uint16_t read = _reader->Read(_file.offset);

		// не прочитали, уходим
		if(read == 0)
		{
			_ParsingFailed(ERROR_READ1_ZERO);
			return false;
		}

		// если что-то прочитали, то получаем указатель на буфер
		_file.buffer_ptr = _reader->GetBufferPtr();


		memcpy(&_pxl.header_frame, _file.buffer_ptr, _frame_header_size_bytes);

		_file.offset += _frame_header_size_bytes;


		//-------------------------------------------------------------------------------------------------------------------------
		
		// Размер пикселя в байтах, Максимально возможный
		uint8_t _frame_pixel_size_bytes = _pxl.pixel_type.size_pixel;
		
		// Размер пикселя в байтах, Упакованный
		uint8_t _frame_pixel_pack_size_bytes = _pxl.pixel_type.size_pack;
		
		// Кол-во байт одного кадра, включая заголовок, Максимально возможное
		uint32_t frame_total_bytes = (_pxl.header_frame.frame_pixels * _frame_pixel_size_bytes) + _frame_header_size_bytes;
		
		// Кол-во прочитанных байт
		uint32_t frame_loaded_bytes_raw = read;
		
		// Кол-во обработанных байт
		uint32_t bytes_processed = _frame_header_size_bytes;
		
		// Кол-во байт, нуждающиеся в дозагрузке
		uint32_t frame_bytes_offset = 0;




		//file_pixel_raw_t pixel_raw = {};

		//file_pixel_t pixel_data = {0};
		//file_pixel_t *pixel_data_pointer = (file_pixel_t *)&buffer[_frame_header_size_bytes];
		//uint8_t *pixel_data_pointer = (uint8_t *)&_file.buffer_ptr[_frame_header_size_bytes];
		_file.buffer_ptr += _frame_header_size_bytes;
		
		
		// Инициализируем index с значением 0xFFFF, для того, чтобы когда появляется первый пиксель без индекса, там мы делаем index++ и попадаем в ячейку 0.
		// Во всех остальных случаях формат гарантирует что первый пикель содержит индекс и значение index будет переписано
		pxl_pixel_t curr_pixel = {};
		curr_pixel.index = 0xFFFF;
		curr_pixel.repeat = 1;
		
		for(uint16_t pixel_idx = 0; pixel_idx < _pxl.header_frame.frame_pixels; ++pixel_idx)
		{
			if(bytes_processed + _frame_pixel_size_bytes > frame_loaded_bytes_raw)
			{
				frame_bytes_offset = ((_pxl.header_frame.frame_pixels - pixel_idx)) * _frame_pixel_size_bytes;
				
				read = _reader->Read(_file.offset, frame_bytes_offset);
				if(read == 0)
				{
					_ParsingFailed(ERROR_READ2_ZERO);
					return false;
				}
				
				frame_loaded_bytes_raw = bytes_processed + read;
				
				_file.buffer_ptr = _reader->GetBufferPtr();
			}
			
			
			uint8_t add_offset = _pxl.pixel_type.Parser( _file.buffer_ptr, curr_pixel );

			if(curr_pixel.index >= (_cfg.width * _cfg.height))
			{
				_ParsingFailed(ERROR_IDX_OVERFLOW);
				return false;
			}


			#warning Пересмотреть концепцию повторяющийхся пикелей. Что есть repeat - кол-во НОВЫХ повторов, или включая текущий.
			callback(curr_pixel);
			if(curr_pixel.repeat > 1)
			{
				curr_pixel.repeat = 1;
				curr_pixel.index += curr_pixel.repeat - 1;
			}
			
			_file.buffer_ptr += add_offset;
			_file.offset += add_offset;
			bytes_processed += add_offset;

		}

		// сохранение текущего смещения в файле как смещение следующего кадра
		_file.offset_next_frame = _file.offset;

		return true;
	}

	void ReInit()
	{
		//_file_offset = _file_header_size_bytes;
		_file.offset = 0;
		_pxl.frame_current = 0;
		_pxl.img_repeats_count = 0;

		_file.offset_current_frame = _file_header_size_bytes;
		_file.offset_next_frame = _file_header_size_bytes;
		_file.offset_last_frame = _file_header_size_bytes;

		_frame_last_draw_time = 0;

		memset(&_pxl.header_frame, 0x00, _frame_header_size_bytes);

		return;
	}

	private:
		
		void _ParseFile()
		{
			if( memcmp(_file.buffer_ptr, "PXL", 3) != 0 )
				return _SetError(ERROR_INVALID_FILE);
			
			memcpy(&_pxl.header_file, _file.buffer_ptr, _file_header_size_bytes);
			
			_AnalysisFile();
			
			return;
		}
		
		void _AnalysisFile()
		{
			if(_pxl.header_file.version != _file_version)
				return _SetError(ERROR_VERSION);
			
			if(_pxl.header_file.size_x != _cfg.width || _pxl.header_file.size_y != _cfg.height)
				return _SetError(ERROR_SIZE);
			
			_pxl.pixel_type = file_pixel_type[ _pxl.header_file.format_color ];
			if(_pxl.header_file.format_strip != 0 || _pxl.pixel_type.size_pixel == 0)
				return _SetError(ERROR_FORMAT);
			
			if(_pxl.header_file.img_frames == 0)
				return _SetError(ERROR_NOFRAMES);
			
			_pxl.header_file.img_repeats = ((_pxl.header_file.img_repeats == 0) ? 1 : _pxl.header_file.img_repeats);
			
			_pxl.animation_finite_repeatable = (_pxl.header_file.img_repeats > 0 && _pxl.header_file.img_repeats < 255);
			_pxl.has_animation = (_pxl.header_file.img_frames > 1 && _pxl.header_file.img_repeats != 0);
			
			return _SetError(ERROR_OK);
		}
		
		error_t _error;
		void _SetError(error_t error)
		{
			_error = error;

			return;
		}


		void _ParsingFailed(error_t error)
		{
			_error = error;

			DEBUG_LOG_TOPIC("Failed", "code: %d\n", error);

			if(error == ERROR_READ1_ZERO && ++_file.read_attempts == 5)
			{
				_file.read_attempts = 0;

				CloseFile();
			}
			
			//CloseFile();
/*			if(_cfg.reopen == true)
			{
				ReOpenFile();
			}
			else
			{
				CloseFile();
			}
*/
			return;
		}


		struct pxl_t
		{
			file_header_t header_file;				// Заголовок файла, структура
			file_frame_t header_frame;				// Заголовок кадра, структура
			file_pixel_type_t pixel_type;			// Конфигурация пикселя + Парсер, структура
			uint16_t frame_current;					// Номер текущего кадра
			uint16_t img_repeats_count;				// Кол-во повторов анимации

			bool has_animation;						// Флаг, что файл является анимацией
			bool animation_finite_repeatable;		// Флаг, что анимация повторяется не бесконечно
		} _pxl;

		struct file_t
		{
			uint32_t offset;						// Смещение чтения файла, байт
			uint32_t offset_last_frame;				// Смещение до последнего кадра, байт
			uint32_t offset_current_frame;			// Смещение до текущего кадра, байт
			uint32_t offset_next_frame;				// Смещение до следующего кадра, байт

			const uint8_t *buffer_ptr = nullptr;	// Указатель на буфер

			const char *filename = nullptr;			// Имя файла
			bool isopen = false;					// Флаг открытого файла
			uint8_t read_attempts = 0;				// Кол-во попыток чтения файла, после чего он закрывается
		} _file;

		struct cfg_t
		{
			const uint8_t width;					// Ширина экрана
			const uint8_t height;					// Высота экрана
			bool reopen;							// Флаг автоматического переоткрытия файла в случае ошибок чтения
			bool ignore_frame_timeout;				// Флаг игнорирования frame_timeout в PXL файле. Каждый вызов GetAutoFrame будет возвращать следующий кадр.
		} _cfg;
		
		PXLReaderInterface *_reader = nullptr;		// Указатель на Reader
		uint32_t _frame_last_draw_time;				// Время рисование последнего кадра
		
};
