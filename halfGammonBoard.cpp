#include "HalfGammonBoard.h"
#include <iostream>
#include <iomanip>

using namespace std;

// Constructor: sets xTurn to true, and then initializes the x and o boards
HalfGammonBoard::HalfGammonBoard() {
	xTurn = true;

	for (int i = 0; i < ARRAY_SIZE; i++) {
		xBoard[i] = 0;
		oBoard[i] = 0;
	}

	int spacingOfSecondCheckers = 2;
	xBoard[1] = 5;
	xBoard[1 + spacingOfSecondCheckers] = 2;

	oBoard[VALID_SPACES] = 5;
	oBoard[VALID_SPACES - spacingOfSecondCheckers] = 2;
}

// Displays the board
const int CHARACTERS_PER_BOARD_POSITION = 3;
void HalfGammonBoard::displayBoard() {
	// Display the board characters
	for (int i = 7; i > 0; i--) {
		cout << " ";
		for (int j = 0; j < ARRAY_SIZE; j++) {
			if (xBoard[j] >= i) {
				cout << setw(CHARACTERS_PER_BOARD_POSITION) << left << "X";
			}
			else if (oBoard[j] >= i) {
				cout << setw(CHARACTERS_PER_BOARD_POSITION) << left << "O";
			}
			else {
				cout << setw(CHARACTERS_PER_BOARD_POSITION) << left << " ";
			}
		}

		cout << endl;
	}

	// Display the numbers below the spaces
	cout << " ";
	for (int j = 0; j < ARRAY_SIZE; j++) {
		if (j == 0 || j == ARRAY_SIZE - 1) {
			cout << setw(CHARACTERS_PER_BOARD_POSITION) << left << " ";
		}
		else {
			cout << setw(CHARACTERS_PER_BOARD_POSITION) << left << j;
		}
	}

	cout << endl << endl;
}

// Display the prompt
// Parameter: roll, the number to be displayed as a die roll
void HalfGammonBoard::displayRoll(int roll) {
	cout << "It's " << (xTurn ? "X" : "O") << "'s turn." << endl;
	cout << "Roll is " << roll << endl;
}

// gameOver, returns true if the game has ended (one player has won)
// Returns true if game is over, false otherwise
bool HalfGammonBoard::gameOver() {
	return isXWin() || isOWin();
}

// isXWin, determines if X has won the game
// Returns true if X has won, false otherwise
bool HalfGammonBoard::isXWin() {
	for (int i = 0; i < ARRAY_SIZE; i++) {
		if (xBoard[i] != 0) {
			return false;
		}
	}

	return true;
}

// isOWin, determines if O has won the game
// Returns true if O has won, false otherwise
bool HalfGammonBoard::isOWin() {
	for (int i = 0; i < ARRAY_SIZE; i++) {
		if (oBoard[i] != 0) {
			return false;
		}
	}

	return true;
}

// Determines if this ending position is valid
// Parameter: position, int, indicating position where checker would end up
// Returns true if the position is a valid destination, false otherwise
bool HalfGammonBoard::isValidDestination(int position) {
	// If position is moving off the board, that is valid
	if (position <= O_HOME_LOCATION || position >= X_HOME_LOCATION) {
		return true;
	}

	// Otherwise check if there is 1 or fewer of opponent at that location, 
	// in which case it is valid
	if (xTurn) {
		return (oBoard[position] <= 1);
	}
	else {
		return (xBoard[position] <= 1);
	}
}

// Determines if the current player has a bumped piece
// Returns true if there is a bumped piece, false otherwise
bool HalfGammonBoard::hasBumpedPiece() {
	if (xTurn) {
		return xBoard[X_BUMP_LOCATION] > 0;
	}
	else {
		return oBoard[O_BUMP_LOCATION] > 0;
	}
}

// Determines if the current player has at least one possible move they can make with the given roll
// Parameter: moveSize, the roll that was chosen
// Returns true if there is at least one possible move, false if there are no possible moves
bool HalfGammonBoard::isMovePossible(int moveSize) {
	// If there is a player's piece that has been bumped, only that piece can move
	if (hasBumpedPiece()) {
		if (xTurn) {
			return isValidDestination(moveSize);
		}
		else {
			return isValidDestination(O_BUMP_LOCATION - moveSize);
		}
	}


	// No player's pieces have been bumped, check if any pieces on board can move
	// Check every square on board, if that square has a move that can be made, return true
	for (int i = 1; i < 1 + VALID_SPACES; i++) {
		if (xTurn) {
			if (xBoard[i] > 0 && isValidDestination(i + moveSize)) {
				return true;
			}
		}
		else {
			if (oBoard[i] > 0 && isValidDestination(i - moveSize)) {
				return true;
			}
		}
	}

	// If we get here, we've been through all the spaces on the board and there were no possible moves
	return false;
}

// Performs the move as O, called by performMove
// Parameters:
// movePosition, int, the location that the player is moving from
// moveSize, int, the amount that the player is moving (the roll)
// Returns true if the move is successful, false otherwise
bool HalfGammonBoard::performOMove(int movePosition, int moveSize) {
	int moveDestination = movePosition - moveSize;
	if (moveDestination <= O_HOME_LOCATION) {
		// This moves O off the board to a winning position, remove piece and return true
		oBoard[movePosition]--;
		return true;
	}
	else if (xBoard[moveDestination] == 0) {
		// There are no Os at that location, add an X
		// (Works whether or not there are already Xs at that location)
		oBoard[moveDestination]++;
		oBoard[movePosition]--;
		return true;
	}
	else if (xBoard[moveDestination] == 1) {
		// There is one O at that location, bump it
		xBoard[moveDestination] = 0;
		xBoard[X_BUMP_LOCATION]++;
		oBoard[moveDestination] = 1;
		oBoard[movePosition]--;
		return true;
	}
	else {
		// There are two or more Xs at that location, can't move there
		return false;
	}
}

// Performs the move as X, called by performMove
// Parameters:
// movePosition, int, the location that the player is moving from
// moveSize, int, the amount that the player is moving (the roll)
// Returns true if the move is successful, false otherwise
bool HalfGammonBoard::performXMove(int movePosition, int moveSize) {
	int moveDestination = movePosition + moveSize;
	if (moveDestination >= X_HOME_LOCATION) {
		// This moves X off the board to a winning position, remove piece and return true
		xBoard[movePosition]--;
		return true;
	}
	else if (oBoard[moveDestination] == 0) {
		// There are no Os at that location, add an X
		// (Works whether or not there are already Xs at that location)
		xBoard[moveDestination]++;
		xBoard[movePosition]--;
		return true;
	}
	else if (oBoard[moveDestination] == 1) {
		// There is one O at that location, bump it
		oBoard[moveDestination] = 0;
		oBoard[O_BUMP_LOCATION]++;
		xBoard[moveDestination] = 1;
		xBoard[movePosition]--;
		return true;
	}
	else {
		// There are two or more Os at that location, can't move there
		return false;
	}
}

// Moves a bumped piece, to be called if player has a piece that has been bumped
// Parameters: roll, int, the amount to move the bumped piece
// Returns false if move is successful, false otherwise
bool HalfGammonBoard::moveBumpedPiece(int roll) {
	return performMove(xTurn ? X_BUMP_LOCATION : O_BUMP_LOCATION, roll);
}

// Change the current player (switches between X and O)
void HalfGammonBoard::changePlayer() {
	xTurn = !xTurn;
}

// Completes the user's move
// Parameters:
// movePosition, int, the location that the player is moving from
// moveSize, int, the amount that the player is moving (the roll)
// Returns true if the move is successful, false otherwise
bool HalfGammonBoard::performMove(int movePosition, int moveSize) {
	// If selected move is invalid, return false
	if (movePosition < 0 || movePosition >= ARRAY_SIZE) {
		return false;
	}

	// If player doesn't have a piece at the location to move from, move isn't possible
	if (xTurn && xBoard[movePosition] == 0) {
		return false;
	}
	else if (!xTurn && oBoard[movePosition] == 0) {
		return false;
	}

	if (xTurn) {
		return performXMove(movePosition, moveSize);
	}
	else {
		return performOMove(movePosition, moveSize);
	}
}
