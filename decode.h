#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types


/* 
 * Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */


typedef struct _DecodeInfo
{
	
	/* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    uint raster_offset;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    uint size_secret_file;

    

} DecodeInfo;




/* Decoding function prototype */

/* decode block data */
Status decode_block(char * block_ptr, uint block_size, DecodeInfo *decInfo);

/* Get image header info */
Status get_image_header_info(DecodeInfo *decInfo);

/* set output file_name and open it for read operation*/
Status decode_set_output_file(DecodeInfo *decInfo);


/* Get File pointers for i/p and o/p files */
void decode_close_files(DecodeInfo *decInfo);


/* Write decoded secrete_data to output file */
Status write_secrete_data(DecodeInfo *decInfo);


/* Get File pointers for i/p and o/p files */
Status decode_open_files(DecodeInfo *decInfo);



/* Read secrete_data of MAX_IMAGE_BUF_SIZE bytes from source image file */
Status read_secret_data(DecodeInfo *decInfo);



/* initialize EncodeInfo with op_mode and pointers to NULL*/
Status init_dec_info(DecodeInfo *decInfo);


/* initialize DecodeInfo with op_mode and pointers to NULL*/
Status decode_validate_files(DecodeInfo *decInfo);



/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo);




/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);




/* Decode Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);



/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);


/* Decode secret file extenstion */
uint decode_secret_file_extn_size(DecodeInfo *decInfo);


/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);



/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);



/* Decode function, which does the real decoding */
Status decode_data_from_image(char *data, int size, FILE *fptr_stego_image, FILE *fptr_secret);



/* Decode a byte into LSB of image data array */
Status decode_byte_from_lsb(char *data, const char *image_buffer);


void decode_close_files(DecodeInfo *decInfo);

#endif
