#include <fstream>
#include "mglTexture.h"

const mglPixelFormat mglTexture::TargaFormat = { 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000, 16, 8, 0, 24 };
mglPixelFormat mglTexture::nativeFormat = mglTexture::TargaFormat;

int	mglTexture::GetBase2Log(int x) const
{
	int i = 0;
	while (x >>= 1) { ++i; }
	return i;
}

bool mglTexture::VerifyDimension(int p_dimension) const
{
	// A tiled texture must be able to have at least the same number of
	// tiles in a dimension as there are pixels in the area of a tile.
	// The reason for this is that negative right bitshifting does not
	// translate to positive left bitshifting (and vice versa), in
	// GetTiledIndex see:
		// int x2 = ( ((y & TILE_MASK) << m_blockTileRatioShift) << TILE_SHIFT );
	// Since m_blockTileRatioShift is calculated as log(2,m_dimension/TILE) - TILE_SHIFT
	// a texture size of less than TILE*TILE results in a negative number used for shifting.
	// This restriction could be lifted if floating point math were used
	// in GetTiledIndex instead:
		// int x2 = ( int((y & TILE_MASK) * float(float(TILE) / float(m_dimension >> TILE_SHIFT))) << TILE_SHIFT );
	// However, it might be a bad idea to introduce floating point in integer operations.
	
	switch (p_dimension) {
		case 1:
		case 2:
		case 4:
		case 8:
		case 16:
		case 32:
		case 64:
		case 128:
		case 256:
		case 512:
		case 1024:
		case 2048:
		case 4096:
			//return p_dimension >= (mglTILE*mglTILE);
			return true;
		default: break;
	}
	return false;
}

bool mglTexture::LoadTGA(const mtlDirectory &p_filename)
{
	std::ifstream fin(p_filename.GetDirectory().GetChars(), std::ios::binary);
	if (!fin.is_open()) {
		m_error.Copy("File unreadable");
		return false;
	}
	enum RelevantHeaderField
	{
		COLOR_MAP_TYPE	= 1,
		IMAGE_TYPE		= 2,
		IMAGE_WIDTH		= 12,
		IMAGE_HEIGHT	= 14,
		PIXEL_DEPTH		= 16,
		HEADER_SIZE		= 18
	};
	enum SupportedColorMapType
	{
		NO_COLOR_MAP
	};
	enum SupportedTextureType
	{
		UNCOMPRESSED_TRUECOLOR_IMAGE	= 2,
		COMPRESSED_TRUECOLOR_IMAGE		= 10
	};
	enum SupportedPixelDepth
	{
		BGR888		= 24,
		BGRA8888	= 32
	};

	unsigned char header[HEADER_SIZE];
	fin.read((char*)header, HEADER_SIZE);
	if (fin.fail()) {
		m_error.Copy("Reading TGA header failed");
		return false;
	} else if (header[COLOR_MAP_TYPE] != NO_COLOR_MAP) {
		m_error.Copy("TGA contains color map");
		return false;
	}
	const int bpp = header[PIXEL_DEPTH] == BGRA8888 ? 4 : (header[PIXEL_DEPTH] == BGR888 ? 3 : -1);
	if (bpp == -1) {
		m_error.Copy("Unsupported pixel depth");
		return false;
	}

	if (header[IMAGE_TYPE] == UNCOMPRESSED_TRUECOLOR_IMAGE) {
		int w = (int)(*(unsigned short*)(header+IMAGE_WIDTH));
		int h = (int)(*(unsigned short*)(header+IMAGE_HEIGHT));
		if (w != h) {
			m_error.Copy("Dimensions are not equal");
			return false;
		}
		if (!Create(w)) {
			return false;
		}
		const int SIZE = GetArea();
		// allocate one byte extra, since we will need to read one garbage byte when
		// converting a pointer to 3 bytes to a pointer 4 (unsigned int)
		unsigned char *image = new unsigned char[SIZE*bpp + 1];

		if (fin.read((char*)image, SIZE*bpp).bad()) {
			m_error.Copy("Reading pixel data failed");
			delete [] image;
			return false;
		}
		unsigned char *imageData = image;
		if (bpp == 3) {
			for (int y = 0; y < m_dimension; ++y) {
				for (int x = 0; x < m_dimension; ++x) {
					unsigned int color = (*(unsigned int*)(imageData));
					unsigned int b = (color & TargaFormat.BMask) >> TargaFormat.BShift;
					unsigned int g = (color & TargaFormat.GMask) >> TargaFormat.GShift;
					unsigned int r = (color & TargaFormat.RMask) >> TargaFormat.RShift;
					m_pixels[GetMortonIndex(x, y)] =
						(r << nativeFormat.RShift) |
						(g << nativeFormat.GShift) |
						(b << nativeFormat.BShift) |
						nativeFormat.AMask;
					imageData += bpp;
				}
			}
		} else {
			for (int y = 0; y < m_dimension; ++y) {
				for (int x = 0; x < m_dimension; ++x) {
					unsigned int color = *(unsigned int*)(imageData);
					unsigned int b = (color & TargaFormat.BMask) >> TargaFormat.BShift;
					unsigned int g = (color & TargaFormat.GMask) >> TargaFormat.GShift;
					unsigned int r = (color & TargaFormat.RMask) >> TargaFormat.RShift;
					unsigned int a = (color & TargaFormat.AMask) >> TargaFormat.AShift;
					m_pixels[GetMortonIndex(x, y)] =
						(r << nativeFormat.RShift) |
						(g << nativeFormat.GShift) |
						(b << nativeFormat.BShift) |
						((a << nativeFormat.AShift) & nativeFormat.AMask); // masks out alpha if it does not exist
					imageData += bpp;
				}
			}
		}

		delete [] image;

	} else if (header[IMAGE_TYPE] == COMPRESSED_TRUECOLOR_IMAGE) {

		/*if (*(unsigned short*)(header+IMAGE_HEIGHT) != *(unsigned short*)(header+IMAGE_WIDTH) || !Create((int)(*(unsigned short*)(header+IMAGE_WIDTH)), (int)(*(unsigned short*)(header+IMAGE_HEIGHT)))) {
			return false;
		}
		const int NUM_BYTES = GetArea()*bpp;
		unsigned char chunkSize;
		int currentByte = 0;
		while (currentByte < NUM_BYTES) {
			fin.read((char*)&chunkSize, 1);
			if (chunkSize < 128) {
				++chunkSize;
				for (int c = 0; c < (int)chunkSize; ++c) {
					fin.read((char*)(m_pixels+currentByte), bpp);
					if (fin.fail()) { return false; }
					Swap((m_pixels+currentByte)[0], (m_pixels+currentByte)[2]); // to RGB(A)
					currentByte += bytesPerPixel;
				}
			} else {
				chunkSize -= 127;
				fin.read((char*)(m_pixels+currentByte), bpp);
				if (fin.fail()) { return false; }
				GLubyte *copyPixel = m_pixels+currentByte;
				Swap(copyPixel[0], copyPixel[2]);
				currentByte += bytesPerPixel;

				for (int i = 0; i < chunkSize-1; ++i) {
					for (int j = 0; j < bpp; ++j) {
						m_pixels[currentByte+j] = copyPixel[j];
					}
					currentByte += bpp;
				}
			}
		}*/
		m_error.Copy("Compressed TGA not supported yet");
		return false;
	} else {
		m_error.Copy("Unknown image type");
		return false;
	}
	return true;
}

mglTexture::mglTexture( void ) : m_pixels(NULL), m_dimension(0), m_dimensionShift(0), m_dimensionMask(0), m_blocksPerDimShift(0), m_blockTileRatioShift(0)
{}

mglTexture::mglTexture(int p_dimension, unsigned int p_color) : m_pixels(NULL), m_dimension(0), m_dimensionShift(0), m_dimensionMask(0), m_blocksPerDimShift(0), m_blockTileRatioShift(0)
{
	Create(p_dimension, p_color);
}

mglTexture::~mglTexture( void )
{
	delete [] m_pixels;
}

bool mglTexture::Load(const mtlDirectory &p_filename)
{
	if (p_filename.GetExtension().Compare("tga")) { // targa
		return LoadTGA(p_filename);
	}
	m_error.Copy(p_filename.GetExtension());
	return false;
}

bool mglTexture::Create(int p_dimension)
{
	Free();
	if (VerifyDimension(p_dimension)) {
		m_dimension = p_dimension;
		m_dimensionShift = GetBase2Log(m_dimension);
		m_dimensionMask = m_dimension - 1;
		m_blocksPerDimShift = GetBase2Log(m_dimension >> mglTILE_SHIFT);
		m_blockTileRatioShift = m_blocksPerDimShift - mglTILE_SHIFT; // essentially, this determines the base 2 log of m_dimension / (TILE*TILE)
		m_pixels = new unsigned int[GetArea()];
	} else {
		m_error.Copy("Bad dimensions");
	}
	return !IsBad();
}

bool mglTexture::Create(int p_dimension, unsigned int p_color)
{
	if (Create(p_dimension)) {
		for (int i = 0; i < GetArea(); ++i) { m_pixels[i] = p_color; }
		return true;
	}
	return false;
}

void mglTexture::Free( void )
{
	delete [] m_pixels;
	m_pixels = NULL;
	m_dimension = 0;
	m_dimensionShift = 0;
	m_dimensionMask = 0;
	m_blocksPerDimShift = 0;
	m_blockTileRatioShift = 0;
	m_error.Copy("");
}

void mglTexture::Copy(const mglTexture &p_texture)
{
	if (Create(p_texture.GetDimension())) {
		for (int i = 0; i < GetArea(); ++i) {
			m_pixels[i] = p_texture.m_pixels[i];
		}
	}
}
