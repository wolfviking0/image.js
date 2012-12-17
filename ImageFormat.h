//
//  ImageFormat.h
//
//
//  Created by Anthony Liot on 17/12/12.
//
//

#ifndef __IMAGE_FORMAT_H__
#define __IMAGE_FORMAT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string>

/*
 */
class ImageFormat {
    
public:
    
    virtual ~ImageFormat();
    
    // load image
    static int load(const char *name, int& witdh, int& height, int& format);
    
    static int swap(unsigned char *dest,size_t size,int components,int channel_0,int channel_1);
    
    static void flip_y(unsigned char *dest,int width,int height,int components);
};

#endif /* __IMAGE_FORMAT_H__ */
