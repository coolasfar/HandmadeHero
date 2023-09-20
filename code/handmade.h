#ifndef _HANDMADE_H
#define _HANDMADE_H


#include <stdint.h>
// four things - timing, controller/keyboard, bitmap and sound buffer

#define internal			static
#define local_persist		static
#define global_variable		static


typedef int8_t				int8;
typedef int16_t				int16;
typedef int32_t				int32;
typedef int64_t				int64;

typedef uint8_t				uint8;
typedef uint16_t			uint16;
typedef uint32_t			uint32;
typedef uint64_t			uint64;

typedef float				real32;
typedef double				real64;

global_variable int			xOffset = 0;
global_variable int			yOffset = 0;

struct game_offscreen_buffer
{
	// NOTE: Pixels are always 32-bit wide,little endian . Memory order BB GG RR XX
	//BITMAPINFO	Info;
	void*		Memory;
	int			Width;
	int			Height;
	int			Pitch;
	int			BytePerPixel;
};

void GameUpdateAndRender(game_offscreen_buffer* buffer,int BlueOffset, int GreenOffset);

#endif
