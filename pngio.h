/*
 * Copyright (C) 2020 Ilya Kurdyukov
 *
 * png support using libpng
 */

#define PNG_DEBUG 0
#include <png.h>

// http://www.libpng.org/pub/png/libpng-1.2.5-manual.html

bitmap_t* bitmap_read_png(const char *filename) {
	unsigned char header[8]; png_structp png_ptr = NULL;
	png_infop info_ptr = NULL, end_info = NULL;
	bitmap_t * volatile bm = NULL; FILE * volatile fp = NULL;
	const char * volatile errstr = "param_check";
	void * volatile mem = NULL;

	if (!filename) goto err;
	errstr = "fopen";
	if (!(fp = fopen(filename, "rb"))) goto err;
	errstr = "sig_cmp";
	if (fread(header, 1, 8, fp) != 8 || png_sig_cmp(header, 0, 8)) goto err;

	errstr = "create_read_struct";
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) goto err;

	errstr = "create_info_struct";
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) goto err;

	if (!setjmp(png_jmpbuf(png_ptr))) {
		int y, width, height, bpp, stride;
		png_bytep *row_pointers;
		uint8_t *data; png_byte b;
		bitmap_t *bm1;

		errstr = "init_io";
		png_init_io(png_ptr, fp);
		png_set_sig_bytes(png_ptr, 8);

		errstr = "read_info";
		png_read_info(png_ptr, info_ptr);

		width = png_get_image_width(png_ptr, info_ptr);
		height = png_get_image_height(png_ptr, info_ptr);
		png_read_update_info(png_ptr, info_ptr);

		// b = png_get_color_type(png_ptr, info_ptr);
		errstr = "bit_depth == 8";
		b = png_get_bit_depth(png_ptr, info_ptr);
		if (b != 8) goto err;

		bpp = png_get_channels(png_ptr, info_ptr);
		bm = bm1 = bitmap_create(width, height, bpp);
		errstr = "malloc";
		if (!bm1) goto err;
		stride = bm1->stride; data = bm1->data;

		mem = malloc(height * sizeof(png_bytep));
		if (!mem) goto err;
		row_pointers = (png_bytep*)mem;
		for (y = 0; y < height; y++)
			row_pointers[y] = (png_byte*)(data + y * stride);

		png_read_image(png_ptr, row_pointers);
		errstr = NULL;
	}
err:
	if (mem) free(mem);
	if (errstr) printf("[read_png] %s failed\n", errstr);
  if (png_ptr) png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	if (fp) fclose(fp);
	return bm;
}

void bitmap_write_png(bitmap_t *bm, const char *filename) {
	png_structp png_ptr = NULL; png_infop info_ptr = NULL;
	int y, width, height, bpp, stride; FILE * volatile fp = NULL;
	png_byte volatile color_type;
	const char * volatile errstr = "param_check";
	void * volatile mem = NULL;

	if (!bm || !filename) goto err;
	width = bm->width; height = bm->height;
	bpp = bm->bpp; stride = bm->stride;
	if (bpp < 1 || bpp > 4) goto err;
	color_type =
		bpp == 3 ? PNG_COLOR_TYPE_RGB :
		bpp == 4 ? PNG_COLOR_TYPE_RGB_ALPHA :
		bpp == 1 ? PNG_COLOR_TYPE_GRAY : PNG_COLOR_TYPE_GRAY_ALPHA;

	errstr = "fopen";
	if (!(fp = fopen(filename, "wb"))) goto err;

	errstr = "create_write_struct";
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) goto err;

	errstr = "create_info_struct";
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) goto err;

	errstr = "init_io";
	if (!setjmp(png_jmpbuf(png_ptr))) {
		uint8_t *data = bm->data; png_byte bit_depth = 8;
		png_bytep *row_pointers; 

		png_init_io(png_ptr, fp);

		errstr = "write_info";
		png_set_IHDR(png_ptr, info_ptr, width, height,
					 bit_depth, color_type, PNG_INTERLACE_NONE,
					 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		png_write_info(png_ptr, info_ptr);

		mem = malloc(height * sizeof(png_bytep));
		errstr = "malloc";
		if (!mem) goto err;
		row_pointers = (png_bytep*)mem;
		for (y = 0; y < height; y++)
			row_pointers[y] = (png_byte*)(data + y * stride);

		errstr = "write_image";
		png_write_image(png_ptr, row_pointers);

		errstr = "write_end";
		png_write_end(png_ptr, NULL);
		errstr = NULL;
	}
err:
	if (mem) free(mem);
	if (errstr) printf("[write_png] %s failed\n", errstr);
	if (png_ptr) png_destroy_write_struct(&png_ptr, &info_ptr);
	if (fp) fclose(fp);
}

