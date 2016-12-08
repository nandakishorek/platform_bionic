/*
 * Copyright (C) 2013 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "private/libc_logging.h"
#include "../tiramisu_include/libc_incognito_io.h"
#define ALOGE(...) __libc_format_log(6, "Tiramisu-DEBUG", __VA_ARGS__)
int stat(const char* path, struct stat* sb) {
	if(libc_check_incognito_mode()) { 
		Libc_File_Status status;

		char *file_path = reinterpret_cast<char*>(malloc(LIBC_MAX_FILE_PATH_SIZE));
		if (file_path == NULL) {
			ALOGE("Tiramisu DEBUG: no mem");
			return ENOMEM;
		}

		memset(file_path, 0, 4096);
		if (libc_lookup_filename(path, file_path,
							4096, &status)) {
			const char *incognito_file_path = reinterpret_cast<const char*>(file_path);
			int rc = fstatat(AT_FDCWD, incognito_file_path, sb, 0);
			free(file_path);
			return rc;
		}
		free(file_path);
	}
  return fstatat(AT_FDCWD, path, sb, 0);
}
__strong_alias(stat64, stat);
