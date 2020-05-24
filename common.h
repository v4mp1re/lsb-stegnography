#ifndef COMMON_H
#define COMMON_H

#include "types.h" // Contains user defined types

/* Magic string to identify whether stegged or not */
#define MAGIC_STRING "#*"
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4
#define DATA_BUFF_SIZE 500

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* print usage message */
void print_message(const char *msg, MessageType msg_type);

/* print usage message */
void print_usage(OperationType);

/* get file size in bytes */
uint get_file_size(FILE *f);

#endif
