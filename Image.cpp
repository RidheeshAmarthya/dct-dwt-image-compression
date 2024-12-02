//*****************************************************************************
//
// Image.cpp : Defines the class operations on images
//
// Author - Parag Havaldar
// Code used by students as starter code to display and modify images
//
//*****************************************************************************

#include "Image.h"

template <typename T>
T clamp(T value, T min, T max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

float cosTable[8][8];

// Constructor and Desctructors
MyImage::MyImage()
{
	Data = NULL;
	Width = -1;
	Height = -1;
	ImagePath[0] = 0;
}

MyImage::~MyImage()
{
	if (Data)
		delete Data;
}


// Copy constructor
MyImage::MyImage(MyImage* otherImage)
{
	Height = otherImage->Height;
	Width = otherImage->Width;
	Data = new char[Width * Height * 3];
	strcpy(otherImage->ImagePath, ImagePath);

	for (int i = 0; i < (Height * Width * 3); i++)
	{
		Data[i] = otherImage->Data[i];
	}


}



// = operator overload
MyImage& MyImage::operator= (const MyImage& otherImage)
{
	Height = otherImage.Height;
	Width = otherImage.Width;
	Data = new char[Width * Height * 3];
	strcpy((char*)otherImage.ImagePath, ImagePath);

	for (int i = 0; i < (Height * Width * 3); i++)
	{
		Data[i] = otherImage.Data[i];
	}

	return *this;

}


// MyImage::ReadImage
// Function to read the image given a path
bool MyImage::ReadImage()
{

	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0)
	{
		fprintf(stderr, "Image or Image properties not defined");
		fprintf(stderr, "Usage is Image.exe Imagefile w h");
		return false;
	}

	// Create a valid output file pointer
	FILE* IN_FILE;
	IN_FILE = fopen(ImagePath, "rb");
	if (IN_FILE == NULL)
	{
		fprintf(stderr, "Error Opening File for Reading");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	char* Rbuf = new char[Height * Width];
	char* Gbuf = new char[Height * Width];
	char* Bbuf = new char[Height * Width];

	for (i = 0; i < Width * Height; i++)
	{
		Rbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width * Height; i++)
	{
		Gbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width * Height; i++)
	{
		Bbuf[i] = fgetc(IN_FILE);
	}

	// Allocate Data structure and copy
	Data = new char[Width * Height * 3];
	for (i = 0; i < Height * Width; i++)
	{
		Data[3 * i] = Bbuf[i];
		Data[3 * i + 1] = Gbuf[i];
		Data[3 * i + 2] = Rbuf[i];
	}

	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(IN_FILE);

	return true;

}


// MyImage functions defined here
bool MyImage::WriteImage()
{
	// Verify ImagePath
	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0)
	{
		fprintf(stderr, "Image or Image properties not defined");
		return false;
	}

	// Create a valid output file pointer
	FILE* OUT_FILE;
	OUT_FILE = fopen(ImagePath, "wb");
	if (OUT_FILE == NULL)
	{
		fprintf(stderr, "Error Opening File for Writing");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	char* Rbuf = new char[Height * Width];
	char* Gbuf = new char[Height * Width];
	char* Bbuf = new char[Height * Width];

	for (i = 0; i < Height * Width; i++)
	{
		Bbuf[i] = Data[3 * i];
		Gbuf[i] = Data[3 * i + 1];
		Rbuf[i] = Data[3 * i + 2];
	}


	// Write data to file
	for (i = 0; i < Width * Height; i++)
	{
		fputc(Rbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width * Height; i++)
	{
		fputc(Gbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width * Height; i++)
	{
		fputc(Bbuf[i], OUT_FILE);
	}

	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(OUT_FILE);

	return true;

}



void MyImage::CopyBlockToData(int blockX, int blockY, unsigned char*** Rblocks, unsigned char*** Gblocks, unsigned char*** Bblocks, int blockSize) {
	for (int y = 0; y < blockSize; y++) {
		for (int x = 0; x < blockSize; x++) {
			int pixelIdx = (blockY * blockSize * Width) + (blockX * blockSize) + y * Width + x;

			// Copy all three channels in one go
			Data[3 * pixelIdx + 2] = Rblocks[blockY][blockX][y * blockSize + x];  // Red channel
			Data[3 * pixelIdx + 1] = Gblocks[blockY][blockX][y * blockSize + x];  // Green channel
			Data[3 * pixelIdx] = Bblocks[blockY][blockX][y * blockSize + x];      // Blue channel
		}
	}
}
void precomputeCosineValues() {
	for (int u = 0; u < 8; u++) {
		for (int x = 0; x < 8; x++) {
			cosTable[u][x] = cos((2 * x + 1) * u * PI / (2 * 8));
		}
	}
}
void dctTransform(float input[8][8], float output[8][8]) {
	precomputeCosineValues();

#pragma omp parallel for collapse(2)
	for (int u = 0; u < 8; u++) {
		for (int v = 0; v < 8; v++) {
			float sum = 0.0;
			for (int x = 0; x < 8; x++) {
				for (int y = 0; y < 8; y++) {
					sum += input[x][y] * cosTable[u][x] * cosTable[v][y];
				}
			}

			// Calculate scaling factors C(u), C(v)
			float Cu = (u == 0) ? 1 / sqrt(2.0) : 1.0;
			float Cv = (v == 0) ? 1 / sqrt(2.0) : 1.0;

			// Apply the DCT formula
			output[u][v] = 0.25 * Cu * Cv * sum;
		}
	}
}
void idctTransform(float input[8][8], float output[8][8], int n) {
	int totalBlocks = 4096; // Number of 8x8 blocks in a 512x512 image
	int m = static_cast<int>(std::round(static_cast<double>(n) / totalBlocks));

	// Zigzag order for an 8x8 block
	int zigzagOrder[64] = {
		0, 1, 5, 6,14,15,27,28,
		2, 4, 7,13,16,26,29,42,
		3, 8,12,17,25,30,41,43,
		9,11,18,24,31,40,44,53,
	   10,19,23,32,39,45,52,54,
	   20,22,33,38,46,51,55,60,
	   21,34,37,47,50,56,59,61,
	   35,36,48,49,57,58,62,63
	};

	// Zero out coefficients beyond the first m in zigzag order
	float temp[8][8] = { 0 }; // Temporary array to store modified coefficients
	for (int i = 0; i < m; i++) {
		int index = zigzagOrder[i];
		int u = index / 8;
		int v = index % 8;
		temp[u][v] = input[u][v];
	}

#pragma omp parallel for collapse(2)
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			float sum = 0.0;
			for (int u = 0; u < 8; u++) {
				for (int v = 0; v < 8; v++) {
					// Calculate scaling factors C(u), C(v)
					float Cu = (u == 0) ? 1 / sqrt(2.0) : 1.0;
					float Cv = (v == 0) ? 1 / sqrt(2.0) : 1.0;

					sum += Cu * Cv * temp[u][v] *
						cosTable[u][x] * cosTable[v][y];
				}
			}
			output[x][y] = sum / 4.0;
		}
	}
}
void MyImage::DCT(int input) {
	unsigned char* Bbuf = new unsigned char[Width * Height];
	unsigned char* Gbuf = new unsigned char[Width * Height];
	unsigned char* Rbuf = new unsigned char[Width * Height];

	// Split data into separate channels
	for (int i = 0; i < Width * Height; i++) {
		Bbuf[i] = Data[3 * i];       // Blue channel
		Gbuf[i] = Data[3 * i + 1];   // Green channel
		Rbuf[i] = Data[3 * i + 2];   // Red channel
	}

	// Number of blocks per channel
	int blockSize = 8;
	int numBlocksX = Width / blockSize;
	int numBlocksY = Height / blockSize;

	// Allocate memory for blocks
	unsigned char*** Bblocks = new unsigned char** [numBlocksY];
	unsigned char*** Gblocks = new unsigned char** [numBlocksY];
	unsigned char*** Rblocks = new unsigned char** [numBlocksY];

	for (int i = 0; i < numBlocksY; i++) {
		Bblocks[i] = new unsigned char* [numBlocksX];
		Gblocks[i] = new unsigned char* [numBlocksX];
		Rblocks[i] = new unsigned char* [numBlocksX];

		for (int j = 0; j < numBlocksX; j++) {
			Bblocks[i][j] = new unsigned char[blockSize * blockSize];
			Gblocks[i][j] = new unsigned char[blockSize * blockSize];
			Rblocks[i][j] = new unsigned char[blockSize * blockSize];
		}
	}

	// Divide buffers into blocks and apply DCT followed by IDCT
	for (int blockY = 0; blockY < numBlocksY; blockY++) {
		for (int blockX = 0; blockX < numBlocksX; blockX++) {

			// Temporary arrays to hold pixel data as floats for DCT/IDCT
			float Rinput[8][8], Ginput[8][8], Binput[8][8];
			float RoutputDCT[8][8], GoutputDCT[8][8], BoutputDCT[8][8];
			float RoutputIDCT[8][8], GoutputIDCT[8][8], BoutputIDCT[8][8];

			// Fill input arrays with pixel values from buffers
			for (int y = 0; y < blockSize; y++) {
				for (int x = 0; x < blockSize; x++) {
					int pixelIdx = (blockY * blockSize * Width) +
						(blockX * blockSize) +
						y * Width + x;

					Rinput[y][x] = Rbuf[pixelIdx];
					Ginput[y][x] = Gbuf[pixelIdx];
					Binput[y][x] = Bbuf[pixelIdx];
				}
			}

			// Apply DCT on each channel's block
			dctTransform(Rinput, RoutputDCT);
			dctTransform(Ginput, GoutputDCT);
			dctTransform(Binput, BoutputDCT);

			// Apply IDCT on each channel's transformed block to recover original values
			idctTransform(RoutputDCT, RoutputIDCT, input);
			idctTransform(GoutputDCT, GoutputIDCT, input);
			idctTransform(BoutputDCT, BoutputIDCT, input);

			// Copy recovered data back into blocks
			for (int y = 0; y < blockSize; y++) {
				for (int x = 0; x < blockSize; x++) {
					Rblocks[blockY][blockX][y * blockSize + x] =
						static_cast<unsigned char>(RoutputIDCT[y][x]);
					Gblocks[blockY][blockX][y * blockSize + x] =
						static_cast<unsigned char>(GoutputIDCT[y][x]);
					Bblocks[blockY][blockX][y * blockSize + x] =
						static_cast<unsigned char>(BoutputIDCT[y][x]);

					Rblocks[blockY][blockX][y * blockSize + x] =
						min(255, max(0, static_cast<int>(RoutputIDCT[y][x])));

					// Clamp Green channel
					Gblocks[blockY][blockX][y * blockSize + x] =
						min(255, max(0, static_cast<int>(GoutputIDCT[y][x])));

					// Clamp Blue channel
					Bblocks[blockY][blockX][y * blockSize + x] =
						min(255, max(0, static_cast<int>(BoutputIDCT[y][x])));
				}
			}
		}
	}

	// Copy blocks back to Data array using CopyBlockToData function
	Data = new char[Width * Height * 3];
	for (int blockY = 0; blockY < numBlocksY; blockY++) {
		for (int blockX = 0; blockX < numBlocksX; blockX++) {
			CopyBlockToData(blockX, blockY, Rblocks, Gblocks, Bblocks, blockSize);
		}
	}

	// Clean up temporary buffers and blocks
	for (int i = 0; i < numBlocksY; i++) {
		for (int j = 0; j < numBlocksX; j++) {
			delete[] Bblocks[i][j];
			delete[] Gblocks[i][j];
			delete[] Rblocks[i][j];
		}
		delete[] Bblocks[i];
		delete[] Gblocks[i];
		delete[] Rblocks[i];
	}

	delete[] Bbuf;
	delete[] Gbuf;
	delete[] Rbuf;
}


void MyImage::processTransform(double* dat, int length, int channel, bool isRow, bool isCompression) {
	// Use smart pointers for automatic memory management
	std::unique_ptr<double[]> transformed(new double[length]);
	std::unique_ptr<double[]> current(new double[length]);

	// Process each line (row or column) starting from bottom-left
	for (int i = length - 1; i >= 0; i--) {  // Reverse iteration
		// Calculate base index for current row/column
		const int baseIndex = isRow ? (i * Width * 3) : (i * 3);

		// Extract current line
		for (int j = 0; j < length; j++) {
			const int index = isRow ?
				baseIndex + (j * 3) + channel :  // Row indexing
				(j * Width * 3) + baseIndex + channel;   // Column indexing
			current[j] = dat[index];
		}

		if (isCompression) {
			// Compression: average pairs and store differences
			for (int j = 0; j < length; j += 2) {
				const double sum = current[j] + current[j + 1];
				const double diff = current[j] - current[j + 1];
				transformed[j / 2] = sum / 2;
				transformed[j / 2 + length / 2] = diff / 2;
			}
		}
		else {
			// Decompression: reconstruct original values
			const int halfLength = length / 2;
			for (int j = 0; j < halfLength; j++) {
				const double sum = current[j];
				const double diff = current[j + halfLength];
				transformed[2 * j] = sum + diff;
				transformed[2 * j + 1] = sum - diff;
			}
		}

		// Write back transformed data
		for (int j = 0; j < length; j++) {
			const int index = isRow ?
				baseIndex + (j * 3) + channel :  // Row indexing
				(j * Width * 3) + baseIndex + channel;   // Column indexing
			dat[index] = transformed[j];
		}
	}
}
void MyImage::compress(double* dat, int length) {
	for (int channel = 0; channel < 3; channel++) {
		processTransform(dat, length, channel, true, true);   // rows
		processTransform(dat, length, channel, false, true);  // columns
	}
}
void MyImage::decompress(double* dat, int length) {
	for (int channel = 0; channel < 3; channel++) {
		processTransform(dat, length, channel, false, false); // columns
		processTransform(dat, length, channel, true, false);  // rows
	}
}
void MyImage::DWT(int input) {
	// Use smart pointer for automatic memory management
	std::unique_ptr<double[]> dat(new double[Height * Width * 3]);

	// Convert original data to doubles
	for (int i = 0; i < Height; i++) {
		const int rowOffset = i * Width * 3;
		for (int j = 0; j < Width; j++) {
			const int baseIdx = rowOffset + j * 3;
			for (int channel = 0; channel < 3; channel++) {
				dat[baseIdx + channel] =
					static_cast<double>(static_cast<unsigned char>(Data[baseIdx + channel]));
			}
		}
	}

	// Compress data using powers of 2
	for (int size = 512; size > 1; size /= 2) {
		compress(dat.get(), size);
	}

	// Integrate copyCData functionality here
	const int length = static_cast<int>(sqrt(input));

	// Process all rows up to length
	for (int i = 0; i < length; i++) {
		// Clear coefficients from length to Width in current row
		if (length < Width) {
			const int rowOffset = i * Width * 3;
			const int startIdx = rowOffset + length * 3;
			const int endIdx = rowOffset + Width * 3;
			std::fill(dat.get() + startIdx, dat.get() + endIdx, 0.0);
		}
	}

	// Clear all remaining rows at once
	if (length < Height) {
		const int startIdx = length * Width * 3;
		const int totalElements = Height * Width * 3;
		std::fill(dat.get() + startIdx, dat.get() + totalElements, 0.0);
	}

	// Decompress data using powers of 2
	for (int size = 2; size < 1024; size *= 2) {
		decompress(dat.get(), size);
	}

	// Convert back to original format with bounds checking
	for (int i = 0; i < Height; i++) {
		const int rowOffset = i * Width * 3;
		for (int j = 0; j < Width; j++) {
			const int baseIdx = rowOffset + j * 3;
			for (int channel = 0; channel < 3; channel++) {
				Data[baseIdx + channel] =
					static_cast<char>(clamp(static_cast<int>(dat[baseIdx + channel]), 0, 255));
			}
		}
	}
}

// Here is where you would place your code to modify an image
// eg Filtering, Transformation, Cropping, etc.
bool MyImage::Modify(int input)
{
	//DCT(input);
	//DWT(input);
	return true;
}

