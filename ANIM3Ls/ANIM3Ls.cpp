//  ANIM3Ls.cpp : Defines the entry point for the application.
// 

#include "stdafx.h"
#include "ANIM3Ls.h"

#define MAX_LOADSTRING 100

// Settings
#define GRID_SIZE 8
#define GRID_TILE_SIZE 64
#define GRID_INTERVAL 96

//  Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

//  Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

Grid g_Grid(GRID_SIZE); // Grid takes one parameter because it will always be a square

// Store two points (first click, second click) for the swaping commands
POINT g_SelectedTile1 = { -1, -1 };
POINT g_SelectedTile2 = { -1, -1 };

// boolean that allows / denies user interaction
bool g_CanPlay = false;
// when chains of matches are triggered by one single input, combo value multiplies the score
int g_Combo = 0;
// score relative to a single input
int g_Score = 0;
// Total score may be a really really long long number
unsigned long long g_TotalScore = 0;

// array of images to use as blocks
Texture textures[BlockType::Count - 1] = {
	Texture(L"A.bmp"),
	Texture(L"B.bmp"),
	Texture(L"C.bmp"),
	Texture(L"D.bmp"),
	Texture(L"E.bmp")
};

int APIENTRY _tWinMain
(
	_In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     LPTSTR    lpCmdLine,
    _In_     int       nCmdShow
)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;
	
	//  Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ANIM3LS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	//  Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ANIM3LS));

	//  Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



// 
//   FUNCTION: MyRegisterClass()
// 
//   PURPOSE: Registers the window class.
// 
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ANIM3LS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0; //  deactivates menu bar from window
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

// 
//    FUNCTION: InitInstance(HINSTANCE, int)
// 
//    PURPOSE: Saves instance handle and creates main window
// 
//    COMMENTS:
// 
//         In this function, we save the instance handle in a global variable and
//         create and display the main program window.
// 
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable
   
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   srand(time(NULL)); // seeding the random generator with the computer time
   SetTimer(hWnd, 256, GRID_INTERVAL, NULL); // Update delay value

   return TRUE;
}

// 
//   FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
// 
//   PURPOSE:  Processes messages for the main window.
// 
//   WM_COMMAND	- process the application menu
//   WM_PAINT	- Paint the main window
//   WM_DESTROY	- post a quit message and return
// 
// 
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_TIMER:
	{
		// tracks changes to window and content, 
		// if true, something has been changed and must be redrawn
		bool redraw = true;

		// if blocks were moved down, then generate new blocks on top
		// while this is happening the player may not interact (canPlay = false)
		if (g_Grid.moveFloatingBlocksDown())
		{
			g_Grid.generateTopBlocks();
			g_CanPlay = false;
		}
		// when there are no more floating blocks
		else
		{
			// destroyBlocks returns the amount of blocks destroyed, which is used to calculate score
			int destroyedBlocks = g_Grid.destroyBlocks();

			// if destroyed blocks > 0
			if (destroyedBlocks)
			{
				// checks if tiles were selected
				if (g_SelectedTile2.x != -1)
				{
					// deselects them
					g_SelectedTile1 = { -1, -1 };
					g_SelectedTile2 = { -1, -1 };
				}

				// each time blocks are destroyed add combo points
				// combo and score are global variables which accumulate points until 
				// there are no more blocks being destroyed and then are reseted to 0
				++g_Combo;
				g_Score += destroyedBlocks;
				// Totalscore adds up throughout the whole game
				g_TotalScore += g_Combo * g_Score;

				g_CanPlay = false;
			}
			// after all blocks were destroyed, destroyedBlocks = 0
			else
			{
				// when all matches were destroyed from grid, reset combo and score
				g_Score = 0;
				g_Combo = 0;

				// when the grid is filled
				if (g_Grid.generateTopBlocks().empty())
				{
					// and if there are more possible matches
					if (g_Grid.anyMovesLeft())
					{
						// nothing has changed, so no need to redraw
						if (g_CanPlay)
							redraw = false;
						// in case the player wasn't able to, he can now play
						else
							g_CanPlay = true;
					}
					// else - if there are no more possible matches, randomize the grid blocks
					else
						g_Grid.randomize();
				}
				// if the grid is not all filled, more calculations must be made, and the player cannot play yet
				else
					g_CanPlay = false;

				//  when you swap and it is unsuccessful
				if (g_SelectedTile2.x != -1)
				{
					// swaps back
					g_Grid.swap(g_SelectedTile1.x, g_SelectedTile1.y, g_SelectedTile2.x, g_SelectedTile2.y);
					g_SelectedTile1 = { -1, -1 };
					g_SelectedTile2 = { -1, -1 };

					redraw = true;
				}
			}
		}
		// if something changed, then redraws
		if (redraw)
			InvalidateRect(hWnd, NULL, FALSE);

		break;
	}
	case WM_LBUTTONDOWN:
	{
		// if there is something being redrawn player input is not processed
		if (!g_CanPlay)
			break;

		POINT mousePosition;
		
		// convert mouse positions from whole screen to window context
		GetCursorPos(&mousePosition);
		ScreenToClient(hWnd, &mousePosition);
		
		// map mouse position to the center of the grid cells
		int gridX = round((mousePosition.x - 0.5 * GRID_TILE_SIZE) / GRID_TILE_SIZE);
		int gridY = round((mousePosition.y - 0.5 * GRID_TILE_SIZE) / GRID_TILE_SIZE);

		// if click is outside the grid, don't process it
		if ((gridX < 0) || (gridX >= g_Grid.size()) || (gridY < 0) || (gridY >= g_Grid.size()))
			break;
		
		// if the first tile has not been selected
		if (g_SelectedTile1.x == -1)
			// assigns the new grid position
			g_SelectedTile1 = { gridX, gridY };
		// if the first selected tile has been assigned already
		else
		{
			// assign the second
			g_SelectedTile2 = { gridX, gridY };

			// check if they are neighbours
			if ((abs(g_SelectedTile2.x - g_SelectedTile1.x) + abs(g_SelectedTile2.y - g_SelectedTile1.y)) == 1)
				// swaps them
				g_Grid.swap(g_SelectedTile1.x, g_SelectedTile1.y, g_SelectedTile2.x, g_SelectedTile2.y);
			// if not, they can't play together :( 
			else
			{
				// deselects both tiles
				g_SelectedTile1 = { -1, -1 };
				g_SelectedTile2 = { -1, -1 };
			}
		}

		InvalidateRect(hWnd, NULL, FALSE);

		break;
	}
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		
		// get blocktype of every position of the grid and draw its corresponding image on screen
		for (int y = 0; y < g_Grid.size(); y++)
			for (int x = 0; x < g_Grid.size(); x++)
			{
				// get type
				int id = g_Grid.getAt(x, y);
				// if the block is empty
				if (id == BlockType::None)
				{
					// draws a white square with a brush with no outline
					// it's better to do it this way so then flickering will be avoided
					LOGBRUSH brush;
					brush.lbColor = RGB(255, 255, 255);
					brush.lbStyle = BS_SOLID;

					HBRUSH hbrush = CreateBrushIndirect(&brush);
					SelectObject(hdc, hbrush);

					SelectObject(hdc, GetStockObject(NULL_PEN));

					Rectangle(hdc, x * GRID_TILE_SIZE, y * GRID_TILE_SIZE, (x + 1) * GRID_TILE_SIZE, (y + 1) * GRID_TILE_SIZE);

					DeleteObject((HGDIOBJ)hbrush);
				}
				// otherwise
				else
					// draws the corresponding image on the screen
					textures[id - 1].draw(hdc, x * GRID_TILE_SIZE, y * GRID_TILE_SIZE);
			}

		// if mouse selection is valid
		bool drawSelectedTile1 = g_SelectedTile1.x != -1;
		bool drawSelectedTile2 = g_SelectedTile2.x != -1;

		// draw circle around selected tile
		if (drawSelectedTile1 || drawSelectedTile2)
		{
			// create a brush with no fill
			LOGBRUSH brush;
			brush.lbStyle = BS_HOLLOW;

			HBRUSH hbrush = CreateBrushIndirect(&brush);
			SelectObject(hdc, hbrush);

			// draws a circle over the first and second selected tiles
			if (drawSelectedTile1)
				Ellipse
				(
					hdc,
					g_SelectedTile1.x * GRID_TILE_SIZE,
					g_SelectedTile1.y * GRID_TILE_SIZE,
					(g_SelectedTile1.x + 1) * GRID_TILE_SIZE,
					(g_SelectedTile1.y + 1) * GRID_TILE_SIZE
				);

			if (drawSelectedTile2)
				Ellipse
				(
					hdc,
					g_SelectedTile2.x * GRID_TILE_SIZE,
					g_SelectedTile2.y * GRID_TILE_SIZE,
					(g_SelectedTile2.x + 1) * GRID_TILE_SIZE,
					(g_SelectedTile2.y + 1) * GRID_TILE_SIZE
				);

			// delete brush object after use
			DeleteObject((HGDIOBJ)hbrush);
		}

		// creates a temporary buffer to format strings
		TCHAR buffer[32];
		// formats the SCORE {0} string, replacing the {0} with the total score and gets its size
		int size = wsprintf(buffer, L"SCORE: %d", g_TotalScore);

		// creates a brush with no outline to erase the text on the side
		LOGBRUSH brush;
		brush.lbColor = RGB(255, 255, 255);
		brush.lbStyle = BS_SOLID;

		HBRUSH hbrush = CreateBrushIndirect(&brush);
		SelectObject(hdc, hbrush);

		SelectObject(hdc, GetStockObject(NULL_PEN));

		// erases the text on the side by drawing a white rectangle over them
		Rectangle(hdc, (g_Grid.size() + 1) * GRID_TILE_SIZE, GRID_TILE_SIZE, (g_Grid.size() + 1) * GRID_TILE_SIZE + size * 16, GRID_TILE_SIZE + 128);

		DeleteObject((HGDIOBJ)hbrush);

		// draws the formatted score string on the screen
		TextOut(hdc, (g_Grid.size() + 1) * GRID_TILE_SIZE, GRID_TILE_SIZE, buffer, size);

		// if the current score > 0
		if (g_Score)
		{
			// formats the +{0} string, replacing the {0} with the current score
			size = wsprintf(buffer, L"+%d", g_Score);
			// draws it on the screen
			TextOut(hdc, (g_Grid.size() + 1) * GRID_TILE_SIZE, GRID_TILE_SIZE + 32, buffer, size);
		}

		// if the current combo value > 0
		if (g_Combo > 1)
		{
			// formats the COMBO {0}x string, replacing the {0} with the current combo value
			size = wsprintf(buffer, L"COMBO %dx", g_Combo);
			// draws it on the screen
			TextOut(hdc, (g_Grid.size() + 1) * GRID_TILE_SIZE, GRID_TILE_SIZE + 64, buffer, size);
		}

		// if the player is not allowed to play
		if (!g_CanPlay)
			// draws BUSY
			TextOut(hdc, (g_Grid.size() + 1) * GRID_TILE_SIZE, GRID_TILE_SIZE + 96, L"BUSY", 4);

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}