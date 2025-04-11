#include <inttypes.h>

// Типы и порядок ленты.
// Заполнить согласно актуальным типам !!!
enum format_strip_t : uint8_t
{
	STRIP_LINE = 0x00,
	STRIP_X01 = 0x01,
	STRIP_ZIGZAG = 0x02,
	STRIP_X03 = 0x03,
	STRIP_X04 = 0x04,
	STRIP_X05 = 0x05,
	STRIP_X06 = 0x06,
	STRIP_X07 = 0x07,
	STRIP_X08 = 0x08,
	STRIP_X09 = 0x09,
	STRIP_X0A = 0x0A,
	STRIP_X0B = 0x0B,
	STRIP_X0C = 0x0C,
	STRIP_X0D = 0x0D,
	STRIP_X0E = 0x0E,
	STRIP_X0F = 0x0F
};

// Типы и порядок цветов.
// Заполнить согласно актуальным типам !!!
enum format_color_t : uint8_t
{
	COLOR_RGB  = 0x00,
	COLOR_RGBA = 0x01,
	COLOR_GRBA = 0x02,
	COLOR_X03  = 0x03,
	COLOR_X04  = 0x04,
	COLOR_X05  = 0x05,
	COLOR_X06  = 0x06,
	COLOR_X07  = 0x07,
	COLOR_X08  = 0x08,
	COLOR_X09  = 0x09,
	COLOR_X0A  = 0x0A,
	COLOR_X0B  = 0x0B,
	COLOR_X0C  = 0x0C,
	COLOR_X0D  = 0x0D,
	COLOR_X0E  = 0x0E,
	COLOR_X0F  = 0x0F
};

// Структура описывает заголовок файла.
struct __attribute__((packed)) file_header_t
{
	// 3 байта, - Заголовка формата, символы таблицы ASCII. Например: 'PXL'.
	char type[3];
	
	// 1 байт, - версия формата файла.
	uint8_t version;
	
	// 1 байт, - Полный размер изображения в пикселях по оси X.
	uint8_t size_x;
	
	// 1 байт, - Полный размер изображения в пикселях по оси Y.
	uint8_t size_y;
	
	// 1 байт, - Формат данных: Порядок цветов и способ подключения ленты.
	struct __attribute__((packed))
	{
		// Младшие 4 бита, - Формат порядка и типа ленты.
		uint8_t format_strip : 4;
		
		// Старшие 4 бита, - Формат порядка и типа цветов.
		uint8_t format_color : 4;
	};
	
	// 2 байта, - Число кадров анимации, от 1 до 65535. ( по сути кол-во структур file_frame_t )
	uint16_t img_frames;
	
	// 1 байт, - Кол-во повторов анимации, от 0 до 255. 0 - статичная картинка, 255 - бесконечно.
	uint8_t img_repeats;
	
	// Далее идут кадры, описанные структурой file_frame_t.
};

// Структура заголовка кадра.
struct __attribute__((packed)) file_frame_t
{
	// 2 байта, - Время, в мс, отображения текущего кадра, от 0 до 65535.
	uint16_t frame_timeout;
	
	// 2 байта, - Число пикселей в текущем кадре. ( по сути кол-во структур file_pixel_t )
	uint16_t frame_pixels;
	
	// Далее идут пиксели, описанные структурой file_pixel_t.
};

// Структура данных каждого пикселя.
struct __attribute__((packed)) file_pixel_t
{
	// Абсолютный индекс пикселя, вычисляемый по формуле index = x + (size_x * y)
	uint16_t index;

	// 4 х 1 байт, - Компоненты цвета согласно переданному формату.
	uint8_t color1;
	uint8_t color2;
	uint8_t color3;
	uint8_t color4;
	
	// Кол-во пикселей - повторов для вставки. Следующие пиксели вставляются, инкрементируя index
	uint8_t count;
};


struct __attribute__((packed)) file_pixel_raw_t
{
	// Абсолютный индекс пикселя, вычисляемый по формуле index = x + (size_x * y)
	uint16_t index;

	// Другие байты пикселя, согластно формата из файла.
	uint8_t value[5];
};


//------------------------------



struct pxl_pixel_t
{
	uint16_t index;
	uint8_t color[4];
};

using color_parser_t = uint8_t (*)(const uint8_t *input, pxl_pixel_t &curr);

struct file_pixel_type_t
{
	uint8_t size_pixel;			// Общее кол-во байт в пикселе (максимальное)
	uint8_t size_pack;			// Общее кол-во байт в пикселе (упакованное)
	uint8_t pack_alg;			// Алгоритм упаковки
	color_parser_t Parser;		// Парсер цвета
};

inline uint8_t parser_dummy(const uint8_t *input, pxl_pixel_t &curr)
{
	return 0;
}

inline uint8_t parser_dummy_3(const uint8_t *input, pxl_pixel_t &curr)
{
	curr.index = (input[0] << 8) | input[1];
	
	if(curr.index & 0x8000)
	{
		curr.index &= 0x7FFF;
		curr.color[0] += (int8_t)(input[2] >> 4) - 7;
		curr.color[1] += (int8_t)(input[2] & 0x0F) - 7;
		curr.color[2] += (int8_t)(input[3] >> 4) - 7;
		curr.color[3] += 0;
		
		return 4;
	}
	else
	{
		curr.color[0] = input[2];
		curr.color[1] = input[3];
		curr.color[2] = input[4];
		curr.color[3] = 255;
		
		return 5;
	}
}

inline uint8_t parser_dummy_4(const uint8_t *input, pxl_pixel_t &curr)
{
	curr.index = (input[0] << 8) | input[1];

	if(curr.index & 0x8000)
	{
		curr.index &= 0x7FFF;
		curr.color[0] += (int8_t)(input[2] >> 4) - 7;
		curr.color[1] += (int8_t)(input[2] & 0x0F) - 7;
		curr.color[2] += (int8_t)(input[3] >> 4) - 7;
		curr.color[3] += (int8_t)(input[3] & 0x0F) - 7;
		
		return 4;
	}
	else
	{
		curr.color[0] = input[2];
		curr.color[1] = input[3];
		curr.color[2] = input[4];
		curr.color[3] = input[5];
		
		return 6;
	}
}

// Формат RGB, сжатие без индекса, 5 бит/цвет формат: FRRRRRGG GGGBBBBB
// Индекс в несжатом виде не должен взводить старший бит, т.е. должен быть < 32768
inline uint8_t parser_pack3_v2(const uint8_t *input, pxl_pixel_t &curr)
{
	if(input[0] & 0x80)
	{
		curr.index += 1;
		curr.color[0] += (int8_t)((input[0] >> 2) & 0x1F) - 15;
		curr.color[1] += (int8_t)(((input[0] << 3) & 0x18) | ((input[1] >> 5) & 0x07)) - 15;
		curr.color[2] += (int8_t)(input[1] & 0x1F) - 15;
		curr.color[3] += 0;
		
		return 2;
	}
	else
	{
		curr.index = (input[0] << 8) | input[1];
		curr.color[0] = input[2];
		curr.color[1] = input[3];
		curr.color[2] = input[4];
		curr.color[3] = 255;
		
		return 5;
	}
}

// Формат RGBA, сжатие без индекса, 4 бит/цвет формат: FRRRRGGG GBBBBAAA
// Индекс в несжатом виде не должен взводить старший бит, т.е. должен быть < 32768
inline uint8_t parser_pack4_v2(const uint8_t *input, pxl_pixel_t &curr)
{
	if(input[0] & 0x80)
	{
		curr.index += 1;
		curr.color[0] += (int8_t)((input[0] >> 3) & 0x0F) - 7;
		curr.color[1] += (int8_t)(((input[0] << 1) & 0x0E) | (input[1] & 0x01)) - 7;
		curr.color[2] += (int8_t)((input[1] >> 3) & 0x0F) - 7;
		curr.color[3] += (int8_t)(input[1] & 0x07) - 3;
		
		return 2;
	}
	else
	{
		curr.index = (input[0] << 8) | input[1];
		curr.color[0] = input[2];
		curr.color[1] = input[3];
		curr.color[2] = input[4];
		curr.color[3] = input[5];
		
		return 6;
	}
}

file_pixel_type_t file_pixel_type[] = 
{
	{ (2 + 3), (2 + 3), 0, parser_dummy_3 },	// RGB
	{ (2 + 4), (2 + 4), 0, parser_dummy_4 },	// RGBA
	{ (2 + 4), (2 + 4), 0, parser_dummy },		// GRBA
	{ (0),     (0),     0, parser_dummy },		// ---
	{ (0),     (0),     0, parser_dummy },		// ---
	{ (2 + 3), (2 + 2), 1, parser_dummy_3 },	// RGB + сжатие с индексом, 4 бит/цвет
	{ (2 + 4), (2 + 2), 1, parser_dummy_4 },	// RGBA + сжатие с индексом, 4 бит/цвет
	{ (2 + 3), (2),     2, parser_pack3_v2 },	// RGB + сжатие без индекса, 5 бит/цвет
	{ (2 + 4), (2),     2, parser_pack4_v2 },	// RGBA + сжатие без индекса, 5 бит/цвет
	{ (0),     (0),     0, parser_dummy },		// ---
	{ (0),     (0),     0, parser_dummy },		// ---
	{ (0),     (0),     0, parser_dummy },		// ---
	{ (0),     (0),     0, parser_dummy },		// ---
	{ (0),     (0),     0, parser_dummy },		// ---
	{ (0),     (0),     0, parser_dummy },		// ---
	{ (0),     (0),     0, parser_dummy },		// ---
};


struct parser_0_t
{

};
