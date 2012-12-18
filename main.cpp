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
