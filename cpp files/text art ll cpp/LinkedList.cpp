#include <iostream>
#include <Windows.h>
#include "Definitions.h"
using namespace std;


Node* newCanvas()
{
	Node* canvas = new Node;


	for (int row = 0; row < MAXROWS; row++)
	{
		for (int col = 0; col < MAXCOLS; col++)
		{
			canvas->item[row][col] = ' ';
		}
	}


	return NULL;
}


Node* newCanvas(Node* oldNode)
{
	Node* copy = new Node;

	for (int row = 0; row < MAXROWS; row++)
	{
		for (int col = 0; col < MAXCOLS; col++)
		{
			copy->item[row][col] = oldNode->item[row][col];
		}
	}

	return NULL;
}


void play(List& clips)
{
	// loops as long as the ESCAPE key is not currently being pressed
	while (!(GetKeyState(VK_ESCAPE) & 0x8000))
	{
		playRecursive(clips.head, clips.count);
	}
}


void playRecursive(Node* head, int count)
{
	// Base Case:
	if (head == NULL || head->next == NULL || count == 0)
		return;
	
	// Recursive call
	playRecursive(head->next, count - 1);
	displayCanvas(head->item);

	// Pause for 100 milliseconds to slow down animation
	Sleep(100);
}


void addUndoState(List& undoList, List& redoList, Node* &current)
{
	
	addNode(undoList, current);
	deleteList(redoList);
	
}


void restore(List& undoList, List& redoList, Node* &current)
{
	if (redoList.count > 0)
	{
		addNode(redoList, current);
		current = removeNode(undoList);
	}
	
}


void addNode(List& list, Node* nodeToAdd)
{
	// create temp node
	Node* temp = new Node;

	//set temp.next to point to addresss of last added node
	temp = list.head;

	//make head point to new node
	list.head = nodeToAdd;

	//make new node point to last added node
	nodeToAdd = temp;

	//update list count
	list.count++;

}


Node* removeNode(List& list)
{
	if (list.head = NULL)
	{
		return NULL;
	}
	else
	{
		//make the temp point to second node
		Node* temp = list.head->next;

		//save old node to return later
		Node* old = list.head;

		//make head point to temp
		list.head = temp;

		list.count--;
		return old;
	}
}


void deleteList(List& list)
{
	Node* temp = new Node;

	while (list.head != NULL)
	{
		temp = list.head->next;
		delete list.head;
		list.head = temp;
	}

	delete temp;
}


bool loadClips(List& clips, char filename[])
{
	// TODO: Write the code for the function

	return true;
}


bool saveClips(List& clips, char filename[])
{
	Node* savedClips = clips.head;
	int i = 0;

	while (savedClips)
	{

	}
	return true;
}