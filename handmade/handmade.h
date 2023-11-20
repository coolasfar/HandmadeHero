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
#define TERABYTES(value)	(GIGABYTES(value)*1024LL)

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


#define DEBUG_PLATFORM_READ_FILE(name) \
debug_read_file_result name(const char* filename)

typedef DEBUG_PLATFORM_READ_FILE(debug_platform_read_file);

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) \
void name(void* memory)

typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

#define DEBUG_PLATFORM_WRITE_FILE(name) \
bool name(const char* filename, void* memory,uint32 memorySize)

typedef DEBUG_PLATFORM_WRITE_FILE(debug_platform_write_file);

//debug_read_file_result DebugPlatformReadEntireFile(const char* filename);
//void DebugPlatformFreeFileMemory(void* memory);
//bool DebugPlatformWriteEntireFile(const char* filename, void* memory,uint32 memorySize);

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

	real32 tSine;

};

struct game_memory
{
	bool isInitialized;
	uint64 permanentStorageSize;
	// required to be cleared to zero at start up
	void* permanentStorage; 
	uint64 transientStorageSize;
	void* transientStorage;

	debug_platform_read_file* debugPlatformReadFile;
	debug_platform_free_file_memory* debugPlatformFreeFileMemory;
	debug_platform_write_file* debugPlatformWriteFile;

};

struct game_clocks
{
	real32 secondsElapsed;
};


#define GAME_UPDATE_AND_RENDER(name) void name								 \
(																			 \
	game_memory* memory, game_input* input,                                  \
	game_offscreen_buffer* graphic_buffer	 \
)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub) { }



#define GAME_GET_SOUND_SAMPLES(name)	void name							 \
(																			 \
	game_memory* memory, game_sound_buffer* sound_buffer					 \
)	                 

typedef GAME_GET_SOUND_SAMPLES(game_get_sound_sample);

GAME_GET_SOUND_SAMPLES(GameGetSoundSamplesStub) { }

//void GameUpdateAndRender(game_memory* memory, game_input* input, game_offscreen_buffer* graphic_buffer);
//
//void GameGetSoundSamples(game_memory* memory, game_sound_buffer* sound_buffer);

#endif
