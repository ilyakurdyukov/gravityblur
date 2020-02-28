/*
 * Copyright (C) 2020 Ilya Kurdyukov
 *
 * This file is part of gravity blur
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

static void gravityblur(bitmap_t *in, bitmap_t *out, float range, float c0, int num_iter) {
	uint8_t *in_data = in->data, *out_data = out->data, *temp;
	int w = in->width, h = in->height, bpp = in->bpp;
	int stride = in->stride, ostride = out->stride;
	int iter, j, x, y;
	float c1 = c0 * sqrtf(0.5f);

#define M4(i) \
	b##i -= (int)roundf(a##i / an); \
	if (b##i < 0) b##i = 0; \
	if (b##i > 255) b##i = 255;

#define M3(b, x, y) int b##0 = in_data[(y)*stride+(x)*bpp+j];

#define M2(M) for (j = 0; j < bpp; j++) { M3(b, x, y) \
	float a0 = 0, an = 0, aw, t; \
	M(1, x-1, y-1) M(0, x, y-1) M(1, x+1, y-1) \
	M(0, x-1, y)                M(0, x+1, y)   \
	M(1, x-1, y+1) M(0, x, y+1) M(1, x+1, y+1) \
	if (an > 0.0f) { M4(0) } \
	out_data[y*ostride+x*bpp+j] = b0; }

#define CORE_CHK(i, x, y) if (x >= 0 && x < w && y >= 0 && y < h) CORE(i, x, y)
#define CORE(i, x, y) { M3(d, x, y) \
	float t0 = b0 - d0; \
	t = range - fabsf(t0); \
	if (t < 0) t = 0; t *= t; aw = c##i * t; \
	a0 += t0 * t * aw; an += aw * aw; }

	for (iter = 0; iter < num_iter; iter++) {
		// slower processing for borders
		for (y = 0; y < h; y++) {
			int xa = y > 0 && y < h-1 ? w-1 : 1;
			for (x = 0; x < w; x += xa) M2(CORE_CHK)
		}
		// processing without checks
		for (y = 1; y < h-1; y++)
		for (x = 1; x < w-1; x++) M2(CORE)

		temp = in_data; in_data = out_data; out_data = temp;
		j = stride; stride = ostride; ostride = j;
	}
#undef M2
#undef M3
#undef CORE

	if (~num_iter & 1) {
		for (y = 0; y < h; y++)
			memcpy(in_data + y * stride, out_data + y * ostride, w * bpp);
	}
}

static void gravityblur3(bitmap_t *in, bitmap_t *out, float range, float c0, int num_iter) {
	uint8_t *in_data = in->data, *out_data = out->data, *temp;
	int w = in->width, h = in->height, bpp = in->bpp;
	int stride = in->stride, ostride = out->stride;
	int iter, j, x, y;
	float c1 = c0 * sqrtf(0.5f);

	// copy extra channels
	for (y = 0; y < h; y++)
	for (x = 0; x < w; x++)
	for (j = 3; j < bpp; j++) {
		int c = in_data[y*stride+x*bpp+j];
		out_data[y*ostride+x*bpp+j] = c;
	}

#define M3(b, x, y) \
	int b##0 = in_data[(y)*stride+(x)*bpp]; \
	int b##1 = in_data[(y)*stride+(x)*bpp+1]; \
	int b##2 = in_data[(y)*stride+(x)*bpp+2];

#define M2(M) { M3(b, x, y) \
	float a0 = 0, a1 = 0, a2 = 0, an = 0, aw, t; \
	M(1, x-1, y-1) M(0, x, y-1) M(1, x+1, y-1) \
	M(0, x-1, y)                M(0, x+1, y)   \
	M(1, x-1, y+1) M(0, x, y+1) M(1, x+1, y+1) \
	if (an > 0.0f) { M4(0) M4(1) M4(2) } \
	out_data[y*ostride+x*bpp] = b0; \
	out_data[y*ostride+x*bpp+1] = b1; \
	out_data[y*ostride+x*bpp+2] = b2; }

#define CORE(i, x, y) { M3(d, x, y) \
	float t0 = b0 - d0, t1 = b1 - d1, t2 = b2 - d2; \
	t = range - sqrtf(t0*t0 + t1*t1 + t2*t2); \
	if (t < 0) t = 0; t *= t; aw = c##i * t; t *= aw; \
	a0 += t0 * t; a1 += t1 * t; a2 += t2 * t; an += aw * aw; }

	for (iter = 0; iter < num_iter; iter++) {
		// slower processing for borders
		for (y = 0; y < h; y++) {
			int xa = y > 0 && y < h-1 ? w-1 : 1;
			for (x = 0; x < w; x += xa) M2(CORE_CHK)
		}

		// processing without checks
		for (y = 1; y < h-1; y++)
		for (x = 1; x < w-1; x++) M2(CORE)
		temp = in_data; in_data = out_data; out_data = temp;
		j = stride; stride = ostride; ostride = j;
	}
#undef M2
#undef M3
#undef CORE

	if (~num_iter & 1) {
		for (y = 0; y < h; y++)
			memcpy(in_data + y * stride, out_data + y * ostride, w * bpp);
	}
}

static void gravityblur4(bitmap_t *in, bitmap_t *out, float range, float c0, int num_iter) {
	uint8_t *in_data = in->data, *out_data = out->data, *temp;
	int w = in->width, h = in->height, bpp = in->bpp;
	int stride = in->stride, ostride = out->stride;
	int iter, j, x, y;
	float c1 = c0 * sqrtf(0.5f);

	// copy extra channels
	for (y = 0; y < h; y++)
	for (x = 0; x < w; x++)
	for (j = 4; j < bpp; j++) {
		int c = in_data[y*stride+x*bpp+j];
		out_data[y*ostride+x*bpp+j] = c;
	}

#define M3(b, x, y) \
	int b##0 = in_data[(y)*stride+(x)*bpp]; \
	int b##1 = in_data[(y)*stride+(x)*bpp+1]; \
	int b##2 = in_data[(y)*stride+(x)*bpp+2]; \
	int b##3 = in_data[(y)*stride+(x)*bpp+3];

#define M2(M) { M3(b, x, y) \
	float a0 = 0, a1 = 0, a2 = 0, an = 0, aw, t; \
	M(1, x-1, y-1) M(0, x, y-1) M(1, x+1, y-1) \
	M(0, x-1, y)                M(0, x+1, y)   \
	M(1, x-1, y+1) M(0, x, y+1) M(1, x+1, y+1) \
	if (an > 0.0f) { M4(0) M4(1) M4(2) } \
	out_data[y*ostride+x*bpp] = b0; \
	out_data[y*ostride+x*bpp+1] = b1; \
	out_data[y*ostride+x*bpp+2] = b2; \
	out_data[y*ostride+x*bpp+3] = b3; }

#define CORE(i, x, y) { M3(d, x, y) \
	float t0 = b0 - d0, t1 = b1 - d1, t2 = b2 - d2; \
	t = range - sqrtf(t0*t0 + t1*t1 + t2*t2); \
	if (t < 0) t = 0; t *= t; t *= d3; aw = c##i * t; t *= aw; \
	a0 += t0 * t; a1 += t1 * t; a2 += t2 * t; an += aw * aw;  }

	for (iter = 0; iter < num_iter; iter++) {
		// slower processing for borders
		for (y = 0; y < h; y++) {
			int xa = y > 0 && y < h-1 ? w-1 : 1;
			for (x = 0; x < w; x += xa) M2(CORE_CHK)
		}

		// processing without checks
		for (y = 1; y < h-1; y++)
		for (x = 1; x < w-1; x++) M2(CORE)
		temp = in_data; in_data = out_data; out_data = temp;
		j = stride; stride = ostride; ostride = j;
	}
#undef M2
#undef M3
#undef M4
#undef CORE
#undef CORE_CHK

	if (~num_iter & 1) {
		for (y = 0; y < h; y++)
			memcpy(in_data + y * stride, out_data + y * ostride, w * bpp);
	}
}

