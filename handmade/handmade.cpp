
#include "handmade.h"

//#pragma comment(linker, "/export:GameUpdateAndRender")
//#pragma comment(linker, "/export:GameGetSoundSamples")

internal void GameOutputSound(game_state * gameState,
	game_sound_buffer* SoundBuffer, int toneHz)
{
	int16 ToneVolume = 3000;
	int WavePeriod = SoundBuffer->SamplePerSecond / toneHz;

	int16* SampleOut = SoundBuffer->Samples;
	for (int SampleIndex = 0;
		SampleIndex < SoundBuffer->SampleCount;
		++SampleIndex)
	{
		real32 SineValue = sinf(gameState->tSine);
		int16 SampleValue = (int16)(SineValue * ToneVolume);

		*SampleOut++ = SampleValue;
		*SampleOut++ = SampleValue;
		gameState->tSine += 2.0f * PI32 * 1.0f / (real32)WavePeriod;

		if (gameState->tSine > 2.0f * PI32)
		{
			gameState->tSine -= 2.0f * PI32;
		}
	}
}

void RenderGradient(game_offscreen_buffer* Buffer, 
	int xOffset, int yOffset)
{

	uint8* Row = (uint8*)Buffer->Memory;
	for (int y = 0; y < Buffer->Height; ++y)
	{
		uint32* Pixel = (uint32*)Row;
		for (int x = 0; x < Buffer->Width; ++x)
		{
			uint8 blue = (x /2 + xOffset);
			uint8 green = (y + yOffset);
			//	xx RR GG BB
			// pixel in memory: 00 00 00 00
			*Pixel++ = ((green) | blue <<8);
		}
		Row += Buffer->Pitch;
	}
}

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
	Assert(sizeof(game_state) <= memory->permanentStorageSize);

	game_state* gameState = (game_state*)memory->permanentStorage;
	if (!memory->isInitialized)
	{
		const char* filename = __FILE__;

		debug_read_file_result file = memory->debugPlatformReadFile(filename);

		if (file.contents)
		{
			memory->debugPlatformWriteFile("e:/HandmadeHero/test.out",
				file.contents, file.contentSize);
			memory->debugPlatformFreeFileMemory(file.contents);
		}

		gameState->toneHZ = 256;
		gameState->tSine = 0.0f;
		memory->isInitialized = true;
	}

	for (int ControllerIndex = 0;
		ControllerIndex < ArrayCount(input->Controllers);
		++ControllerIndex)
	{
	
		game_controller_input* controller = &input->Controllers[0];
		if (controller->IsAnalog)
		{
			gameState->toneHZ += (int)(128 * (controller->stickAverageX));
			gameState->blueOffset += (int)4.0f * ((int)controller->stickAverageY);
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
	}

	RenderGradient(graphic_buffer, 
		gameState->blueOffset,
		gameState->greenOffset);
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
	game_state* gameState = (game_state*)memory->permanentStorage;
	GameOutputSound(gameState, sound_buffer, gameState->toneHZ);
}


