#include <fstream>
#include "mglTexture.h"
#include "mglImage.h"
#include "../MML/mmlMath.h"
#include "../MTL/mtlArray.h"

mglTexture::MipMap::MipMap( void ) :
	pixels(NULL), width(0), height(0), width_mask(0), height_mask(0), width_shift(0), height_shift(0), level(0), format()
{
	format.bytes_per_pixel = 0;
	format.byte_order = mglTexture::GetByteOrder();
	format.color = mglPixelFormat::Color_Truecolor;
}

mglTexture::MipMap::~MipMap( void )
{
	delete [] pixels;
}

void mglTexture::MipMap::Swizzle( void )
{
	const int area = width * height * format.bytes_per_pixel;
	if (area <= 0) { return; }

	mtlByte *dst = new mtlByte[area];
	const mtlByte *src = pixels;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			const int z_index = mtlEncodeMorton2(x, y) * format.bytes_per_pixel;
			for (int i = 0; i < format.bytes_per_pixel; ++i) {
				dst[z_index + i] = *src;
				++src;
			}
		}
	}

	delete [] pixels;
	pixels = dst;
}

void mglTexture::MipMap::CreateFrom(const mglTexture::MipMap &m)
{
	delete [] pixels;

	width        = m.width >> 1;
	height       = m.height >> 1;
	width_mask   = m.width_mask >> 1;
	height_mask  = m.height_mask >> 1;
	width_shift  = m.width_shift  - 1;
	height_shift = m.height_shift - 1;
	format       = m.format;
	pixels       = new mtlByte[width * height * format.bytes_per_pixel];

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int px = x << 1;
			int py = y << 1;
			mglPixel32 c00 = m.GetPixelXY(px, py),   c01 = m.GetPixelXY(px+1, py);
			mglPixel32 c10 = m.GetPixelXY(px, py+1), c11 = m.GetPixelXY(px+1, py+1);
			mtlByte *dst_p = pixels + GetPixelIndex(x, y);
			dst_p[0] = (c00.bytes[0] + c01.bytes[0] + c10.bytes[0] + c11.bytes[0]) >> 2;
			dst_p[1] = (c00.bytes[1] + c01.bytes[1] + c10.bytes[1] + c11.bytes[1]) >> 2;
			dst_p[2] = (c00.bytes[2] + c01.bytes[2] + c10.bytes[2] + c11.bytes[2]) >> 2;
			dst_p[3] = (c00.bytes[3] + c01.bytes[3] + c10.bytes[3] + c11.bytes[3]) >> 2;
		}
	}
	//Compress();
	Swizzle();
}

int mglTexture::MipMap::GetPixelIndex(int x, int y) const
{
	return mtlEncodeMorton2((x >> level) & width_mask, (y >> level) & height_mask) * format.bytes_per_pixel;
}

mglPixel32 mglTexture::MipMap::GetPixelXY(int x, int y) const
{
	return *(mglPixel32*)(pixels + GetPixelIndex(x, y));
}

enum TGA_RelevantHeaderField
{
	COLOR_MAP_TYPE = 1,
	IMAGE_TYPE     = 2,
	ORIGIN_X       = 8,
	ORIGIN_Y       = 10,
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

bool mglTexture::LoadTGA(const mtlPath &p_filename)
{
	// Take into account origin of the image axis

	std::ifstream fin(p_filename.GetPath().GetChars(), std::ios::binary);
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
	mtlByte *dst = m_main->pixels;

	// DEBUG
	mtlString tmp;
	tmp.FromInt(m_main->width);
	m_format_str.Append(tmp).Append("x");
	tmp.FromInt(m_main->height);
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

		unsigned short origin_x = *(unsigned short*)(header+ORIGIN_X);
		unsigned short origin_y = *(unsigned short*)(header+ORIGIN_Y);

		if (origin_x != 0) {
			for (int y = 0; y < m_main->height; ++y) {
				int x_right = 0;
				int x_left  = m_main->width - 1 - m_main->format.bytes_per_pixel;
				for (int x = 0; x < (m_main->width >> 1); ++x) {
					for (int b = 0; b < m_main->format.bytes_per_pixel; ++b) {
						mmlSwap(m_main->pixels[y + x_right + b], m_main->pixels[y + x_left + b]);
					}
					x_right += m_main->format.bytes_per_pixel;
					x_left  -= m_main->format.bytes_per_pixel;
				}
				y += m_main->width * m_main->format.bytes_per_pixel;
			}
		}

		if (origin_y != 0) {
			int y_top    = 0;
			int y_bottom = (m_main->height - 1) * m_main->format.bytes_per_pixel;
			for (int y = 0; y < (m_main->height >> 1); ++y) {
				for (int x = 0; x < m_main->width * m_main->format.bytes_per_pixel; x += m_main->format.bytes_per_pixel) {
					for (int b = 0; b < m_main->format.bytes_per_pixel; ++b) {
						mmlSwap(m_main->pixels[y_top + x + b], m_main->pixels[y_bottom + x + b]);
					}
				}
				y_top    += m_main->width * m_main->format.bytes_per_pixel;
				y_bottom -= m_main->width * m_main->format.bytes_per_pixel;
			}
		}

	} else {
		SetError("[TGA] Unknown image type");
		return false;
	}
	return true;
}

mglTexture::mglTexture( void ) : m_main(NULL)
{
	m_mips.Create(1);
	m_main = &m_mips[0];
}

mglTexture::mglTexture(int p_width, int p_height) : m_main(NULL)
{
	Create(p_width, p_height);
}

mglTexture::mglTexture(int p_width, int p_height, mglPixelFormat format) : m_main(NULL)
{
	Create(p_width, p_height, format);
}

bool mglTexture::Create(int width, int height)
{
	return Create(width, height, m_main->format);
}

bool mglTexture::Create(int width, int height, mglPixelFormat format)
{
	Free();
	if ((format.color == mglPixelFormat::Color_Grayscale && (format.bytes_per_pixel == 1 || format.bytes_per_pixel == 2)) || (format.color == mglPixelFormat::Color_Truecolor && (format.bytes_per_pixel == 2 || format.bytes_per_pixel == 3 || format.bytes_per_pixel == 4))) {
		if (VerifyDimension(width) && VerifyDimension(height)) {
			int width_shift = 0;
			int height_shift = 0;
			for (int i = width;  i != 1; i >>= 1) { ++width_shift; }
			for (int i = height; i != 1; i >>= 1) { ++height_shift; }

			m_mips.Create(mmlMax(1, mmlMin(width_shift, height_shift)));
			m_main = &m_mips[0];

			m_main->pixels       = new mtlByte[width * height * format.bytes_per_pixel];
			m_main->width        = width;
			m_main->height       = height;
			m_main->width_mask   = width - 1;
			m_main->height_mask  = height - 1;
			m_main->width_shift  = width_shift;
			m_main->height_shift = height_shift;
			m_main->format       = format;

			return true;
		} else {
			SetError("Invalid dimensions");
		}
	} else {
		SetError("Invalid pixel format");
	}
	m_mips.Create(1);
	m_main = &m_mips[0];
	return false;
}

bool mglTexture::Load(const mtlPath &p_filename)
{
	bool ret_val = false;
	if (p_filename.GetFileExtension().Compare("tga")) { // targa
		ret_val = LoadTGA(p_filename);
	} else {
		mtlString error;
		error.Copy("Cannot load format: ");
		error.Append(p_filename.GetFileExtension());
		SetError(error);
	}

	if (ret_val) {
		m_main->Swizzle();
		UpdateMipMaps();
	} else {
		mtlString error;
		error.Copy(GetError());
		Free();
		SetError(error);
	}
	return ret_val;
}

void mglTexture::UpdateMipMaps( void )
{
	for (int i = 1; i < m_mips.GetSize(); ++i) {
		m_mips[i].CreateFrom(m_mips[i - 1]);
	}
}

void mglTexture::Free( void )
{
	m_mips.Create(1);
	m_main = &m_mips[0];
	m_format_str.Copy("");
	SetError("");
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
