#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include <limits>
#include <cstring>
#include "Definitions.h"
using namespace std;

//Variables that could proably be in a better place but work fine for now here
char temp[MAXROWS][MAXCOLS];
int row = 0;
int col = 0;
int trunk = 270;
char oldCh;
char newCh;
bool animate = false;

DrawPoint start;
DrawPoint endP;
Point startPt;
Point center;
Point endPt;

char input;
int height;

Point::Point(DrawPoint p)
{
	row = (int)round(p.row);
	col = (int)round(p.col);
}

// https://math.stackexchange.com/questions/39390/determining-end-coordinates-of-line-with-the-specified-length-and-angle
DrawPoint findEndPoint(DrawPoint start, int len, int angle)
{
	DrawPoint end;
	end.col = start.col + len * cos(degree2radian(angle));
	end.row = start.row + len * sin(degree2radian(angle));
	return end;
}


// Use this to draw characters into the canvas, with the option of performing animation
void drawHelper(char canvas[][MAXCOLS], Point p, char ch, bool animate)
{
	// Pause time between steps (in milliseconds)
	const int TIME = 50;

	// Make sure point is within bounds
	if (p.row >= 0 && p.row < MAXROWS && p.col >= 0 && p.col < MAXCOLS)
	{
		// Draw character into the canvas
		canvas[p.row][p.col] = ch;

		// If animation is enabled, draw to screen at same time
		if (animate)
		{
			gotoxy(p.row, p.col);
			printf("%c", ch);
			Sleep(TIME);
		}
	}
}


// Fills gaps in a row caused by mismatch between match calculations and screen coordinates
// (i.e. the resolution of our 'canvas' isn't very good)
void drawLineFillRow(char canvas[][MAXCOLS], int col, int startRow, int endRow, char ch, bool animate)
{
	// determine if we're counting up or down
	if (startRow <= endRow)
		for (int r = startRow; r <= endRow; r++)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
	else
		for (int r = startRow; r >= endRow; r--)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
}


// Draw a single line from start point to end point
void drawLine(char canvas[][MAXCOLS], DrawPoint start, DrawPoint end, bool animate)
{
	char ch;

	Point scrStart(start);
	Point scrEnd(end);

	// vertical line
	if (scrStart.col == scrEnd.col)
	{
		ch = '|';

		drawLineFillRow(canvas, scrStart.col, scrStart.row, scrEnd.row, ch, animate);
	}
	// non-vertical line
	else
	{
		int row = -1, prevRow;

		// determine the slope of the line
		double slope = (start.row - end.row) / (start.col - end.col);

		// choose appropriate characters based on 'steepness' and direction of slope
		if (slope > 1.8)  ch = '|';
		else if (slope > 0.08)  ch = '`';
		else if (slope > -0.08)  ch = '-';
		else if (slope > -1.8) ch = '\'';
		else ch = '|';

		// determine if columns are counting up or down
		if (scrStart.col <= scrEnd.col)
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col <= scrEnd.col; col++)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
		else
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col >= scrEnd.col; col--)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
	}
}


// Draws a single box around a center point
void drawBox(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	int sizeHalf = height / 2;
	int ratio = (int)round(MAXCOLS / (double)MAXROWS * sizeHalf);

	// Calculate where the four corners of the box should be
	DrawPoint points[4];
	points[0] = DrawPoint(center.row - sizeHalf, center.col - ratio);
	points[1] = DrawPoint(center.row - sizeHalf, center.col + ratio);
	points[2] = DrawPoint(center.row + sizeHalf, center.col + ratio);
	points[3] = DrawPoint(center.row + sizeHalf, center.col - ratio);

	// Draw the four lines of the box
	for (int x = 0; x < 3; x++)
	{
		drawLine(canvas, points[x], points[x + 1], animate);
	}
	drawLine(canvas, points[3], points[0], animate);

	// Replace the corners with a better looking character
	for (int x = 0; x < 4; x++)
	{
		drawHelper(canvas, points[x], '+', animate);
	}
}


// Menu for the drawing tools
void menuTwo(char canvas[][MAXCOLS], char backupCanvas[][MAXCOLS], bool& animate)
{

	const int CLEAR = 90;
	const int MENUSPACE = MAXROWS + 1;
	char menu[] = { "<A>nimate: N / <F>ill / <L>ine / <B>ox / <N>ested Boxes / <T>ree / <M>ain Menu: " };
	char choice;

	do
	{
		displayCanvas(canvas);
		//clear both lines that normally have prompts written on them
		clearLine(MENUSPACE, CLEAR);
		clearLine(MAXROWS + 2, CLEAR);
		//go to where menu should be printed
		gotoxy(MAXROWS + 1, 0);
		//display menu
		cout << menu;
		//input choice and clear line
		cin >> choice;
		cin.ignore();
		clearLine(MENUSPACE, CLEAR);

		switch (toupper(choice))
		{
		case 'A':
			//toggle false/true animate bool and change menu option between N and Y
			animate = !animate;
			menu[11] = animate ? 'Y' : 'N';
			break;
		case 'F':
			copyCanvas(temp, canvas);
			//fill function - I can't select this at all from the menu. 
			cout << "Enter character to fill with from current location / <ESC> to cancel\n";
			input = getPoint(startPt);
			
			cin.get(newCh);
			cin.ignore();
			//Hitting ESC doesnt work
			if (input == ESC)
			{
				break;
			}
			fillRecursive(canvas, row, col, oldCh, newCh, animate);
			break;
		case 'L':
			copyCanvas(temp, canvas);
			//line function - the function works, does not print start point or end point to screen
			//but correctly draws lines from selected points
			cout << "Type any letter to choose a start point / <ESC> to cancel\n";
			input = getPoint(startPt);
			start = startPt;
			clearLine(MENUSPACE, CLEAR);

			cout << "Type any letter to choose an end point / <ESC> to cancel\n";
			input = getPoint(endPt);

			//If input is escape key, no line
			if (input == ESC)
			{
				break;
			}

			endP = endPt;

			drawLine(canvas, startPt, endPt, animate);
			break;
		case 'B':
			copyCanvas(temp, canvas);
			//boxes function

			cout << "Enter size: ";
			cin >> height;
			clearLine(MENUSPACE, CLEAR);
			cout << "Type any letter to choose a box center, or <C> for screen center / <ESC> to cancel\n";
			input = getPoint(center);

			//if input is c, print box in dead center of screen
			if (toupper(input) == 'C')
			{
				center.row = MAXROWS / 2;
				center.col = MAXCOLS / 2;
			}
			//If input is escape key, no boxes
			if (input == ESC)
			{
				break;
			}

			drawBox(canvas, center, height, animate);
			break;
		case 'N':
			copyCanvas(temp, canvas);
			//drawBoxesRecursive function
			cout << "Enter size of largest box: ";
			cin >> height;

			clearLine(MENUSPACE, CLEAR);

			cout << "Type any letter to choose a box center, or <C> for screen center / <ESC> to cancel\n";
			input = getPoint(center);

			//If input is c, place boxes in dead center of canvas
			if (toupper(input) == 'C')
			{
				center.row = MAXROWS / 2;
				center.col = MAXCOLS / 2;
			}
			//If input is escape key, no boxes
			if (input == ESC)
			{
				break;
			}

			drawBoxesRecursive(canvas, center, height, animate);
			break;
		case 'T':
			copyCanvas(temp, canvas);
			//tree function - draws a tree correctly, does not center the tree correctly if you press 'c'.
			cout << "Enter approximate tree height: ";
			cin >> height;

			clearLine(MENUSPACE, CLEAR);

			cout << "Enter branch angle: ";
			int branchAngle;
			cin >> branchAngle;

			clearLine(MENUSPACE, CLEAR);

			cout << "Type any letter to choose a start point, or <C> for bottom center / <ESC> to cancel\n";
			input = getPoint(startPt);

			//if input is c, print tree at center bottom
			if (toupper(input) == 'C')
			{
				startPt.row = MAXROWS  - 1;
				startPt.col = MAXCOLS / 2;
			}
			//If input is escape key, no boxes
			if (input == ESC)
			{
				break;
			}
			start = startPt;

			treeRecursive(canvas, start, height, trunk, branchAngle, animate);

			break;
		case 'M':
			return;
		}
	} while (choice != 'M');
}


// Get a single point from screen, with character entered at that point
char getPoint(Point& pt)
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
		//arrow keys
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

		//whitespace chars
		else if (input == '\0')
		{
			input = _getch();
			pt.row = row;
			pt.col = col;
			return input;
		}
		//other characters
		else if (32 <= input && input <= 126)
		{
			//save row and col as the point
			pt.row = row;
			pt.col = col;
			//print the character
			printf("%c", input);
			return input;
		}

		gotoxy(row, col);
		input = _getch();
	}


	return input;
}



// Recursively fill a section of the screen
void fillRecursive(char canvas[][MAXCOLS], int row, int col, char oldCh, char newCh, bool animate)
{
	//Base Case:
	if (row >= MAXROWS || col >= MAXCOLS || canvas[row][col] == newCh || canvas[row][col] != ' ')
	{
		return;
	}
	
	if (canvas[row][col] == oldCh || canvas[row][col] == ' ')
	{
		canvas[row][col] = newCh;
				
		//Fills north
		fillRecursive(canvas, row - 1, col, oldCh, newCh, animate);
		//Fills south
		fillRecursive(canvas, row + 1, col, oldCh, newCh, animate);
		//Fills east
		fillRecursive(canvas, row, col - 1, oldCh, newCh, animate);
		//Fills west
		fillRecursive(canvas, row, col + 1, oldCh, newCh, animate);
	
	}
}


// Recursively draw a tree
void treeRecursive(char canvas[][MAXCOLS], DrawPoint start, int height, int startAngle, int branchAngle, bool animate)
{

	if (start.row >= MAXROWS || start.col >= MAXCOLS || start.row < 0 || start.col < 0)
	{

		return;
	}
	if (height < 3)
	{
		return;
	}



	DrawPoint end = findEndPoint(start, height / 3, startAngle);
	drawLine(canvas, start, end, animate);

	treeRecursive(canvas, end, height - 2, startAngle - branchAngle, branchAngle, animate);
	treeRecursive(canvas, end, height - 2, startAngle + branchAngle, branchAngle, animate);

}


// Recursively draw nested boxes
void drawBoxesRecursive(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	//base case
	if (height < 2)
	{
		return;
	}
	//otherwise draw a bax and recursivley draw nested boxes
	drawBox(canvas, center, height, animate);
	drawBoxesRecursive(canvas, center, height - 2, animate);

}