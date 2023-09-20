
#include "handmade.h"



static void RenderGradient(game_offscreen_buffer* Buffer, int xOffset, int yOffset)
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
			//					xx RR GG BB
			// pixel in memory: 00 00 00 00
			*Pixel++ = ((green << 8) | blue);
		}
		Row += Buffer->Pitch;
	}
}

void GameUpdateAndRender(game_offscreen_buffer* Buffer, int BlueOffset, int GreenOffset)
{
	RenderGradient(Buffer, BlueOffset, GreenOffset);
}
