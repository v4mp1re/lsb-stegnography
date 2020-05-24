#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "decode.h"
#include "types.h"
#include <regex.h>

/* decode function definitions */


int x;

/* Decode secret file extenstion */


/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo)
{
	/* decode the magic string from the steged image */
	fprintf(stdout,"INFO: Decoding magic string.\n");

	if(decode_magic_string(MAGIC_STRING,decInfo) == e_failure)
	{
		fprintf(stderr,"ERROR: Magic string not found image corrupted or not encoded.\n");
		
		return e_failure;	
	}



	/* decode the magic string from the steged image */
	fprintf(stdout,"INFO: Decoding output file extension.\n");

	if(decode_secret_file_extn(decInfo) == e_failure)
	{
		fprintf(stderr,"ERROR: Output file extension not found file corrupted.\n");
		
		return e_failure;
	}


	/* create output file and open it for writing */

	if(decode_set_output_file(decInfo) == e_failure)
	{
		fprintf(stderr,"ERROR: Failed to create output file.\n");
		
		return e_failure;
	}



	/* decode the secrete file size */
	fprintf(stdout,"INFO: Decoding secrete file size.\n");

	if(decode_secret_file_size(decInfo) == e_failure)
	{
		fprintf(stderr,"ERROR: Reading secrete file size failed.\n");
		
		return e_failure;
	}

	
	/* decode secrete data */
	fprintf(stdout,"INFO: Decoding secrete data.\n");

	if(decode_secret_file_data(decInfo) == e_failure)
	{
		fprintf(stderr,"ERROR: Reading secrete file data failed.\n");
		
		return e_failure;
	}

	
}



Status decode_secret_file_data(DecodeInfo *decInfo)
{
	char *data_buffer = malloc(decInfo->size_secret_file);

	//printf("\n\n%d",decInfo->size_secret_file);

	if(data_buffer == NULL)
		return e_failure;


	if(decode_block(data_buffer, decInfo->size_secret_file, decInfo) == e_failure)
		return e_failure;


	int write_count = fwrite(data_buffer, 1,decInfo->size_secret_file, decInfo->fptr_secret);


	if(ferror(decInfo->fptr_secret))
		return e_failure;

	free(data_buffer);

	return e_success;
}


Status decode_secret_file_size(DecodeInfo *decInfo)
{
	if(decode_block((char*)&decInfo->size_secret_file, sizeof(decInfo->size_secret_file), decInfo) == e_failure)
		return e_failure;
	
	return e_success;
}




Status decode_set_output_file(DecodeInfo *decInfo)
{
	if(decInfo->secret_fname == NULL)
	{
		fprintf(stdout,"INFO: Output File not mentioned. Creating decoded%s as default\n", decInfo->extn_secret_file);
		
		decInfo->secret_fname = malloc(strlen("decoded") + 1);

		if(decInfo->secret_fname == NULL)
			return e_failure;
		
		strcpy(decInfo->secret_fname,"decoded");
		
	}

	decInfo->secret_fname = realloc(decInfo->secret_fname,strlen(decInfo->secret_fname) + strlen(decInfo->extn_secret_file) + 1);

	if(!decInfo->secret_fname)
		return e_failure;

	strcat(decInfo->secret_fname,decInfo->extn_secret_file);

	decInfo->fptr_secret = fopen(decInfo->secret_fname,"w");

	if(decInfo->fptr_secret == NULL)
		return e_failure;
	
	return e_success;
}




uint decode_secret_file_extn_size(DecodeInfo *decInfo)
{
	uint ext_size=0;

	if(decode_block((char*) &ext_size, sizeof(ext_size), decInfo) == e_failure)
	{
		fprintf(stderr,"ERROR: Failed decode extension size.\n");
		exit(0);
	}

	return ext_size;
}


/* Decode secret file extenstion 
 * @param DecodeInfo
 * return e_success if decode succesful otherwise e_failure
 */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
	/* first find out the secrete file extension size */
	uint extn_size;

	extn_size = decode_secret_file_extn_size(decInfo);

	//printf("\n\n%d", extn_size);

	/* excluding '.' used for verification from extension name */
	if((extn_size - 1) > MAX_FILE_SUFFIX)
	{
		return e_failure;
	}

	if(decode_block(decInfo->extn_secret_file, extn_size, decInfo) == e_failure)
		return e_failure;
	decInfo->extn_secret_file[extn_size] = '\0';

	return e_success;
}


/* Decode data of various bolck size data
 * input (char *) block address, block size, DecodeInfo
 * e_success on succesful operation else e_failure
 */
Status decode_block(char * block_ptr, uint block_size, DecodeInfo *decInfo)
{

	for(uint i = 0; i < block_size; i++)
	{
		if(read_secret_data(decInfo) == e_failure)
			return e_failure;

		
		if( decode_byte_from_lsb(block_ptr + i, decInfo->image_data) == e_failure)
		{
			fprintf(stderr,"error");
			return e_failure;
		}

		if(x==1)
			exit(0),printf("chr:  %hhx\n",block_ptr+i);
	}


	return e_success;
}



/* Decode 1 byte from lsb of 8 byte image data
 * @ decoded character , 8 bytes encoded data
 * return e_success on true else e_failure
 */
Status decode_byte_from_lsb(char *data, const char *image_buffer)
{
	if(image_buffer == NULL)
	
		return e_failure;

	for(int i = 0; i < MAX_IMAGE_BUF_SIZE; i++)
	{
		/* shifting bits */
		*data <<= 1;
		/* clearing lsb of data and setting it with lsb bit of img_data[i] */
		*data = (*data & 0xfe) | (*(image_buffer + MAX_IMAGE_BUF_SIZE - i - 1) & 0x01);
	}
	
	return e_success;

}


/* Read MAX_IMAGE_BUF_SIZE bytes from image file 
 * input DecodeInfo
 * return e_success if reading is succesful otherwise e_failure
 */

Status read_secret_data(DecodeInfo *decInfo)
{
	fread(decInfo->image_data, 1, MAX_IMAGE_BUF_SIZE, decInfo->fptr_stego_image);

	if(ferror(decInfo->fptr_stego_image))
		return e_failure;
	if(x==1)
	{
		for(int j=0; j<8;j++)
		printf("%hhx ",decInfo->image_data[j]);
	}

	return e_success;
}

/* Decode Magic string to verify image is steged
 * @param DecodeInfo
 * return e_success if decoding success otherwise e_false
 */

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
	char decoded_magic_string;

	char file_ext_delim;

	/* setting offset to read magic string data */ 
	fseek(decInfo->fptr_stego_image, decInfo->raster_offset, SEEK_SET);

	if(read_secret_data(decInfo) == e_failure)
	{
		fprintf(stderr,"ERROR: Reading image file failed.\n");
		
		return e_failure;
	}
	
	decode_byte_from_lsb(&decoded_magic_string, decInfo->image_data);

	/* setting offset to read '.' of extension for verification */ 
	fseek(decInfo->fptr_stego_image, sizeof(int) * MAX_IMAGE_BUF_SIZE, SEEK_CUR);

	if(read_secret_data(decInfo) == e_failure)
	{
		fprintf(stderr,"ERROR: Reading image file failed.\n");
		
		return e_failure;
	}
	
	decode_byte_from_lsb(&file_ext_delim, decInfo->image_data);


	if(decoded_magic_string != magic_string[1] || file_ext_delim != '.')
		return e_failure;

	/* setting offset to back to magic string starting position */ 
	fseek(decInfo->fptr_stego_image, - sizeof(int) * MAX_IMAGE_BUF_SIZE - 8, SEEK_CUR);

		
	return e_success;
}





/* Get Imgage header information
 * @param DecodeInfo
 * return e_success on success otherwise e_failure
 */

Status get_image_header_info(DecodeInfo *decInfo)
{
	decInfo->image_capacity = get_image_size_for_bmp(decInfo->fptr_stego_image);

	/* moving seek head to read raster data offset value */ 
	fseek(decInfo->fptr_stego_image, 10, SEEK_SET);

	fread(&decInfo->raster_offset, 4, 1, decInfo->fptr_stego_image);

	if(ferror(decInfo->fptr_stego_image))
	{
		fprintf(stderr, "ERROR: Reading raster data offset failed.\n");

		return e_failure;
	}
}

/* Validate files for decoding 
 * validate stego image file is not empty
 * @param DecodeInfo
 * return e_success on true otherwise e_failure
 */
Status decode_validate_files(DecodeInfo *decInfo)
{
	if(get_file_size(decInfo->fptr_stego_image) == 0)
	{
		return e_failure;
	}

	return e_success;
}



/* 
 * Get File pointers for i/p and o/p files
 * Inputs: DecodeInfo
 * Return Value: e_success or e_failure, on file errors
 */
Status decode_open_files(DecodeInfo *decInfo)
{
	// Stego Image file
	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
		
	// Do Error handling
	if (decInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
			
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

		return e_failure;
	}

	fprintf(stdout,"INFO: Opened %s\n",decInfo->stego_image_fname);		

	return e_success;
		
}




/* Read and validate Encode args from argv 
 * @param CLA **argv , DecodeInfo
 * returns e_success on succesful execution otherwise e_failure
 */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{

	/* regexp comparison for file extension */
	regex_t stego_file_ext_regex;
	
	regcomp(&stego_file_ext_regex,".bmp",0);


    	/* setting source file name */
    	decInfo->stego_image_fname = malloc((strlen(argv[2]) + 1)* sizeof(char));

    	if(!decInfo->stego_image_fname)
    	{
		fprintf(stderr,"ERROR: Failed allocate memory.\n");
	    
	    	return e_failure;
    	}

    	strcpy(decInfo->stego_image_fname,argv[2]);


	
	/* secrete file name is provided */
	if(argc > 3)
	{
		/* setting secrete file name */
		decInfo->secret_fname = malloc(strlen(argv[3]) * sizeof(char) + 1);

		if(!decInfo->secret_fname)
		{
			fprintf(stderr,"ERROR: Failed allocate memory.\n");
		    
			return e_failure;
		}

	    strcpy(decInfo->secret_fname,argv[3]);

	    char *scrt_file_ext = strchr(decInfo->secret_fname,'.');

	    /* check if the input name for the secrete file contains '.ext' then remove it */
	    if(scrt_file_ext)
	    {
		    *scrt_file_ext = '\0';    
	    }

	}


    	/* regexec() returns 0 if match found */
    	if(regexec(&stego_file_ext_regex, decInfo->stego_image_fname, 0, NULL, 0))
    	{
	    fprintf(stderr,"\nSource file is not a bmp file..!\n");
	    
	    regfree(&stego_file_ext_regex);
	    
	    return e_failure;
    	}



    	return e_success;

}



/* Initialize decInfo structure 
 * @params DecodeInfo, OperationType
 * return Status
 */

Status init_dec_info(DecodeInfo *decInfo)
{

	decInfo->secret_fname = NULL;
	
	decInfo->stego_image_fname = NULL;

	
	
	
	decInfo->fptr_secret = NULL;
	
	decInfo->fptr_stego_image = NULL;

	return e_success;
}



/* closes the files opened
 * @param EncodeInfo
 * return void
 */

void decode_close_files(DecodeInfo *decInfo)
{
	if(decInfo->fptr_stego_image != NULL)
	{
		fclose(decInfo->fptr_stego_image);
		
		decInfo->fptr_stego_image = NULL;
	}
	
	
	if(decInfo->fptr_secret != NULL)
	{
		fclose(decInfo->fptr_secret);
		
		decInfo->fptr_secret = NULL;
	}
}
