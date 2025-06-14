#pragma once
#include <inttypes.h>
#include <math.h>

class PrimitiveDraw
{
	using color_t = FrameBuffer::color_t;

	public:

		typedef struct
		{
			uint8_t x0, y0, x1, y1, thickness;
			color_t color;
		} line_params_t;

		typedef struct
		{
			uint8_t x, y, w, h, border_thickness, filled;
			color_t border_color, fill_color;
		} rectangle_params_t;

		typedef struct
		{
			uint8_t cx, cy, radius, border_thickness, filled;
			color_t border_color, fill_color;
		} circle_params_t;

		PrimitiveDraw(uint8_t frame_width, uint8_t frame_height) 
			: _frame_width(frame_width), _frame_height(frame_height)
		{
			return;
		}
		
		void SetFrameBuffer(FrameBuffer &frame_buffer)
		{
			this->frame_buffer = &frame_buffer;
			
			return;
		}
		
		void draw_line(line_params_t &params)
		{
			if(frame_buffer == nullptr) return;

			int dx = abs(params.x1 - params.x0), sx = params.x0 < params.x1 ? 1 : -1;
			int dy = abs(params.y1 - params.y0), sy = params.y0 < params.y1 ? 1 : -1;
			int err = (dx > dy ? dx : -dy) / 2, e2;
			uint8_t x0 = params.x0, y0 = params.y0;
			while (1) {
				for (int i = -params.thickness / 2; i <= params.thickness / 2; i++) {
					for (int j = -params.thickness / 2; j <= params.thickness / 2; j++) {
						uint16_t idx = (y0 + j) * _frame_width + (x0 + i);
						if (idx < _frame_width * _frame_height) {
							frame_buffer->SetPixel(idx, params.color);
						}
					}
				}
				if (x0 == params.x1 && y0 == params.y1) break;
				e2 = err;
				if (e2 > -dx) { err -= dy; x0 += sx; }
				if (e2 < dy) { err += dx; y0 += sy; }
			}
		}

		void draw_rectangle(rectangle_params_t &params)
		{
			if(frame_buffer == nullptr) return;

			if (params.filled) {
				for (uint8_t i = 0; i < params.h; i++) {
					for (uint8_t j = 0; j < params.w; j++) {
						uint16_t idx = (params.y + i) * _frame_width + (params.x + j);
						if (idx < _frame_width * _frame_height) {
							frame_buffer->SetPixel(idx, params.fill_color);
						}
					}
				}
			}
			
			line_params_t line;
			line.thickness = 1;
			line.color = params.border_color;
			
			for(uint8_t t = 0; t < params.border_thickness; t++)
			{
				line.x0 = params.x + t;
				line.y0 = params.y + t;
				line.x1 = params.x + params.w - t - 1;
				line.y1 = params.y + t;
				draw_line(line);
				
				line.x0 = params.x + params.w - t - 1; line.y0 = params.y + t;
				line.x1 = params.x + params.w - t - 1; line.y1 = params.y + params.h - t - 1;
				draw_line(line);
				
				line.x0 = params.x + params.w - t - 1; line.y0 = params.y + params.h - t - 1;
				line.x1 = params.x + t; line.y1 = params.y + params.h - t - 1;
				draw_line(line);
				
				line.x0 = params.x + t; line.y0 = params.y + params.h - t - 1;
				line.x1 = params.x + t; line.y1 = params.y + t;
				draw_line(line);
			}
		}

		void draw_circle(circle_params_t &params)
		{
			if(frame_buffer == nullptr) return;

			for (int8_t y = -params.radius; y <= params.radius; y++) {
				for (int8_t x = -params.radius; x <= params.radius; x++) {
					uint16_t dist = x * x + y * y;
					if (dist <= params.radius * params.radius) {
						uint16_t idx = (params.cy + y) * _frame_width + (params.cx + x);
						if (idx < _frame_width * _frame_height) {
							if (params.filled || dist >= (params.radius - params.border_thickness) * (params.radius - params.border_thickness)) {
								frame_buffer->SetPixel(idx, (params.filled ? params.fill_color : params.border_color));
							}
						}
					}
				}
			}
		}

	private:
		
		FrameBuffer *frame_buffer = nullptr;
		const uint8_t _frame_width;
		const uint8_t _frame_height;
};
