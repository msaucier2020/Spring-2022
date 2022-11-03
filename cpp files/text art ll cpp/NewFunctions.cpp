#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include <limits>
#include <cstring>
#include "Definitions.h"
using namespace std;

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
void menuTwo(Node*& current, List& undoList, List& redoList, List& clips, bool& animate)
{

	const int CLEAR = 90;
	const int MENUSPACE = MAXROWS + 2;
	char menu[] = { "<F>ill / <L>ine / <B>ox / <N>ested Boxes / <T>ree / <M>ain Menu: " };
	char choice;
	char input;
	int height;
	int row = 0;
	int col = 0;
	//keep track of redo states
	int redoState = 0;
	//keept track of undo states
	int undoState = 0;
	//keep track of clips
	int clipCount = 0;
	//tells whether animate is on or not
	char animateYN = 'N';

	DrawPoint start;
	DrawPoint endP;
	Point startPt;
	Point center;
	Point endPt;

	do
	{
		system("cls");
		displayCanvas(current->item);
		animateYN = animate ? 'Y' : 'N';
		menuOne(current->item, redoList, undoList, clips, animate, animateYN);
		//display draw menu
		cout << menu;
		//input choice and clear line
		cin >> choice;
		cin.ignore();
		clearLine(MENUSPACE, CLEAR);
		clearLine(MENUSPACE - 1, CLEAR);
		switch (toupper(choice))
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
			//add to undo list in case user wants to undo
			addUndoState(undoList, redoList, current);

			//write code here for play

			break;
		//Draw Menu Options
		case 'F':
		{
			//variables
			Point fill;

			//add to undo list in case user wants to undo
			addUndoState(undoList, redoList, current);

			//fill a section on canvas
			cout << "Enter character to fill with from current location / <ESC> to cancel\n";
			input = getPoint(fill);

			if (input == ESC)
			{
				break;
			}

			fillRecursive(current->item, fill.row, fill.col, current->item[fill.row][fill.col], input, animate);
		}
		break;
		case 'L':
			//add to undo list in case user wants to undo
			addUndoState(undoList, redoList, current);

			//draws a line from two points
			cout << "Type any letter to choose start point / <ESC> to cancel\n";
			input = getPoint(startPt);
			if (input == ESC)
			{
				break;
			}
			printf("%c", input);
			start = startPt;
			clearLine(MENUSPACE-1, CLEAR);

			cout << "Type any letter to choose end point / <ESC> to cancel\n";
			input = getPoint(endPt);
			if (input == ESC)
			{
				break;
			}
			printf("%c", input);
			endP = endPt;

			drawLine(current->item, startPt, endPt, animate);
			break;
		case 'B':
			//add to undo list in case user wants to undo
			addUndoState(undoList, redoList, current);

			//draws a box
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

			drawBox(current->item, center, height, animate);
			break;
		case 'N':
			//add to undo list in case user wants to undo
			addUndoState(undoList, redoList, current);

			//draws nested boxes
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

			drawBoxesRecursive(current->item, center, height, animate);
			break;
		case 'T':
		{
			//variable for tree angle
			int trunk = 270;

			//add to undo list in case user wants to undo
			addUndoState(undoList, redoList, current);

			//recursivley draw a tree
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
				startPt.row = MAXROWS - 1;
				startPt.col = MAXCOLS / 2;
			}
			//If input is escape key, no boxes
			if (input == ESC)
			{
				break;
			}
			start = startPt;

			treeRecursive(current->item, start, height, trunk, branchAngle, animate);
		}
		break;
		case 'M':
			// main menu
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
			pt.row = row;
			pt.col = col;
			return input;
		}
		else if (32 <= input && input <= 126)
		{

			pt.row = row;
			pt.col = col;
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
	if (row >= MAXROWS || col >= MAXCOLS || row < 0 || col < 0 || canvas[row][col] != oldCh || oldCh == newCh)
	{
		return;
	}

	Point fill(row, col);

	drawHelper(canvas, fill, newCh, animate);
	//Fills south
	fillRecursive(canvas, row + 1, col, oldCh, newCh, animate);
	//Fills north
	fillRecursive(canvas, row - 1, col, oldCh, newCh, animate);
	//Fills west
	fillRecursive(canvas, row, col + 1, oldCh, newCh, animate);
	//Fills east
	fillRecursive(canvas, row, col - 1, oldCh, newCh, animate);
}



// Recursively draw a tree
void treeRecursive(char canvas[][MAXCOLS], DrawPoint start, int height, int startAngle, int branchAngle, bool animate)
{
	// Base case:
	if (start.row >= MAXROWS || start.col >= MAXCOLS || start.row < 0 || start.col < 0)
	{

		return;
	}
	if (height < 3)
	{
		return;
	}


	// recusivley draws a tree with branches
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
	//otherwise draw a box and recursivley draw nested boxes
	drawBox(canvas, center, height, animate);
	drawBoxesRecursive(canvas, center, height - 2, animate);

}