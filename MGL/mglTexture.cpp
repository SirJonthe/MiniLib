#include <fstream>
#include "mglTexture.h"
#include "mglImage.h"
#include "../MML/mmlMath.h"
#include "../MTL/mtlArray.h"

enum TGA_RelevantHeaderField
{
	COLOR_MAP_TYPE = 1,
	IMAGE_TYPE     = 2,
	IMAGE_WIDTH    = 12,
	IMAGE_HEIGHT   = 14,
	PIXEL_DEPTH    = 16,
	HEADER_SIZE    = 18
};
enum TGA_SupportedColorMapType
{
	NO_COLOR_MAP
};
enum TGA_SupportedTextureType
{
	UNCOMPRESSED_TRUECOLOR_IMAGE = 2,
	UNCOMPRESSED_GRAYSCALE_IMAGE = 3,
	COMPRESSED_TRUECOLOR_IMAGE   = 10,
	COMPRESSED_GRAYSCALE_IMAGE   = 11
};
enum TGA_SupportedPixelDepth
{
	GRAY8        = 8,
	BGRA5551     = 16,
	GRAY_ALPHA16 = 16,
	BGR888       = 24,
	BGRA8888     = 32
};

mglPixel32 mglTexture::UnpackTGAPixel(unsigned char *pixel_data, int bpp, int type) const
{
	mglPixel32 color;
	switch (type) {
	case COMPRESSED_GRAYSCALE_IMAGE:
	case UNCOMPRESSED_GRAYSCALE_IMAGE:
		switch (bpp) {
		case 1:
			// XXXXXXXX
			color.bytes[m_format.index.r] = pixel_data[0];
			color.bytes[m_format.index.g] = pixel_data[0];
			color.bytes[m_format.index.b] = pixel_data[0];
			color.bytes[m_format.index.a] = 0xff;
			break;
		case 2:
			// XXXXXXXX AAAAAAAA
			color.bytes[m_format.index.r] = pixel_data[0];
			color.bytes[m_format.index.g] = pixel_data[0];
			color.bytes[m_format.index.b] = pixel_data[0];
			color.bytes[m_format.index.a] = pixel_data[1];
			break;
		default:
			color.color = 0;
		}
		break;
	case COMPRESSED_TRUECOLOR_IMAGE:
	case UNCOMPRESSED_TRUECOLOR_IMAGE:
		switch (bpp) {
		// 1 byte color map?
		case 2:
			// GGGBBBBB ARRRRRGG
			color.bytes[m_format.index.r] = (pixel_data[1] & 0x7c) << 1;
			color.bytes[m_format.index.g] = ((pixel_data[1] & 0x30) << 5) | ((pixel_data[0] & 0xe0) >> 2);
			color.bytes[m_format.index.b] = (pixel_data[0] & 0x1f) << 3;
			color.bytes[m_format.index.a] = 0xff * ((pixel_data[1] & 0x80) >> 7);
			break;
		case 3:
			// BBBBBBBB GGGGGGGG RRRRRRRR
			color.bytes[m_format.index.b] = pixel_data[0];
			color.bytes[m_format.index.g] = pixel_data[1];
			color.bytes[m_format.index.r] = pixel_data[2];
			color.bytes[m_format.index.a] = 0xff;
			break;
		case 4:
			// BBBBBBBB GGGGGGGG RRRRRRRR AAAAAAAA
			color.bytes[m_format.index.b] = pixel_data[0];
			color.bytes[m_format.index.g] = pixel_data[1];
			color.bytes[m_format.index.r] = pixel_data[2];
			color.bytes[m_format.index.a] = pixel_data[3];
			break;
		default:
			color.color = 0;
		}
		break;
	default:
		color.color = 0;
	}
	return color;
}

bool mglTexture::LoadTGA(const mtlDirectory &p_filename)
{
	// Take into account origin of the image axis

	std::ifstream fin(p_filename.GetDirectory().GetChars(), std::ios::binary);
	if (!fin.is_open()) {
		SetError("[TGA] Failed to open file");
		return false;
	}

	unsigned char header[HEADER_SIZE];
	fin.read((char*)header, HEADER_SIZE);
	if (fin.fail()) {
		SetError("[TGA] Failed to read header");
		return false;
	}
	if (header[COLOR_MAP_TYPE] != NO_COLOR_MAP) {
		SetError("[TGA] Color map not supported");
		return false;
	}

	int bpp = header[PIXEL_DEPTH] / 8;

	if (!Create((int)(*(unsigned short*)(header+IMAGE_WIDTH)), (int)(*(unsigned short*)(header+IMAGE_HEIGHT)))) {
		return false;
	}

	const int NUM_PIXELS = GetArea();
	const int NUM_BYTES = NUM_PIXELS * bpp;

	if (header[IMAGE_TYPE] == UNCOMPRESSED_TRUECOLOR_IMAGE || header[IMAGE_TYPE] == UNCOMPRESSED_GRAYSCALE_IMAGE) {

		mtlArray<unsigned char> image_data(NUM_BYTES);
		unsigned char *image_ptr = image_data;
		if (fin.read((char*)((unsigned char*)image_data), image_data.GetSize()).bad()) {
			SetError("[TGA] Failed to read uncompressed pixel data");
			return false;
		}
		for (int i = 0; i < NUM_PIXELS; ++i) {
			m_pixels[i] = UnpackTGAPixel(image_ptr, bpp, header[IMAGE_TYPE]);
			image_ptr += bpp;
		}

	} else if (header[IMAGE_TYPE] == COMPRESSED_TRUECOLOR_IMAGE || header[IMAGE_TYPE] == COMPRESSED_GRAYSCALE_IMAGE) {

		int currentIndex = 0;
		while (currentIndex < NUM_PIXELS) {
			unsigned char chunk_header;
			if (fin.read((char*)(&chunk_header), 1).bad()) {
				SetError("[TGA] Failed to read compressed chunk header");
				return false;
			}
			const int size = 1 + (chunk_header & 0x7f);
			if (currentIndex + size > NUM_PIXELS) {
				SetError("[TGA] Chunk size exceeds pixel count");
				return false;
			}
			if (chunk_header & 0x80) { // RLE compressed
				unsigned char color_bytes[4]; // maximum number of bytes
				if (fin.read((char*)color_bytes, bpp).bad()) {
					SetError("[TGA] Failed to read compressed chunk");
					return false;
				}
				mglPixel color = UnpackTGAPixel(color_bytes, bpp, header[IMAGE_TYPE]);
				for (int i = 0; i < size; ++i) {
					m_pixels[currentIndex] = color;
					++currentIndex;
				}
			} else { // non-compressed
				unsigned char chunk_data[128*4];
				unsigned char *chunk_ptr = chunk_data;
				if (fin.read((char*)chunk_data, size).bad()) {
					SetError("[TGA] Failed to read uncompressed chunk");
					return false;
				}
				for (int i = 0; i < size; ++i) {
					m_pixels[currentIndex] = UnpackTGAPixel(chunk_ptr, bpp, header[IMAGE_TYPE]);
					chunk_ptr += bpp;
					++currentIndex;
				}
			}
		}

	} else {
		SetError("[TGA] Unknown image type");
		return false;
	}
	return true;
}

bool mglTexture::VerifyDimension(int p_dimension) const
{
	return p_dimension >= 4  && mmlIsPow2((unsigned int)p_dimension);
}

bool mglTexture::Create(int width, int height)
{
	Free();
	if (VerifyDimension(width) && VerifyDimension(height)) {
		m_width = width;
		m_height = height;
		m_width_mask = m_width - 1;
		m_height_mask = m_height - 1;
		m_pixels = new mglByte[m_width*m_height*m_format.bytes_per_pixel];
	} else {
		SetError("Invalid dimensions"); // Add more descriptive error here
	}
}

bool mglTexture::Create(int width, int height, mglPixelFormat format)
{
	m_format = format;
	return Create(width, height);
}

bool mglTexture::Create(int width, int height, mglByte r, mglByte g, mglByte b, mglByte a)
{
	if (Create(width, height)) {
		// add pixel conversion here
		return true;
	}
	return false;
}

bool mglTexture::Create(int width, int height, mglPixelFormat format, mglByte r, mglByte g, mglByte b, mglByte a)
{
	if (Create(width, height, format)) {
		// add pixel conversion here
		return true;
	}
	return false;
}

bool mglTexture::Load(const mtlDirectory &p_filename)
{
	if (p_filename.GetExtension().Compare("tga")) { // targa
		return LoadTGA(p_filename);
	}
	mtlString error;
	error.Copy("Cannot load format: ");
	error.Append(p_filename.GetExtension());
	SetError(error);
	return false;
}

void mglTexture::Free( void )
{
	delete [] m_pixels;
	m_pixels = NULL;
	m_width = 0;
	m_height = 0;
	m_width_mask = 0;
	m_height_mask = 0;
	SetError("");
}
