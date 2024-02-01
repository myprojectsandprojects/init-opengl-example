#include <stdio.h>
#include <windows.h>
#include <GL/gl.h>

HBRUSH Brushes[3];

void MakeBrushes()
{
	Brushes[0] = CreateSolidBrush(RGB(255, 0, 0));
	Brushes[1] = CreateSolidBrush(RGB(0, 255, 0));
	Brushes[2] = CreateSolidBrush(RGB(0, 0, 255));
}

LRESULT CALLBACK MainWindowProc(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = 0;

	switch(Message)
	{
		case WM_CLOSE:
		{
			printf("WM_CLOSE\n");
			DestroyWindow(Window);
		}
		break;
		case WM_DESTROY:
		{
			printf("WM_DESTROY\n");
			PostQuitMessage(0);
		}
		break;
		case WM_PAINT:
		{
			static int Count = 1;
			printf("WM_PAINT (%d)\n", Count);

			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);

			// GDI drawing:
			// RECT Rect = Paint.rcPaint;
			// printf("left: %ld, top: %ld, right: %ld, bottom: %ld\n", Rect.left, Rect.top, Rect.right, Rect.bottom);

			RECT Rect;
			Rect.left = 10;
			Rect.right = 100;
			Rect.top = 10;
			Rect.bottom = 100;

			HBRUSH Brush = Brushes[(unsigned int)Count % 3];
			FillRect(DeviceContext, &Rect, Brush);

			EndPaint(Window, &Paint);

			Count += 1;

			// Result = DefWindowProc(Window, Message, wParam, lParam); // validates update region
		}
		break;

		default:
			Result = DefWindowProc(Window, Message, wParam, lParam);
	}

	return Result;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MakeBrushes();

	// printf("Hello world!\n");

	// MessageBox(NULL, "Hello world!", "Greeting", MB_OK | MB_ICONINFORMATION);

	WNDCLASS MainWindowClass = {};
	MainWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	MainWindowClass.lpfnWndProc = MainWindowProc;
	MainWindowClass.hInstance = hInstance;
	// MainWindowClass.hInstance = GetModuleHandle(NULL);
	MainWindowClass.lpszClassName = "MainWindowClass";
	// MainWindowClass.hbrBackground = CreateSolidBrush(RGB(255,0,255));

	if(RegisterClass(&MainWindowClass) == 0)
	{
		fprintf(stderr, "error\n");
		return 1;
	}

	HWND Window = CreateWindow(
		MainWindowClass.lpszClassName,
		"window name",
		WS_OVERLAPPEDWINDOW,
		100, 100, 300, 300,
		0,
		0,
		hInstance,
		NULL);
	if(Window == NULL)
	{
		DWORD Error = GetLastError();
		fprintf(stderr, "error: creating a window (%lu)\n", Error);
		return 1;
	}

	HDC DeviceContext = GetDC(Window);
	if(DeviceContext == NULL)
	{
		fprintf(stderr, "error: GetDC()\n");
		return 0;
	}
	else
	{
		fprintf(stderr, "ok: GetDC()\n");

	}

	// initialize OpenGL
	PIXELFORMATDESCRIPTOR PixelFormat = {};
	PixelFormat.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	PixelFormat.nVersion = 1;
	PixelFormat.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	PixelFormat.iPixelType = PFD_TYPE_RGBA;
	PixelFormat.cColorBits = 24;
	PixelFormat.cDepthBits = 32;
	PixelFormat.iLayerType = PFD_MAIN_PLANE;
	int Index = ChoosePixelFormat(DeviceContext, &PixelFormat);
	if(Index == 0)
	{
		fprintf(stderr, "error: ChoosePixelFormat()\n");
		return 0;
	}
	else
	{
		fprintf(stderr, "pixel format index: %d\n", Index);
	}
	SetPixelFormat(DeviceContext, Index, &PixelFormat);
	HGLRC GLRenderingContext = wglCreateContext(DeviceContext);
	if(GLRenderingContext == NULL)
	{
		fprintf(stderr, "error: wglCreateContext()\n");
		return 0;
	}
	else
	{
		fprintf(stderr, "ok: wglCreateContext()\n");
	}

	if(wglMakeCurrent(DeviceContext, GLRenderingContext) == FALSE)
	{
		fprintf(stderr, "error: wglMakeCurrent()\n");
		return 0;
	}
	else
	{
		fprintf(stderr, "ok\n");
	}

	typedef BOOL (wglSwapIntervalEXT)(int interval);
	wglSwapIntervalEXT *WglSwapInterval = (wglSwapIntervalEXT *)wglGetProcAddress("wglSwapIntervalEXT");
	if(WglSwapInterval == NULL)
	{
		fprintf(stderr, "error: wglGetProcAddress()\n");
		return 0;
	}
	else
	{
		fprintf(stderr, "ok!!!\n");
	}
	WglSwapInterval(1);

	// ReleaseDC(Window, DeviceContext);

	RECT WindowClientArea;
	GetClientRect(Window, &WindowClientArea);
	printf("left: %ld, right: %ld, top: %ld, bottom: %ld\n",
		WindowClientArea.left,
		WindowClientArea.right,
		WindowClientArea.top,
		WindowClientArea.bottom);
	
	GLsizei WindowWidth = WindowClientArea.right - WindowClientArea.left;
	GLsizei WindowHeight = WindowClientArea.bottom - WindowClientArea.top;
	printf("window width: %u, window height: %u\n", WindowWidth, WindowHeight);
	glViewport(10, 10, WindowWidth, WindowHeight);

	ShowWindow(Window, nCmdShow);
	// UpdateWindow(Window);

	int FrameCount = 0;
	while(true)
	{
		FrameCount += 1;
		// printf("frame count: %d\n", FrameCount);

		MSG Message;
		while(PeekMessage(&Message, NULL, 0, 0, PM_REMOVE) != 0)
		{
			if(Message.message == WM_QUIT)
			{
				// handle quit
				printf("WM_QUIT\n");
				goto exit;
			}
			else
			{
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			}
		}

		// BOOL Result = GetMessage(&Message, NULL, 0, 0);
		// if(Result == -1)
		// {
		// 	fprintf(stderr, "error: GetMessage()\n");
		// 	break;
		// }
		// else if(Result == 0) // WM_QUIT
		// {
		// 	// handle quit
		// 	printf("WM_QUIT\n");
		// 	break;
		// }
		// else
		// {
		// 	TranslateMessage(&Message);
		// 	DispatchMessage(&Message);
		// }

		// OpenGL drawing:
		glClearColor(0.1f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_TRIANGLES);

		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(0.0f, 0.5f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(0.5f, -0.5f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(-0.5f, -0.5f);

		glEnd();

		// SwapBuffers() could fail if the window was destroyed after we left the message loop (?)
		if(SwapBuffers(DeviceContext) == FALSE)
		{
			fprintf(stderr, "error: SwapBuffers()\n");
			return 0;
		}
		else
		{
			// fprintf(stderr, "SwapBuffers ok\n");
		}
	}

	exit:

	return 0;
}