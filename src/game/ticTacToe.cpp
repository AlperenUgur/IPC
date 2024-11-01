#include <iostream>
using namespace std;

class Board
{
private:
	char board[3][3];

public:
	// Constructor initializes an empty board
	Board()
	{
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				board[i][j] = ' ';
			}
		}
	}
	// Function to draw the Tic-Tac-Toe board
	void drawBoard()
	{
		cout << "-------------\n";
		for(int i = 0; i < 3; i++)
		{
			cout << "| ";
			for(int j = 0; j < 3; j++)
			{
				cout << board[i][j] << " | ";
			}
			cout << "\n-------------\n";
		}
	}
	string getBoardAsString()
	{
		string boardString;
		boardString += "-------------\n";
		for(int i = 0; i < 3; i++)
		{
			boardString += "| ";
			for(int j = 0; j < 3; j++)
			{
				boardString += (board[i][j] == ' ') ? ' ' : board[i][j];
				boardString += " | ";
			}
			boardString += "\n-------------\n";
		}
		return boardString;
	}
	// Function to make a move, returns true if successful
	bool move(int row, int col, char player)
	{
		if(row >= 0 && row < 3 && col >= 0 && col < 3 && board[row][col] == ' ')
		{
			board[row][col] = player;
			return true;
		};
		return false;
	}
	// Function to check if a player has won
	bool checkWin(char player)
	{
		// check rows and columns
		for(int i = 0; i < 3; i++)
		{
			if(board[i][0] == player && board[i][1] == player && board[i][2] == player)
			{
				return true;
			}
			if(board[0][i] == player && board[1][i] == player && board[2][i] == player)
			{
				return true;
			}
		}
		// check diag
		if(board[0][0] == player && board[1][1] == player && board[2][2] == player)
		{
			return true;
		}
		if(board[0][2] == player && board[1][1] == player && board[2][0] == player)
		{
			return true;
		}
		return false;
	}
	// Function to check if the board is full(draw)
	bool isFull()
	{
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				if(board[i][j] == ' ')
				{
					return false;
				}
			}
		}
		return true;
	}
};
// Player class: Manages a player's actions
class Player
{
private:
	char symbol; // 'X' or 'O'
public:
	// Constructor to initialize player's symbol
	Player(char sym)
		: symbol(sym)
	{ }

	char getSymbol()
	{
		return symbol;
	}
	// Function to allow the player to make a move on the board
	void makeMove(Board& Board)
	{
		int row, col;
		while(true)
		{
			cout << "player " << symbol << ", enter row (0-2) and column (0-2): \n";
			cin >> row >> col;
			if(Board.move(row, col, symbol))
			{
				break;
			}
			else
			{
				cout << "invalid move ";
			}
		}
	}
};

// Game class: Manages the game flow
class Game
{
private:
	Board board; // Game board
	Player player1; // player "X"
	Player player2; // player "O"
	Player* currentPlayer; // Pointer to the current player

public:
	// Constructor initializes the game with two players and sets the first player
	// to X
	Game()
		: player1('X')
		, player2('O')
		, currentPlayer(&player1)
	{ }
	// Function to switch turns between players
	void switchTurn()
	{
		currentPlayer = (currentPlayer == &player1) ? &player2 : &player1;
	}
	// Function to handle the main game loop
	void play()
	{
		cout << "Welcome to Tic-Tac-Toe!\n";
		board.drawBoard();

		// Main game loop
		while(true)
		{
			currentPlayer->makeMove(board); // Current player makes a move
			board.drawBoard(); // Draw the board after the move

			// Check for a win
			if(board.checkWin(currentPlayer->getSymbol()))
			{
				cout << "Player " << currentPlayer->getSymbol() << " wins!\n";
				break;
			}
			// check for a draw
			if(board.isFull())
			{
				cout << "It's a draw!\n";
				break;
			}
			switchTurn();
		}
	}
};
// // Main function to start the game
// int main(){
//     Game game;   // Create a new game instance
//     game.play(); // Start the game
//     return 0;

// }