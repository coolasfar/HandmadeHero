
#include "handmade.h"



internal void GameOutputSound(game_sound_buffer* SoundBuffer, int toneHz)
{
	local_persist real32 tSine;
	int16 ToneVolume = 3000;
	int wavePeriod = SoundBuffer->SamplePerSecond / toneHz;

	int16* SampleOut = SoundBuffer->Samples;
	for (int SampleIndex = 0; SampleIndex < SoundBuffer->SampleCount; ++SampleIndex)
	{
		real32 SineValue = sinf(tSine);
		int16 SampleValue = (int16)(SineValue * ToneVolume);

		*SampleOut++ = SampleValue;
		*SampleOut++ = SampleValue;

		tSine += 2.0f * PI32 / (real32)wavePeriod;
	}
}


// TODO:
internal void RenderGradient(game_offscreen_buffer* Buffer, int xOffset, int yOffset)
{
	int width = Buffer->Width;
	int height = Buffer->Height;

	uint8* Row = (uint8*)Buffer->Memory;
	for (int y = 0; y < Buffer->Height; ++y)
	{
		uint32* Pixel = (uint32*)Row;
		for (int x = 0; x < Buffer->Width; ++x)
		{
			uint8 blue = (x + xOffset);
			uint8 green = (y + yOffset);
			//	xx RR GG BB
			// pixel in memory: 00 00 00 00
			*Pixel++ = ((green << 8) | blue);
		}
		Row += Buffer->Pitch;
	}
}

internal void GameUpdateAndRender(game_offscreen_buffer* graphic_buffer, game_sound_buffer* sound_buffer, int BlueOffset, int GreenOffset, int toneHz)
{
	GameOutputSound(sound_buffer,toneHz);
	RenderGradient(graphic_buffer, BlueOffset, GreenOffset);
}

