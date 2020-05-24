
# Description
* The application accept an image file .bmp along with the a text file which
  contains the message to be steged.
* Analyze the size of the message file and the data part of the .bmp file to check
  whether the messsage could fit in the provided .bmp image
* Provide a option to steg a magic string which could be usefull to identify whether the
  image is steged or not
* The application decrypts a passed image and extracts the secrete data if the image is already steged.

# Compilation
* Gcc version > 8.0 .
* make can be used in optional.

# Processing
* Secrete file is bit by by bit encoded to lsb of each byte of the RGB data section of bmp image.
* We'll need the RGB data section of the bmp image to be ateast 8 times of the secrete data file bytes size (additional metadata are also required to encode).
* First 8 bytes contains Magic String Signature which can be any ascii special character.
* other meta data will be encoded in the following order File Extenstion Size -> file extension -> File Size (int) -> File Data




# References:
* https://en.wikipedia.org/wiki/Steganography
* https://en.wikipedia.org/wiki/BMP
