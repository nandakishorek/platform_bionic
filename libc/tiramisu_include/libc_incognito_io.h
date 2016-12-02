/*
 * Copyright (c) 2016 Tiramisu
 */
#ifndef _LIBC_INCOGNITO_IO_H
#define _LIBC_INCOGNITO_IO_H

#include <stdlib.h>
#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS
#define LIBC_MAX_NUM_FDS_PER_FILE 32
#define LIBC_MAX_FILE_PATH_SIZE 4096
#define LIBC_MAX_FILENAME_SIZE 256
#define LIBC_MAX_DIRNAME_SIZE  3840
#define LIBC_MAX_FILES_PER_PROCESS 128


enum Libc_File_Status {
	INCOGNITO_VALID = 0,
	INCOGNITO_DELETED
};

int libc_incognito_io_init();
void libc_incognito_io_stop();
bool libc_check_incognito_mode();
bool libc2_check_incognito_mode();

int libc_incognito_file_open(const char *pathname, int flags, int *path_set,
						char *incognito_file_path, int incog_pathname_sz,
						int *add_entry, int *update_entry);
int libc_add_file_entry(const char *original_filename, const char *new_filename,
				   Libc_File_Status status, int fd);

bool libc_lookup_filename(const char *pathname, char *incognito_pathname,
					 size_t incog_pathname_sz, Libc_File_Status *status);
int libc_add_or_update_file_delete_entry(const char *pathname, bool *need_delete,
						   	  	    char *new_filename,
									size_t new_filename_size);
int libc_update_file_status(const char *pathname, Libc_File_Status statusa);
__END_DECLS
#endif /* _LIBC_INCOGNITO_IO_H */
