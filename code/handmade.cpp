
#include "handmade.h"

internal void GameOutputSound(game_sound_buffer* SoundBuffer, int toneHz)
{
	local_persist real32 tSine;
	int16 ToneVolume = 3000;
	int wavePeriod = SoundBuffer->SamplePerSecond / toneHz;

	int16* SampleOut = SoundBuffer->Samples;
	for (int SampleIndex = 0; 
		SampleIndex < SoundBuffer->SampleCount; 
		++SampleIndex)
	{
		real32 SineValue = sinf(tSine);
		int16 SampleValue = (int16)(SineValue * ToneVolume);

		*SampleOut++ = SampleValue;
		*SampleOut++ = SampleValue;

		tSine += 2.0f * PI32 / (real32)wavePeriod;

	}
}

internal void RenderGradient(game_offscreen_buffer* Buffer, 
	int xOffset, int yOffset)
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

internal void GameUpdateAndRender(game_memory* memory, 
	game_input* input,game_offscreen_buffer* graphic_buffer, 
	game_sound_buffer* sound_buffer)
{
	Assert(sizeof(game_state) <= memory->permanentStorageSize);

	game_state* gameState = (game_state*)memory->permanentStorage;
	if (!memory->isInitialized)
	{
		const char* filename = __FILE__;

		debug_read_file_result file = DebugPlatformReadEntireFile(filename);
		
		if (file.contents)
		{
			DebugPlatformWriteEntireFile("e:/HandmadeHero/test.out", 
				file.contents,file.contentSize);
			DebugPlatformFreeFileMemory(file.contents);
		}

		gameState->toneHZ = 256;
		memory->isInitialized = true;
	}


	game_controller_input* controller= &input->Controllers[0];
	if (controller->IsAnalog)
	{
		gameState->toneHZ += (int)(128 * (controller->stickAverageX));
		gameState->blueOffset+= (int)4.0f * (controller->stickAverageY);
	}
	else
	{
		if (controller->actionLeft.EndedDown)
		{
			gameState->blueOffset -= 10;
		}
		if (controller->actionRight.EndedDown)
		{
			gameState->blueOffset += 10;
		}
	}

	if (controller->actionDown.EndedDown)
	{
		gameState->greenOffset += 10;
	}

	if (controller->actionDown.EndedDown)
	{
		gameState->blueOffset += 10;
	}

	GameOutputSound(sound_buffer,gameState->toneHZ);
	RenderGradient(graphic_buffer, 
		gameState->blueOffset,
		gameState->greenOffset);
}

