
#ifndef _fw_window_h_
#define _fw_window_h_

#include <windows.h>

// �ėp�E�B���h�E

namespace fw
{

// �t���[�����
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

	virtual int init(); // �h�����
	virtual int run(); // loop���s
	virtual int loop() = 0; // �h����Ŏ�ȏ���������

	void setWindowSize(int width, int height);
	HWND getHWND() const { return window_handle; }

	static LRESULT CALLBACK WinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam); // �C�x���g�����������Ȃ�h����ŐV������������
};


}

#endif