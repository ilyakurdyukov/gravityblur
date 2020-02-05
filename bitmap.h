/*
 * Copyright (C) 2020 Kurdyukov Ilya
 *
 * common bitmap routines
 */

typedef struct {
	int width, height, bpp, stride; uint8_t *data;
} bitmap_t;

static bitmap_t *bitmap_create(int width, int height, int bpp) {
	bitmap_t *image; int stride = width * bpp;
	if ((unsigned)((width - 1) | (height - 1)) >= 0x8000) return 0;
	image = (bitmap_t*)malloc(sizeof(bitmap_t) + stride * height);
	if (!image) return 0;
	image->width = width;
	image->height = height;
	image->bpp = bpp;
	image->stride = stride;
	image->data = (uint8_t*)(image + 1);
	return image;
}

static void bitmap_free(bitmap_t *in) {
	if (in) free(in);
}

#define DESCALE(a, x) a = (x + 0x2000) >> 14; a = a < 0 ? 0 : a > 255 ? 255 : a;
#define FIX(a) ((int)((a) * (1 << 14) + 0.5))

#define FIX_Y_R FIX(0.299)
#define FIX_Y_G FIX(0.587)
#define FIX_Y_B FIX(0.114)
#define FIX_Cb_BY FIX(1 / 1.772)
#define FIX_Cr_RY FIX(1 / 1.402)
#define FIX_R_Cr FIX(1.402)
#define FIX_Y_Cb FIX(0.34414)
#define FIX_Y_Cr FIX(0.71414)
#define FIX_B_Cb FIX(1.772)

static void RGB2YCbCr(bitmap_t *in) {
	int x, y, w = in->width, h = in->height, bpp = in->bpp, stride = in->stride;
	uint8_t *data = in->data;

	if (bpp >= 3)
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			uint8_t *ptr = &data[y*stride + x*bpp];
			int R = ptr[0], G = ptr[1], B = ptr[2], Y, Cb, Cr;
			Y = (R * FIX_Y_R + G * FIX_Y_G + B * FIX_Y_B + 0x2000) >> 14;
			DESCALE(Cb, (B - Y) * FIX_Cb_BY + (128 << 14));
			DESCALE(Cr, (R - Y) * FIX_Cr_RY + (128 << 14));
			ptr[0] = Y; ptr[1] = Cb; ptr[2] = Cr;
		}
	}
}

static void YCbCr2RGB(bitmap_t *in) {
	int x, y, w = in->width, h = in->height, bpp = in->bpp, stride = in->stride;
	uint8_t *data = in->data;

	if (bpp >= 3)
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			uint8_t *ptr = &data[y*stride + x*bpp];
			int Y = ptr[0], Cb = ptr[1], Cr = ptr[2], R, G, B;
			DESCALE(R, (Y << 14) + (Cr - 128) * FIX_R_Cr);
			DESCALE(G, (Y << 14) - (Cb - 128) * FIX_Y_Cb - (Cr - 128) * FIX_Y_Cr);
			DESCALE(B, (Y << 14) + (Cb - 128) * FIX_B_Cb);
			ptr[0] = R; ptr[1] = G; ptr[2] = B;
		}
	}
}
#undef FIX
#undef DESCALE

