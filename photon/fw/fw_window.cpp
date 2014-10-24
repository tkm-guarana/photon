

#include <assert.h>
#include "fw_window.h"

namespace fw
{

// ウィンドウの生成
int AppWindow::init()
{
	WNDCLASSEX	wcex;
	RECT cr, wr;

	wcex.cbSize			= sizeof(wcex);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= this->window_proc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= instance_handle;
	wcex.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "GAME_APP";
	wcex.hIconSm		= NULL;
	
	if(!RegisterClassEx(&wcex)) return 1;
	
	window_handle = CreateWindowEx(
			WS_EX_APPWINDOW,
			"GAME_APP", "Window",
			WS_OVERLAPPEDWINDOW ^ (WS_MAXIMIZEBOX | WS_THICKFRAME), // 最大化、サイズ変更不可
			CW_USEDEFAULT, CW_USEDEFAULT,
			width, height,
			NULL, NULL, instance_handle, NULL);

	// サイズ調整
	GetWindowRect(window_handle, &wr);
	GetClientRect(window_handle, &cr);
	SetWindowPos(window_handle, HWND_TOP, 0, 0, (wr.right - wr.left - cr.right) + width, (wr.bottom - wr.top - cr.bottom) + height, SWP_NOMOVE);
	GetClientRect(window_handle, &cr);

	ShowWindow(window_handle, SW_SHOW);
	UpdateWindow(window_handle);

	return 0;
}

int AppWindow::run()
{
	assert(window_handle != NULL); // 初期化した？
	MSG msg;

	while(loop_flag)
	{
		while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if(msg.message == WM_QUIT) loop_flag = false;

			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		this->loop();
	}

	return 0;
}

// ウィンドウリサイズ
void AppWindow::setWindowSize(int width, int height)
{
	RECT cr, wr;

	GetWindowRect(window_handle, &wr);
	GetClientRect(window_handle, &cr);
	SetWindowPos(window_handle, HWND_TOP, 0, 0, (wr.right - wr.left - cr.right) + width, (wr.bottom - wr.top - cr.bottom) + height, SWP_NOMOVE);
	GetClientRect(window_handle, &cr);
}

LRESULT CALLBACK AppWindow::WinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if(wParam == VK_ESCAPE) PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	
	return 0L;
}


}