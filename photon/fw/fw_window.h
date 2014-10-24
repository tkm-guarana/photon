
#ifndef _fw_window_h_
#define _fw_window_h_

#include <windows.h>

// 汎用ウィンドウ

namespace fw
{

// フレーム基底
class AppWindow
{
private:
	HINSTANCE	instance_handle;
	HWND		window_handle;
	WNDPROC		window_proc;
	int			width, height;
	bool		loop_flag;

public:
	AppWindow(HINSTANCE inst, WNDPROC proc, int w = 640, int h = 480) :
	  instance_handle(inst), window_handle(NULL), window_proc(proc), width(w), height(h), loop_flag(true) {}
	virtual ~AppWindow() {}

	virtual int init(); // 派生先で
	virtual int run(); // loop実行
	virtual int loop() = 0; // 派生先で主な処理を実装

	void setWindowSize(int width, int height);
	HWND getHWND() const { return window_handle; }

	static LRESULT CALLBACK WinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam); // イベント処理したいなら派生先で新しく実装する
};


}

#endif