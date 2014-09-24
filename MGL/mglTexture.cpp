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
		SetError("File unreadable");
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
		UNCOMPRESSED_GRAYSCALE_IMAGE	= 3,
		COMPRESSED_TRUECOLOR_IMAGE		= 10,
		COMPRESSED_GRAYSCALE_IMAGE		= 11
	};
	enum SupportedPixelDepth
	{
		GRAY8		= 8,
		BGRA5551	= 16,
		BGR888		= 24,
		BGRA8888	= 32
	};

	unsigned char header[HEADER_SIZE];
	fin.read((char*)header, HEADER_SIZE);
	if (fin.fail()) {
		SetError("Reading TGA header failed");
		return false;
	}
	if (header[COLOR_MAP_TYPE] != NO_COLOR_MAP) {
		SetError("TGA contains color map");
		return false;
	}
	
	int bpp = header[PIXEL_DEPTH] == BGRA8888 ? 4 : (header[PIXEL_DEPTH] == BGR888 ? 3 : -1);
	if (header[IMAGE_TYPE] == UNCOMPRESSED_TRUECOLOR_IMAGE || header[IMAGE_TYPE] == COMPRESSED_TRUECOLOR_IMAGE) {
		if (header[PIXEL_DEPTH] == BGRA8888) {
			bpp = 4;
		} else if (header[PIXEL_DEPTH] == BGR888) {
			bpp = 3;
		} else {
			bpp = -1;
		}
	} else if (header[IMAGE_TYPE] == UNCOMPRESSED_GRAYSCALE_IMAGE || header[IMAGE_TYPE] == COMPRESSED_GRAYSCALE_IMAGE) {
		if (header[PIXEL_DEPTH] == GRAY8) {
			bpp = 1;
		} else {
			bpp = -1;
		}
	}
	
	if (bpp == -1) {
		SetError("Unsupported pixel depth");
		return false;
	}
	
	int w = (int)(*(unsigned short*)(header+IMAGE_WIDTH));
	int h = (int)(*(unsigned short*)(header+IMAGE_HEIGHT));
	if (w != h) {
		SetError("Dimensions are not equal");
		return false;
	}
	if (!Create(w)) {
		return false;
	}

	if (header[IMAGE_TYPE] == UNCOMPRESSED_TRUECOLOR_IMAGE) {
		
		const int SIZE = GetArea();
		// allocate one byte extra, since we will need to read one garbage byte when
		// converting a pointer to 3 bytes to a pointer 4 (unsigned int)
		unsigned char *image = new unsigned char[SIZE*bpp + 1];

		if (fin.read((char*)image, SIZE*bpp).bad()) {
			SetError("Reading pixel data failed");
			delete [] image;
			return false;
		}
		
		unsigned char *imageData = image;
		if (bpp == 3) {
			
			for (int i = 0; i < SIZE; ++i) {
				const unsigned int color = (*(unsigned int*)(imageData));
				const unsigned int b = (color & TargaFormat.BMask) >> TargaFormat.BShift;
				const unsigned int g = (color & TargaFormat.GMask) >> TargaFormat.GShift;
				const unsigned int r = (color & TargaFormat.RMask) >> TargaFormat.RShift;
				m_pixels[i] =
				(r << nativeFormat.RShift) |
				(g << nativeFormat.GShift) |
				(b << nativeFormat.BShift) |
				nativeFormat.AMask;
				imageData += bpp;
			}
		} else {
			
			for (int i = 0; i < SIZE; ++i) {
				const unsigned int color = *(unsigned int*)(imageData);
				const unsigned int b = (color & TargaFormat.BMask) >> TargaFormat.BShift;
				const unsigned int g = (color & TargaFormat.GMask) >> TargaFormat.GShift;
				const unsigned int r = (color & TargaFormat.RMask) >> TargaFormat.RShift;
				const unsigned int a = (color & TargaFormat.AMask) >> TargaFormat.AShift;
				m_pixels[i] =
				(r << nativeFormat.RShift) |
				(g << nativeFormat.GShift) |
				(b << nativeFormat.BShift) |
				(a << nativeFormat.AShift);
				imageData += bpp;
			}
		}

		delete [] image;

	} else if (header[IMAGE_TYPE] == COMPRESSED_TRUECOLOR_IMAGE) {
		
		unsigned char chunk[128*4];
		const int PixelCount = GetArea();
		unsigned char chunkSize;
		int currentIndex = 0;
		while (currentIndex < PixelCount) {
			
			fin.read((char*)&chunkSize, 1); // reads the chunk header (chunk size)
			
			if (chunkSize < 128) {
				
				++chunkSize;
				const int ChunkBytes = chunkSize * bpp;
				
				fin.read((char*)(chunk), ChunkBytes);
				if (fin.fail()) { return false; }
				
				if (bpp == 4) {
					for (int i = 0; i < chunkSize; ++i) {
						const unsigned int color = *(unsigned int*)(chunk+i);
						const unsigned int b = (color & TargaFormat.BMask) >> TargaFormat.BShift;
						const unsigned int g = (color & TargaFormat.GMask) >> TargaFormat.GShift;
						const unsigned int r = (color & TargaFormat.RMask) >> TargaFormat.RShift;
						const unsigned int a = (color & TargaFormat.AMask) >> TargaFormat.AShift;
						m_pixels[i] =
						(r << nativeFormat.RShift) |
						(g << nativeFormat.GShift) |
						(b << nativeFormat.BShift) |
						(a << nativeFormat.AShift);
						++currentIndex;
					}
				} else {
					for (int i = 0; i < chunkSize; ++i) {
						const unsigned int color = *(unsigned int*)(chunk+i);
						const unsigned int b = (color & TargaFormat.BMask) >> TargaFormat.BShift;
						const unsigned int g = (color & TargaFormat.GMask) >> TargaFormat.GShift;
						const unsigned int r = (color & TargaFormat.RMask) >> TargaFormat.RShift;
						m_pixels[i] =
						(r << nativeFormat.RShift) |
						(g << nativeFormat.GShift) |
						(b << nativeFormat.BShift) |
						nativeFormat.AMask;
						++currentIndex;
					}
				}
				
			} else { // compressed chunk
				
				chunkSize -= 127;
				unsigned int color = 0xffffffff;
				fin.read((char*)(&color), bpp);
				if (fin.fail()) { return false; }
				
				if (bpp == 4) {
					unsigned int b = (color & TargaFormat.BMask) >> TargaFormat.BShift;
					unsigned int g = (color & TargaFormat.GMask) >> TargaFormat.GShift;
					unsigned int r = (color & TargaFormat.RMask) >> TargaFormat.RShift;
					unsigned int a = (color & TargaFormat.AMask) >> TargaFormat.AShift;
					color =
					(r << nativeFormat.RShift) |
					(g << nativeFormat.GShift) |
					(b << nativeFormat.BShift) |
					(a << nativeFormat.AShift);
				} else {
					color |= (nativeFormat.AMask);
				}

				for (int i = 0; i < chunkSize-1; ++i) {
					m_pixels[currentIndex] = color;
					++currentIndex;
				}
			}
		}
	} else if (header[IMAGE_TYPE] == UNCOMPRESSED_GRAYSCALE_IMAGE) {
		
		const int SIZE = GetArea();
		unsigned char *image = new unsigned char[SIZE];
		
		if (fin.read((char*)image, SIZE).bad()) {
			SetError("Reading pixel data failed");
			delete [] image;
			return false;
		}
		
		unsigned char *imageData = image;
		
		for (int i = 0; i < SIZE; ++i) {
			const unsigned char color = *imageData;
			m_pixels[i] =
			((unsigned int)(color) << nativeFormat.RShift) |
			((unsigned int)(color) << nativeFormat.GShift) |
			((unsigned int)(color) << nativeFormat.BShift) |
			nativeFormat.AMask;
			++imageData;
		}
		
		delete [] image;
		
	} else if (header[IMAGE_TYPE] == COMPRESSED_GRAYSCALE_IMAGE) {
		
		unsigned char chunk[128*4];
		const int PixelCount = GetArea();
		unsigned char chunkSize;
		int currentIndex = 0;
		while (currentIndex < PixelCount) {
			
			fin.read((char*)&chunkSize, 1); // reads the chunk header (chunk size)
			
			if (chunkSize < 128) {
				
				++chunkSize;
				const int ChunkBytes = chunkSize;
				
				fin.read((char*)(chunk), ChunkBytes);
				if (fin.fail()) { return false; }
				
				for (int i = 0; i < chunkSize; ++i) {
					const unsigned int color = *(chunk+i);
					m_pixels[i] =
					((unsigned int)(color) << nativeFormat.RShift) |
					((unsigned int)(color) << nativeFormat.GShift) |
					((unsigned int)(color) << nativeFormat.BShift) |
					nativeFormat.AMask;
					++currentIndex;
				}
				
			} else { // compressed chunk
				
				chunkSize -= 127;
				unsigned int color = 0xffffffff;
				fin.read((char*)(&color), 1);
				if (fin.fail()) { return false; }
				
				
				for (int i = 0; i < chunkSize-1; ++i) {
					m_pixels[currentIndex] =
					((unsigned int)(color) << nativeFormat.RShift) |
					((unsigned int)(color) << nativeFormat.GShift) |
					((unsigned int)(color) << nativeFormat.BShift) |
					nativeFormat.AMask;
					++currentIndex;
				}
			}
		}
		
	} else {
		SetError("Unknown image type");
		Free();
		return false;
	}
	return true;
}

void mglTexture::OrderAsMorton( void )
{
	const int Area = GetArea();
	const int Dim = GetDimension();
	unsigned int *mortonPixels = new unsigned int[Area];
	unsigned int *pixels = m_pixels;
	for (int y = 0; y < Dim; ++y) {
		for (int x = 0; x < Dim; ++x) {
			mortonPixels[GetMortonIndex(x, y)] = *pixels;
			++pixels;
		}
	}
	delete [] m_pixels;
	m_pixels = mortonPixels;
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
		bool retval = LoadTGA(p_filename);
		OrderAsMorton();
		return retval;
	} else if (p_filename.GetExtension().Compare("tgam")) { // Morton ordered targa
		return LoadTGA(p_filename);
	}
	mtlString error;
	error.Copy("Cannot load format: ");
	error.Append(p_filename.GetExtension());
	SetError(error);
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
		SetError("Bad dimensions");
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
	SetError("");
}

/*#include "../MML/mmlMath.h"
unsigned int mglTexture::GetPixelXY(float x, float y) const
{
	struct color {
		unsigned int r, g, b, a;
	};
	
	struct {
		unsigned int operator()(unsigned int xy00, unsigned int xy10, unsigned int xy01, unsigned int xy11, int u, int v) const
		{
			struct {
				unsigned int operator()(unsigned int x0, unsigned int x1, int u) const { return x0 + (((x1 - x0) * u) >> 8); }
			} lerp;
			return lerp(lerp(xy00, xy10, u), lerp(xy01, xy11, u), v);
		}
	} blerp;
	
	int ix = int(x);
	int iy = int(y);
	int u = (x - ix) * 256.0f;
	int v = (y - iy) * 256.0f;
	
	unsigned int c00 = GetPixel(ix, iy);
	unsigned int c10 = GetPixel(ix+1, iy);
	unsigned int c01 = GetPixel(ix, iy+1);
	unsigned int c11 = GetPixel(ix+1, iy+1);
	
	color u00 = { (unsigned int)(GetRed(c00)) << 8, (unsigned int)(GetGreen(c00)) << 8, (unsigned int)(GetBlue(c00)) << 8, (unsigned int)(GetAlpha(c00)) << 8 };
	color u10 = { (unsigned int)(GetRed(c10)) << 8, (unsigned int)(GetGreen(c10)) << 8, (unsigned int)(GetBlue(c10)) << 8, (unsigned int)(GetAlpha(c10)) << 8 };
	color u01 = { (unsigned int)(GetRed(c01)) << 8, (unsigned int)(GetGreen(c01)) << 8, (unsigned int)(GetBlue(c01)) << 8, (unsigned int)(GetAlpha(c01)) << 8 };
	color u11 = { (unsigned int)(GetRed(c11)) << 8, (unsigned int)(GetGreen(c11)) << 8, (unsigned int)(GetBlue(c11)) << 8, (unsigned int)(GetAlpha(c11)) << 8 };
	color uuv = { blerp(u00.r, u10.r, u01.r, u11.r, u, v) >> 8, blerp(u00.g, u10.g, u01.g, u11.g, u, v) >> 8, blerp(u00.b, u10.b, u01.b, u11.b, u, v) >> 8, blerp(u00.a, u10.a, u01.a, u11.a, u, v) >> 8 };
	
	return GetColor(uuv.r, uuv.g, uuv.b, uuv.a);
}*/

void mglTexture::Copy(const mglTexture &p_texture)
{
	if (Create(p_texture.GetDimension())) {
		for (int i = 0; i < GetArea(); ++i) {
			m_pixels[i] = p_texture.m_pixels[i];
		}
	}
}
