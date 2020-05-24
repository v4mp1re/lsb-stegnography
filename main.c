#include <stdio.h>
#include <stdlib.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"


int main(int argc, char *argv[])
{
	EncodeInfo encInfo;

	DecodeInfo decInfo;

	uint img_size;

#if defined (ENCODE)

	// testing the encode_bytes_to_lsb() //
	char data[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
	
	encode_byte_to_lsb(0x2A,data);
	
	for(int i =0; i<8; i++)
		printf("%#hhx  ",data[i]);
	exit(0);



#elif defined (DECODE)
	//testing decode_byte_from_lsb(data,image_data) 
	char image_buffer[8] = {0xc ,0x8 ,0x0 ,0x10 ,0x9 ,0x3 ,0x13 ,0xa};
	char data;
	//for(int i=0; i<4; i++)
	{
		decode_byte_from_lsb(&data ,image_buffer);
		//printf("\n byte: %hhx\n", *((char*)&data + i));
	}
	printf("%#hhx",data);
	exit(0);
#endif
	


	if(argc <= 1)
	{
		fprintf(stderr,"No arguments passed..!\n");
		
		print_usage(e_unsupported);
		
		exit(1);
	}


	if(check_operation_type(argv) == e_encode) /* encoding mode*/
	{
		/* setting the operation mode  and initialising file name pointers */
		init_enc_info(&encInfo);

		if(argc < 4)
		{
			printf("Missing arguments..!\n");
			
			print_usage(e_encode);
			
			exit(1);
		}
		
		/* reading file names from CLA and validating */
		if(read_and_validate_encode_args(argv, &encInfo) == e_failure)
		{
			print_usage(e_encode);
			
			exit(1);
		}
		
		
		
		/* open requied files */
		fprintf(stdout,"INFO: Opening required files.\n");

		if(encode_open_files(&encInfo) == e_failure)
		{
			encode_close_files(&encInfo);
			
			exit(1);		
		}

		
		
		fprintf(stdout,"INFO: ## Encoding Procedure Started ##\n");

		/* validate files */

		if(validate_files(&encInfo) == e_failure)
		{
			encode_close_files(&encInfo);
			
			exit(1);
		}
		
		
		
		
		/* check capacity of image file */
		fprintf(stdout,"INFO: Checking for %s capacity to handle %s\n",encInfo.src_image_fname, encInfo.secret_fname);
		
		if(check_capacity(&encInfo) == e_failure)
		{
			encode_close_files(&encInfo);

			fprintf(stderr,"ERROR: Image file doesn't have capacity to hold secrete file.\n");
			
			exit(1);
		}

		fprintf(stdout,"INFO: Done. Found OK\n");

		
		
		/* creating output file */	
		if(encode_set_output_file(argc, argv, &encInfo) == e_failure)
		{
			encode_close_files(&encInfo);
			
			exit(1);
		}


		
		
		/* start encoding procedure */
		fprintf(stdout,"INFO: Encoding started\n");

		if(do_encoding(&encInfo) == e_failure)
		{
			encode_close_files(&encInfo);
			if(remove(encInfo.stego_image_fname))
			{
				fprintf(stderr,"ERROR: Deleting unfinished output file failed\n");
			}
			exit(1);
		}

		fprintf(stdout,"INFO: Encoding done.\n");

		encode_close_files(&encInfo);

		exit(0);

	}
	else if(check_operation_type(argv) == e_decode) /* decoding mode */
	{
		/* steged image file-name is requied */
	    	if(argc < 3)
	    	{
			print_usage(e_decode);

		  	exit(1);
	    	}

	    

		/* setting the operation mode  and initialising file name pointers */
		if(init_dec_info(&decInfo) == e_failure)
		{
		    exit(1);
		}


		/* read and validate arguments */
		if(read_and_validate_decode_args(argc, argv, &decInfo) == e_failure)
		{
		    exit(1);
		}

		/* open required files */
		fprintf(stdout,"INFO: Opening required files.\n");

		if(decode_open_files(&decInfo) == e_failure)
		{
			fprintf(stderr,"ERROR: Failed to open files.\n");

			decode_close_files(&decInfo);

			exit(1);
		}

		/* validate stego file */
		fprintf(stdout,"INFO: Validating files.\n");

		if(decode_validate_files(&decInfo) == e_failure)
		{
			fprintf(stderr,"ERROR: Steged image is empty or corrupted.\n");

			decode_close_files(&decInfo);
			
			exit(0);
		}


		if(get_image_header_info(&decInfo) == e_failure)
		{
			fprintf(stderr,"ERROR: Steged image is empty or corrupted.\n");

			decode_close_files(&decInfo);
			
			exit(0);
		}


	    	fprintf(stdout,"INFO: ## Decoding Procedure started ##\n");

		
		/* decode the magic string from the steged image */
		fprintf(stdout,"INFO: Decoding magic string.\n");

		if(do_decoding(&decInfo) == e_failure)
		{
			fprintf(stderr,"ERROR: Decoding failed exiting...\n");

			decode_close_files(&decInfo);
			
			exit(1);
		}

		fprintf(stdout,"INFO: Decoding successful.\n");

		decode_close_files(&decInfo);
		
		exit(0);


	}
	else
	{
		printf("unsupported mode..!\n");
		
		print_usage(e_unsupported);
		
		exit(0);
	}

	exit(0);

}

