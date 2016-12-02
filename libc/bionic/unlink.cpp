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
#include <unistd.h>
#include "private/libc_logging.h"
#include <libc_incognito_io.h>
#include <stdlib.h>
#include <string.h>

#include "../tiramisu_include/libc_incognito_io.h"

#define ALOGE(...) __libc_format_log(6, "Tiramisu-DEBUG", __VA_ARGS__)

int unlink(const char* path) {

	ALOGE("Tiramisu: Delete: unlink called for %s mode %d", path, libc_check_incognito_mode());

	if (libc_check_incognito_mode() &&
		(strstr(path, "INCOGNITO_TIRAMISU") == NULL) &&
		(strstr(path, "/data/user") != NULL)) {
		char *incognito_pathname = reinterpret_cast<char*>(malloc(LIBC_MAX_FILE_PATH_SIZE));
		bool need_remove = false;
	
		memset(incognito_pathname, 0, LIBC_MAX_FILE_PATH_SIZE);
		if (libc_add_or_update_file_delete_entry(path, &need_remove,
										    incognito_pathname, LIBC_MAX_FILE_PATH_SIZE)) {
			ALOGE("Tiramisu: Remove system call failed for %s", path);
			// Throw error if there is an error in processing the system call.
			return -1;
    	} 

		ALOGE("Tiramisu: need_remove=%d path=%s incog_path=%s", need_remove, path, incognito_pathname);

		if (!need_remove) {
			return 0;
		}

  		return unlinkat(AT_FDCWD, incognito_pathname, 0);
	}
  return unlinkat(AT_FDCWD, path, 0);
}
