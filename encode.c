#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encode.h"
#include "common.h"
#include "types.h"
#include <regex.h>

/* Function Definitions */

int x;

/* Perform the encoding 
 * @param EncodeInfo * 
 * return e_success on succesful execution otherwise e_failure
 * multiple steps are done in this method
 */
Status do_encoding(EncodeInfo *encInfo)
{

	fprintf(stdout,"INFO: Copying Image Header\n");

	/* copy image header */
	if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
	{
	    fprintf(stderr,"ERROR: Copying header info failed.\n");
	    
	    return e_failure;
	}

	
	/* encode magic string */
	fprintf(stdout,"INFO: Encoding Magic string signature\n");
	
	if(encode_magic_string(MAGIC_STRING,encInfo) == e_failure)
	{
	    fprintf(stderr,"ERROR: Encoding magic string failed.\n");
	    
	    return e_failure;
	}
	
	fprintf(stdout,"INFO: Done.\n");

	/* encode secrete file extenstion size */
	fprintf(stdout,"INFO: Encoding %s file extenstion size.\n", encInfo->secret_fname);

	if(encode_secret_file_extn(encInfo) == e_failure)
	{
	    fprintf(stderr,"ERROR: Encoding file extension failed.\n");
	    
	    return e_failure;
	}
	
	fprintf(stdout,"INFO: Done.\n");

	
	/* encode secrete file size */
	fprintf(stdout,"INFO: Encoding %s file size.\n", encInfo->secret_fname);
	
	if(encode_secret_file_size(encInfo) == e_failure)
	{
	    fprintf(stderr,"ERROR: Encoding file size failed.\n");
	    
	    return e_failure;
	}

	fprintf(stdout,"INFO: Done.\n");

	
	
	/* encode secrete file data */
	fprintf(stdout,"INFO: Encoding %s file data.\n", encInfo->secret_fname);

	if(encode_secret_file_data(encInfo) == e_failure)
	{
	    fprintf(stderr,"ERROR: Encoding file data failed.\n");
	    
	    return e_failure;
	}

	fprintf(stdout,"INFO: Done.\n");



	/* copying remaining image file data to output file */
	fprintf(stdout,"INFO: Copying Left Over Data\n");

	if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
	{
	    fprintf(stderr,"ERROR: Copying Left over data failed.\n");
	    
	    return e_failure;
	}

	fprintf(stdout,"INFO: Done.\n");

}





/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	char data_buff[DATA_BUFF_SIZE];

    	int read_bytes_count = 0;

    	int write_bytes_count = 0;

	while(!feof(fptr_src))
	{
	    read_bytes_count = fread(data_buff, 1, DATA_BUFF_SIZE, fptr_src);

	    if(ferror(fptr_src))
	    {
		  fprintf(stderr,"ERROR: Reading source file failed.\n");
		  
		  return e_failure;
	    }

	    write_bytes_count = fwrite(data_buff, 1, read_bytes_count, fptr_dest);

	    if(ferror(fptr_dest) || (write_bytes_count != read_bytes_count))
	    {
		  fprintf(stderr,"ERROR: Writing to output file failed.\n");
		  
		  return e_failure;
	    }
	}

	return e_success;
}





/* Encode secret file data
 * @param EncodeInfo
 * return e_success on succesful execution otherwise e_failure
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char data_buffer[DATA_BUFF_SIZE];
	
	int read_byte_count = 0;

	/* setting seek head o secrete file to begning */
	fseek(encInfo->fptr_secret,0,SEEK_SET);


	/* encode the content of secrete file */
	while(!feof(encInfo->fptr_secret))
	{
	    /* read MAX_DATA_BUFF_SIZE of data from secrete file at once no of bytes read will be store in byte_count */
	    read_byte_count = fread(data_buffer, 1, DATA_BUFF_SIZE, encInfo->fptr_secret);

	    if(ferror(encInfo->fptr_secret))
	    {
		  fprintf(stderr,"ERROR: Reading data from secrete file failed.\n");
		  
		  return e_failure;
	    }

	    /* for each byte in data_buffer encode it and write it to ouput file */
	    for(int i = 0; i < read_byte_count; i++)
	    {
		  if(read_image_data(encInfo) == e_failure)
			return e_failure;
		  
		  if(encode_byte_to_lsb(data_buffer[i], encInfo->image_data) == e_failure)	  
			return e_failure;

		  if(write_image_data(encInfo) == e_failure)
			return e_failure;
	    }
	}

	return e_success;
}




/* Encode secret file size 
 * @param EncodeIno
 * return e_success on succesful execution otherwise e_failure
 */
Status encode_secret_file_size(EncodeInfo *encInfo)
{
	for(int i = 0; i < sizeof(encInfo->size_secret_file); i++)
	{
	    if(read_image_data(encInfo) == e_failure)
		  return e_failure;
	    
	    if(encode_byte_to_lsb(*((char*) &encInfo->size_secret_file + i), encInfo->image_data) == e_failure)	  
		  return e_failure;

	    if(write_image_data(encInfo) == e_failure)
		  return e_failure;
	}

	
	/* no error */
	return e_success;	
}



/* Encode secret file extenstion 
 * @param EncodeInfo
 * returns e_success on succesful execution otherwise e_failure
 */
Status encode_secret_file_extn(EncodeInfo *encInfo)
{
	/* encode the file extension size first */
	int ext_size = strlen(encInfo->extn_secret_file);
	for(int i = 0; i < sizeof(ext_size); i++)
	{
	    if(read_image_data(encInfo) == e_failure)
		  return e_failure;

	    if(encode_byte_to_lsb(*((char*) &ext_size + i), encInfo->image_data) == e_failure)	  
		  return e_failure;

	    if(write_image_data(encInfo) == e_failure)
		  return e_failure;
	}
	/* encode secrete file extension to the image byte by byte */

	for(int i = 0; i < strlen(encInfo->extn_secret_file); i++)
	{
	    if(read_image_data(encInfo) == e_failure)
		  return e_failure;
	    
	    if(encode_byte_to_lsb(encInfo->extn_secret_file[i], encInfo->image_data) == e_failure)	  
		  return e_failure;

	    if(write_image_data(encInfo) == e_failure)
		  return e_failure;
	}
	
	return e_success;
}




/* Write encoded image_data to steged image file */
Status write_image_data(EncodeInfo *encInfo)
{
	fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);

	if(ferror(encInfo->fptr_stego_image))
	{
	    fprintf(stderr,"ERROR: File write error.\n");
	    
	    return e_failure;
	}

	return e_success;
}


/* Read image_data of MAX_IMAGE_BUF_SIZE bytes from source image file */
Status read_image_data(EncodeInfo *encInfo)
{
	fread(encInfo->image_data,8,1,encInfo->fptr_src_image);

	if(ferror(encInfo->fptr_src_image))
	{
	    fprintf(stderr,"ERROR: File read error.\n");
	    
	    return e_failure;
	}

	return e_success;
}



/* Encode a byte into LSB of image data array 
 * @param data to encode, image_data to which data to be encoded 
 * return e_success on succesful execution otherwise e_failure
*/
Status encode_byte_to_lsb(char data, char *image_buffer)
{
	if(image_buffer == NULL)
	
		return e_failure;

	for(int i = 0; i < 8; i++)
	{
		/* clearing lsb of img_data and setting it with lsb bit of c */
		image_buffer[i] = (image_buffer[i] & 0xfe) | (data & 0x01);
		
		/* shifting bits */
		data >>= 1;
	}

	return e_success;
}



/* Store Magic String 
 * @param const char *magic_string, EncodeInfo * 
 * return e_success on successful completion otherwise e_failure 
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	/* read the 8 bytes required to encode 1 byte of encode data 
	 * data will ne stored in EncodeInfo.image_data
	 */
	if(read_image_data(encInfo) == e_failure)
	{ 
		fprintf(stderr,"ERROR: Failed to read from %s.\n", encInfo->src_image_fname);

		return e_failure;
	}



	/* encoding magic string */
	if(encode_byte_to_lsb(magic_string[1], encInfo->image_data) == e_failure)
	{
		fprintf(stderr,"ERROR: Failed to encode data\n");

		return e_failure;
	}


	/* writing the encoded image_data to steged_image */
	if(write_image_data(encInfo) == e_failure)
	{
		fprintf(stderr,"ERROR: Failed write to %s\n", encInfo->stego_image_fname);

		return e_failure;
	}

}

/* Copy Image header to output file as it is
 * @params FILE *fptr_src_image, FILE *fptr_dest_image
 * return e_success on sucessful copy otherwise e_failure
 */

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	int raster_data_offset;
	
	char *data_header;

	/* read the raster data offset of source file
	 * header file will be wihin this limit */
	fseek(fptr_src_image, 10L, SEEK_SET);
	
	fread(&raster_data_offset, sizeof(raster_data_offset), 1, fptr_src_image);

	if(ferror(fptr_src_image))
	{
		fprintf(stderr,"ERROR: Failed read from source file.\n");
	
		return e_failure;
	}

	/* set both file position indicator to begning */
	fseek(fptr_dest_image, 0L, SEEK_SET);
	fseek(fptr_src_image, 0L, SEEK_SET);
	
	/* copy header data of source to destination */
	data_header = malloc(raster_data_offset * sizeof(char));

	if(data_header == NULL)
	{
		fprintf(stderr,"ERROR: Failed allocate memory.\n");
		
		return e_failure;
	}

	fread(data_header, raster_data_offset, 1, fptr_src_image);

	if(ferror(fptr_src_image))
	{
		fprintf(stderr,"ERROR: Failed read from source file.\n");
		
		return e_failure;
	}
	
	fwrite(data_header, raster_data_offset, 1, fptr_dest_image);

	if(ferror(fptr_dest_image))
	{
		fprintf(stderr,"ERROR: Failed write to destination file.\n");
		
		return e_failure;
	}

	free(data_header);

	return e_success;
}

/* Read and validate Encode args from argv 
 * input: CLA from main EncodeInfo pointer 
 * return: enum Status 
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	/* regexp comparison for file extension */
	regex_t src_file_ext_regex;
	
	regcomp(&src_file_ext_regex,".bmp",0);

	regex_t scrt_file_ext_regex;
	
	regcomp(&scrt_file_ext_regex,".[a-z]",0);

	char *scrt_file_ext;

    	/* setting source file name */
    	encInfo->src_image_fname = malloc(strlen(argv[2]) * sizeof(char) + 1);

    	if(!encInfo->src_image_fname)
    	{
		fprintf(stderr,"ERROR: Failed allocate memory.\n");
	    
	    	return e_failure;
    	}

    	strcpy(encInfo->src_image_fname,argv[2]);

	/* setting secrete file name */
    	encInfo->secret_fname = malloc(strlen(argv[3]) * sizeof(char) + 1);

    	if(!encInfo->secret_fname)
    	{
		fprintf(stderr,"ERROR: Failed allocate memory.\n");
	    
	    	return e_failure;
    	}

    	strcpy(encInfo->secret_fname,argv[3]);


    	/* regexec() returns 0 if match found */
    	if(regexec(&src_file_ext_regex, encInfo->src_image_fname, 0, NULL, 0))
    	{
	    fprintf(stderr,"\nSource file is not a bmp file..!\n");
	    
	    regfree(&src_file_ext_regex);
	    
	    return e_failure;
    	}

    	
	scrt_file_ext = strchr(encInfo->secret_fname,'.');

	/*file doesn't have an extension  */
    	if(!scrt_file_ext)
    	{
	    fprintf(stderr,"\nSecrete file doesn't not a have an extension..!\n");
	    
	    regfree(&scrt_file_ext_regex);
	    
	    return e_failure;
    	}



    	if(strlen(scrt_file_ext) - 1 >= MAX_FILE_SUFFIX)
    	{
	    fprintf(stderr,"\nERROR: Secrete file extension size exceeded..!\n");
	    
	    return e_failure;
    	}

    	/* copying secrete file extension including . to struct encInfo */
    	strcpy(encInfo->extn_secret_file,scrt_file_ext); 

    	return e_success;
}




/* set output filename and open it for read operation
 * @param const char *output_file_name
 * return e_success on succesful, otherwise e_failure
 */

Status encode_set_output_file(int argc, char *argv[], EncodeInfo *encInfo)
{
	char *output_file_name;

		
	if(argc > 4) /* optional o/p file_name is passed */
	{
		output_file_name = malloc(strlen(argv[4]) * sizeof(char) + 5);
		  
		if(!output_file_name)
		{
			fprintf(stderr,"ERROR: Failed allocate memory.\n");
			  
			return e_failure;
		}

		strcpy(output_file_name,argv[4]);
		  
		strcat(output_file_name,".bmp");
		  
		encInfo->stego_image_fname = malloc(strlen(output_file_name) * sizeof(char) + 1); 
		  
		if(!encInfo->stego_image_fname)
		{
			fprintf(stderr,"ERROR: Failed allocate memory.\n");
		  
			return e_failure;
		}
		  
		strcpy(encInfo->stego_image_fname,output_file_name);
	  }
	  else /* o/p file_name is not passed setting default */
	  {
		fprintf(stdout,"INFO: Output File not mentioned. Creating steged_img.bmp as default\n");
		  
		encInfo->stego_image_fname = malloc(strlen("steged_img.bmp") * sizeof(char) + 1);
		  
		if(!encInfo->stego_image_fname)
		{
			fprintf(stderr,"ERROR: Failed allocate memory.\n");
			  
			return e_failure;
		}
		
		strcpy(encInfo->stego_image_fname,"steged_img.bmp");
	  }

	  // Stego Image file
	  encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");

	  // Do Error handling

	  if (encInfo->fptr_stego_image == NULL)
	  {
		  perror("fopen");
		  
		  fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		  return e_failure;
	  }

	  return e_success;
}






/* Initialize encInfo structure 
 * @params EncodeInfo, OperationType
 * return Status
 */

Status init_enc_info(EncodeInfo *encInfo)
{

	encInfo->secret_fname = NULL;
	
	encInfo->src_image_fname = NULL;
	
	encInfo->stego_image_fname = NULL;

	
	
	
	encInfo->fptr_secret = NULL;
	
	encInfo->fptr_src_image = NULL;
	
	encInfo->fptr_stego_image = NULL;

	
	
	return e_success;
}







/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_open_files(EncodeInfo *encInfo)
{
	// Src Image file
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
		
	// Do Error handling
	if (encInfo->fptr_src_image == NULL)
	{
		perror("fopen");
			
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

		return e_failure;
	}

	fprintf(stdout,"INFO: Opened %s\n",encInfo->src_image_fname);		
		
	// Secret file
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
		
	// Do Error handling
	if (encInfo->fptr_secret == NULL)
	{
		perror("fopen");
			
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

		return e_failure;
	}

	fprintf(stdout,"INFO: Opened %s\n",encInfo->secret_fname);		


	fprintf(stdout,"INFO: Done\n");		

	// No failure return e_success
	return e_success;

}






/* validate files size
 * @param EncodeInfo
 * return Status
 */ 

Status validate_files(EncodeInfo *encInfo)
{
	if(get_file_size(encInfo->fptr_src_image) == 0)
	{
		fprintf(stderr,"ERROR: source file is empty\n");
		
		return e_failure;
	}
	
	fprintf(stdout,"INFO: Checking for %s size\n",encInfo->secret_fname);
	
	if(get_file_size(encInfo->fptr_secret) == 0)
	{
		fprintf(stderr,"ERROR: %s file is empty\n",encInfo->secret_fname);
		
		return e_failure;
	}

	fprintf(stdout,"INFO: Done not empty.\n");

	return e_success;
}


/* Check if the source image can hold the secrete file
 * @param EncodeInfo
 * return e_success on true otherwise e_false
 */

Status check_capacity(EncodeInfo *encInfo)
{
    	/* storing image capacity */
    	encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

	/* storing secrete file size */
	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

	/* magic_string + size_of_file_ + size_of_extension */
	int file_info_size = sizeof(char) * MAX_IMAGE_BUF_SIZE + sizeof(int) * MAX_IMAGE_BUF_SIZE + sizeof(int) * MAX_IMAGE_BUF_SIZE;

	int file_extn_chars = strlen(encInfo->extn_secret_file) * MAX_IMAGE_BUF_SIZE;

	int file_data_size =  MAX_IMAGE_BUF_SIZE * encInfo->size_secret_file;

	if(encInfo->image_capacity < (file_info_size + file_extn_chars + file_data_size))
	{
		return e_failure;
	}
	
	return e_success;
}





/* closes the files opened
 * @param EncodeInfo
 * return void
 */

void encode_close_files(EncodeInfo *encInfo)
{
	if(encInfo->fptr_src_image != NULL)
	{
		fclose(encInfo->fptr_src_image);
		
		encInfo->fptr_src_image = NULL;
	}
	
	
	if(encInfo->fptr_stego_image != NULL)
	{
		fclose(encInfo->fptr_stego_image);
		
		encInfo->fptr_stego_image = NULL;
	}
	
	
	if(encInfo->fptr_secret != NULL)
	{
		fclose(encInfo->fptr_secret);
		
		encInfo->fptr_secret = NULL;
	}
}
