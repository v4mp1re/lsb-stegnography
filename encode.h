#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types

#include "common.h" // Contains common declarations 


/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */


typedef struct _EncodeInfo
{
	
	/* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    uint size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} EncodeInfo;


/* Encoding function prototype */

/* Write encoded image_data to steged image file */
Status write_image_data(EncodeInfo *encInfo);

/* Read image_data of MAX_IMAGE_BUF_SIZE bytes from source image file */
Status read_image_data(EncodeInfo *encInfo);


/* Get File pointers for i/p and o/p files */
void encode_close_files(EncodeInfo *encInfo);

/* set output file_name and open it for read operation*/
Status encode_set_output_file(int argc, char *argv[], EncodeInfo *encInfo);

/* check files are not empty and pointers to NULL*/
Status validate_files(EncodeInfo *encInfo);

/* initialize EncodeInfo with op_mode and pointers to NULL*/
Status init_enc_info(EncodeInfo *encInfo);




/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status encode_open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif
