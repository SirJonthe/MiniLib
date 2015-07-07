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

bool mglTexture::VerifyDimension(int p_dimension) const
{
	return p_dimension >= 4  && mmlIsPow2((unsigned int)p_dimension);
}

mglPixel32 mglTexture::UnpackTGAPixel(unsigned char *pixel_data, int bpp, int type) const
{
	mglPixel32 color;
	switch (type) {
	case COMPRESSED_GRAYSCALE_IMAGE:
	case UNCOMPRESSED_GRAYSCALE_IMAGE:
		switch (bpp) {
		case 1:
			// XXXXXXXX
			color.bytes[m_order.index.r] = pixel_data[0];
			color.bytes[m_order.index.g] = pixel_data[0];
			color.bytes[m_order.index.b] = pixel_data[0];
			color.bytes[m_order.index.a] = 0xff;
			break;
		case 2:
			// XXXXXXXX AAAAAAAA
			color.bytes[m_order.index.r] = pixel_data[0];
			color.bytes[m_order.index.g] = pixel_data[0];
			color.bytes[m_order.index.b] = pixel_data[0];
			color.bytes[m_order.index.a] = pixel_data[1];
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
			color.bytes[m_order.index.r] = (pixel_data[1] & 0x7c) << 1;
			color.bytes[m_order.index.g] = ((pixel_data[1] & 0x30) << 5) | ((pixel_data[0] & 0xe0) >> 2);
			color.bytes[m_order.index.b] = (pixel_data[0] & 0x1f) << 3;
			color.bytes[m_order.index.a] = 0xff * ((pixel_data[1] & 0x80) >> 7);
			break;
		case 3:
			// BBBBBBBB GGGGGGGG RRRRRRRR
			color.bytes[m_order.index.b] = pixel_data[0];
			color.bytes[m_order.index.g] = pixel_data[1];
			color.bytes[m_order.index.r] = pixel_data[2];
			color.bytes[m_order.index.a] = 0xff;
			break;
		case 4:
			// BBBBBBBB GGGGGGGG RRRRRRRR AAAAAAAA
			color.bytes[m_order.index.b] = pixel_data[0];
			color.bytes[m_order.index.g] = pixel_data[1];
			color.bytes[m_order.index.r] = pixel_data[2];
			color.bytes[m_order.index.a] = pixel_data[3];
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

	// DEBUG
	mtlString tmp;
	tmp.FromInt(m_width);
	m_format_str.Append(tmp).Append("x");
	tmp.FromInt(m_height);
	m_format_str.Append(tmp).Append("x");
	tmp.FromInt(bpp);
	m_format_str.Append(tmp);
	if (header[IMAGE_TYPE] == COMPRESSED_TRUECOLOR_IMAGE || header[IMAGE_TYPE] == UNCOMPRESSED_TRUECOLOR_IMAGE) {
		m_format_str.Append(" Truecolor");
	} else if (header[IMAGE_TYPE] == COMPRESSED_GRAYSCALE_IMAGE || header[IMAGE_TYPE] == UNCOMPRESSED_GRAYSCALE_IMAGE) {
		m_format_str.Append(" Grayscale");
	} else {
		m_format_str.Append(" ???");
	}
	if (header[IMAGE_TYPE] == COMPRESSED_TRUECOLOR_IMAGE || header[IMAGE_TYPE] == COMPRESSED_GRAYSCALE_IMAGE) {
		m_format_str.Append(" (RLE)");
	}
	// DEBUG

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
				mglPixel32 color = UnpackTGAPixel(color_bytes, bpp, header[IMAGE_TYPE]);
				for (int i = 0; i < size; ++i) {
					m_pixels[currentIndex] = color;
					++currentIndex;
				}
			} else { // non-compressed
				unsigned char chunk_data[128*4];
				unsigned char *chunk_ptr = chunk_data;
				if (fin.read((char*)chunk_data, size*bpp).bad()) {
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

void mglTexture::Swizzle_Z( void )
{
	const int area = GetArea();
	if (area <= 0) { return; }

	mglPixel32 *dst = new mglPixel32[area];
	const mglPixel32 *src = m_pixels;

	for (int y = 0; y < m_height; ++y) {
		for (int x = 0; x < m_width; ++x) {
			dst[mtlEncodeMorton2(x, y)] = *src;
			++src;
		}
	}

	delete [] m_pixels;
	m_pixels = dst;
}

void mglTexture::Compress_VQ( void )
{

}

mglTexture::mglTexture( void ) : m_pixels(NULL), m_width(0), m_height(0), m_width_mask(0), m_height_mask(0), m_width_shift(0), m_height_shift(0)
{
	m_format.bytes_per_pixel = 4;
	m_format.color           = mglPixelFormat::Color_Truecolor;
	m_order.index.r          = 0;
	m_order.index.g          = 1;
	m_order.index.b          = 2;
	m_order.index.a          = 3;
}

mglTexture::mglTexture(int p_width, int p_height) : m_pixels(NULL), m_width(0), m_height(0), m_width_mask(0), m_height_mask(0), m_width_shift(0), m_height_shift(0)
{
	m_format.bytes_per_pixel = 4;
	m_format.color           = mglPixelFormat::Color_Truecolor;
	m_order.index.r          = 0;
	m_order.index.g          = 1;
	m_order.index.b          = 2;
	m_order.index.a          = 3;
	Create(p_width, p_height);
}

bool mglTexture::Create(int width, int height)
{
	Free();
	if (VerifyDimension(width) && VerifyDimension(height)) {
		m_width        = width;
		m_height       = height;
		m_width_mask   = m_width - 1;
		m_height_mask  = m_height - 1;
		m_width_shift  = 0;
		m_height_shift = 0;
		int tmp = m_width;
		while (tmp != 1) { ++m_width_shift; tmp >>= 1; }
		tmp = m_height;
		while (tmp != 1) { ++m_height_shift; tmp >>= 1; }
		//m_pixels = new mglByte[m_width*m_height*m_format.bytes_per_pixel];
		m_pixels = new mglPixel32[m_width*m_height];
		return true;
	} else {
		SetError("Invalid dimensions"); // Add more descriptive error here
	}
	return false;
}

bool mglTexture::Load(const mtlDirectory &p_filename)
{
	bool ret_val = false;
	if (p_filename.GetExtension().Compare("tga")) { // targa
		ret_val = LoadTGA(p_filename);
	} else {
		mtlString error;
		error.Copy("Cannot load format: ");
		error.Append(p_filename.GetExtension());
		SetError(error);
	}

	if (ret_val) {
		Swizzle_Z();
		Pack_SOA();
		Compress_VQ();
	} else {
		mtlString error;
		error.Copy(GetError());
		Free();
		SetError(error);
	}
	return ret_val;
}

void mglTexture::Free( void )
{
	delete [] m_pixels;
	m_pixels       = NULL;
	m_width        = 0;
	m_height       = 0;
	m_width_mask   = 0;
	m_height_mask  = 0;
	m_width_shift  = 0;
	m_height_shift = 0;
	SetError("");
	m_format_str.Copy("");
}
