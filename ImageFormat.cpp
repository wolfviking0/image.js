//
//  ImageFormat.cpp
//
//
//  Created by Anthony Liot on 17/12/12.
//
//

#include <ImageFormat.h>
#include <setjmp.h>

#define kTempararyName "image.raw"

/******************************************************************************\
 *
 * ImageFormatPNG
 *
 \******************************************************************************/

#include <png.h>

/*
 */
class ImageFormatPNG {
    
    ImageFormatPNG();
    
public:
    
    // load image
    static int load(const char *name, int& width, int& height, int& format);

private:
    
    static void file_read_function(png_structp png_ptr,png_bytep ptr,png_size_t size);
};

/*
 */
ImageFormatPNG::ImageFormatPNG() {
	
}

/*
 */
void ImageFormatPNG::file_read_function(png_structp png_ptr,png_bytep ptr,png_size_t size) {
    FILE *file = (FILE*)png_get_io_ptr(png_ptr);
    fread(ptr,sizeof(png_byte),size,file);
}

/*
 */
int ImageFormatPNG::load(const char *name, int& width, int& height, int& format) {
 	printf("ImageFormatPNG::load\n");

    // Open File
 	FILE* file = fopen(name,"rb");
 	if (file == 0) {
        printf("ImageFormatPNG::load(): can't open file '%s'\n",name);
 		return 0;
 	}

    // init width / height / format value to 0
    width  = 0;
 	height = 0;
    format = 0;
    
 	//Allocate a buffer of 8 bytes, where we can put the file signature.
    png_byte sig[8];

    //Read the 8 bytes from the file into the sig buffer.
    fread((char*)sig, sizeof(png_byte), 8, file);
 	if(!png_check_sig(sig,8)) {
 		fclose(file);
 		return 0;
 	}

 	//Here we create the png read struct.
 	png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
 	if(pngPtr == 0) {
 		fclose(file);
 		return 0;
 	}

 	//Here we create the png info struct.
 	png_infop infoPtr = png_create_info_struct(pngPtr);
 	if(infoPtr == 0) {
 		png_destroy_read_struct(&pngPtr, (png_infopp)0, (png_infopp)0);
 		fclose(file);
 		return 0;
 	}

 	png_set_read_fn(pngPtr,file, file_read_function);

    //Set the amount signature bytes we've already read:
    png_set_sig_bytes(pngPtr, 8);

    //Now call png_read_info with our pngPtr as image handle, and infoPtr to receive the file info.
    png_read_info(pngPtr, infoPtr);

    png_uint_32 imgWidth =  png_get_image_width(pngPtr, infoPtr);
    png_uint_32 imgHeight = png_get_image_height(pngPtr, infoPtr);

    //bits per CHANNEL! note: not per pixel!
    png_uint_32 bitdepth   = png_get_bit_depth(pngPtr, infoPtr);
    //Number of channels
    png_uint_32 channels   = png_get_channels(pngPtr, infoPtr);
    //Color type. (RGB, RGBA, Luminance, luminance alpha... palette... etc)
    png_uint_32 color_type = png_get_color_type(pngPtr, infoPtr);

    switch (color_type) {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(pngPtr);
            //Don't forget to update the channel info (thanks Tom!)
            //It's used later to know how big a buffer we need for the image
            channels = 3;
            break;
        case PNG_COLOR_TYPE_GRAY:
            if (bitdepth < 8)
                png_set_expand_gray_1_2_4_to_8(pngPtr);
            //And the bitdepth info
            bitdepth = 8;
        break;
    }

    /*if the image has a transperancy set.. convert it to a full Alpha channel..*/
    if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(pngPtr);
        channels+=1;
    }

    //We don't support 16 bit precision.. so if the image Has 16 bits per channel
    //precision... round it down to 8.
    if (bitdepth == 16)
        png_set_strip_16(pngPtr);

    //Array of row pointers. One for every row.
    png_bytep*  rowPtrs = new png_bytep[imgHeight];

    width = imgWidth;
    height = imgHeight;
    format = bitdepth * channels / 8;
    
    int data_decoded_size = width * height * format;
    
    // raw data image container
    unsigned char * img_data_decoded = new unsigned char[data_decoded_size];
    memset(img_data_decoded,0x00,data_decoded_size + 1);
    
	int stride = width * format;
    
    for (size_t i = 0; i < imgHeight; i++) {
        //Set the pointer to the data pointer + i times the row stride.
        png_uint_32 q = (imgHeight- i - 1) * stride;
        rowPtrs[i] = (png_bytep)img_data_decoded + q;
    }

    //And here it is! The actuall reading of the image!
    png_read_image(pngPtr, rowPtrs);

    //Delete the row pointers array....
    delete[] (png_bytep)rowPtrs;
    //And don't forget to clean up the read and info structs !
    png_destroy_read_struct(&pngPtr, &infoPtr,(png_infopp)0);

    // create temporary file for glue code javascript
    ImageFormat::flip_y((unsigned char *)img_data_decoded,width,height,format);
    
    FILE* raw = fopen(kTempararyName,"wb");
    if (raw) {
        fwrite(img_data_decoded,width * height * format ,1 , raw);
        fclose(raw);
    }
     
    delete img_data_decoded;
    
    fclose(file);
    
	return 1;
}


/******************************************************************************\
 *
 * ImageFormatJPG
 *
 \******************************************************************************/

extern "C" {
    #include <jpeglib.h>
    #include <jerror.h>
}

/*
 */
class ImageFormatJPG {
    
    ImageFormatJPG();
    
public:
    
    // load image
    static int load(const char *name, int& width, int& height, int& format);
    
private:
    
    struct ErrorData {
        jpeg_error_mgr pub;
        jmp_buf setjmp_buffer;
    };
    
    struct FileSource {
        jpeg_source_mgr pub;
        JOCTET buffer[4096];
        FILE *file;
    };
    
    static void error_exit(j_common_ptr common);
    static void output_message(j_common_ptr common);
    
    static void init_source(j_decompress_ptr decompress);
    static void term_source(j_decompress_ptr decompress);
    
    static boolean file_fill_input_buffer(j_decompress_ptr decompress);
    static void file_skip_input_data(j_decompress_ptr decompress,long num_bytes);
};

/*
 */
ImageFormatJPG::ImageFormatJPG() {
	
}

/*
 */
void ImageFormatJPG::error_exit(j_common_ptr common) {
	ErrorData *data = (ErrorData*)common->err;
	(*common->err->output_message)(common);
	longjmp(data->setjmp_buffer,1);
}

void ImageFormatJPG::output_message(j_common_ptr common) {
	char buffer[JMSG_LENGTH_MAX];
	(*common->err->format_message)(common,buffer);
	printf("ImageFormatJPG::output_message(): %s\n",buffer);
}

/*
 */
void ImageFormatJPG::init_source(j_decompress_ptr decompress) {
	
}

void ImageFormatJPG::term_source(j_decompress_ptr decompress) {
	
}

/*
 */
boolean ImageFormatJPG::file_fill_input_buffer(j_decompress_ptr decompress) {
	FileSource *source = (FileSource*)decompress->src;
	size_t bytes = fread(source->buffer,sizeof(char),sizeof(source->buffer),source->file);
	if(bytes <= 0) {
		if(ftell(source->file) == 0) ERREXIT(decompress,JERR_INPUT_EMPTY);
		WARNMS(decompress,JWRN_JPEG_EOF);
		source->buffer[0] = (JOCTET)0xff;
		source->buffer[1] = (JOCTET)JPEG_EOI;
		bytes = 2;
	}
	source->pub.next_input_byte = source->buffer;
	source->pub.bytes_in_buffer = bytes;
	return true;
}

void ImageFormatJPG::file_skip_input_data(j_decompress_ptr decompress,long num_bytes) {
	FileSource *source = (FileSource*)decompress->src;
	if(num_bytes > 0) {
		while(num_bytes > (long)source->pub.bytes_in_buffer) {
			num_bytes -= (long)source->pub.bytes_in_buffer;
			file_fill_input_buffer(decompress);
		}
		source->pub.next_input_byte += num_bytes;
		source->pub.bytes_in_buffer -= num_bytes;
	}
}

/*
 */
int ImageFormatJPG::load(const char *name, int& width, int& height, int& format) {
 	printf("ImageFormatJPG::load\n");
    
    // Open File
 	FILE* file = fopen(name,"rb");
 	if (file == 0) {
        printf("ImageFormatJPG::load(): can't open file '%s'\n",name);
 		return 0;
 	}
    
    // init width / height / format value to 0
    width  = 0;
 	height = 0;
    format = 0;
    
    // create jpeg struct decompress
	ErrorData error;
	jpeg_decompress_struct decompress;
	decompress.err = jpeg_std_error(&error.pub);
	error.pub.error_exit = error_exit;
	error.pub.output_message = output_message;
	if(setjmp(error.setjmp_buffer)) {
		printf("ImageFormatJPG::load(): can't load \"%s\" file\n",name);
		jpeg_destroy_decompress(&decompress);
		fclose(file);
		return 0;
	}
	
	jpeg_create_decompress(&decompress);
	   
    decompress.src = (jpeg_source_mgr*)(*decompress.mem->alloc_small)((j_common_ptr)&decompress,JPOOL_PERMANENT,sizeof(FileSource));
	FileSource *source = (FileSource*)decompress.src;
	source->pub.init_source = init_source;
	source->pub.fill_input_buffer = file_fill_input_buffer;
	source->pub.skip_input_data = file_skip_input_data;
	source->pub.resync_to_restart = jpeg_resync_to_restart;
	source->pub.term_source = term_source;
	source->pub.bytes_in_buffer = 0;
	source->pub.next_input_byte = NULL;
	source->file = file;
	
	jpeg_read_header(&decompress,true);
	jpeg_start_decompress(&decompress);

    width = decompress.output_width;
    height = decompress.output_height;
    format = decompress.output_components;

    
    int data_decoded_size = width * height * format;
    
    // raw data image container
    unsigned char * img_data_decoded = new unsigned char[data_decoded_size];
    memset(img_data_decoded,0x00,data_decoded_size + 1);
    
	int stride = decompress.output_width * decompress.output_components;
    
    while (decompress.output_scanline < decompress.output_height) {
        unsigned char *rowp[1];
        rowp[0] = (unsigned char *) img_data_decoded + stride * decompress.output_scanline;
        jpeg_read_scanlines(&decompress, rowp, 1);
    }
	
	jpeg_finish_decompress(&decompress);
	jpeg_destroy_decompress(&decompress);
	   
    // create temporary file for glue code javascript
    FILE* raw = fopen(kTempararyName,"wb");
    if (raw) {
        fwrite(img_data_decoded,data_decoded_size ,1 , raw);
        fclose(raw);
    }

    //delete img_data_decoded;// give a crash !!!
	fclose(file);
    
	return 1;
}


/******************************************************************************\
 *
 * ImageFormatWEBP
 *
 \******************************************************************************/

#include <webp/decode.h>

/*
 */
class ImageFormatWEBP {
    
    ImageFormatWEBP();
    
public:
    
    // load image
    static int load(const char *name, int& width, int& height, int& format);

};

/*
 */
ImageFormatWEBP::ImageFormatWEBP() {
	
}

/*
 */
int ImageFormatWEBP::load(const char *name, int& width, int& height, int& format) {
 	printf("ImageFormatWEBP::load\n");
    
    // Open File
 	FILE* file = fopen(name,"rb");
 	if (file == 0) {
        printf("ImageFormatWEBP::load(): can't open file '%s'\n",name);
 		return 0;
 	}
    
    // init width / height / format value to 0
    width  = 0;
 	height = 0;
    format = 0;
    
    // size of file
    fseek(file,0,SEEK_END);
 	const long data_size = ftell(file);
 	fseek(file,0,SEEK_SET);
    
    unsigned char * img_data = new unsigned char[data_size];
    fread(img_data,data_size,1,file);
    fclose(file);
    
    // Get header info
    int webpinfo = WebPGetInfo(img_data, data_size, &width, &height);
    if (webpinfo == 0) {
        printf("ImageFormatWEBP::load(): wrong header in \"%s\" file\n",name);
 		delete img_data;
 		return 0;
    }

    // Get betstream
    WebPBitstreamFeatures features;
    VP8StatusCode status = WebPGetFeatures(img_data, data_size, &features);

    if (status != VP8_STATUS_OK) {
        printf("ImageFormatWEBP::load(): bad bitstream in \"%s\" file, error:\"%d\"\n",name,status);
 		delete img_data;
 		return 0;
    }

    format = features.has_alpha ? 4 : 3;
    
    int data_decoded_size = width * height * format;

    // raw data image container
    unsigned char * img_data_decoded = new unsigned char[data_decoded_size];
    memset(img_data_decoded,0x00,data_decoded_size);

    // decode image inside container (RGB and RGBA)
    if (format == 3 ) {
        if ( WebPDecodeRGBInto(img_data, data_size, img_data_decoded, data_decoded_size, width * format) == NULL ) {
            printf("ImageFormatWEBP::load(): can't decode RGB \"%s\" file\n",name);
            delete img_data;
            delete img_data_decoded;
            return 0;
        }
    } else {
        if ( WebPDecodeRGBAInto(img_data, data_size, img_data_decoded, data_decoded_size, width * format) == NULL ) {
            printf("ImageFormatWEBP::load(): can't decode RGBA \"%s\" file\n",name);
            delete img_data;
            delete img_data_decoded;
           return 0;
        }
    }
    
    // create temporary file for glue code javascript
    FILE* raw = fopen(kTempararyName,"wb");
    if (raw) {
        fwrite(img_data_decoded,data_decoded_size ,1 , raw);
        fclose(raw);
    }
    
    delete img_data;
    delete img_data_decoded;
    
	return 1;
}

/************************/
/*			TGA 		*/
/************************/

class ImageFormatTGA {

    ImageFormatTGA();

    public:

        // load image
        static int load(const char *name, int& width, int& height, int& format);

    private:

        static void read(unsigned char *data,FILE* file,size_t size,int pixel_size,int compression);

        struct Header {
            unsigned char id_length;
            unsigned char colormap_type;
            unsigned char image_type;
            unsigned short colormap_index;
            unsigned short colormap_length;
            unsigned char colormap_size;
            unsigned short x_orign;
            unsigned short y_orign;
            unsigned short width;
            unsigned short height;
            unsigned char pixel_size;
            unsigned char attributes;
        };

};

/*
 */
ImageFormatTGA::ImageFormatTGA() {

}

/*
 */
void ImageFormatTGA::read(unsigned char *data,FILE* file,size_t size,int pixel_size,int compression) {

    if(compression == 0) {
        fread(data,pixel_size,size,file);
    } else {
        unsigned char *d = data;
        for(size_t i = 0; i < size;) {
            unsigned char rep = 0;
            if(fread(&rep,sizeof(unsigned char),1,file) != 1) break;
            if(rep & 0x80) {
                rep ^= 0x80;
                fread(d,pixel_size,1,file);
                d += pixel_size;
                for(int j = 0; j < rep * pixel_size; j++) {
                    *d = *(d - pixel_size);
                    d++;
                }
            } else {
                fread(d,pixel_size,rep + 1,file);
                d += pixel_size * (rep + 1);
            }
            i += rep + 1;
        }
    }
}

int ImageFormatTGA::load(const char * name, int& width, int& height, int& format) {
    printf("ImageFormatTGA::load\n");

    char* data = NULL;

    FILE* file = fopen(name,"rb");
    if (file == 0) {
        printf("ImageFormatTGA::load(): can't open file '%s'\n",name);
        return 0;
    }

    // read header
    Header header;

    unsigned char ret_uchar;
    unsigned short ret_ushort;

    fread(&ret_uchar,sizeof(unsigned char),1,file);
    header.id_length = ret_uchar;
    fread(&ret_uchar,sizeof(unsigned char),1,file);
    header.colormap_type = ret_uchar;
    fread(&ret_uchar,sizeof(unsigned char),1,file);
    header.image_type = ret_uchar;
    fread(&ret_ushort,sizeof(unsigned short),1,file);
    header.colormap_index = ret_ushort;
    fread(&ret_ushort,sizeof(unsigned short),1,file);
    header.colormap_length = ret_ushort;
    fread(&ret_uchar,sizeof(unsigned char),1,file);
    header.colormap_size = ret_uchar;
    fread(&ret_ushort,sizeof(unsigned short),1,file);
    header.x_orign = ret_ushort;
    fread(&ret_ushort,sizeof(unsigned short),1,file);
    header.y_orign = ret_ushort;
    fread(&ret_ushort,sizeof(unsigned short),1,file);
    header.width = ret_ushort;
    fread(&ret_ushort,sizeof(unsigned short),1,file);
    header.height = ret_ushort;
    fread(&ret_uchar,sizeof(unsigned char),1,file);
    header.pixel_size = ret_uchar;
    fread(&ret_uchar,sizeof(unsigned char),1,file);
    header.attributes = ret_uchar;

    fseek(file,header.id_length,SEEK_CUR);

    width = header.width;
    height = header.height;
    format = header.pixel_size / 8;

    int data_decoded_size = width * height * format;
    
    // raw data image container
    unsigned char * img_data_decoded = new unsigned char[data_decoded_size];
    memset(img_data_decoded,0x00,data_decoded_size);

    size_t size = header.width * header.height;
    int pixel_size = header.pixel_size / 8;

    read((unsigned char *)img_data_decoded,file,size,pixel_size,(header.image_type == 10));

    ImageFormat::swap((unsigned char *)img_data_decoded,data_decoded_size ,format,0,2);

    // flip image vertically
    if((header.attributes & 0x20) == 0) {
        ImageFormat::flip_y((unsigned char *)img_data_decoded,header.width,header.height,format);
    }

    //close file
    fclose(file);

    /*** SECOND VERSION WORK ***/
    FILE* raw = fopen("image.raw","wb");

    if (raw) {
        fwrite(img_data_decoded,data_decoded_size ,1 , raw);
        fclose(raw);
    }

    delete img_data_decoded;

    return 1;
}


/******************************************************************************\
 *
 * ImageFormat
 *
 \******************************************************************************/

/*
 */
ImageFormat::~ImageFormat() {
	
}

/*
 */
int ImageFormat::load(const char *name, int& width, int& height, int& format) {
	
    if(strstr(name,".jpg") || strstr(name,".JPG")) {
        return ImageFormatJPG::load(name,width,height,format);
    }

    if(strstr(name,".png") || strstr(name,".PNG")) {
        return ImageFormatPNG::load(name,width,height,format);
    }
    
    if(strstr(name,".webp") || strstr(name,".WEB¨")) {
        return ImageFormatWEBP::load(name,width,height,format);
    }
    
    if(strstr(name,".tga") || strstr(name,".TGA¨")) {
        return ImageFormatTGA::load(name,width,height,format);
    }
    
	printf("ImageFormat::load(): unknown format of \"%s\" file\n",name);
	return 0;
}


int ImageFormat::swap(unsigned char *dest,size_t size,int components,int channel_0,int channel_1) {
 	unsigned char *d = (unsigned char *)dest;
 	for(size_t i = 0; i < size; i += components) {
 		unsigned char c = d[channel_0];
 		d[channel_0] = d[channel_1];
 		d[channel_1] = c;
 		d += components;
 	}
 	return 1;
}


void ImageFormat::flip_y(unsigned char *dest,int width,int height,int components) {
    for(int y = 0; y < height / 2; y++) {
        unsigned char *s = &((unsigned char *)dest)[width * y * components];
 		unsigned char *d = &((unsigned char *)dest)[width * (height - y - 1) * components];
 		for(int x = 0; x < width * components; x++) {
 			unsigned char c = *d;
 			*d++ = *s;
 			*s++ = c;
 		}
 	}
 }