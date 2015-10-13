#include "stdafx.h"

// Constructor
Grid::Grid(int size)
{
	assert(size > 0); // grid size has to be greater than one

	// size of grid is used as both the width and height
	_size  = size; 
	// resizes the vector to fit all the data what will be needed
	_data.resize(size * size);
	// constructor calls the randomize function 
	randomize();
}

int Grid::size()
{
	// like a get, returns the size of grid (height = width)
	return _size;
}

void Grid::randomize()
{
	// fills the vector with random (not None) BlockType for each position in the grid
	for (int i = 0; i < _size * _size; i++)
		_data[i] = static_cast<BlockType>(rand() % (BlockType::Count - 1) + 1);
}

BlockType Grid::getAt(int x, int y)
{
	// returns the BlockType at location x , y
	assert((x >= 0) && (x < _size) && (y >= 0) && (y < _size));
	return _data[x + y * _size];
}

void Grid::setAt(int x, int y, BlockType value)
{
	// sets the BlockType at position x, y to that passed in as a parameter
	assert((x >= 0) && (x < _size) && (y >= 0) && (y < _size));
	_data[x + y * _size] = value;
}

void Grid::clearAt(int x, int y)
{
	// sets the BlockType to None (none = empty) at the position x, y
	assert((x >= 0) && (x < _size) && (y >= 0) && (y < _size));
	_data[x + y * _size] = BlockType::None;
}

void Grid::swap(int x1, int y1, int x2, int y2)
{
	assert
	(
		(x1 >= 0) && (x1 < _size) && (y1 >= 0) && (y1 < _size) &&
		(x2 >= 0) && (x2 < _size) && (y2 >= 0) && (y2 < _size)
	);
	
	// gets the address in memory and swaps them therefore the position (x, y) now points at the other's BlockType
	std::swap
	(
		_data[x1 + y1 * _size],
		_data[x2 + y2 * _size]
	);
}

// takes memory pointers as parameters
bool Grid::pushBackPointIfListDoesntContainIt(std::list<POINT>* list, POINT* item)
{
	assert((list != NULL) && (item != NULL));
	 
	// for each item (point) in the list check if equal to the point given (parameter)
	// if the point is in the list return false
	for (std::list<POINT>::const_iterator point = list->begin(), end = list->end(); point != end; ++point)
		if ((point->x == item->x) && (point->y == item->y))
			return false;
	
    // else add the point to the list
	list->push_back(*item);
	return true;
}

// checks if there are any moves left for the player to take
bool Grid::anyMovesLeft()
{
	// goes through the whole grid 
	for (int y = 0; y < _size; y++)
		for (int x = 0; x < _size; x++)
		{
			// uses pointers to check the value in memory
			// selects a BlockType to check to see if with one move the player could create three in a row
			BlockType* thisBlock = &_data[x + y * _size];


			// check the BlockType's position on the x axis backwards
			if (x > 2)
			{
				BlockType* midBlock = &_data[x - 2 + y * _size];
				BlockType* farBlock = &_data[x - 3 + y * _size];

				if ((*midBlock == *thisBlock) && (*farBlock == *thisBlock))
					// if the block two away and three away are equal
					// then with one move the player can create a row of three
					// then there are moves left -no need to cheak anymore return true
					return true;
			}

			// check the BlockType's position on the x axis forward
			if (x < (_size - 3))
			{
				BlockType* midBlock = &_data[x + 2 + y * _size];
				BlockType* farBlock = &_data[x + 3 + y * _size];

				if ((*midBlock == *thisBlock) && (*farBlock == *thisBlock))
					return true;
			}

			// check the BlockType's position on the y axis upwards
			if (y > 2)
			{
				BlockType* midBlock = &_data[x + (y - 2) * _size];
				BlockType* farBlock = &_data[x + (y - 3) * _size];

				if ((*midBlock == *thisBlock) && (*farBlock == *thisBlock))
					return true;
			}

			// check the BlockType's position on the y axis downwards
			if (y < (_size - 3))
			{
				BlockType* midBlock = &_data[x + (y + 2) * _size];
				BlockType* farBlock = &_data[x + (y + 3) * _size];

				if ((*midBlock == *thisBlock) && (*farBlock == *thisBlock))
					return true;
			}
		}

	// there is no potential rows of three the player can make - return false
	return false;
}

// returns a list of the BlockTypes that are 'floating', have empty, None grid positions below
std::list<POINT> Grid::floatingBlocks()
{
	std::list<POINT> list;

	for (int y = 0; y < _size - 1; y++)
		for (int x = 0; x < _size; x++)
		{
			// memory pointer 
			BlockType* thisBlock = &_data[x + y * _size];

			if (*thisBlock == BlockType::None)
				// if the cell equals to None, keeps looping through the list of BlockTypes
				continue; 

			// if not equal to None check the BlockType underneath it
			BlockType* nearBlock = &_data[x + (y + 1) * _size];

			// if that BlockType is equals to None, add thisBlock position to the list as it is 'floating'
			if (*nearBlock == BlockType::None)
				list.push_back({x, y});
		}

	// return the list containing the positions of the 'floating' BlockType
	return list;
}

bool Grid::moveFloatingBlocksDown()
{
	// get the list from the return of floatingBlocks function
	std::list<POINT> list = floatingBlocks();

	// if it's empty end function and return false ie there is no floatingblocks there may still be empty spots at the very top of the grid
	if (list.empty())
		return false;
	
	// foreach point in the list swap the memory pointers so that the empty (none) BlockType grid postion is ontop of the filled with shape BlockType
	for (std::list<POINT>::const_iterator point = list.begin(), end = list.end(); point != end; ++point)
		swap(point->x, point->y, point->x, point->y + 1);

	// return true ie positions have been wrapped therefore empty spots are still in the grid
	return true;
}

std::list<POINT> Grid::generateTopBlocks()
{
	std::list<POINT> list;

	// goes through the top line, when a BlcokType value is None it generates a random block to fill it
	for (int x = 0; x < _size; x++)
		if (_data[x] == BlockType::None)
		{
			// converts a random generated number within the enumeration to a BlockType
			// and adds it to the list
			_data[x] = static_cast<BlockType>(rand() % (BlockType::Count - 1) + 1);
			list.push_back({x, 0});
		}

	// returns a list of positions of the newly filled BlockTypes
	return list;
}

// This is the workhorse function
// returns a list of blocks to be destroyed
std::list<POINT> Grid::blocksToDestroy()
{
	std::list<POINT> list;

	// iterates through the list of BlockTypes
	for (int y = 0; y < _size; y++)
		for (int x = 0; x < _size; x++)
		{
			// thisBlock is the memory address of the current point
			POINT      thisPoint = {x, y};
			BlockType* thisBlock = &_data[thisPoint.x + thisPoint.y * _size];

			if (*thisBlock == BlockType::None)
				// if this BlockType is equal to None it can't be destroyed so move to the next block
				continue; 

			// if the BlockType is not equal to None and is in range
			// check BlockType* thisBlock against BlockTypes to the right of it (x-axis)
			if (x < (_size - 2))
			{
				// get the BlockType of the next block
				POINT      nearPoint = {x + 1, y};
				BlockType* nearBlock = &_data[nearPoint.x + nearPoint.y * _size];

				// get the BlockType of the next block after the previous one
				POINT      midPoint = {x + 2, y};
				BlockType* midBlock = &_data[midPoint.x + midPoint.y * _size];

				// if all three BlockTypes are equal
				if ((*nearBlock == *thisBlock) && (*midBlock == *thisBlock))
				{
					// check to make sure the position isn't in the list already
					// if it isn't add it to the list
					pushBackPointIfListDoesntContainIt(&list, &thisPoint);
					pushBackPointIfListDoesntContainIt(&list, &nearPoint);
					pushBackPointIfListDoesntContainIt(&list, &midPoint);

					// checks if more than three in a row match
					for (int dx = x + 3; dx < _size; dx++)
						if (_data[dx + y * _size] == *thisBlock)
						{
							POINT farPoint = {dx, y};

							// check to make sure the position isn't in the list already
							// if it isn't add it to the list
							if (!pushBackPointIfListDoesntContainIt(&list, &farPoint))
								break;
						}
						// when no more match 
						else
							break;
				}
			}

			// check BlockType* thisBlock against BlockTypes below it (y-axis)
			// does the same thing as above
			if (y < (_size - 2))
			{
				POINT      nearPoint = {x, y + 1};
				BlockType* nearBlock = &_data[nearPoint.x + nearPoint.y * _size];

				POINT      midPoint = {x, y + 2};
				BlockType* midBlock = &_data[midPoint.x + midPoint.y * _size];

				if ((*nearBlock == *thisBlock) && (*midBlock == *thisBlock))
				{
					pushBackPointIfListDoesntContainIt(&list, &thisPoint);
					pushBackPointIfListDoesntContainIt(&list, &nearPoint);
					pushBackPointIfListDoesntContainIt(&list, &midPoint);

					for (int dy = y + 3; dy < _size; dy++)
						if (_data[x + dy * _size] == *thisBlock)
						{
							POINT farPoint = {x, dy};

							if (!pushBackPointIfListDoesntContainIt(&list, &farPoint))
								break;
						}
						else
							break;
				}
			}
		}

	// returns a list of blocks to be destroyed
	return list;
}

int Grid::destroyBlocks()
{
	// blocksToDestroy function returns a list of positons of blocks to be destroyed
	std::list<POINT> list = blocksToDestroy();
	// counter used to calculate score 
	int destroyedBlocks = 0;
	
	// if the list isn't empty
	// foreach item in the list
	// call the clearAt function fills the position with BlockType None
	// increase the score by one
	if (!list.empty())
		for (std::list<POINT>::const_iterator point = list.begin(), end = list.end(); point != end; ++point)
		{
			clearAt(point->x, point->y);
			++destroyedBlocks;
		}

	return destroyedBlocks;
}

Grid::~Grid()
{
}