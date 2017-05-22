#define NAME "TextConv"
#define WINDOW_SIZE_W	800
#define WINDOW_SIZE_H	600
#define CHARA_MAX		5
#define BUTTON_CHARA	100
#define BUTTON_CHARA_EX	101
#define BUTTON_SAVE		102
#define BUTTON_LOAD		103
#include <Windows.h>
#include <windowsx.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "resource.h"


LPCTSTR strItem[] = {
	TEXT("シオン"),
	TEXT("メルエル"),
	TEXT("コウネ")
};

LPCTSTR strFace[] = {
	TEXT("標準"),
	TEXT("喜ぶ"),
	TEXT("怒る"),
	TEXT("哀しい"),
	TEXT("楽しい"),
};

//プロトタイプ
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int     WINAPI   WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND	hWnd;
	MSG		msg;

	// Set up and register window class
	WNDCLASS wc = { CS_CLASSDC,
		WndProc,                                //イベントcallback関数
		0,
		0,
		hInstance,
		NULL,                                   //アイコン
		LoadCursor(NULL, IDC_ARROW),          //マウスカーソル
		(HBRUSH)GetStockObject(WHITE_BRUSH), //背景色
		NULL,                                   //メニュー
		NAME };                                 //クラス名
	if (RegisterClass(&wc) == 0) return FALSE;

	//ウインドウ生成
	hWnd = CreateWindow(NAME,                  //タイトルバーテキスト
		NAME,                  //クラス名
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,   //ウインドウスタイル
		CW_USEDEFAULT,         //ウインドウ左上x座標
		CW_USEDEFAULT,         //ウインドウ左上y座標
		WINDOW_SIZE_W,         //ウインドウ幅
		WINDOW_SIZE_H,         //ウインドウ高さ
		NULL,                  //親ウインドウのハンドル
		NULL,
		hInstance,
		NULL);
	if (!hWnd) return FALSE;

	CreateWindow(
		TEXT("BUTTON"), TEXT("キャラ追加"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		WINDOW_SIZE_W - 160, 25, 160, 25,
		hWnd, (HMENU)BUTTON_CHARA, hInstance, NULL
	);

	CreateWindow(
		TEXT("BUTTON"), TEXT("表情"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		WINDOW_SIZE_W - 160, 75, 160, 25,
		hWnd, (HMENU)BUTTON_CHARA_EX, hInstance, NULL
	);

	CreateWindow(
		TEXT("BUTTON"), TEXT("保存"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		WINDOW_SIZE_W - 85, WINDOW_SIZE_H - 63, 73, 25,
		hWnd, (HMENU)BUTTON_SAVE, hInstance, NULL
	);

	CreateWindow(
		TEXT("BUTTON"), TEXT("読込"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		WINDOW_SIZE_W - 160, WINDOW_SIZE_H - 63, 75, 25,
		hWnd, (HMENU)BUTTON_LOAD, hInstance, NULL
	);

	ShowWindow(hWnd, nCmdShow);     //Window を表示
	UpdateWindow(hWnd);             //表示を初期化
	SetFocus(hWnd);                 //フォーカスを設定

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//Windws イベント用関数
LRESULT  CALLBACK  WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static HWND charabox, facebox, inputbox, statusbar;
	static OPENFILENAME ofn = { 0 };
	static char szFilePath[MAX_PATH] = "";

	char chtmp[64];
	std::string strtmp;
	int i, len;

	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CREATE:
		//-----------------
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hWnd;
		ofn.lpstrFilter = "会話テキスト(*.bin)\0*.bin\0";
		ofn.lpstrFile = szFilePath;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrDefExt = "bin";
		ofn.Flags = NULL;
		//--------------------------------------------------------------------
		inputbox = CreateWindow(
			TEXT("EDIT"), NULL,
			WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_MULTILINE | ES_WANTRETURN,
			0, 0, 640, 560, hWnd, (HMENU)1,
			((LPCREATESTRUCT)(lParam))->hInstance, NULL
		);
		//--------------------------------------------------------------------
		charabox = CreateWindow(
			TEXT("COMBOBOX"), NULL,
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWN,
			WINDOW_SIZE_W - 160, 0, 145, 300, hWnd, (HMENU)1,
			((LPCREATESTRUCT)(lParam))->hInstance, NULL);

		for (i = 0; i < 3; i++)
			SendMessage(charabox, CB_ADDSTRING, 0, (LPARAM)strItem[i]);

		ComboBox_SetCurSel(charabox, 0);
		//--------------------------------------------------------------------
		facebox = CreateWindow(
			TEXT("COMBOBOX"), NULL,
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWN,
			WINDOW_SIZE_W - 160, 50, 145, 300, hWnd, (HMENU)1,
			((LPCREATESTRUCT)(lParam))->hInstance, NULL);

		for (i = 0; i < 5; i++)
			SendMessage(facebox, CB_ADDSTRING, 0, (LPARAM)strFace[i]);

		ComboBox_SetCurSel(facebox, 0);
		//--------------------------------------------------------------------
		return 0;
		//-----------------
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case BUTTON_CHARA: {
			ComboBox_GetText(charabox, chtmp, 256);

			strtmp = chtmp;
			strtmp.insert(0, "[");
			strtmp += "]\r\n";
			strncpy_s(chtmp, strtmp.c_str(), 64);

			len = Edit_GetTextLength(inputbox);
			Edit_SetSel(inputbox, len, len);
			Edit_ReplaceSel(inputbox, chtmp);
			break;
		}
		case BUTTON_CHARA_EX: {
			ComboBox_GetText(facebox, chtmp, 256);

			strtmp = chtmp;
			strtmp.insert(0, "[");
			strtmp += "]\r\n";
			strncpy_s(chtmp, strtmp.c_str(), 64);

			len = Edit_GetTextLength(inputbox);
			Edit_SetSel(inputbox, len, len);
			Edit_ReplaceSel(inputbox, chtmp);
			break;
		}
		case BUTTON_SAVE: {
			ofn.Flags |= OFN_OVERWRITEPROMPT;
			if (GetSaveFileName(&ofn)) {
				char tmpstr[2048];
				int linecount = Edit_GetLineCount(inputbox);
				int textlen = Edit_GetTextLength(inputbox);
				int arrsize = 2048;
				int dummy = 0;

				Edit_GetText(inputbox, tmpstr, 2048);

				std::ofstream wfp(szFilePath, std::ios::out | std::ios::binary);
				wfp.write(reinterpret_cast<const char*>(&textlen), sizeof(textlen));
				wfp.write(reinterpret_cast<const char*>(&linecount), sizeof(linecount));
				wfp.write(reinterpret_cast<const char*>(&arrsize), sizeof(arrsize));
				wfp.write(reinterpret_cast<const char*>(&dummy), sizeof(dummy));

				wfp.write(reinterpret_cast<const char*>(&tmpstr), textlen * sizeof(char) + 1);

				wfp.close();

				MessageBox(hWnd, TEXT("保存しました。"), "確認", MB_OK);
			}
			break;
		}
		case BUTTON_LOAD: {
			ofn.Flags |= OFN_FILEMUSTEXIST;
			if (GetOpenFileName(&ofn)) {
				char tmpstr[2048];
				int linecount = 0;
				int textlen = 0;
				int arrsize = 0;
				int dummy = 0;

				std::ifstream ifw(szFilePath, std::ios::in | std::ios::binary);

				ifw.read(reinterpret_cast<char*>(&textlen), sizeof(textlen));
				ifw.read(reinterpret_cast<char*>(&linecount), sizeof(linecount));
				ifw.read(reinterpret_cast<char*>(&arrsize), sizeof(arrsize));
				ifw.read(reinterpret_cast<char*>(&dummy), sizeof(dummy));

				ifw.read(reinterpret_cast<char*>(&tmpstr), textlen * sizeof(char) + 1);

				ifw.close();

				Edit_SetText(inputbox, tmpstr);

				MessageBox(hWnd, TEXT("読み込みしました。"), "確認", MB_OK);
			}
			break;
		}
		}
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}