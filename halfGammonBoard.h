#pragma once
class HalfGammonBoard
{
private:
	

public:
	static const int VALID_SPACES = 16;
	static const int ARRAY_SIZE = 18;
	static const int O_BUMP_LOCATION = 17;
	static const int X_BUMP_LOCATION = 0;
	static const int O_HOME_LOCATION = 0;
	static const int X_HOME_LOCATION = 17;

	int xBoard[ARRAY_SIZE];
	int oBoard[ARRAY_SIZE];
	bool xTurn;
	bool performXMove(int movePosition, int moveSize);
	bool performOMove(int movePosition, int moveSize);
	HalfGammonBoard();
	void displayBoard();
	void displayRoll(int roll);
	bool gameOver();
	bool isMovePossible(int move);
	bool performMove(int movePosition, int moveSize);
	bool moveBumpedPiece(int moveSize);
	bool isXWin();
	bool isOWin();
	bool isValidDestination(int position);
	bool hasBumpedPiece();
	void changePlayer();
};

