/*
 * Copyright (C) 2008 The Android Open Source Project
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
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "private/libc_logging.h"
#include <libc_incognito_io.h>

#define ALOGE(...) __libc_format_log(6, "Tiramisu-DEBUG", __VA_ARGS__)
//extern bool libc_check_incognito_mode();

extern "C" int __openat(int, const char*, int, int);

static inline int force_O_LARGEFILE(int flags) {
#if __LP64__
  return flags; // No need, and aarch64's strace gets confused.
#else
  return flags | O_LARGEFILE;
#endif
}

int creat(const char* pathname, mode_t mode) {

  __libc_format_log(1, "tiramisu-debug", " creat call for %s\n", pathname);
  return open(pathname, O_CREAT | O_TRUNC | O_WRONLY, mode);
}
__strong_alias(creat64, creat);

int init_incognito_mode(const char *pathname) {
	if (strcmp(pathname, "mahesh_kishore_vardhana_vandana_sammok") == 0) {
		libc_incognito_io_init();
		return 1;
	}
	return 0;
}

int open(const char* pathname, int flags, ...) {
  mode_t mode = 0;

  if (init_incognito_mode(pathname)) return 0;

  if ((flags & O_CREAT) != 0) {
    va_list args;
    va_start(args, flags);
    mode = static_cast<mode_t>(va_arg(args, int));
    va_end(args);
  }

  if (libc_check_incognito_mode()) {
		char *incognito_file_path = reinterpret_cast<char *>(malloc(LIBC_MAX_FILE_PATH_SIZE));
		if (incognito_file_path == NULL) {
			ALOGE("Tiramisu 1 LIBC No memory");
			return -1;
		}
		int path_set = 0;
		int add_entry = 0;
		int update_entry = 0;

		ALOGE("Posix_open: 1 %s flags=0x%x", pathname, flags);
		memset(incognito_file_path, 0, 4096);
		libc_incognito_file_open(pathname, flags, &path_set, incognito_file_path,
						 	LIBC_MAX_FILE_PATH_SIZE, &add_entry, &update_entry);
		ALOGE("pathname 1 %s new pathname %s add_entry %d update_entry %d  path_set=%d\n",
			pathname, incognito_file_path, add_entry, update_entry, path_set);
		#if 1
		if (path_set) {
    		int fd = __openat(AT_FDCWD, incognito_file_path, force_O_LARGEFILE(flags), mode);
			if (add_entry) {
				libc_add_file_entry(pathname, incognito_file_path, INCOGNITO_VALID, fd);
			} else if (update_entry) {
				// If the old was file deleted, then an entry in the global state exists.
				// This open is to create a new file, so mark the existing file status as INCOGNITO_VALID.
				libc_update_file_status(pathname, INCOGNITO_VALID /* file status */);
			}
			free(incognito_file_path);
			return fd;
		}
		#endif
		free(incognito_file_path);
  }
  return __openat(AT_FDCWD, pathname, force_O_LARGEFILE(flags), mode);
}
__strong_alias(open64, open);

int __open_2(const char* pathname, int flags) {
  if (init_incognito_mode(pathname)) return 0;

  if (__predict_false((flags & O_CREAT) != 0)) {
    __fortify_chk_fail("open(O_CREAT): called without specifying a mode", 0);
  }

  if (libc_check_incognito_mode()) {
		char *incognito_file_path = reinterpret_cast<char *>(malloc(LIBC_MAX_FILE_PATH_SIZE));
		if (incognito_file_path == NULL) {
			ALOGE("Tiramisu 2 LIBC No memory");
			return -1;
		}
		int path_set = 0;
		int add_entry = 0;
		int update_entry = 0;

		ALOGE("Posix_open:2  %s flags=0x%x", pathname, flags);
		memset(incognito_file_path, 0, 4096);
		libc_incognito_file_open(pathname, flags, &path_set, incognito_file_path,
						 	LIBC_MAX_FILE_PATH_SIZE, &add_entry, &update_entry);
		ALOGE("pathname 2 %s new pathname %s add_entry %d update_entry %d \n",
			pathname, incognito_file_path, add_entry, update_entry);
		#if 1
		if (path_set) {
    		int fd = __openat(AT_FDCWD, incognito_file_path, force_O_LARGEFILE(flags), 0);
			if (add_entry) {
				libc_add_file_entry(pathname, incognito_file_path, INCOGNITO_VALID, fd);
			} else if (update_entry) {
				// If the old was file deleted, then an entry in the global state exists.
				// This open is to create a new file, so mark the existing file status as INCOGNITO_VALID.
				libc_update_file_status(pathname, INCOGNITO_VALID /* file status */);
			}
			free(incognito_file_path);
			return fd;
		}
		#endif
		free(incognito_file_path);
  }
  return __openat(AT_FDCWD, pathname, force_O_LARGEFILE(flags), 0);
}

int openat(int fd, const char *pathname, int flags, ...) {
  mode_t mode = 0;
  if (init_incognito_mode(pathname)) return 0;

  if ((flags & O_CREAT) != 0) {
    va_list args;
    va_start(args, flags);
    mode = static_cast<mode_t>(va_arg(args, int));
    va_end(args);
  }

  if (libc_check_incognito_mode()) {
		char *incognito_file_path = reinterpret_cast<char *>(malloc(LIBC_MAX_FILE_PATH_SIZE));
		if (incognito_file_path == NULL) {
			ALOGE("Tiramisu LIBC No memory");
			return -1;
		}
		int path_set = 0;
		int add_entry = 0;
		int update_entry = 0;

		ALOGE("Posix_open: 3 %s flags=0x%x", pathname, flags);
		memset(incognito_file_path, 0, 4096);
		libc_incognito_file_open(pathname, flags, &path_set, incognito_file_path,
						 	LIBC_MAX_FILE_PATH_SIZE, &add_entry, &update_entry);
		ALOGE("pathname 3 %s new pathname %s path_set %d add_entry %d update_entry %d \n",
			pathname, incognito_file_path, path_set, add_entry, update_entry);
		#if 1
		if (path_set) {
    		int rc_fd = __openat(fd, incognito_file_path, force_O_LARGEFILE(flags), mode);
			if (add_entry) {
				libc_add_file_entry(pathname, incognito_file_path, INCOGNITO_VALID, rc_fd);
			} else if (update_entry) {
				// If the old was file deleted, then an entry in the global state exists.
				// This open is to create a new file, so mark the existing file status as INCOGNITO_VALID.
				libc_update_file_status(pathname, INCOGNITO_VALID /* file status */);
			}
			free(incognito_file_path);
			return rc_fd;
		}
		#endif
		free(incognito_file_path);
	}

  return __openat(fd, pathname, force_O_LARGEFILE(flags), mode);
}
__strong_alias(openat64, openat);

int __openat_2(int fd, const char* pathname, int flags) {
  if (init_incognito_mode(pathname)) return 0;

  if ((flags & O_CREAT) != 0) {
    __fortify_chk_fail("openat(O_CREAT): called without specifying a mode", 0);
  }

  if (libc_check_incognito_mode()) {
		char *incognito_file_path = reinterpret_cast<char *>(malloc(LIBC_MAX_FILE_PATH_SIZE));
		if (incognito_file_path == NULL) {
			ALOGE("Tiramisu 4 LIBC No memory");
			return -1;
		}
		int path_set = 0;
		int add_entry = 0;
		int update_entry = 0;

		ALOGE("Posix_open: 4 %s flags=0x%x", pathname, flags);
		memset(incognito_file_path, 0, 4096);
		libc_incognito_file_open(pathname, flags, &path_set, incognito_file_path,
						 	LIBC_MAX_FILE_PATH_SIZE, &add_entry, &update_entry);
		ALOGE("pathname 4 %s new pathname %s add_entry %d update_entry %d \n",
			pathname, incognito_file_path, add_entry, update_entry);
		#if 1
		if (path_set) {
    		int rc_fd = __openat(fd, incognito_file_path, force_O_LARGEFILE(flags), 0);
			if (add_entry) {
				libc_add_file_entry(pathname, incognito_file_path, INCOGNITO_VALID, rc_fd);
			} else if (update_entry) {
				// If the old was file deleted, then an entry in the global state exists.
				// This open is to create a new file, so mark the existing file status as INCOGNITO_VALID.
				libc_update_file_status(pathname, INCOGNITO_VALID /* file status */);
			}
			free(incognito_file_path);
			return rc_fd;
		}
		#endif
		free(incognito_file_path);
	}

  return __openat(fd, pathname, force_O_LARGEFILE(flags), 0);
}
