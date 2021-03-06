/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if !defined(_DATA_H)
#define _DATA_H

#include "view.h"
#include <stdio.h>
#include <unistd.h>
#include <storage.h>
#include <camera.h>

#define BUFLEN 512
#define MAX_FILTER 30
#define MAX_STICKER 5

typedef struct{
	camera_pixel_format_e format;
	int width;
	int height;

	unsigned char* rgb_r;
	unsigned char* rgb_g;
	unsigned char* rgb_b;
} rgbmat;

void create_buttons_in_main_window(void);

#endif
