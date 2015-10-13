#include "stdafx.h"
#include "Texture.h"

// This class is used to create the image tile for the blocks
// It loads an image and creates its own small drawing context

Texture::Texture(LPCWSTR filename)
{
	_bitmap = LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	// if the image was successfully loaded
	if (_bitmap != NULL)
	{
		// create a drawing context
		_dc = CreateCompatibleDC(NULL);

		// if the drawing context was created successfully and the image was successfully copied to it
		if ((_dc != NULL) && (SelectObject(_dc, _bitmap) != NULL))
			// extracts the header out of hte bitmap
			GetObject(_bitmap, sizeof(BITMAP), &_header);
	}
}

int Texture::width()
{
	// get width of image
	return _header.bmWidth;
}

int Texture::height()
{
	// get height of image
	return _header.bmHeight;
}

void Texture::draw(HDC hdc, int x, int y)
{
	// this function copies the small dc containing the image to the destination dc, which is the main window
	BitBlt(hdc, x, y, width(), height(), _dc, 0, 0, SRCCOPY);
}

// destructor
Texture::~Texture()
{
	// destroys the drawing context
	if (_dc != NULL)
		DeleteDC(_dc);

	// and the image
	if (_bitmap != NULL)
		DeleteObject(_bitmap);
}
