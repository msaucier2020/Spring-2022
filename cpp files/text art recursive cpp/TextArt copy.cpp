/*
* This program allows display and editing of text art (also called ASCII art).
*/

#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include <limits>
#include <cstring>
#include "Definitions.h"
using namespace std;


int main()
{
	char canvas[MAXROWS][MAXCOLS];
	char mainMenu[] = { "<E>dit / <M>ove / <R>eplace / <D>raw / <C>lear / <U>ndo / <L>oad / <S>ave / <Q>uit: " };
	int length = sizeof(mainMenu) / sizeof(mainMenu[0]);
	bool animate = false;
	char option;

	//temporary canvas for undo option
	char temp[MAXROWS][MAXCOLS];

	system("cls");
	initCanvas(canvas);

	do
	{
		displayCanvas(canvas);
		cout << mainMenu;
		cin >> option;
		cin.ignore();
		clearLine(MAXROWS + 1, length);
		switch (toupper(option))
		{
		case 'E':
			//copy to temp in case user wants to undo
			copyCanvas(temp, canvas);
			editCanvas(canvas);
			break;
		case 'M':
			//copy to temp in case user wants to undo
			copyCanvas(temp, canvas);
			// variables for how far to move columns/rows
			int rowValue;
			int colValue;
			cout << "Enter columns to move: ";
			cin >> colValue;
			cout << "Enter rows to move: ";
			cin >> rowValue;
			moveCanvas(canvas, rowValue, colValue);
			break;
		case 'R':
			//copy to temp in case user wants to undo
			copyCanvas(temp, canvas);
			//variables to get which char to replace and what to replace it with
			char old;
			char newCh;
			//get old character and ignore any input after 1 keystroke
			cout << "Enter character to replace: ";
			cin.get(old);
			cin.ignore();
			//get new character and ignore any input after 1 keystroke
			cout << "Enter character to replace with: ";
			cin.get(newCh);
			cin.ignore();
			//replace old characters with new
			replace(canvas, old, newCh);
			break;
		case 'D':
			//not positive what to do with this
			char backupCanvas[MAXROWS][MAXCOLS];
			menuTwo(canvas, backupCanvas, animate);
			break;
		case 'C':
			//I don't know if this is right, but it works. 
			copyCanvas(temp, canvas);
			initCanvas(canvas);
			break;
		case 'U':
			//revert canvas to temp canvas
			copyCanvas(canvas, temp);
			break;
		case 'L':
			copyCanvas(temp, canvas);
			loadCanvas(canvas);
			break;
		case 'S':
			saveCanvas(canvas);
			break;
		case 'Q':
			return 1;
			break;
		}
	} while (option != 'Q');

	return 0;
}


void gotoxy(short row, short col)
{
	COORD pos = { col, row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


void clearLine(int lineNum, int numOfChars)
{
	// Move cursor to the beginning of the specified line on the console
	gotoxy(lineNum, 0);

	// Write a specified number of spaces to overwrite characters
	for (int x = 0; x < numOfChars; x++)
		cout << " ";

	// Move cursor back to the beginning of the line
	gotoxy(lineNum, 0);
}


void replace(char canvas[][MAXCOLS], char oldCh, char newCh)
{
	// loop to go through each row
	for (int row = 0; row < MAXROWS; row++)
	{
		//loop to go through each column
		for (int col = 0; col < MAXCOLS; col++)
		{
			//check if it is the char that needs to be replaced
			if (canvas[row][col] == oldCh)
			{
				//replace old char with new char
				canvas[row][col] = newCh;
			}
		}
	}
}


void editCanvas(char canvas[][MAXCOLS])
{
	cout << "Press <ESC> to stop editing";
	char input;
	int row = 0, col = 0;

	// Move cursor to row,col and then get
	// a single character from the keyboard
	gotoxy(row, col);
	input = _getch();

	//while loop for continuous input until user hits escape key
	while (input != ESC)
	{
		if (input == SPECIAL)
		{
			input = _getch();

			switch (input)
			{
			case DOWNARROW:
				if (row < MAXROWS - 1)
				{
					row++;
				}
				break;
			case UPARROW:
				if (row > 0)
				{
					row--;
				}
				break;
			case LEFTARROW:
				if (col > 0)
				{
					col--;
				}
				break;
			case RIGHTARROW:
				if (col < MAXCOLS - 1)
				{
					col++;
				}
				break;
			}
		}

		else if (input == '\0')
		{
			input = _getch();
		}
		else if (32 <= input && input <= 126)
		{
			canvas[row][col] = input;
			printf("%c", input);
		}

		gotoxy(row, col);
		input = _getch();
	}
}

void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue)
{
	char temp[MAXROWS][MAXCOLS];
	initCanvas(temp);
	int rows, cols;

	for (rows = 0; rows < MAXROWS; rows++) {
		for (cols = 0; cols < MAXCOLS; cols++) {
			if (rows + rowValue < MAXROWS && cols + colValue < MAXCOLS && rows + rowValue >= 0 && cols + colValue >= 0) {
				temp[rows + rowValue][cols + colValue] = canvas[rows][cols];
			}
		}
	}

	copyCanvas(canvas, temp);
}


void initCanvas(char canvas[][MAXCOLS])
{
	//Searches through rows and columns and adds a space
	for (int row = 0; row < MAXROWS; row++) {
		for (int col = 0; col < MAXCOLS; col++) {
			canvas[row][col] = ' ';
		}
	}
}


void displayCanvas(char canvas[][MAXCOLS])
{
	// Clear the screen
	system("cls");

	//count through all rows
	for (int x = 0; x < MAXROWS; x++)
	{
		//count through all columns
		for (int y = 0; y < MAXCOLS; y++)
		{
			//print whatever is in said columns
			printf("%c", canvas[x][y]);

		}

		cout << "|\n";
	}

	for (int i = 0; i < MAXCOLS; i++)
	{
		cout << "-";
	}

	cout << endl;
}


void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS])
{
	for (int row = 0; row < MAXROWS; row++)
	{
		for (int col = 0; col < MAXCOLS; col++)
		{
			to[row][col] = from[row][col];
		}

	}

}


void saveCanvas(char canvas[][MAXCOLS])
{

	char fileTitle[FILENAMESIZE - 15];
	char fileType[5] = ".txt";
	char filePath[FILENAMESIZE] = "SavedFiles\\";

	//TO GET THE NAME OF THE FILE

	//get user input for file name to open
	cout << "Enter the file name (don't enter 'txt): ";
	cin.getline(fileTitle, FILENAMESIZE - 15);

	if (cin.fail())
	{
		cin.clear();
		cin.ignore((numeric_limits<streamsize>::max)(), '\n');

	}
	strncat_s(filePath, fileTitle, FILENAMESIZE);

	//put together filePath and fileType
	strncat_s(filePath, fileType, FILENAMESIZE);

	ofstream outFile;
	outFile.open(filePath);

	if (!outFile)
	{
		cerr << "ERROR: File could not be opened\n";
		system("pause");
		return;
	}
	else
	{

		for (int x = 0; x < MAXROWS; x++)
		{
			//count through all columns
			for (int y = 0; y < MAXCOLS; y++)
			{
				//print whatever is in said columns
				outFile << canvas[x][y];

			}

			outFile << "\n";
		}

	}
	cout << "File saved!\n";
	system("pause");
	return;
}


void loadCanvas(char canvas[][MAXCOLS])
{
	char fileTitle[FILENAMESIZE - 15];
	char fileType[5] = ".txt";
	char filePath[FILENAMESIZE] = "SavedFiles\\";

	//TO GET THE NAME OF THE FILE

	//get user input for file name to open
	cout << "Enter the file name (don't enter 'txt): ";
	cin.getline(fileTitle, FILENAMESIZE - 15);

	if (cin.fail())
	{
		cin.clear();
		cin.ignore((numeric_limits<streamsize>::max)(), '\n');

	}
	strncat_s(filePath, fileTitle, FILENAMESIZE);

	//put together filePath and fileType
	strncat_s(filePath, fileType, FILENAMESIZE);

	ifstream usingFile;
	usingFile.open(filePath);

	if (!usingFile)
	{
		cerr << "ERROR: File could not be opened\n";
		system("pause");
		return;
	}
	else
	{
		initCanvas(canvas);
		//reading the file
		char ch;
		ch = usingFile.get();


		for (int r = 0; r < MAXROWS && !usingFile.eof(); r++)
		{
			for (int c = 0; c < MAXCOLS && ch != '\n'; c++)
			{
				canvas[r][c] = ch;
				ch = usingFile.get();
			}
			while (ch != '\n' && !usingFile.eof()) {
				ch = usingFile.get();
			}
			ch = usingFile.get();
		}

		usingFile.close();
	}
}