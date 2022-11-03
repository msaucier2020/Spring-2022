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
	Node* current = new Node;
	char mainMenu[] = { "<E>dit / <M>ove / <R>eplace / <D>raw / <C>lear / <L>oad / <S>ave / <Q>uit: " };
	bool animate = false;
	char option;
	char filename[FILENAMESIZE - 15];
	List redoList;
	List undoList;
	List clips;
	
	//tells whether animate is on or not
	char animateYN = 'N';

	//temporary canvas for undo option
	char temp[MAXROWS][MAXCOLS];

	system("cls");
	initCanvas(current->item);

	do
	{
		system("cls");
		displayCanvas(current->item);
		animateYN = animate ? 'Y' : 'N';
		menuOne(current->item, undoList, redoList, clips, animate, animateYN);
		cout << mainMenu;
		cin >> option;
		cin.ignore();
		clearLine(MENUSPACE + 1, CLEAR);
		clearLine(MENUSPACE, CLEAR);
		switch (toupper(option))
		{
		case 'A':
			//toggle false/true animate bool and change menu option between N and Y
			animate = !animate;
			break;
		case 'U':
			restore(undoList, redoList, current);
			break;
		case 'I':
			//write code here to clip canvas
			addNode(clips, current);

			break;
		case 'O':
			//write code here for redo
			restore(redoList, undoList, current);
			break;
		case 'P':
			//write code here for play
			copyCanvas(temp, current->item);
			//play(current->item);
			break;
		//Main Menu Options
		case 'E':
			//add to undo list in case user wants to undo
			addUndoState(undoList, redoList, current);
			editCanvas(current->item);
			break;
		case 'M':
			//add to undo list in case user wants to undo
			addUndoState(undoList, redoList, current);
			// variables for how far to move columns/rows
			int rowValue;
			int colValue;
			cout << "Enter columns to move: ";
			cin >> colValue;
			cout << "Enter rows to move: ";
			cin >> rowValue;
			moveCanvas(current->item, rowValue, colValue);
			break;
		case 'R':
			//add to undo list in case user wants to undo
			addUndoState(undoList, redoList, current);
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
			replace(current->item, old, newCh);
			break;
		case 'D':
			// goes into draw menu
			menuTwo(current, undoList, redoList, clips, animate);
			break;
		case 'C':
			// clears the screen 
			//add to undo list in case user wants to undo
			addUndoState(undoList, redoList, current);
			initCanvas(current->item);
			break;
		case 'L':
			//add to undo list in case user wants to undo
			addUndoState(undoList, redoList, current);

			cout << "Enter file name: ";
			//allow user to enter file name

			loadCanvas(current->item, filename);
			break;
		case 'S':
			
			cout << "Enter file name: ";
			//allow user to enter file name

			saveCanvas(current->item, filename);
			break;
		case 'Q':
			delete current;
			deleteList(redoList);
			deleteList(undoList);
			deleteList(clips);
			return 1;
			break;
		}

	} while (option != 'Q');

	return 0;
}


void menuOne(char canvas[][MAXCOLS], List& undoList, List& redoList, List& clips, bool& animate, char animateYN)
{
	const int CLEAR = 90;
	const int MENUSPACE = MAXROWS + 1;

	clearLine(MENUSPACE, CLEAR);

	if (redoList.count > 0)
	{
		cout << "<A>nimate: " << animateYN << " / <U>ndo: " << undoList.count << " / Red<O>: " << redoList.count << " / Cl<I>p: " << clips.count << endl;
	}
	else if (clips.count >= 2)
	{
		cout << "<A>nimate: " << animateYN << " / <U>ndo: " << undoList.count << " / Cl<I>p: " << clips.count << " / <P>lay " << endl;
	}
	else if (redoList.count > 0 && clips.count >= 2)
	{
		cout << "<A>nimate: " << animateYN << " / <U>ndo: " << undoList.count << " / Red<O>: " << redoList.count << " / Cl<I>p: " << clips.count << " / <P>lay " << endl;
	}
	else
	{
		cout << "<A>nimate: " << animateYN << " / <U>ndo: " << undoList.count << " / Cl<I>p: " << clips.count << endl;
	}

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


bool saveCanvas(char canvas[][MAXCOLS], char filename[])
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
		return false;
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
	return true;
}


bool loadCanvas(char canvas[][MAXCOLS], char filename[])
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
		return false;
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
	return true;
}
