/*=========================================================

created date: 2022/04/16

===========================================================*/
#include <windows.h>

#include <dsound.h>
#include <math.h>
#include <cwchar>
#include <intrin.h>
#include <iostream>

#include "../../handmade.h"
//#include "../../handmade.h"
#define MessageBox MessageBoxW

//===========================================================

/* TODO
	- save game locations
	- getting a handle to our own executable file
	- asset loading path
	- threading
	- raw input
	- sleep/timeBeginPeriod
	- clipCursor
	- fullscreen support
	- WM_SETCURSOR
	- QueryCancelAutoplay
	- WM_ACTIVATEAPP
	- Blit speed improvement
	- hardware acceleration(OpenGL or Direct3D)
	- GetKeyboardLayout
*/

#define internal			static
#define local_persist		static
#define global_variable		static

#define PI32				3.141592653F

//typedef int8_t				int8;
//typedef int16_t				int16;
//typedef int32_t				int32;
//typedef int64_t				int64;
//
//typedef uint8_t				uint8;
//typedef uint16_t			uint16;
//typedef uint32_t			uint32;
//typedef uint64_t			uint64;
//
//typedef float				real32;
//typedef double				real64;
//
//global_variable int			xOffset = 0;
//global_variable int			yOffset = 0;


struct offscreen_buffer
{
	BITMAPINFO	Info;
	void* Memory;
	int			Width;
	int			Height;
	int			Pitch;
	int			BytePerPixel;
};

global_variable bool				GlobalRunning;
global_variable	offscreen_buffer	GlobalBackBuffer;
global_variable LPDIRECTSOUNDBUFFER	GlobalSecondaryBuffer;

struct window_dimension
{
	int Width;
	int Height; 
};

window_dimension GetWindowDimension(HWND window)
{
	window_dimension result;
	RECT clientRect;
	GetClientRect(window, &clientRect);
	result.Width = clientRect.right - clientRect.left;
	result.Height = clientRect.bottom - clientRect.top;

	return result;
}

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);

//void* PlatformLoadFile(const char* filename)
//{
//	// Implements the win file loading
//	return 0;
//}


static void InitDSound(HWND Window, int32 SamplePerSecond, int32 BufferSize)
{
	// load the library
	HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");

	if (DSoundLibrary)
	{
		// get a directsound object
		direct_sound_create* DirectSoundCreate = (direct_sound_create*)GetProcAddress(DSoundLibrary, "DirectSoundCreate");

		LPDIRECTSOUND DirectSound;

		if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0)))
		{
			WAVEFORMATEX WaveFormat;
			WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
			WaveFormat.nChannels = 2;
			WaveFormat.nSamplesPerSec = SamplePerSecond;
			WaveFormat.wBitsPerSample = 16;
			WaveFormat.nBlockAlign = WaveFormat.nChannels * WaveFormat.wBitsPerSample / 8;
			WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
			WaveFormat.cbSize = 0;

			if (SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY)))
			{
				DSBUFFERDESC BufferDescription = {};
				BufferDescription.dwSize = sizeof(BufferDescription);
				BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

				// create primary buffer
				LPDIRECTSOUNDBUFFER PrimaryBuffer;
				if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0)))
				{
					if (SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat)))
					{
						OutputDebugString(L"Primary buffer format was set\n");
					}
					else
					{
						//TODO: Diagnostic
					}
				}
				else
				{
					//TODO: Diagnostic
				}
			}
			else
			{
				//TODO: Diagnostic
			}

			// create a secondary buffer

			DSBUFFERDESC BufferDescription = {};
			BufferDescription.dwSize = sizeof(BufferDescription);
			BufferDescription.dwFlags = 0;
			BufferDescription.dwBufferBytes = BufferSize;
			BufferDescription.lpwfxFormat = &WaveFormat;
			HRESULT error = DirectSound->CreateSoundBuffer(&BufferDescription, &GlobalSecondaryBuffer, 0);
			if (SUCCEEDED(error))
			{
				if (SUCCEEDED(GlobalSecondaryBuffer->SetFormat(&WaveFormat)))
				{
					OutputDebugString(L"secondary buffer format was set\n");
				}
				else
				{
					//TODO: Diagnostic
				}
			}
		}
		else
		{
			//TODO: Diagnostic
		}

	}
	else
	{
		//TODO: Diagnostic
	}
}

static void ResizeDIBSection(offscreen_buffer* Buffer, int width, int height)
{

	if (Buffer->Memory)
	{
		VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
	}

	Buffer->Width = width;
	Buffer->Height = height;
	Buffer->BytePerPixel = 4;

	// biHeight is negative, this is windows to treat this bitmap as top-down
	// not bottom_up, meanning that the first three bytes are  the color for the
	// top-left pixel in the bitmap, but not the bottom left.
	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;

	int BitmapMemorySize = (Buffer->Width * Buffer->Height) * Buffer->BytePerPixel;
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	Buffer->Pitch = width * Buffer->BytePerPixel;

}

static void UpdateWindow(HDC DeviceContext, int windowWidth, int windowHeight, offscreen_buffer* Buffer, int x, int y, int width, int height)
{
	StretchDIBits(DeviceContext,
		0, 0, windowWidth, windowHeight,
		0, 0, Buffer->Width, Buffer->Height,
		Buffer->Memory,
		&Buffer->Info,
		DIB_RGB_COLORS,
		SRCCOPY
	);
}

LRESULT CALLBACK MainWindowCallback(HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	LRESULT Result = 0;

	HDC DeviceContext = GetDC(hwnd);
	switch (uMsg)
	{
	case WM_SIZE:
	{
		window_dimension dimension = GetWindowDimension(hwnd);
		ResizeDIBSection(&GlobalBackBuffer, dimension.Width, dimension.Height);

	} break;

	case WM_DESTROY:
	{
		GlobalRunning = false;
	} break;

	// system keystrokes
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:

		// non-system keystrokes
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		WORD vkCode = LOWORD(wParam);

		WORD keyFlags = HIWORD(lParam);
		WORD scanCode = LOBYTE(keyFlags);
		BOOL isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED;

		if (isExtendedKey)
			scanCode = MAKEWORD(scanCode, 0xE0);

		BOOL wasKeyDown = (keyFlags & KF_REPEAT) == KF_REPEAT;
		BOOL wasAltDown = (keyFlags & KF_ALTDOWN) == KF_ALTDOWN;
		WORD repeatCOunt = LOWORD(lParam);

		BOOL isKeyReleased = (keyFlags & KF_UP) == KF_UP;

		switch (vkCode)
		{
		case VK_ESCAPE:
			GlobalRunning = false;
			break;
		case VK_UP:
			yOffset += 10;
			break;
		case VK_DOWN:
			yOffset -= 10;
			break;
		case VK_LEFT:
			xOffset += 10;
			break;
		case VK_RIGHT:
			xOffset -= 10;
			break;
		case 0x41:
			OutputDebugString(L"A\n");
			break;
		case VK_SPACE:
			OutputDebugString(L"Space\n");
			break;
		case VK_SHIFT:
			OutputDebugString(L"Shift\n");
			break;
		case VK_CONTROL:
			OutputDebugString(L"Ctrl\n");
			break;
		case VK_MENU: // ALT key
			vkCode = LOWORD(MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX));
			break;
		}
		if (wasAltDown && (vkCode == VK_F4))
		{
			GlobalRunning = false;
		}

	}
	break;
	case WM_CLOSE:
	{
		GlobalRunning = false;
	} break;

	case WM_ACTIVATEAPP:
	{

	} break;

	case WM_PAINT:
	{
		PAINTSTRUCT Paint;
		HDC DeviceContext = BeginPaint(hwnd, &Paint);

		int X = Paint.rcPaint.left;
		int Y = Paint.rcPaint.top;
		int Width = Paint.rcPaint.right - Paint.rcPaint.left;
		int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;

		RECT clientRect;
		GetClientRect(hwnd, &clientRect);

		window_dimension dimension = GetWindowDimension(hwnd);
		UpdateWindow(DeviceContext, dimension.Width, dimension.Height, &GlobalBackBuffer, X, Y, dimension.Width, dimension.Height);

		EndPaint(hwnd, &Paint);
	}	break;

	default:
	{
		Result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	} break;

	}

	return Result;
}

struct SoundOutput
{
	int SamplePerSecond;
	int SquareWaveCounter;
	int Hz;
	int ToneVolume;
	uint32 RunningSampleIndex;
	int BytesPerSample;
	int WavePeriod;

	int SecondaryBufferSize;
};

static void FillSoundBuffer(SoundOutput* soundOutput, DWORD ByteToLock, DWORD ByteToWrite)
{
	VOID* Region1;
	DWORD	Region1Size;
	VOID* Region2;
	DWORD	Region2Size;


	GlobalSecondaryBuffer->Lock(
		ByteToLock,
		ByteToWrite,
		&Region1,
		&Region1Size,
		&Region2,
		&Region2Size,
		0
	);

	int16* SampleOut = (int16*)Region1;
	DWORD	Region1SampleCount = Region1Size / soundOutput->BytesPerSample;
	DWORD	Region2SampleCount = Region2Size / soundOutput->BytesPerSample;
	for (DWORD SampleIndex = 0; SampleIndex < Region1SampleCount; ++SampleIndex)
	{
		real32 t = 2.0f * PI32 * (real32)soundOutput->RunningSampleIndex / (real32)soundOutput->WavePeriod;
		real32 SineValue = sinf(t);
		int16 SampleValue = (int16)(SineValue * soundOutput->ToneVolume);

		*SampleOut++ = SampleValue;
		*SampleOut++ = SampleValue;
		++(soundOutput->RunningSampleIndex);
	}

	SampleOut = (int16*)Region2;
	for (DWORD SampleIndex = 0; SampleIndex < Region2SampleCount; ++SampleIndex)
	{
		real32 t = 2.0f * PI32 * (real32)soundOutput->RunningSampleIndex / (real32)soundOutput->WavePeriod;
		real32 SineValue = sinf(t);
		int16 SampleValue = (int16)(SineValue * soundOutput->ToneVolume);

		*SampleOut++ = SampleValue;
		*SampleOut++ = SampleValue;
		++(soundOutput->RunningSampleIndex);
	}


	GlobalSecondaryBuffer->Unlock(
		Region1,
		Region1Size,
		Region2,
		Region2Size);
}

//struct platform_window
//{
//	HWND Handle;
//};
//
//platform_window* PlatformOpenWindow(char* title)
//{
//	platform_window* result = allocate
//}
//void PlatformCloseWindow(platform_window* window)
//{
//	if (window)
//	{
//		CloseWindow(window->Handle);
//	}
//}
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	LARGE_INTEGER PerfFrequency;
 	QueryPerformanceFrequency(&PerfFrequency);
	int64 PerforFrequency = PerfFrequency.QuadPart;

	WNDCLASS windowClass = {};								//窗口属性结构
	ResizeDIBSection(&GlobalBackBuffer, 1280, 720);

	windowClass.style = CS_VREDRAW | CS_HREDRAW;
	windowClass.lpfnWndProc = MainWindowCallback;
	windowClass.hInstance = hInstance;

	windowClass.lpszClassName = L"HandmadeHero";

	if (RegisterClass(&windowClass))
	{
		HWND WindowHanlde =
			CreateWindowEx(
				0,
				windowClass.lpszClassName,
				L"handmade hero",
				WS_OVERLAPPEDWINDOW | WS_VISIBLE,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				0,
				0,
				hInstance,
				0
			);
		if (WindowHanlde)
		{
			GlobalRunning = true;
			HDC DeviceContext = GetDC(WindowHanlde);

			SoundOutput soundOutput = {};

			soundOutput.SamplePerSecond = 48000;
			soundOutput.SquareWaveCounter = 0;
			soundOutput.Hz = 256;
			soundOutput.ToneVolume = 6000;
			soundOutput.RunningSampleIndex = 0;
			soundOutput.BytesPerSample = sizeof(int16) * 2;
			soundOutput.WavePeriod = soundOutput.SamplePerSecond / soundOutput.Hz;
			soundOutput.SecondaryBufferSize = soundOutput.SamplePerSecond * soundOutput.BytesPerSample;

			InitDSound(WindowHanlde, soundOutput.SamplePerSecond, soundOutput.SecondaryBufferSize);
			FillSoundBuffer(&soundOutput, 0, soundOutput.SecondaryBufferSize);

			GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

			LARGE_INTEGER LastCounter;
			QueryPerformanceCounter(&LastCounter);

			int64 LastCycleCount = __rdtsc();

			while (GlobalRunning)
			{

				MSG Message;
				while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
				{
					if (Message.message == WM_QUIT)
					{
						GlobalRunning = false;
					}
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}

				game_offscreen_buffer Buffer = {};
				Buffer.Memory = GlobalBackBuffer.Memory;
				Buffer.Width = GlobalBackBuffer.Width;
				Buffer.Height = GlobalBackBuffer.Height;
				Buffer.Pitch = GlobalBackBuffer.Pitch;

				GameUpdateAndRender(&Buffer,xOffset,yOffset);
				//RenderGradient(&GlobalBackBuffer, xOffset, yOffset);

				DWORD PlayCursor;
				DWORD WriteCursor;
				if (SUCCEEDED(GlobalSecondaryBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor)))
				{
					DWORD ByteToLock = (soundOutput.RunningSampleIndex * soundOutput.BytesPerSample) % soundOutput.SecondaryBufferSize;
					DWORD ByteToWrite;
					if (ByteToLock == PlayCursor)
					{
						ByteToWrite = 0;
					}
					else if (ByteToLock > PlayCursor)
					{
						ByteToWrite = (soundOutput.SecondaryBufferSize - ByteToLock);
						ByteToWrite += PlayCursor;
					}
					else
					{
						ByteToWrite = PlayCursor - ByteToLock;
					}


					FillSoundBuffer(&soundOutput, ByteToLock, ByteToWrite);

				}


				window_dimension dimension = GetWindowDimension(WindowHanlde);
				UpdateWindow(DeviceContext, dimension.Width, dimension.Height,
					&GlobalBackBuffer, 0, 0, dimension.Width, dimension.Height);
				ReleaseDC(WindowHanlde, DeviceContext);

			/*	++xOffset;
				++yOffset;*/
				LARGE_INTEGER EndCounter;
				QueryPerformanceCounter(&EndCounter);

				int64 EndCycleCount = __rdtsc();

				int64 CyclesElapsed = EndCycleCount - LastCycleCount;

				int64 CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;
				real32 MsPerFrame = (real32)(1000 * CounterElapsed) / PerforFrequency;
				int32 FPS = (int32)(PerforFrequency / CounterElapsed);
#if 0
				char buffer[256];

				sprintf_s(buffer, "Milliseconds/frame: %.2f ms, FPS: %d , Mcycle/frame: %d\n", MsPerFrame, FPS, CyclesElapsed / (1000 * 1000));
				OutputDebugStringA(buffer);
#endif
				

				//MainLoop();
				LastCounter = EndCounter;
				LastCycleCount = EndCycleCount;
			}
		}
		else
		{
			// TODO
		}
	}
	else
	{
		//TODO
	}

	return 0;
}



