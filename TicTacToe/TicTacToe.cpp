// TicTacToe.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TicTacToe.h"
#include "Windowsx.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TICTACTOE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);			//change background color to gray
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TICTACTOE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

ShowWindow(hWnd, nCmdShow);
UpdateWindow(hWnd);

return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

//Global Variables
const int cellsize = 100;
HBRUSH bluebr1, redbr2;
int playerturn = 1;
int gameboard[9] = {};
int winner = 0;
int wincell[3] = {};

BOOL GetGameboardRect(HWND hwnd, RECT *prect)	//function for obtaining coordinates for centering rectangle
{
	RECT	rc;
	if (GetClientRect(hwnd, &rc))		//obtains the client window size (top left is zero, bottom and right is width/length)
	{
		int width = rc.right - rc.left;
		int length = rc.bottom - rc.top;
		prect->left = (width - cellsize * 3) / 2;
		prect->top = (length - cellsize * 3) / 2;
		prect->right = prect->left + cellsize * 3;
		prect->bottom = prect->top + cellsize * 3;
		return TRUE;
	}
	SetRectEmpty(prect);
	return FALSE;
}

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2)		//function to draw line from one coordinate to another
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

int GetCellNumFromPoint(HWND hwnd, int x, int y)
{
	RECT rc;
	if (GetGameboardRect(hwnd, &rc))
	{
		POINT	pt = { x,y };		//same as pt.x=x and pt.y=y
		if (PtInRect(&rc, pt))	//clicked inside the box
		{
			//normalize the coordinate so 0,0 is the upper left corner of rect
			int x = pt.x - rc.left;
			int y = pt.y - rc.top;
			int col = x / cellsize;
			int row = y / cellsize;
			return col + row * 3;

		}
		else //user clicked outside box
		{
			return -1;
		}
	}
	SetRectEmpty(&rc);
}

BOOL GetCellCoord(HWND hwnd, int index, RECT *cell)		//obtain coordinates for the clicked cell
{
	RECT rc;
	SetRectEmpty(cell);
	if (index < 0 || index > 8)
	{
		return FALSE;
	}
	if (GetGameboardRect(hwnd, &rc))
	{
		int x = index % 3;	//col
		int y = index / 3;	//row
		cell->left = rc.left + cellsize*x + 1;
		cell->top = rc.top + cellsize*y + 1;
		cell->right = cell->left + cellsize - 1;
		cell->bottom = cell->top + cellsize - 1;
		return TRUE;
	}
	return FALSE;
}

int checkWinner(int gameboard[])
{
	int winningCell[] = { 0,1,2,3,4,5,6,7,8,0,3,6,1,4,7,2,5,8,0,4,8,2,4,6 };
	for (int i = 0; i < ARRAYSIZE(winningCell); i += 3)
	{
		if (0 != gameboard[winningCell[i]] && gameboard[winningCell[i]] == gameboard[winningCell[i + 1]]
			&& gameboard[winningCell[i]] == gameboard[winningCell[i + 2]])
		{
			wincell[0] = gameboard[winningCell[i]];
			wincell[1] = gameboard[winningCell[i + 1]];
			wincell[2] = gameboard[winningCell[i + 2]];
			return gameboard[winningCell[i]];
		}
	}

	for (int i = 0; i < ARRAYSIZE(winningCell); i++)
	{
		if (0 == gameboard[winningCell[i]])		//check if there are any empty cells (zeroes)
		{
			return 0;	//continue if there is at least one empty cell
		}
	}
			return 3;	//draw
}

void showTurn(HWND hwnd,HDC hdc)
{
	static const WCHAR plr1[] = L"Turn: Player 1";
	static const WCHAR plr2[] = L"Turn: Player 2";
	const WCHAR * plrturn = NULL;
	
	switch (winner)
	{
	case 0:	
		plrturn = (playerturn == 1) ? plr1 : plr2;
		break;
	case 1:
		plrturn = L"Player 1 Wins!";
		break;
	case 2:
		plrturn = L"Player 2 Wins!";
		break;
	case 3:
		plrturn = L"It's a draw!";
		break;
	}

	RECT rc;
	if (NULL != plrturn && GetClientRect(hwnd, &rc))
	{
		rc.top = rc.bottom - 48;
		FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		DrawText(hdc, plrturn, lstrlen(plrturn), &rc, DT_CENTER);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		bluebr1 = CreateSolidBrush(RGB(0, 0, 255));
		redbr2 = CreateSolidBrush(RGB(255, 0, 0));
	}
	break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_FILE_NEWGAME:
			{
				int yesno = MessageBox(hWnd, L"Are you sure you want to start a new game?", L"New Game", MB_YESNO | MB_ICONQUESTION);
				if (IDYES == yesno)
				{
					playerturn = 1;
					winner = 0;
					ZeroMemory(gameboard, sizeof(gameboard));
					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd);
				}
			}
				break;
				
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_GETMINMAXINFO:		//setting a minimum size for resizing the window
		{
			MINMAXINFO *pminmax = (MINMAXINFO*)lParam;
			pminmax->ptMinTrackSize.x = cellsize * 5;
			pminmax->ptMinTrackSize.y = cellsize * 5;
		}
	case WM_LBUTTONDOWN:
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		//only handle clicks if its player's turn (if playerturn==0 game ended)
		if (0 == playerturn)
			break;

		int index = GetCellNumFromPoint(hWnd, xPos, yPos);		//tell value based on where it is clicked
		HDC	hdc = GetDC(hWnd);
		if (NULL != hdc) 
		{
			//WCHAR txt[100];		//must define in wchar because textout will take in tchar format
			//wsprintf(txt, L"Index = %d", index);
			//TextOut(hdc, xPos, yPos, txt, lstrlen(txt));
			
			//coloring clicked cell
			if (-1 != index)
			{
				RECT cell;
				if (0==gameboard[index] && GetCellCoord(hWnd, index, &cell))
				{
					gameboard[index] = playerturn;
					FillRect(hdc, &cell, (playerturn == 1) ? bluebr1 : redbr2);

					//check for winner
					winner = checkWinner(gameboard);
					if (winner == 1 || winner == 2)
					{
						MessageBox(hWnd, (winner == 1) ? L"Player 1 is the winner!" 
							: L"Player 2 is the Winner!", L"You win!", MB_OK | MB_ICONINFORMATION);
						playerturn = 0;
					}
					else if (3 == winner)
					{
						MessageBox(hWnd, L"No one wins!", L"It's a draw!", MB_OK | MB_ICONEXCLAMATION);
						playerturn = 0;
					}
					else if (0 == winner)
					{
						playerturn = (playerturn == 1) ? 2 : 1;
					}	
				}
				//display player turn
				showTurn(hWnd, hdc);
			}

			ReleaseDC(hWnd, hdc);
		}

		
	}
	break;

    case WM_PAINT:				//The UI code, where you modify the client size window
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			RECT	rc;
			if (GetGameboardRect(hWnd, &rc))
			{
				RECT rect;

				if (GetClientRect(hWnd, &rect))
				{
					const WCHAR player1[] = L"Player 1";
					const WCHAR	player2[] = L"Player 2";

					SetBkMode(hdc, TRANSPARENT);
					//place player 1 and 2 text
					SetTextColor(hdc, RGB(255, 255, 0));
					TextOut(hdc, 16, 16, player1, lstrlen(player1));
					SetTextColor(hdc, RGB(0, 0, 255));
					TextOut(hdc, rect.right - 72, 16, player2, lstrlen(player2));

					//display player turn
					showTurn(hWnd, hdc);
				}
				

				FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
				//Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
			}

			//Draw lines
			for (int i = 0; i < 4; i++)
			{
				//vertical
				DrawLine(hdc, rc.left+cellsize*i, rc.top, rc.left+cellsize*i, rc.bottom);
				//horizontal
				DrawLine(hdc, rc.left, rc.top+cellsize*i, rc.right, rc.top+cellsize*i);
			}
			RECT cell;
			for (int i = 0; i < 9; i++)
			{
				if (0 != gameboard[i] && GetCellCoord(hWnd, i, &cell))
				{
					FillRect(hdc, &cell, (gameboard[i]==1) ? bluebr1 : redbr2);
				}
			}
			
			EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		DeleteObject(bluebr1);
		DeleteObject(redbr2);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
