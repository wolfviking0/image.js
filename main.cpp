//
//  main.cpp
//
//
//  Created by Anthony Liot on 17/12/12.
//
//

#include <emscripten/emscripten.h>
#include <ImageFormat.h>

int output_width;
int output_height;
int output_format;

extern "C" {
 	int EMSCRIPTEN_KEEPALIVE width() { return output_width; };
 	int EMSCRIPTEN_KEEPALIVE height() { return output_height; };
    int EMSCRIPTEN_KEEPALIVE format() { return output_format; };
}

/************************/
/*			MAIN		*/
/************************/

int main(int argc, char *argv[])
{
	printf("Library Image Javascript...\n");
	
	if (argc != 2) {
		printf("main(): number of argument incorrect : %d\n",argc);
		return 1;
	}

    return ImageFormat::load(argv[1],output_width,output_height,output_format) ? EXIT_SUCCESS : EXIT_FAILURE;
}

// 
// 
// 
// #define JPEG_INTERNALS
// #include "jinclude.h"
// #include "jpeglib.h"
// #include "jerror.h"
// 
// #include <png.h>
// #include <webp/decode.h>
// 
// #include <stdio.h>
// #include <stdlib.h>
// #include <string>
// 
// 
// 	
// int output_width;
// int output_height;
// int output_format;
// 
// extern "C" {
// 	int EMSCRIPTEN_KEEPALIVE width() { return output_width; };
// 	int EMSCRIPTEN_KEEPALIVE height() { return output_height; };
// 	int EMSCRIPTEN_KEEPALIVE format() { return output_format; };
// }
// 
// template <class Type>
// static int swap(unsigned char *dest,size_t size,int components,int channel_0,int channel_1) {
// 	Type *d = (Type*)dest;
// 	for(size_t i = 0; i < size; i += components) {
// 		Type c = d[channel_0];
// 		d[channel_0] = d[channel_1];
// 		d[channel_1] = c;
// 		d += components;
// 	}
// 	return 1;
// }
// 
// template <class Type>
// static void flip_y(unsigned char *dest,int width,int height,int components) {
// 	for(int y = 0; y < height / 2; y++) {
// 		Type *s = &((Type*)dest)[width * y * components];
// 		Type *d = &((Type*)dest)[width * (height - y - 1) * components];
// 		for(int x = 0; x < width * components; x++) {
// 			Type c = *d;
// 			*d++ = *s;
// 			*s++ = c;
// 		}
// 	}
// }
// 
// /************************/
// /*			PNG 		*/
// /************************/
// 
// class ImagePNG {
// 
// 	ImagePNG();
// 		
// 	public:
// 		
// 		// load image
// 		static int load(const char *name);
// 		
// 	private:
// 
// 		static void file_read_function(png_structp png_ptr,png_bytep ptr,png_size_t size);
// };
// 
// /*
//  */
// ImagePNG::ImagePNG() {
// 	
// }
// 
// void ImagePNG::file_read_function(png_structp png_ptr,png_bytep ptr,png_size_t size) {
// 	FILE *file = (FILE*)png_get_io_ptr(png_ptr);
// 	fread(ptr,sizeof(png_byte),size,file);
// }
// 
// 
// int ImagePNG::load(const char * name) {
// 	printf("ImagePNG::load\n");
// 	png_bytep* rowPtrs = NULL;
// 	unsigned char* data = NULL;
// 
// 	FILE* file = fopen(name,"rb");
// 	if (file == 0) {
// 		printf("ImagePNG::load(): can't open file '%s'\n",name);
// 		return 0;
// 	}
// 	
// 	//Allocate a buffer of 8 bytes, where we can put the file signature.
//     png_byte sig[8];
//     
//     //Read the 8 bytes from the file into the sig buffer.
//    	fread((char*)sig, sizeof(png_byte), 8, file);
// 	if(!png_check_sig(sig,8)) {
// 		fclose(file);
// 		return 0;
// 	}
// 	
// 	//Here we create the png read struct. 
// 	png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
// 	if(pngPtr == 0) {
// 		fclose(file);
// 		return 0;
// 	}
// 	
// 	//Here we create the png info struct.
// 	png_infop infoPtr = png_create_info_struct(pngPtr);
// 	if(infoPtr == 0) {
// 		png_destroy_read_struct(&pngPtr, (png_infopp)0, (png_infopp)0);
// 		fclose(file);
// 		return 0;
// 	}
// 	
// 	png_set_read_fn(pngPtr,file, file_read_function);
//    
//     //Set the amount signature bytes we've already read:
//     png_set_sig_bytes(pngPtr, 8);
// 
//     //Now call png_read_info with our pngPtr as image handle, and infoPtr to receive the file info.
//     png_read_info(pngPtr, infoPtr);
//     
//     png_uint_32 imgWidth =  png_get_image_width(pngPtr, infoPtr);
//     png_uint_32 imgHeight = png_get_image_height(pngPtr, infoPtr);
// 
//     //bits per CHANNEL! note: not per pixel!
//     png_uint_32 bitdepth   = png_get_bit_depth(pngPtr, infoPtr);
//     //Number of channels
//     png_uint_32 channels   = png_get_channels(pngPtr, infoPtr);
//     //Color type. (RGB, RGBA, Luminance, luminance alpha... palette... etc)
//     png_uint_32 color_type = png_get_color_type(pngPtr, infoPtr);
//    
//    	switch (color_type) {
//         case PNG_COLOR_TYPE_PALETTE:
//             png_set_palette_to_rgb(pngPtr);
//             //Don't forget to update the channel info (thanks Tom!)
//             //It's used later to know how big a buffer we need for the image
//             channels = 3;           
//             break;
//         case PNG_COLOR_TYPE_GRAY:
//             if (bitdepth < 8)
//             png_set_expand_gray_1_2_4_to_8(pngPtr);
//             //And the bitdepth info
//             bitdepth = 8;
//             break;
//     }
// 
//     /*if the image has a transperancy set.. convert it to a full Alpha channel..*/
//     if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS)) {
//         png_set_tRNS_to_alpha(pngPtr);
//         channels+=1;
//     }
// 
//     //We don't support 16 bit precision.. so if the image Has 16 bits per channel
//     //precision... round it down to 8.
//     if (bitdepth == 16)
//         png_set_strip_16(pngPtr);
// 	
//     //Array of row pointers. One for every row.
//     rowPtrs = new png_bytep[imgHeight];
// 
//     //Alocate a buffer with enough space.
//     data = new unsigned char[imgWidth * imgHeight * bitdepth * channels / 8];
//     
//     //This is the length in bytes, of one row.
//     const unsigned int stride = imgWidth * bitdepth * channels / 8;
// 
//     for (size_t i = 0; i < imgHeight; i++) {
//         //Set the pointer to the data pointer + i times the row stride.
//         png_uint_32 q = (imgHeight- i - 1) * stride;
//         rowPtrs[i] = (png_bytep)data + q;
//     }
// 
//     //And here it is! The actuall reading of the image!
//     png_read_image(pngPtr, rowPtrs);
// 	   
// 	//Delete the row pointers array....
//  	delete[] (png_bytep)rowPtrs;
//     //And don't forget to clean up the read and info structs !
//     png_destroy_read_struct(&pngPtr, &infoPtr,(png_infopp)0);
//     
// 	fclose(file);
// 
// 	output_width = imgWidth;
// 	output_height = imgHeight;
// 	output_format = channels;
// 	
// 	flip_y<unsigned char>((unsigned char *)data,output_width,output_height,output_format);
// 	
// 	/*** SECOND VERSION WORK ***/
// 	FILE* raw = fopen("image.raw","wb");
// 	
// 	if (raw) {
// 		fwrite(data,imgWidth * imgHeight * bitdepth * channels / 8 ,1 , raw);
// 		fclose(raw);
// 	}
// 	
// 	return 1;
// }
// 
// /************************/
// /*			JPG 		*/
// /************************/
// 
// class ImageJPG {
// 
// 	ImageJPG();
// 		
// 	public:
// 		
// 		// load image
// 		static int load(const char *name);
// 		
// 	private:
// 	
// 		struct my_error_mgr {
// 			struct jpeg_error_mgr pub;
// 			jmp_buf setjmp_buffer;
// 		};
// 
// 		typedef struct my_error_mgr * my_error_ptr;
// 
// 		static void my_error_exit(j_common_ptr cinfo);
// };
// 
// /*
//  */
// ImageJPG::ImageJPG() {
// 	
// }
// 
// 
// void ImageJPG::my_error_exit (j_common_ptr cinfo)
// {
//   my_error_ptr myerr = (my_error_ptr) cinfo->err;
//   (*cinfo->err->output_message) (cinfo);
//   longjmp(myerr->setjmp_buffer, 1);
// }
// 
// 
// int ImageJPG::load(const char * name) {
// 	printf("ImageJPG::load\n");
// 	struct jpeg_decompress_struct cinfo;
// 	struct my_error_mgr jerr;
// 
//   	FILE * infile;	
//   	int row_stride;
// 	char* data = NULL;
// 	
// 	if ((infile = fopen(name, "rb")) == NULL) {
// 		printf("ImageJPG::load(): can't open file '%s'\n",name);
//     	return 0;
//   	}
// 
//   	/* Step 1: allocate and initialize JPEG decompression object */
//   	cinfo.err = jpeg_std_error(&jerr.pub);
//   	jerr.pub.error_exit = my_error_exit;
//   	
//   	if (setjmp(jerr.setjmp_buffer)) {
//     	jpeg_destroy_decompress(&cinfo);
//     	fclose(infile);
//     	return 0;
//   	}
//   	
//   	jpeg_create_decompress(&cinfo);
// 
//   	/* Step 2: specify data source (eg, a file) */
//   	jpeg_stdio_src(&cinfo, infile);
// 
//   	/* Step 3: read file parameters with jpeg_read_header() */
// 
//   	(void) jpeg_read_header(&cinfo, TRUE);
// 	
// 	/* Step 4: set parameters for decompression */
// 	/* Step 5: Start decompressor */
// 
// 	(void) jpeg_start_decompress(&cinfo);
// 	row_stride = cinfo.output_width * cinfo.output_components;
// 
// 	output_width = cinfo.output_width;
// 	output_height = cinfo.output_height;
// 	output_format = cinfo.output_components;
// 	
//     //Alocate a buffer with enough space.
//     data = new char[output_width * output_height * output_format];
//     
// 	/* Step 6: while (scan lines remain to be read) */
//   	/*           jpeg_read_scanlines(...); */
// 	while (cinfo.output_scanline < cinfo.output_height) {
//     	(void) jpeg_read_scanlines(&cinfo, (unsigned char**)&data, 1);
// 		data += row_stride;
//   	}
// 
// 	/* Step 7: Finish decompression */
// 	(void) jpeg_finish_decompress(&cinfo);
// 
// 	/* Step 8: Release JPEG decompression object */
// 	jpeg_destroy_decompress(&cinfo);
// 
// 	fclose(infile);
// 
// 	/*** SECOND VERSION WORK ***/
// 	FILE* raw = fopen("image.raw","wb");
// 	
// 	if (raw) {
// 		fwrite(data,output_width * output_height * output_format ,1 , raw);
// 		fclose(raw);
// 	}
// 	
// 	return 1;
// }




