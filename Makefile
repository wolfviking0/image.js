EMSCRIPTEN = ~/Desktop/DRIVE/GoogleDrive/Workspace/WebGL/Emscripten
CXX = $(EMSCRIPTEN)/emcc

# Version 1.2.7
LIBZSRCC = 	libz/adler32.c\
			libz/compress.c\
			libz/crc32.c\
			libz/deflate.c\
			libz/infback.c\
			libz/inffast.c\
			libz/inflate.c\
			libz/inftrees.c\
			libz/trees.c\
			libz/uncompr.c\
			libz/zutil.c
			
# Version 1.5.2
PNGSRCC = 	libpng/png.c\
			libpng/pngerror.c\
			libpng/pngget.c\
			libpng/pngmem.c\
			libpng/pngpread.c\
			libpng/pngread.c\
			libpng/pngrio.c\
			libpng/pngrtran.c\
			libpng/pngrutil.c\
			libpng/pngset.c\
			libpng/pngtrans.c\
			libpng/pngwio.c\
			libpng/pngwrite.c\
			libpng/pngwtran.c\
			libpng/pngwutil.c
			
# Version 6b
JPGSRCC =  	libjpeg/jcomapi.c\
			libjpeg/jutils.c\
			libjpeg/jerror.c\
			libjpeg/jmemmgr.c\
			libjpeg/jmemnobs.c\
			libjpeg/jcapimin.c\
			libjpeg/jcapistd.c\
			libjpeg/jctrans.c\
			libjpeg/jcparam.c\
			libjpeg/jdatadst.c\
			libjpeg/jcinit.c\
			libjpeg/jcmaster.c\
			libjpeg/jcmarker.c\
			libjpeg/jcmainct.c\
			libjpeg/jcprepct.c\
			libjpeg/jccoefct.c\
			libjpeg/jccolor.c\
			libjpeg/jcsample.c\
			libjpeg/jchuff.c\
			libjpeg/jcphuff.c\
			libjpeg/jcdctmgr.c\
			libjpeg/jfdctfst.c\
			libjpeg/jfdctflt.c\
			libjpeg/jfdctint.c\
			libjpeg/jdapimin.c\
			libjpeg/jdapistd.c\
			libjpeg/jdtrans.c\
			libjpeg/jdatasrc.c\
			libjpeg/jdmaster.c\
			libjpeg/jdinput.c\
			libjpeg/jdmarker.c\
			libjpeg/jdhuff.c\
			libjpeg/jdphuff.c\
			libjpeg/jdmainct.c\
			libjpeg/jdcoefct.c\
			libjpeg/jdpostct.c\
			libjpeg/jddctmgr.c\
			libjpeg/jidctfst.c\
			libjpeg/jidctflt.c\
			libjpeg/jidctint.c\
			libjpeg/jidctred.c\
			libjpeg/jdsample.c\
			libjpeg/jdcolor.c\
			libjpeg/jquant1.c\
			libjpeg/jquant2.c\
			libjpeg/jdmerge.c
            

# Version 0.2.1    
WEBPSRCC = 	libwebp/src/dec/alpha.c\
          	libwebp/src/dec/buffer.c\
          	libwebp/src/dec/idec.c\
          	libwebp/src/dec/io.c\
          	libwebp/src/dec/tree.c\
          	libwebp/src/dec/webp.c\
            libwebp/src/dec/vp8.c\
            libwebp/src/dec/vp8l.c\
            libwebp/src/dec/quant.c\
            libwebp/src/dec/frame.c\
            libwebp/src/dsp/yuv.c\
            libwebp/src/dsp/cpu.c\
            libwebp/src/dsp/upsampling.c\
            libwebp/src/dsp/dec.c\
            libwebp/src/dsp/lossless.c\
            libwebp/src/utils/rescaler.c\
            libwebp/src/utils/quant_levels.c\
            libwebp/src/utils/huffman.c\
          	libwebp/src/utils/thread.c\
         	libwebp/src/utils/filters.c\
          	libwebp/src/utils/color_cache.c\
          	libwebp/src/utils/bit_reader.c\
   	      	libwebp/src/utils/utils.c\
          	
MAINSRCC = 	main.cpp\
            ImageFormat.cpp

INCLUDES = \
	-I./\
	-I./libjpeg\
	-I./libpng\
	-I./libz\
	-I./libwebp/src\
	-I./libwebp/src\utils\
	-I$(EMSCRIPTEN)/system/include\
	
DEFINES = \
	-DEMSCRIPTEN\
	-DNO_GETENV\
	-DNDEBUG
	
all: 
	$(CXX) $(LIBZSRCC) $(JPGSRCC) $(PNGSRCC) $(WEBPSRCC) $(INCLUDES) $(DEFINES) -o externs.js.o
	$(CXX) $(MAINSRCC) $(INCLUDES) $(DEFINES) -o image.js.o
	EMCC_DEBUG=1 $(CXX) -O2 *.o --clean_cache -s EXPORTED_FUNCTIONS="['_main', '_malloc', '_free', '_width', '_height', '_format']" -o libimage.raw.js --pre-js pre.js -s ALLOW_MEMORY_GROWTH=1
	rm -f *.o
