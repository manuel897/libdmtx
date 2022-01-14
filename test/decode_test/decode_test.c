/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2008, 2009 Mike Laughton. All rights reserved.
 * Copyright 2010-2016 Vadim A. Misbakh-Soloviov. All rights reserved.
 * Copyright 2016 Tim Zaman. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact:
 * Vadim A. Misbakh-Soloviov <dmtx@mva.name>
 * Mike Laughton <mike@dragonflylogic.com>
 *
 * \file simple_test.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <dirent.h> 
#include "../../dmtx.h"
#include "../../lodepng.h"

#define NORMAL_COLOR  "\x1B[0m"
#define GREEN  "\x1B[32m"
#define BLUE  "\x1B[34m"
#define BG_RED "\x1b[41m"
#define RED "\x1b[31m"
#define BG_GREEN "\x1b[42m"
#define UNDERSCORE "\x1b[4m"
#define BRIGHT "\x1b[1m"

DmtxMessage *decode(DmtxImage *img, int timeout_ms) {
    printf("decode()\n");
    DmtxDecode *dec;
    DmtxRegion *reg;
    DmtxMessage *msg;
    DmtxTime timeout;
    unsigned char output[] = "No codes found";

    timeout = dmtxTimeAdd(dmtxTimeNow(), timeout_ms);
    dec = dmtxDecodeCreate(img, 1);
    reg = dmtxRegionFindNext(dec, &timeout); // NULL
    if (reg != NULL) {
        printf("reg not null\n");
        msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);
        if (msg != NULL) {
            printf("msg not null\n");
            fwrite(msg->output, sizeof(unsigned char), msg->outputIdx, stdout);

            fputs("output: \"", stdout);
            fwrite(msg->output, sizeof(unsigned char), msg->outputIdx, stdout);
            fputs("\"\n", stdout);
            //dmtxMessageDestroy(&msg); 
            // if msg is destroyed, then returned pointer wíll be NULL 
            free(img);
            dmtxRegionDestroy(&reg);
            dmtxDecodeDestroy(&dec);
            return msg;
        }
    }
    else {
        free(img);
        dmtxRegionDestroy(&reg);
        dmtxDecodeDestroy(&dec); 
        return NULL; 
    }
}

void decode_images(char *path, int timeout_ms)
{
    int successCounter = 0;
    int testedCounter = 0;

    DIR *d = opendir(path); // open the path
    if (d == NULL) return; // if was not able, return
    struct dirent *dir; // for the directory entries
    while ((dir = readdir(d)) != NULL) // if we were able to read somehting from the directory
    {
        int len = strlen(dir->d_name);
        if (dir->d_type != DT_DIR && len >= 4 && strcmp(&(dir->d_name[len - 4]), ".png") == 0) {// if the type is not directory just print it with blue color
            // printf("%s%s\n", BLUE, dir->d_name);

            char reuseable_path[100];
            strcpy(reuseable_path, path);

            const char* file_name = dir->d_name;
            strcat(reuseable_path, file_name);
            printf("%s%s ", NORMAL_COLOR, reuseable_path);

            // reuseable path is full path to image 
            size_t bytesPerPixel;
            DmtxImage* img;
            DmtxDecode* dec;
            DmtxRegion* reg;
            DmtxMessage* msg;
            unsigned error;
            unsigned char* image = 0;
            unsigned w, h;
            unsigned char* png = 0;
            size_t pngsize;

            error = lodepng_load_file(&png, &pngsize, reuseable_path);

            if (!error) error = lodepng_decode32(&image, &w, &h, png, pngsize);
            if (error) printf("error %u: %s\n", error, lodepng_error_text(error));

            assert(image != NULL);
            free(png);

            clock_t start, end;
            double cpu_time_used;
            /*use image here*/

            /* start decoding process */
            start = clock();

            img = dmtxImageCreate(image, w, h, 602);
            assert(img != NULL);

            bytesPerPixel = dmtxImageGetProp(img, DmtxPropBytesPerPixel);
            printf("%s %i %dx%d size %llu bpp %llu ", NORMAL_COLOR, testedCounter, w, h, pngsize, bytesPerPixel);

            DmtxMessage *output;
            output = decode(img, 500);
            printf("%s%sOUTPUT %u\n", BRIGHT, GREEN, output);
         
            ++testedCounter;
        }
        else
            if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) // if it is a directory
            {
                printf("%s%s\n", GREEN, dir->d_name); // print its name in green
                char d_path[255]; // here I am using sprintf which is safer than strcat
                sprintf(d_path, "%s/%s", path, dir->d_name);
                // show_dir_content(d_path); // recall with the new path
            }
    }
    printf("\n %s%s%s: %i out of %i", BRIGHT, GREEN, path, successCounter, testedCounter);
    closedir(d); // finally close the directory
}

int
main(int argc, char* argv[])
{
    printf("Decode test \n");

    char sampleImagesDir[] = "D:/Manuel/SAMPLE_IMAGES/SAMPLE_IMAGES_W_LABELS_1920x1080/";
    char sampleImagesDir2[] = "D:/Manuel/SAMPLE_IMAGES/SAMPLE_IMAGES_W_LABELS_1280x720/";
    char sampleImagesDir3[] = "D:/Manuel/SAMPLE_IMAGES/SAMPLE_IMAGES_3264x2448/";
    char sampleImagesDir4[] = "D:/Manuel/SAMPLE_IMAGES/SAMPLE_IMAGES_1440x1080/";
    char sampleImagesDir5[] = "D:/Manuel/SAMPLE_IMAGES/SAMPLE_IMAGES_960x720/";
    char imagePath[] = "D:/Manuel/SAMPLE_IMAGES/SAMPLE_IMAGES_960x720/IMG_0187.png";
    char imagePath2[] = "D:/Manuel/SAMPLE_IMAGES/datamatrix.png";

    char imagePath3[] = "D:/Manuel/SAMPLE_IMAGES/SAMPLE_IMAGES_W_LABELS_1280x720/1623480558__True.png";

    
    /*printf("%s\n", NORMAL_COLOR);
    int timeout_ms = 1000;
    decode_images(sampleImagesDir, timeout_ms);
    printf("%s\n", NORMAL_COLOR);*/
 
    unsigned error;
    unsigned char *png = 0;
    unsigned char *img = 0;
    unsigned width, height;
    size_t pngsize;
    error = lodepng_load_file(&png, &pngsize, imagePath3);

    if (!error) error = lodepng_decode32(&img, &width, &height, png, pngsize);
    if (error) printf("error %u: %s\n", error, lodepng_error_text(error));

    assert(img != NULL);
    free(png);

    size_t bytesPerPixel;
    DmtxImage *dmtxImg;
    DmtxDecode *dec;
    DmtxRegion *reg;

    dmtxImg = dmtxImageCreate(img, width, height, 602);
    assert(dmtxImg != NULL);
    bytesPerPixel = dmtxImageGetProp(dmtxImg, DmtxPropBytesPerPixel);
    DmtxMessage *resultMsg;
    resultMsg = decode(dmtxImg, 1000);
    if (resultMsg == NULL)
    {
        printf("%s\n", NORMAL_COLOR);
        printf("%s NO REGION FOUND", RED);
        printf("%s\n", NORMAL_COLOR);
    }
    else {
        printf("%s\n", GREEN);
        printf("result: %p", resultMsg);
        fwrite(resultMsg->output, sizeof(unsigned char), resultMsg->outputIdx, stdout);
        printf("%s\n", NORMAL_COLOR);
    }
   
    exit(0);
}
