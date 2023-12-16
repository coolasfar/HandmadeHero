#ifndef _WIN_HANDMADE_H
#define _WIN_HANDMADE_H


#include "../handmade/handmade.h"

struct offscreen_buffer
{
	BITMAPINFO	Info;
	void*		Memory;
	int			Width;
	int			Height;
	int			Pitch;
	int			BytePerPixel;
};


struct sound_output
{
	int SamplePerSecond;
	int SquareWaveCounter;
	uint32  RunningSampleIndex;
	int BytesPerSample;
	int SecondaryBufferSize;
};

struct window_dimension
{
	int Width;
	int Height;
};

struct game_code
{
	HMODULE gameCodeDLL;
	FILETIME DLLlastWriteTime;
	game_update_and_render* updateAndRender;
	game_get_sound_sample* getSoundSamples;

	bool isValid;
};

#endif