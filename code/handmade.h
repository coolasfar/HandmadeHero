#ifndef _HANDMADE_H
#define _HANDMADE_H

#include <stdint.h>
#include <math.h>
// four things - timing, controller/keyboard, bitmap and sound buffer

// TODO swap min max Macro
#define Assert(Expression)	if(!(Expression)) {*(int *)0 = 0;}

#define ArrayCount(array)	(sizeof(array)/sizeof(array[0]))

#define KILOBYTES(value)	((value)*1024LL)
#define MEGABYTES(value)	(KILOBYTES(value)*1024LL)
#define GIGABYTES(value)	(MEGABYTES(value)*1024LL)

#define internal			static
#define local_persist		static
#define global_variable		static

#define PI32				3.141592653F

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


struct debug_read_file_result
{
	void* contents;
	uint32 contentSize;
};


debug_read_file_result DebugPlatformReadEntireFile(const char* filename);
void DebugPlatformFreeFileMemory(void* memory);
bool DebugPlatformWriteEntireFile(const char* filename, void* memory,uint32 memorySize);

inline uint32 SafeTruncateUint64(uint64 value)
{
	Assert(value <= 0xFFFFFFFF);
	uint32 result = (uint32)value;
	return result;
}

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

struct game_sound_buffer
{
	int SampleCount;
	int16* Samples;
	int SamplePerSecond;
};

struct game_button_state
{
	int HalfTransitionCount;
	bool EndedDown;
};

struct game_controller_input
{
	bool isConnected;
	bool IsAnalog;

	real32 stickAverageX;
	real32 stickAverageY;

	union 
	{
		game_button_state Buttons[10];
		struct 
		{
			game_button_state moveUp;
			game_button_state moveDown;
			game_button_state moveLeft;
			game_button_state moveRight;

			game_button_state actionUp;
			game_button_state actionDown;
			game_button_state actionLeft;
			game_button_state actionRight;

			game_button_state LeftShoulder;
			game_button_state RightShoulder;

			game_button_state start;
			game_button_state back;
		};

	};

};

struct game_input
{
	game_controller_input Controllers[5];

};

struct game_state
{
	int toneHZ;
	int greenOffset;
	int blueOffset;

};

struct game_memory
{
	bool isInitialized;
	uint64 permanentStorageSize;
	void* permanentStorage; // required to be cleared to zero at start up
	uint64 transientStorageSize;
	void* transientStorage;
};

struct game_clocks
{
	real32 secondsElapsed;
};

internal void GameUpdateAndRender(game_memory* memory, game_input* input, game_offscreen_buffer*
	graphic_buffer, game_sound_buffer* sound_buffer);


#endif
