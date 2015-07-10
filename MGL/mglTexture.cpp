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
	return p_dimension >= 0x4 && p_dimension <= 0xffff && mmlIsPow2((unsigned int)p_dimension);
}

void mglTexture::UnpackTGAPixel(mtlByte *out, const unsigned char *pixel_data, int bpp, int type) const
{
	switch (type) {
	case COMPRESSED_GRAYSCALE_IMAGE:
	case UNCOMPRESSED_GRAYSCALE_IMAGE:
		switch (bpp) {
		case 1:
			// XXXXXXXX
			out[0] = pixel_data[0];
			break;
		case 2:
			// XXXXXXXX AAAAAAAA
			out[0] = pixel_data[0];
			out[1] = pixel_data[1];
			break;
		default: break;
		}
		break;
	case COMPRESSED_TRUECOLOR_IMAGE:
	case UNCOMPRESSED_TRUECOLOR_IMAGE:
		switch (bpp) {
		// 1 byte color map?
		case 2:
			// GGGBBBBB ARRRRRGG
			out[0] = (pixel_data[1] << 1) | (pixel_data[0] >> 7);
			out[1] = (pixel_data[1] << 1) | (pixel_data[1] >> 7);
			break;
		case 3:
			// BBBBBBBB GGGGGGGG RRRRRRRR
			out[0] = pixel_data[2];
			out[1] = pixel_data[1];
			out[2] = pixel_data[0];
			break;
		case 4:
			// BBBBBBBB GGGGGGGG RRRRRRRR AAAAAAAA
			out[0] = pixel_data[2];
			out[1] = pixel_data[1];
			out[2] = pixel_data[0];
			out[3] = pixel_data[3];
			break;
		default: break;
		}
		break;
	default: break;
	}
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

	mglPixelFormat fmt;
	fmt.bytes_per_pixel = bpp;
	fmt.color = (header[IMAGE_TYPE] == COMPRESSED_GRAYSCALE_IMAGE || header[IMAGE_TYPE] == UNCOMPRESSED_GRAYSCALE_IMAGE) ? mglPixelFormat::Color_Grayscale : mglPixelFormat::Color_Truecolor;

	if (!Create((int)(*(unsigned short*)(header+IMAGE_WIDTH)), (int)(*(unsigned short*)(header+IMAGE_HEIGHT)), fmt)) {
		return false;
	}

	const int NUM_PIXELS = GetArea();
	const int NUM_BYTES = NUM_PIXELS * bpp;
	mtlByte *dst = m_pixels;

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
		unsigned char *src = image_data;
		if (fin.read((char*)src, image_data.GetSize()).bad()) {
			SetError("[TGA] Failed to read uncompressed pixel data");
			return false;
		}
		for (int i = 0; i < NUM_PIXELS; ++i) {
			UnpackTGAPixel(dst, src, bpp, header[IMAGE_TYPE]);
			src += bpp;
			dst += bpp;
		}

	} else if (header[IMAGE_TYPE] == COMPRESSED_TRUECOLOR_IMAGE || header[IMAGE_TYPE] == COMPRESSED_GRAYSCALE_IMAGE) {

		int pixel_count = 0;

		while (pixel_count < NUM_PIXELS) {
			unsigned char chunk_header;
			if (fin.read((char*)(&chunk_header), 1).bad()) {
				SetError("[TGA] Failed to read compressed chunk header");
				return false;
			}
			const int size = 1 + (chunk_header & 0x7f);
			pixel_count += size;
			if (pixel_count > NUM_PIXELS) {
				SetError("[TGA] Chunk size exceeds pixel count");
				return false;
			}
			if (chunk_header & 0x80) { // RLE compressed
				unsigned char color_data[4]; // maximum number of bytes
				if (fin.read((char*)color_data, bpp).bad()) {
					SetError("[TGA] Failed to read compressed chunk");
					return false;
				}
				unsigned char src[4];
				UnpackTGAPixel(src, color_data, bpp, header[IMAGE_TYPE]);
				for (int i = 0; i < size; ++i) {
					for (int b = 0; b < bpp; ++b) {
						*dst = src[b];
						++dst;
					}
				}
			} else { // non-compressed
				unsigned char chunk_data[128*4];
				unsigned char *src = chunk_data;
				if (fin.read((char*)src, size*bpp).bad()) {
					SetError("[TGA] Failed to read uncompressed chunk");
					return false;
				}
				for (int i = 0; i < size; ++i) {
					UnpackTGAPixel(dst, src, bpp, header[IMAGE_TYPE]);
					src += bpp;
					dst += bpp;
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
	const int area = GetArea() * m_format.bytes_per_pixel;
	if (area <= 0) { return; }

	mtlByte *dst = new mtlByte[area];
	const mtlByte *src = m_pixels;

	for (int y = 0; y < m_height; ++y) {
		for (int x = 0; x < m_width; ++x) {
			const int z_index = mtlEncodeMorton2(x, y) * m_format.bytes_per_pixel;
			for (int i = 0; i < m_format.bytes_per_pixel; ++i) {
				dst[z_index + i] = *src;
				++src;
			}
		}
	}

	delete [] m_pixels;
	m_pixels = dst;
}

// Super duper slow?
void mglTexture::Compress_VQ(const mtlByte *pixels, int total_size)
{
	if (total_size <= 1) { return; }

	// allocate node?

	unsigned long long r = 0, g = 0, b = 0, a = 0;

	for (int i = 0; i < total_size; i += m_format.bytes_per_pixel) {
		mglPixel32 p = DecodePixel(pixels+i);
		r += p.bytes[0];
		g += p.bytes[1];
		b += p.bytes[2];
		a += p.bytes[3];
	}
	mglPixel32 center;
	center.bytes[0] = r / total_size;
	center.bytes[1] = g / total_size;
	center.bytes[2] = b / total_size;
	center.bytes[3] = a / total_size;

	// Compress_VQ(pixels, total_size >> 1, node);
	// Compress_VQ(pixels + (total_size >> 1), total_size >> 1, node);
}

mglPixel32 mglTexture::DecodePixel(const mtlByte *in) const
{
	mglPixel32 out;
	switch (m_format.color) {
	case mglPixelFormat::Color_Truecolor:
		switch (m_format.bytes_per_pixel) {
		case 2:
			// RRRRRGGG GGBBBBBA
			out.bytes[0] = in[0] & 0xf8;
			out.bytes[1] = ((in[0] & 0x07) << 5) | ((in[1] & 0xc0) >> 6);
			out.bytes[2] = (in[1] & 0x3e) << 2;
			out.bytes[3] = (in[1] & 1) * 0xff;
			break;
		case 3:
			out.bytes[0] = in[0];
			out.bytes[1] = in[1];
			out.bytes[2] = in[2];
			out.bytes[3] = 0xff;
			break;
		case 4:
			out.bytes[0] = in[0];
			out.bytes[1] = in[1];
			out.bytes[2] = in[2];
			out.bytes[3] = in[3];
			break;
		default: break;
		}
		break;
	case mglPixelFormat::Color_Grayscale:
		switch (m_format.bytes_per_pixel) {
		case 1:
			out.bytes[0] = in[0];
			out.bytes[1] = in[0];
			out.bytes[2] = in[0];
			out.bytes[3] = 0xff;
			break;
		case 2:
			out.bytes[0] = in[0];
			out.bytes[1] = in[0];
			out.bytes[2] = in[0];
			out.bytes[3] = in[1];
			break;
		default: break;
		}
		break;
	default: break;
	}
	return out;
}

mglTexture::mglTexture( void ) : m_pixels(NULL), m_width(0), m_height(0), m_width_mask(0), m_height_mask(0), m_width_shift(0), m_height_shift(0)
{
	m_format.bytes_per_pixel = 4;
	m_format.color           = mglPixelFormat::Color_Truecolor;
}

mglTexture::mglTexture(int p_width, int p_height) : m_pixels(NULL), m_width(0), m_height(0), m_width_mask(0), m_height_mask(0), m_width_shift(0), m_height_shift(0)
{
	m_format.bytes_per_pixel = 4;
	m_format.color           = mglPixelFormat::Color_Truecolor;
	Create(p_width, p_height);
}

mglTexture::mglTexture(int p_width, int p_height, mglPixelFormat format) : m_pixels(NULL), m_width(0), m_height(0), m_width_mask(0), m_height_mask(0), m_width_shift(0), m_height_shift(0)
{
	m_format.bytes_per_pixel = 4;
	m_format.color           = mglPixelFormat::Color_Truecolor;
	Create(p_width, p_height, format);
}

bool mglTexture::Create(int width, int height)
{
	mglPixelFormat format = { 4, mglPixelFormat::Color_Truecolor };
	return Create(width, height, format);
}

bool mglTexture::Create(int width, int height, mglPixelFormat format)
{
	Free();
	if ((format.color == mglPixelFormat::Color_Grayscale && (format.bytes_per_pixel == 1 || format.bytes_per_pixel == 2)) || (format.color == mglPixelFormat::Color_Truecolor && (format.bytes_per_pixel == 2 || format.bytes_per_pixel == 3 || format.bytes_per_pixel == 4))) {
		if (VerifyDimension(width) && VerifyDimension(height)) {
			m_width        = width;
			m_height       = height;
			m_width_mask   = m_width - 1;
			m_height_mask  = m_height - 1;
			m_width_shift  = 0;
			m_height_shift = 0;
			m_format       = format;
			m_pixels       = new mtlByte[m_width*m_height*m_format.bytes_per_pixel];
			for (int i = m_width;  i != 1; i >>= 1) { ++m_width_shift; }
			for (int i = m_height; i != 1; i >>= 1) { ++m_height_shift; }
			return true;
		} else {
			SetError("Invalid dimensions");
		}
	} else {
		SetError("Invalid pixel format");
	}
	return false;
}

bool mglTexture::Load(const mtlDirectory &p_filename)
{
	bool ret_val = false;
	bool native_load = false;
	if (p_filename.GetExtension().Compare("tga")) { // targa
		ret_val = LoadTGA(p_filename);
	} else if (p_filename.GetExtension().Compare(".pqz")) { // packed quantized z-order
		ret_val = LoadPQZ(p_filename);
		native_load = true;
	} else {
		mtlString error;
		error.Copy("Cannot load format: ");
		error.Append(p_filename.GetExtension());
		SetError(error);
	}

	if (ret_val) {
		if (!native_load) {
			Swizzle_Z();
			Pack_SOA();
			Compress_VQ(m_pixels, GetArea() * m_format.bytes_per_pixel);
		}
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
	m_pixels                 = NULL;
	m_width                  = 0;
	m_height                 = 0;
	m_width_mask             = 0;
	m_height_mask            = 0;
	m_width_shift            = 0;
	m_height_shift           = 0;
	m_format.bytes_per_pixel = 0;
	SetError("");
	m_format_str.Copy("");
}

mglByteOrder32 mglTexture::GetByteOrder( void )
{
	mglByteOrder32 byte_order;
	byte_order.index.r = 0;
	byte_order.index.g = 1;
	byte_order.index.b = 2;
	byte_order.index.a = 3;
	return byte_order;
}
