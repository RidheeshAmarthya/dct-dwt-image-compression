//*****************************************************************************
//
// Image.h : Defines the class operations on images
//
// Author - Parag Havaldar
// Main Image class structure 
//
//*****************************************************************************

#ifndef IMAGE_DISPLAY
#define IMAGE_DISPLAY

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "resource.h"
#include "afxwin.h"

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <thread>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <vector>
#define PI 3.14159265358979323846

#define NOMINMAX    

// Class structure of Image 
// Use to encapsulate an RGB image
class MyImage 
{

private:
	int		Width;					// Width of Image
	int		Height;					// Height of Image
	char	ImagePath[_MAX_PATH];	// Image location
	char*	Data;					// RGB data of the image


public:
	// Constructor
	MyImage();
	// Copy Constructor
	MyImage::MyImage( MyImage *otherImage);
	// Destructor
	~MyImage();

	// operator overload
	MyImage & operator= (const MyImage & otherImage);

	// Reader & Writer functions
	void	setWidth( const int w)  { Width = w; }; 
	void	setHeight(const int h) { Height = h; }; 
	void	setImageData( const char *img ) { Data = (char *)img; };
	void	setImagePath( const char *path) { strcpy(ImagePath, path); }
	int		getWidth() { return Width; };
	int		getHeight() { return Height; };
	char*	getImageData() { return Data; };
	char*	getImagePath() { return ImagePath; }

	// Input Output operations
	bool	ReadImage();
	bool	WriteImage();

	void CopyBlockToData(int blockX, int blockY, unsigned char*** Rblocks, unsigned char*** Gblocks, unsigned char*** Bblocks, int blockSize);
	void CopyBlocksToData(unsigned char*** dataBlocks, int blockSize);
	void DCT(int input);

	void compress(double* DWTorigin, int length);
	void decompress(double* DWTorigin, int length);
	void processTransform(double* DWTorigin, int length, int channel, bool isRow, bool isCompression);
	void DWT(int input);

	bool	Modify(int input);

};

#endif //IMAGE_DISPLAY
