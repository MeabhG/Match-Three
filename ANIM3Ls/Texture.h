#pragma once

// Texture class handles the image files
// image files work as smaller dcs that are drawn onto the main dc

class Texture
{
private:
	// image
	HANDLE _bitmap;
	// image header
	BITMAP _header;
	// drawing context
	HDC    _dc;
public:
	// constructor
	Texture(LPCWSTR filename);
	// gets the image width
	int width();
	// gets the image height
	int height();
	// draw image in main screen
	void draw(HDC hdc, int x, int y);
	// destructor
	~Texture();
};