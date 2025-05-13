// Phillip Mantatsky 5/1/25
// This project is the same as the game half gammon except uses a linked list that allows any turn
// to be changed to the previous and played from there.

#include <iostream>
#include <string>
#include "HalfGammonBoard.h"
#include "mersenne-twister.h"

// Snapshot of board state for undo
struct captureBoard {
	int xBoard[18];
	int oBoard[18];
	bool xTurn; // whose turn it is
	bool isAutoBump; // flag for forced bump move
	captureBoard* next; // stack link
	captureBoard(const HalfGammonBoard& board, bool autoBump = false) : xTurn(board.xTurn), isAutoBump(autoBump), next(nullptr) {
		for (int i = 0; i < 18; i++) {
			xBoard[i] = board.xBoard[i];
			oBoard[i] = board.oBoard[i];
		}
	}
};

// stack of captureBoard nodes
class UndoStack {
	captureBoard* head = nullptr;

	public:
		UndoStack() : head(nullptr) {}
		~UndoStack() {
			while(head) {
				auto toDelete = head;
				head = head->next;
				delete toDelete;
			}
		}
	
	// can undo past all auto bumps to a user move
	bool canUndo() const {
		captureBoard* temp = head;
		while (temp && temp->isAutoBump) {
			temp = temp->next;
		}
		return temp && temp->next;
	}


	// push current board onto stack
	void push(const HalfGammonBoard& board, bool autoBump) {
		captureBoard* node = new captureBoard(board, autoBump);
		node->next = head;
		head = node;
	}

	// pop nodes (skips auto bumps + one user move) and restore
	bool undo(HalfGammonBoard& board) {
		if (!canUndo()) {
			return false;
		}
		// drop all auto bump states
		while (head && head->isAutoBump) {
			auto* toDelete = head;
			head = head->next;
			delete toDelete;
		}
	
		// drop the user move state
		auto* toDelete = head;
		head = head->next;
		delete toDelete;

		if (!head) {
			return false;
		}
		
		// restore board arrays + turn
		for (int i = 0; i < 18; i++) {
			board.xBoard[i] = head->xBoard[i];
			board.oBoard[i] = head->oBoard[i];
		}
		board.xTurn = head->xTurn;
		return true;
	}
};

using namespace std;

int rollDie();

// Simulates rolling a die, choosing a result 1 to 6
// The seed function must have already been called
// Returns an int, chosen randomly, 1-6
int rollDie() {
	return chooseRandomNumber(1, 6);
}


int main() {
	// Initializes the random number generator with seed from the user
	int randSeed;
	cout << "Enter seed: ";
	cin >> randSeed;
	seed(randSeed);

	// Repeat, allows user to play multiple games
	string keepPlaying;
	do {
		UndoStack undo;
		// Game board used to keep track of the current game
		HalfGammonBoard board;
		undo.push(board, false); // save start state

		// Display the board and roll dice
		
		int roll = rollDie();
		board.displayBoard();
        board.displayRoll(roll);
		while (!board.gameOver()) {
			// handle forced bump
			if (board.hasBumpedPiece()) {
				cout << "Bumped checker must move." << endl;
				if (!board.isMovePossible(roll)) {
					cout << "No move possible." << endl;
					continue;
				}
				board.moveBumpedPiece(roll);
				// treat exactly like a normal move:
				board.changePlayer();
				undo.push(board, true); // mark auto bump
				roll = rollDie();
				board.displayBoard();
                board.displayRoll(roll);
				continue;
			}
			// user prompt
			cout << "What position would you like to move (Q to quit, U to undo)? ";
			string cmd;
			cin >> cmd;

			// undo
			if (cmd == "U" || cmd == "u") {
				if (!undo.canUndo()) {
					cout << "Cannot undo." << endl;
				} else {
					undo.undo(board);
					roll = rollDie();
					board.displayBoard();
            		board.displayRoll(roll);
				}
				continue;
			}

			// quit game
			if (cmd == "Q" || cmd == "q") {
				break;
			}


			// Regular move 
			if (!board.isMovePossible(roll)) {
				cout << "No move possible." << endl << endl;
				continue;
			}

			int pos = stoi(cmd);
			if (!board.performMove(pos, roll)) {
				cout << "Invalid move. Try again." << endl;
				continue;
			}

			// a successful user move:
			board.changePlayer();
			undo.push(board, false); // user move
			roll = rollDie();
			board.displayBoard();
            board.displayRoll(roll);
		}
		// If we have left the loop, someone has won-determine whether it's X or O
		if (board.isXWin()) {
			cout << "Player X Wins!" << endl;
		}
		else if (board.isOWin()) {
			cout << "Player O Wins!" << endl;
		}

		cout << endl;
		cout << "Do you want to play again (y/n)? ";
		cin >> keepPlaying;
	} while (tolower(keepPlaying.at(0)) == 'y');
}
