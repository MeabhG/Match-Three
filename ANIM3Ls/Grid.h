#pragma once
#include "stdafx.h"

class Grid
{
private:
	std::vector<BlockType> _data; // vector containing all the grid data
	int                    _size; // grid size (width = height)
	
	// util function that checks if a point is in the given list, if it's not
	// adds the point to its
	bool pushBackPointIfListDoesntContainIt(std::list<POINT>* list, POINT* point);

public:
	// constructor
	Grid(int size);
	// gets the grid size
	int size();
	// randomizes the entire grid
	void randomize();

	BlockType getAt  (int x,  int y                  ); // gets a block from the grid
	void      setAt  (int x,  int y,  BlockType value); // replaces the value of a block
	void      clearAt(int x,  int y                  ); // replaces the value of a block with None
	void      swap   (int x1, int y1, int x2, int y2 ); // switches the values of two blocks

	// returns if there is at least one possible match
	bool anyMovesLeft();

	std::list<POINT> floatingBlocks();         // returns a list of blocks with an empty space underneath
	bool             moveFloatingBlocksDown(); // moves all the blocks with empty spaces underneath down

	// fills up the top row
	std::list<POINT> generateTopBlocks();

	std::list<POINT> blocksToDestroy(); // gets a list of matching blocks
	int              destroyBlocks();   // destroys all matching blocks

	~Grid();
};