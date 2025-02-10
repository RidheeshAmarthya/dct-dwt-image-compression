# Image Compression Comparison: DCT vs DWT

This project implements and compares image compression techniques using Discrete Cosine Transform (DCT) and Discrete Wavelet Transform (DWT). It provides a visual comparison of the two methods by displaying the compressed images side by side.

## Output (-1)
<img src="https://github.com/RidheeshAmarthya/portfolio-website/blob/main/assets/MM-P3.gif">

## Features

- Reads RGB image files (512x512 pixels)
- Implements DCT and DWT compression techniques
- Allows comparison of compression quality with varying coefficients
- Supports progressive encoding analysis

## Usage

The program takes two command-line arguments:

1. Input image file name (RGB format, 512x512 pixels)
2. Number of coefficients for decoding (n)

```bash
image Image.rgb <n>
```

Where `<n>` can be:
- A power of 4 between 4096 and 262144
- -1 for progressive encoding analysis (DCT: 64 iterations, DWT: 10 iterations)
- -2 for equivalent quality comparison (64 iterations for both DCT and DWT)

## Examples

```bash
image Image.rgb 262144  # Full quality (no loss)
image Image.rgb 65536   # Half of the coefficients
image Image.rgb 16384   # 1/16th of the coefficients
image Image.rgb -1      # Progressive encoding analysis
image Image.rgb -2      # Equivalent quality comparison
```

## Implementation Details

### Encoding

- **DCT**: The image is divided into 8x8 contiguous blocks, and DCT is applied to each block for each channel.
- **DWT**: Conversion is performed on rows and columns recursively, applying low-pass and high-pass filters.

### Decoding

- **DCT**: Selects the first m coefficients in zigzag order for each 8x8 block, where m = round(n/4096).
- **DWT**: Selects n coefficients from the lower left square of the transformed image.

### Progressive Analysis

- **DCT**: 64 iterations, incrementing coefficients used in each 8x8 block.
- **DWT**: 10 iterations, using coefficients in powers of 4.

## Output

The program displays two images side by side:
- Left: Image compressed and decompressed using DCT
- Right: Image compressed and decompressed using DWT

## Dependencies

- Windows API for GUI
- C++ Standard Library

## Compilation

Ensure you have a C++ compiler that supports C++11 or later. Compile the source files and link against the Windows API.

## Notes

- The program is designed for Windows operating systems.
- Input images must be in RGB format and 512x512 pixels in size.
- For best results in progressive analysis, experiment with coefficient selection in both DCT and DWT methods.
