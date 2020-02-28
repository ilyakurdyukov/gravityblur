/*
 * Copyright (C) 2020 Ilya Kurdyukov
 *
 * jpeg support using libjpeg
 */

#include "jpeglib.h"

#define bitmap_read_jpeg(fn) bitmap_read_jpeg_impl(fn, JCS_RGB)
#define bitmap_read_jpeg_gray(fn) bitmap_read_jpeg_impl(fn, JCS_GRAYSCALE)
#define bitmap_read_jpeg_yuv(fn) bitmap_read_jpeg_impl(fn, JCS_YCbCr)

typedef struct {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
} bitmap_jpeg_err_ctx;

static void bitmap_jpeg_err(j_common_ptr cinfo) {
	char errorMsg[JMSG_LENGTH_MAX];
	bitmap_jpeg_err_ctx* jerr = (bitmap_jpeg_err_ctx*)cinfo->err;
	(*(cinfo->err->format_message))(cinfo, errorMsg);
	printf("%s\n", errorMsg);
	longjmp(jerr->setjmp_buffer, 1);
}

bitmap_t* bitmap_read_jpeg_impl(const char *filename, J_COLOR_SPACE color) {
	struct jpeg_decompress_struct ci;
	FILE * volatile fp; int volatile ok = 0;
	bitmap_t * volatile bm = NULL;
	void * volatile mem = NULL;

	bitmap_jpeg_err_ctx jerr;
	ci.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = bitmap_jpeg_err;
	if (!setjmp(jerr.setjmp_buffer)) {
		unsigned y, width, height, stride; uint8_t *data;
		bitmap_t *bm1; JSAMPROW *scanline;

		jpeg_create_decompress(&ci);
		if (!(fp = fopen(filename, "rb"))) goto err;
		jpeg_stdio_src(&ci, fp);
		jpeg_read_header(&ci, TRUE);
		ci.out_color_space = color;
		// ci.do_fancy_upsampling = FALSE;
		// ci.dct_method = JDCT_ISLOW;
		jpeg_start_decompress(&ci);

		width = ci.output_width;
		height = ci.output_height;
		bm = bm1 = bitmap_create(width, height, ci.output_components);
		if (bm1) {
			stride = bm1->stride; data = bm1->data;
			mem = malloc(height * sizeof(JSAMPROW));
			if (mem) {
				scanline = (JSAMPROW*)mem;
				for (y = 0; y < height; y++)
					scanline[y] = (JSAMPLE*)(data + y * stride);

				while ((y = ci.output_scanline) < height) {
					jpeg_read_scanlines(&ci, scanline + y, height - y);
				}
				ok = 1;
			}
		}
		jpeg_finish_decompress(&ci);
	}
err:
	if (mem) free(mem);
	if (!ok) { bitmap_free(bm); bm = NULL; }
	jpeg_destroy_decompress(&ci);
	if (fp) fclose(fp);
	return bm;
}

#define bitmap_write_jpeg(bm, fn) bitmap_write_jpeg_impl(bm, fn, JCS_RGB)
#define bitmap_write_jpeg_yuv(bm, fn) bitmap_write_jpeg_impl(bm, fn, JCS_YCbCr)

void bitmap_write_jpeg_impl(bitmap_t *bm, const char *filename, J_COLOR_SPACE color) {
  struct jpeg_compress_struct ci;
	FILE * volatile fp = NULL;
	void * volatile mem = NULL;
	J_COLOR_SPACE volatile in_color;

	if (!bm || !filename) goto err;
	if (bm->bpp == 1) color = JCS_GRAYSCALE;
	else if (bm->bpp != 3) goto err;
	in_color = color;

	bitmap_jpeg_err_ctx jerr;
	ci.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = bitmap_jpeg_err;
	if (!setjmp(jerr.setjmp_buffer)) {
		unsigned y, width, height, stride; uint8_t *data;
		JSAMPROW *scanline; int quality = 100;

	  jpeg_create_compress(&ci);
		if (!(fp = fopen(filename, "wb"))) goto err;
		jpeg_stdio_dest(&ci, fp);

		width = bm->width; height = bm->height;
		data = bm->data; stride = bm->stride;

		ci.image_width = width;
		ci.image_height = height;
		ci.input_components = bm->bpp;
		ci.in_color_space = in_color;
		jpeg_set_defaults(&ci);
		ci.optimize_coding = TRUE;
		// ci.dct_method = JDCT_ISLOW;
		// for JCS_YCbCr libjpeg selects 4:2:0 subsampling by default
		ci.comp_info[0].h_samp_factor = 1;
		ci.comp_info[0].v_samp_factor = 1;
		jpeg_set_quality(&ci, quality, TRUE);
		jpeg_start_compress(&ci, TRUE);

		mem = malloc(height * sizeof(JSAMPROW));
		if (mem) {
			scanline = (JSAMPROW*)mem;
			for (y = 0; y < height; y++)
				scanline[y] = (JSAMPLE*)(data + y * stride);

			while ((y = ci.next_scanline) < height) {
				jpeg_write_scanlines(&ci, scanline + y, height - y);
			}
		}
	  jpeg_finish_compress(&ci);
	}
err:
	if (mem) free(mem);
	jpeg_destroy_compress(&ci);
	if (fp) fclose(fp);
}

