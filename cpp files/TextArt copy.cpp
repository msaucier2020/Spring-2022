/*
* This program allows display and editing of text art (also called ASCII art).
*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include <limits>
#include <cstring>
using namespace std;

const int BUFFERSIZE = 20;
const int FILENAMESIZE = 255;
const int MAXROWS = 22;
const int MAXCOLS = 80;

// ASCII codes for special keys; for editing
const char ESC = 27;
const char LEFTARROW = 75;
const char UPARROW = 72;
const char RIGHTARROW = 77;
const char DOWNARROW = 80;
const char SPECIAL = (char)224;

/*
* Gets a filename from the user. If file can be opened for reading,
* this function loads the file's contents into canvas.
* File is a TXT file located in the SavedFiles folder.
* If file cannot be opened, error message is displayed and
* canvas is left unchanged.
*/
void loadCanvas(char canvas[][MAXCOLS]);

/*
* Gets a filename from the user. If file can be opened for writing,
* this function writes the canvas contents into the file.
* File is a TXT file located in the SavedFiles folder.
* If file cannot be opened, error message is displayed.
*/
void saveCanvas(char canvas[][MAXCOLS]);

/*
* Initializes canvas to contain all spaces.
*/
void initCanvas(char canvas[][MAXCOLS]);

/*
* Displays canvas contents on the screen, with a border
* around the right and bottom edges.
*/
void displayCanvas(char canvas[][MAXCOLS]);

/*
* Allows user to edit the canvas by moving the cursor around and
* entering characters. Editing continues until the ESC key is pressed.
*/
void editCanvas(char canvas[][MAXCOLS]);

/*
* Copies contents of the "from" canvas into the "to" canvas.
*/
void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS]);

/*
* Replaces all instances of a character in the canvas.
* oldCh is the character to be replaced.
* newCh character is the character to replace with.
*/
void replace(char canvas[][MAXCOLS], char oldCh, char newCh);

/*
* Shifts contents of the canvas by a specified number of rows and columns.
* rowValue is the number of rows by which to shift
*    positive numbers shift downward; negative numbers shift upward
* colValue is the number of rows by which to shift
*    positive numbers shift right; negative numbers shift left
*/
void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue);

/*
* Clears a line on the output screen, then resets the cursor back to the
* beginning of this line.
* lineNum is the line number on the output screen to clear
* numOfChars is the number of characters to clear on this line
*/
void clearLine(int lineNum, int numOfChars);

/*
* Moves the cursor in the output window to a specified row and column.
* The next output produced by the program will begin at this position.
*/
void gotoxy(short row, short col);


int main()
{
	char canvas[MAXROWS][MAXCOLS];
	char option;

	//temporary canvas for undo option
	char temp[MAXROWS][MAXCOLS];

	system("cls");
	initCanvas(canvas);
	displayCanvas(canvas);
	cout << "<E>dit / <M>ove / <R>eplace / <U>ndo / <L>oad / <S>ave / <Q>uit: ";
	do
	{
		cin >> option;
		switch (option)
		{
		case 'E':
		case 'e':
			//copy to temp in case user wants to undo
			copyCanvas(temp, canvas);
			editCanvas(canvas);
			break;
		case 'M':
		case 'm':

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
		case 'r':

			//copy to temp in case user wants to undo
			copyCanvas(temp, canvas);
			//variables to get which char to replace and what to replace it with
			char old;
			char newCh;
			cout << "Enter character to replace: ";
			cin >> old;
			cout << "Enter character to replace with: ";
			cin >> newCh;
			replace(canvas, old, newCh);
			break;
		case 'U':
		case 'u':
			//revert canvas to temp canvas
			copyCanvas(canvas, temp);
			break;
		case 'L':
		case 'l':
			loadCanvas(canvas);
			break;
		case 'S':
		case 's':
			saveCanvas(canvas);
			break;
		case 'Q':
		case 'q':
			return 1;
			break;
		}
	} while (option != 'E' || 'e' && option != 'M' || 'm' && option != 'R' || 'r' && option != 'U' || 'u' && option != 'L' || 'l' && option != 'S' || 's' && option != 'Q' || 'q');

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
				oldCh = newCh;
			}
		}
	}
}


void editCanvas(char canvas[][MAXCOLS])
{
	char input;
	int row = 0, col = 0;

	// Move cursor to row,col and then get
	// a single character from the keyboard
	gotoxy(row, col);
	input = _getch();

	//while loop for continuous input until user hits escape key
	while (input != ESC)
	{
		switch (input)
		{
		case SPECIAL:
			input = _getch();

			if (input == DOWNARROW)
			{
				col++;
				gotoxy(row, col);
			}
			else if (input == UPARROW)
			{
				col--;
				gotoxy(row, col);
			}
			else if (input == LEFTARROW)
			{
				row--;
				gotoxy(row, col);
			}
			else if (input == RIGHTARROW)
			{
				row++;
				gotoxy(row, col);
			}
			break;
		case '\0':
			input = _getch();
			break;
		default:
			if (input >= 32 && input <= 126)
			{
				canvas[row][col] = input;
				cout << input;
			}
			break;
		}

		input = _getch();
	}

	gotoxy(MAXROWS, MAXCOLS - 16);
}


void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue)
{
	char temp[MAXROWS][MAXCOLS];
	int rows, cols;

	for (rows = 0; rows == MAXROWS; rows++) {
		for (cols = 0; cols == MAXCOLS; cols++) {
			temp[rows + rowValue][cols + colValue] = canvas[rows][cols];
		}
	}
}


void initCanvas(char canvas[][MAXCOLS])
{
	// 
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

	// limit is max #s of chars the file name can be without SavedFiles\\ and .txt
	const int titleLimit = FILENAMESIZE - 15;
	char fileTitle[titleLimit];
	char fullName[FILENAMESIZE] = "SavedFiles\\";
	const char fileType[5] = ".txt";

	cout << "Enter file name: ";
	cin.getline(fileTitle, titleLimit);

	if (cin.fail())
	{
		cin.clear();
		cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	}

	//find size of name array for strncat parameter
	int length = strlen(fileTitle);
	//put together SavedFile\ and name from user
	strncat(fullName, fileTitle, FILENAMESIZE - length);

	//find size of fullName for strncat
	length = strlen(fullName);
	//put .txt at the end of file name
	strncat(fullName, fileType, FILENAMESIZE - length);


	ofstream outFile(fullName);

	if (!outFile)
	{
		cerr << "ERROR: File could not be written\n";
		system("pause");
		return;
	}

	for (int row = 0; row < MAXROWS; row++)
	{
		for (int col = 0; col < MAXCOLS; col++)
		{
			outFile << canvas[row][col];
		}

		outFile << endl;
	}


	outFile.close();
	cout << "File saved!\n";
	system("pause");
	return;
}


void loadCanvas(char canvas[][MAXCOLS])
{
	int length;	
	const int titleLimit = FILENAMESIZE - 15;
	char fileTitle[titleLimit];
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

	//put together filePath and fileTitle
	length = strlen(fileTitle);
	strncat(filePath, fileTitle, FILENAMESIZE - length);

	//put together filePath and fileType
	length = strlen(filePath);
	strncat(filePath, fileType, FILENAMESIZE - length);

	ifstream usingFile;

	usingFile.open(filePath);
	
		if (!usingFile)
		{
			cerr << "ERROR: File could not be opened\n";
			system("pause");
			return;
		}	
		
			//reading the file
			char ch;
			ch = usingFile.get();

			while (!usingFile.eof())
			{
				for (int r = 0; r < MAXROWS; r++)
				{
					for (int c = 0; c < MAXCOLS; c++)
					{
						canvas[r][c] = ch;
						ch = usingFile.get();
					}
				}
			}
			usingFile.close();

			for (int i = 0; i < MAXROWS; i++)
			{
				for(int j = 0; i < MAXCOLS; j++)
				{
					cout << canvas[i][j];
				}
			}
}