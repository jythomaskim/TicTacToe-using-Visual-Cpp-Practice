#include "stdafx.h"
#include "ctictactoe.h"


ctictactoe::ctictactoe()
{
}


ctictactoe::~ctictactoe()
{
}

void ctictactoe::setPlayerTurn(int &pturn)
{
	playerturn = pturn;
}
void ctictactoe::setGameboard(int cellnum, int &cellval)
{
	gameboard[cellnum] = cellval;
}
void ctictactoe::setWinner(int &win)
{
	winner = win;
}
//void ctictactoe::setWinCell(int val1, int val2, int val3)
//{
//	wincell[0] = val1;
//	wincell[1] = val2;
//	wincell[2] = val3;
//}
int ctictactoe::getPlayerTurn()
{
	return playerturn;
}
int ctictactoe::getGameboard(int cellnum)
{
	return gameboard[cellnum];
}
int ctictactoe::getWinner()
{
	return winner;
}
//int ctictactoe::getWinCell(int cellnum)
//{
//	return wincell[cellnum];
//}
void ctictactoe::restartGameboard()
{
	ZeroMemory(gameboard, sizeof(gameboard));
}
int ctictactoe::getcellsize() const
{
	return cellsize;
}

BOOL ctictactoe::GetGameboardRect(HWND hwnd, RECT *prect)	//function for obtaining coordinates for centering rectangle
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

void ctictactoe::DrawLine(HDC hdc, int x1, int y1, int x2, int y2)		//function to draw line from one coordinate to another
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

int ctictactoe::GetCellNumFromPoint(HWND hwnd, int x, int y)
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

BOOL ctictactoe::GetCellCoord(HWND hwnd, int index, RECT *cell)		//obtain coordinates for the clicked cell
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

int ctictactoe::checkWinner()
{
	int winningCell[] = { 0,1,2,3,4,5,6,7,8,0,3,6,1,4,7,2,5,8,0,4,8,2,4,6 };
	for (int i = 0; i < ARRAYSIZE(winningCell); i += 3)
	{
		if (0 != gameboard[winningCell[i]] && gameboard[winningCell[i]] == gameboard[winningCell[i + 1]]
			&& gameboard[winningCell[i]] == gameboard[winningCell[i + 2]])
		{
			wincell[0] = winningCell[i];
			wincell[1] = winningCell[i + 1];
			wincell[2] = winningCell[i + 2];
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

void ctictactoe::showTurn(HWND hwnd, HDC hdc)
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

void ctictactoe::DrawIconCentered(HDC hdc, RECT * cell, HICON hicon)
{
	const int iwidth = GetSystemMetrics(SM_CXICON);
	const int ilength = GetSystemMetrics(SM_CYICON);
	if (NULL != cell)
	{
		int left = cell->left + (cell->right - cell->left - iwidth) / 2;
		int top = cell->top + (cell->bottom - cell->top - ilength) / 2;
		DrawIcon(hdc, left, top, hicon);
	}
}

void ctictactoe::highlightWinner(HWND hWnd, HDC hdc, HBRUSH br1, HBRUSH br2, HICON h1, HICON h2)
{
	RECT winrc;
	for (int i = 0; i < 3; i++)
	{
		if (GetCellCoord(hWnd, wincell[i], &winrc))
		{
			FillRect(hdc, &winrc, (winner == 1) ? br1 : br2);
			DrawIconCentered(hdc, &winrc, (winner == 1) ? h1 : h2);
		}

	}
}