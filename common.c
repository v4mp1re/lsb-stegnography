#include "types.h" // Contains user defined types
#include <stdio.h>
#include <string.h>
#include "common.h"

/* common used function defention */


/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    
	// Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    
	// Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);

    
	// Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);

    fseek(fptr_image, 0, SEEK_SET);
    
	// Return image capacity
    return width * height * 3;
}





/* Get Operation Type 
 * Input: commandline arguments from main
 * output: OperationType enum 
 */

OperationType check_operation_type(char *argv[])
{
	if(!strcmp("-e", argv[1]))
		
		return e_encode;
	
	if(!strcmp("-d", argv[1]))
		
		return e_decode;
	
	return e_unsupported;
}




/* Get file size
 * @param FILE *
 * return uint file_size
 */

uint get_file_size(FILE *f)
{
	fseek(f,0l,SEEK_END);
	
	return ftell(f);
}


/*
 * print usage of the program
 * @param OperationType mode
 */
void print_usage(OperationType operation_mode)
{
	switch(operation_mode)
	{
		case e_encode:
			
			fprintf(stdout,"usage : \nencoding -> -e <.bmp_file> <.text_file> [output file] \n");
			
			break;

		case e_decode:
			
			fprintf(stdout,"usage : \ndecoding -> -d <.bmp_file> [output file]\n");
			
			break;

		default:
			
			fprintf(stdout,"usage : \nencoding -> -e <.bmp_file> <.text_file> [output file] \ndecoding -> -d <.bmp_file> [output file]\n");
			
			break;
	}
}




/*
 * print messages to stdout
 * @params const char *, MessageType 
 * return void 
 */

void print_message(const char *msg, MessageType type)
{
	if(type == e_error)
		
		fprintf(stderr,"Error: %s\n",msg);

	else if(type == e_info)
		
		fprintf(stdin,"INFO: %s\n",msg);
}
