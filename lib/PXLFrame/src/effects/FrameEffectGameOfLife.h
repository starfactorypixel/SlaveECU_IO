#pragma once
#include <inttypes.h>
#include "FrameEffectInterface.h"

class FrameEffectGameOfLife : public FrameEffectInterface
{
	public:
		
		virtual void Init() override
		{
			_initRandomGeneration();

			return;
		}
		
		virtual void Tick(uint32_t time) override
		{
			return;
		}

		virtual bool FramePrepare(uint32_t time) override
		{
			_updateGeneration();
			_drawGeneration();
			
			return true;
		}
		
	private:
		
		void _initRandomGeneration()
		{
			for(uint16_t i = 0; i < frame_width * frame_height; i++)
			{
				_setBit(_current_generation, i, rand() % 3 == 0);
			}
			
			return;
		}
		
		void _updateGeneration()
		{
			uint32_t neighbors_count = 0;
			
			uint16_t index_2d;
			uint8_t neighbors;
			bool isAlive;
			for(uint8_t y = 0; y < frame_height; y++)
			{
				for(uint8_t x = 0; x < frame_width; x++)
				{
					index_2d = x + (y * frame_width);
					neighbors = _countNeighbors(x, y);
					isAlive = _getBit(_current_generation, index_2d);
					
					_setBit(_next_generation, index_2d, (isAlive && (neighbors == 2 || neighbors == 3)) || (!isAlive && neighbors == 3) || (!isAlive && neighbors == 2 && rand() % 333 == 0));
					
					neighbors_count += neighbors;
				}
			}
			memcpy_dma32(_current_generation, _next_generation, sizeof(_current_generation));
			
			if(neighbors_count == 0)
				_initRandomGeneration();
			
			return;
		}
		
		void _drawGeneration()
		{
			uint16_t index_2d;
			color_t color;
			for(uint8_t y = 0; y < frame_height; y++)
			{
				for(uint8_t x = 0; x < frame_width; x++)
				{
					index_2d = x + (y * frame_width);
					
					if(_getBit(_current_generation, index_2d))
						color = {0x00, 0x10, 0x00};
					else
						color = {0x00, 0x00, 0x00};
					
					frame_buffer->SetPixel(index_2d, color);
				}
			}

			return;
		}
		
		uint8_t _countNeighbors(uint8_t x, uint8_t y)
		{
			int8_t offsets[8][2] = {{-1, -1}, {0, -1}, {1, -1}, 
									{-1,  0},          {1,  0}, 
									{-1,  1}, {0,  1}, {1,  1}};
			uint8_t count = 0;
			
			for(auto &offset : offsets)
			{
				int16_t nx = x + offset[0];
				int16_t ny = y + offset[1];
				
				if(nx >= 0 && nx < frame_width && ny >= 0 && ny < frame_height)
				{
					uint16_t neighborIndex = ny * frame_width + nx;
					count += _getBit(_current_generation, neighborIndex);
				}
			}
			
			return count;
		}
		
		void _setBit(uint8_t *buffer, uint16_t index, bool value)
		{
			uint16_t bitPos = index % 8;
			uint16_t arrayPos = index / 8;
			
			if(value)
				buffer[arrayPos] |= (1 << bitPos);
			else
				buffer[arrayPos] &= ~(1 << bitPos);
			
			return;
		}
		
		bool _getBit(const uint8_t *buffer, uint16_t index)
		{
			uint16_t bitPos = index % 8;
			uint16_t arrayPos = index / 8;
			
			return (buffer[arrayPos] & (1 << bitPos)) != 0;
		}
		
		uint8_t _current_generation[ (frame_width * frame_height + 7) / 8 ] = {0};
		uint8_t _next_generation[ (frame_width * frame_height + 7) / 8 ] = {0};
		
};
