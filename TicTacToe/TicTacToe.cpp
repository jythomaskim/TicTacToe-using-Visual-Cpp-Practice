// TicTacToe.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TicTacToe.h"
#include "Windowsx.h"
#include "ctictactoe.h"
#include "wchar.h"

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


ctictactoe ttt;
HBRUSH bluebr1, redbr2;		//defining color
HICON hicon1, hicon2;		//defining icons
int pturn = 0;
int win = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		bluebr1 = CreateSolidBrush(RGB(0, 0, 255));
		redbr2 = CreateSolidBrush(RGB(255, 0, 0));
		hicon1 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER1));
		hicon2 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER22));
		ttt.resetScore();
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
					pturn = 1;
					ttt.setPlayerTurn(pturn);
					win = 0;
					ttt.setWinner(win);
					ttt.restartGameboard();
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
			pminmax->ptMinTrackSize.x = ttt.getcellsize() * 5;
			pminmax->ptMinTrackSize.y = ttt.getcellsize() * 5;
		}
	case WM_LBUTTONDOWN:
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		//only handle clicks if its player's turn (if playerturn==0 game ended)
		if (0 == ttt.getPlayerTurn())
			break;

		int index = ttt.GetCellNumFromPoint(hWnd, xPos, yPos);		//tell value based on where it is clicked
		HDC	hdc = GetDC(hWnd);
		if (NULL != hdc) 
		{
			//WCHAR txt[100];		//must define in wchar because textout will take in tchar format
			//wsprintf(txt, L"Index = %d", index);
			//TextOut(hdc, xPos, yPos, txt, lstrlen(txt));
			
			//fill clicked cell with icon
			if (-1 != index)
			{
				RECT cell;
				if (0==ttt.getGameboard(index) && ttt.GetCellCoord(hWnd, index, &cell))
				{
					win = ttt.getPlayerTurn();
					ttt.setGameboard(index,win);
					//FillRect(hdc, &cell, (playerturn == 1) ? bluebr1 : redbr2);
					//DrawIcon(hdc, cell.left, cell.top, (playerturn == 1) ? hicon1 : hicon2);
					ttt.DrawIconCentered(hdc, &cell, (ttt.getPlayerTurn() == 1) ? hicon1 : hicon2);

					//check for winner
					win = ttt.checkWinner();
					ttt.setWinner(win);
					if (ttt.getWinner() == 1 || ttt.getWinner() == 2)
					{
						//highlight the winning line
						ttt.highlightWinner(hWnd, hdc, bluebr1, redbr2, hicon1, hicon2);
						
						//update score
						ttt.incrementScore(ttt.getWinner());

						MessageBox(hWnd, (ttt.getWinner() == 1) ? L"Player 1 is the winner!" 
							: L"Player 2 is the Winner!", L"You win!", MB_OK | MB_ICONINFORMATION);
						ttt.setPlayerTurn(0);
					}
					else if (3 == ttt.getWinner())
					{
						MessageBox(hWnd, L"No one wins!", L"It's a draw!", MB_OK | MB_ICONEXCLAMATION);
						ttt.setPlayerTurn(0);
					}
					else if (0 == ttt.getWinner())
					{
						ttt.setPlayerTurn((ttt.getPlayerTurn() == 1) ? 2 : 1);
					}	
				}
				//player1 score display
				ttt.displayScore(hWnd, hdc, 1);
				//player2 score display
				ttt.displayScore(hWnd, hdc, 2);

				//display player turn
				ttt.showTurn(hWnd, hdc);
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
			if (ttt.GetGameboardRect(hWnd, &rc))
			{
				RECT rect;

				if (GetClientRect(hWnd, &rect))
				{
					const WCHAR player1[] = L"Player 1";
					const WCHAR	player2[] = L"Player 2";

					SetBkMode(hdc, TRANSPARENT);
					//display PLAYER1 text
					SetTextColor(hdc, RGB(0, 0, 0));
					TextOut(hdc, 16, 16, player1, lstrlen(player1));
					DrawIcon(hdc, 24, 34, hicon1);

					//player1 score display
					ttt.displayScore(hWnd,hdc,1);
					
					//display PLAYER2 text
					SetTextColor(hdc, RGB(0, 0, 0));
					TextOut(hdc, rect.right - 72, 16, player2, lstrlen(player2));
					DrawIcon(hdc, rect.right - 64, 34, hicon2);

					//player2 score display
					ttt.displayScore(hWnd,hdc,2);

					//display player turn
					ttt.showTurn(hWnd, hdc);
				}
				

				FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
				//Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
			}

			//Draw lines
			for (int i = 0; i < 4; i++)
			{
				//vertical
				ttt.DrawLine(hdc, rc.left + ttt.getcellsize()*i, rc.top, rc.left + ttt.getcellsize()*i, rc.bottom);
				//horizontal
				ttt.DrawLine(hdc, rc.left, rc.top + ttt.getcellsize()*i, rc.right, rc.top + ttt.getcellsize()*i);
			}
			RECT cell;
			for (int i = 0; i < 9; i++)
			{
				if (0 != ttt.getGameboard(i) && ttt.GetCellCoord(hWnd, i, &cell))
				{
					//FillRect(hdc, &cell, (gameboard[i]==1) ? bluebr1 : redbr2);
					ttt.DrawIconCentered(hdc, &cell, (ttt.getGameboard(i) == 1) ? hicon1 : hicon2);

				}
			}
			
			//highlight the winning line
			if (ttt.getWinner() == 1 || ttt.getWinner() == 2)
			{
				ttt.highlightWinner(hWnd, hdc, bluebr1, redbr2, hicon1, hicon2);
			}
			
			EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		DeleteObject(bluebr1);
		DeleteObject(redbr2);
		DestroyIcon(hicon1);
		DestroyIcon(hicon2);
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
