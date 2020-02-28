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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

#include <time.h>
#include <sys/time.h>
static int64_t get_time_usec() {
	struct timeval time;
	gettimeofday(&time, NULL);
	return (int64_t)time.tv_sec * 1000000 + time.tv_usec;
}

#include "bitmap.h"
#ifndef WASM
#include "pngio.h"
#include "jpegio.h"
#define stricmp stricmp_simple
static int stricmp(const char *s1, const char *s2) {
	char a, b;
	do {
		a = *s1++; b = *s2++;
		a += (unsigned)(a - 'a') <= 'z' - 'a' ? 'A' - 'a' : 0;
		b += (unsigned)(b - 'a') <= 'z' - 'a' ? 'A' - 'a' : 0;
	} while (a == b && a);
	return a - b;
}
#endif

#include "gravityblur.h"

#define BITMAP_RGB 0
#define BITMAP_YUV 1

static bitmap_t* process_image(bitmap_t *in,
		int type1, int type2, int type3,
		int flags, float range, float sharp, int num_iter) {
	int w = in->width, h = in->height, bpp = in->bpp;
	bitmap_t *out;

	if (type1 != type2) {
		if (type2 == BITMAP_RGB) YCbCr2RGB(in);
		else RGB2YCbCr(in);
	}
	out = bitmap_create(w, h, bpp);
	if (out) {
		float range2 = range * 256, sharp2 = 1.0f / sharp;
		if (bpp < 3 || flags & 1) gravityblur(in, out, range2, sharp2, num_iter);
		if (bpp > 3 && flags & 2) gravityblur4(in, out, range2, sharp2, num_iter);
		else gravityblur3(in, out, range2, sharp2, num_iter);

		if (type2 != type3) {
			if (type3 == BITMAP_RGB) YCbCr2RGB(out);
			else RGB2YCbCr(out);
		}
	}
	return out;
}

#ifndef LESS_CONV
#define LESS_CONV 1
#endif

#ifdef WASM
static char** make_argv(char *str, int *argc_ret) {
	int i = 0, eol = 0, argc = 1; char **argv;
	for (;;) {
		char a = str[i++];
		if (!a) break;
		if (eol) {
			if (a == eol) eol = 0;
		} else {
			if (a != ' ') {
				eol = ' ';
				if (a == '"' || a == '\'') eol = a;
				argc++;
			}
		}
	}
	*argc_ret = argc;
	argv = (char**)malloc(argc * sizeof(char*));
	if (!argv) return argv;
	argv[0] = NULL; eol = 0; argc = 1;
	for (;;) {
		char a = *str++;
		if (!a) break;
		if (eol) {
			if (a == eol) { str[-1] = 0; eol = 0; }
		} else {
			if (a != ' ') {
				eol = ' ';
				if (a == '"' || a == '\'') { eol = a; str++; }
				argv[argc++] = str - 1;
			}
		}
	}
	return argv;
}

EMSCRIPTEN_KEEPALIVE
void* web_process(void *web_ptr, char *cmdline) {
#else
int main(int argc, char **argv) {
#endif
	int64_t time = 0; bitmap_t *in, *out;
	int type = BITMAP_RGB, in_type = type, out_type = type;

	int flags = 0, num_iter = 3, info = 3;
	float range = 10, sharp = 50;
#ifdef WASM
	int argc = 0;
	char **argv_ptr = make_argv(cmdline, &argc), **argv = argv_ptr;
	in = (bitmap_t*)web_ptr;
#else
	const char *ifn = 0, *ofn = 0; const char *ext;
	int read_jpeg = 0, write_jpeg = 0;
	const char *progname = "gravblur";
#endif

	while (argc > 1) {
		const char *arg1 = argv[1], *arg2 = argc > 2 ? argv[2] : NULL, *arg = arg1; char c;
		if (arg[0] != '-' || !(c = arg[1])) break;
		if (c != '-') switch (c) {
			case 'S': arg = "--separate"; c = 0; break;
			case 'a': arg = "--alpha"; c = 0; break;
			case 'r': arg = "--range"; break;
			case 's': arg = "--sharp"; break;
			case 'n': arg = "--niter"; break;
			case 'i': arg = "--info"; break;
			case 'R': arg = "--rgb"; c = 0; break;
			case 'Y': arg = "--yuv"; c = 0; break;
			default: c = '-';
		}
		if (c != '-' && arg1[2]) {
			if (!c) break;
			arg2 = arg1 + 2; argc++; argv--;
		}

#define CHECKNUM if ((unsigned)(arg2[0] - '0') > 9) break;
		if (!strcmp("--separate", arg)) {
			flags |= 1; argc--; argv++;
		} else if (!strcmp("--alpha", arg)) {
			flags |= 2; argc--; argv++;
		} else if (argc > 2 && !strcmp("--range", arg)) {
			CHECKNUM
			range = atof(arg2);
			argc -= 2; argv += 2;
		} else if (argc > 2 && !strcmp("--sharp", arg)) {
			CHECKNUM
			sharp = atof(arg2);
			argc -= 2; argv += 2;
		} else if (argc > 2 && !strcmp("--niter", arg)) {
			CHECKNUM
			num_iter = atoi(arg2);
			argc -= 2; argv += 2;
		} else if (argc > 2 && !strcmp("--info", arg)) {
			CHECKNUM
			info = atoi(arg2);
			argc -= 2; argv += 2;
		} else if (!strcmp("--rgb", arg)) {
			type = BITMAP_RGB; argc--; argv++;
		} else if (!strcmp("--yuv", arg)) {
			type = BITMAP_YUV; argc--; argv++;
		} else break;
#undef CHECKNUM
	}

#ifdef WASM
	free(argv_ptr);
	if (argc != 1) {
		printf("Unrecognized command line option.\n");
		return NULL;
	}
#else
	if (argc != 3) {
		printf(
"Gravity Blur : Copyright (c) 2020 Ilya Kurdyukov\n"
"Build date: " __DATE__ "\n"
"\n"
"Usage:\n"
"  %s [options] input.[png|jpg|jpeg] output.[png|jpg|jpeg]\n"
"\n"
"Options:\n"
"  -r, --range f     Gravity range (0-100)\n"
"  -s, --sharp f     Sharpness (0-100)\n"
"  -n, --niter n     Number of iterations (default is 3)\n"
"  -R, --rgb         Process in RGB (default)\n"
"  -Y, --yuv         Process in YUV\n"
"  -S, --separate    Separate color components\n"
"  -a, --alpha       Use alpha channel\n"
"  -i, --info n      Print gravblur debug output:\n"
"                      0 - silent, 3 - all (default)\n"
"\n", progname);
		return 1;
	}

	ifn = argv[1];
	ext = strrchr(ifn, '.');
	if (ext && (!stricmp(ext, ".jpg") || !stricmp(ext, ".jpeg"))) read_jpeg = 1;
	ofn = argv[2];
	ext = strrchr(ofn, '.');
	if (ext && (!stricmp(ext, ".jpg") || !stricmp(ext, ".jpeg"))) write_jpeg = 1;

	if (read_jpeg) {
		if (LESS_CONV && type == BITMAP_YUV) {
			in = bitmap_read_jpeg_yuv(ifn);
			in_type = BITMAP_YUV;
		} else {
			in = bitmap_read_jpeg(ifn);
		}
		if (!in) { printf("read_jpeg failed\n"); return 1; }
	} else {
		in = bitmap_read_png(ifn);
		if (!in) { printf("read_png failed\n"); return 1; }
	}

	if (LESS_CONV && write_jpeg) out_type = type;
#endif

	range /= 100; sharp /= 100;
	if (range > 2) range = 2;
	if (range < 0) range = 0;
	if (sharp > 1) sharp = 1;
	if (sharp < 0.01f) sharp = 0.01f;

	if (info & 1) time = get_time_usec();
	out = process_image(in, in_type, type, out_type,
			flags, range, sharp, num_iter);
	if (info & 1) {
		time = get_time_usec() - time;
		printf("gravblur: %.2f ms\n", time * 0.001);
	}

#ifdef WASM
	return out;
#else
	if (out) {
		if (info & 2) time = get_time_usec();
		if (write_jpeg) {
			if (out_type == BITMAP_YUV) bitmap_write_jpeg_yuv(out, ofn);
			else bitmap_write_jpeg(out, ofn);
		} else {
			bitmap_write_png(out, ofn);
		}
		if (info & 2) {
			time = get_time_usec() - time;
			printf("writing %s: %.2f ms\n",
					write_jpeg ? "jpeg" : "png", time * 0.001);
		}
		bitmap_free(out);
	}
	bitmap_free(in);
	return 0;
#endif
}

