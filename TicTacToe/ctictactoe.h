#pragma once
class ctictactoe
{
private:
	const int cellsize = 100;	//cell size of gameboard
	int playerturn = 1;			//track player turn
	int gameboard[9] = {};		//represents board, keeps track of which player clicked which cell
	int winner = 0;				//state of the winner; 0: continue to play, 1/2: winner, 3: draw
	int wincell[3] = {};		//array for holding the winning line in terms of the cell index

public:
	ctictactoe();
	~ctictactoe();

	void setPlayerTurn(int &pturn);
	void setGameboard(int cellnum,int &cellval);
	void setWinner(int &winner);
	//void setWinCell(int val1, int val2, int val3);
	int getPlayerTurn();
	int getGameboard(int cellnum);
	int getWinner();
	//int getWinCell(int cellnum);
	void restartGameboard();
	int getcellsize() const;

	BOOL GetGameboardRect(HWND hwnd, RECT *prect);												//function for obtaining coordinates for centering rectangle
	void DrawLine(HDC hdc, int x1, int y1, int x2, int y2);										//function to draw line from one coordinate to another
	int GetCellNumFromPoint(HWND hwnd, int x, int y);											//function to obtain cell index num based on the clicked position, returns index of cell
	BOOL GetCellCoord(HWND hwnd, int index, RECT *cell);										//obtain coordinates for the clicked cell based on its index
	int checkWinner();																			//checks status of game, returns winner status (0:cont, 1/2: winner, 3:draw)
	void showTurn(HWND hwnd, HDC hdc);															//Displays player turn in window below boardgame
	void DrawIconCentered(HDC hdc, RECT * cell, HICON hicon);									//Displays icon centered at the cell
	void highlightWinner(HWND hWnd, HDC hdc, HBRUSH br1, HBRUSH br2, HICON h1, HICON h2);		//colors the winning line/cells
};

