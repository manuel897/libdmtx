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

unsigned char decode(DmtxImage* img, int timeout_ms) {
    DmtxDecode* dec;
    DmtxRegion* reg;
    DmtxMessage* msg;
    DmtxTime timeout;
    unsigned char* output;

    timeout = dmtxTimeAdd(dmtxTimeNow(), timeout_ms);

    dec = dmtxDecodeCreate(img, 1);

    reg = dmtxRegionFindNext(dec, &timeout); // NULL

    if (reg != NULL) {
        // msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);
        // output = msg->output;
        // dmtxMessageDestroy(&msg);

        msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);
        if (msg != NULL) {
            // fwrite(msg->output, sizeof(unsigned char), msg->outputIdx, stdout);
            // fputc('\n', stdout);
            output = msg->output;
            fputs("output: \"", stdout);
            fwrite(output, sizeof(unsigned char), msg->outputIdx, stdout);
            fputs("\"\n", stdout);
            printf("!!!!#####: %c", &output);
            dmtxMessageDestroy(&msg);
        }
        dmtxRegionDestroy(&reg);
    }
    else {
        output = "";
    }
    dmtxRegionDestroy(&reg);
    dmtxDecodeDestroy(&dec);
    return output;
}

void decode_images(char* path, int timeout_ms)
{
    int successCounter = 0;
    int testedCounter = 0;

    DIR* d = opendir(path); // open the path
    if (d == NULL) return; // if was not able, return
    struct dirent* dir; // for the directory entries
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

            unsigned char* output;
            output = decode(img, 500);
            printf("%s%sOUTPUT %u\n", BRIGHT, GREEN, output);
            ///** --------
            //dec = dmtxDecodeCreate(img, 1);
            //assert(dec != NULL);

            //DmtxTime timeout;
            //timeout = dmtxTimeAdd(dmtxTimeNow(), timeout_ms);
            //reg = dmtxRegionFindNext(dec, &timeout); // NULL or &timeout
            //if (reg != NULL) {
            //    printf("%s", GREEN);

            //    msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);
            //    // printf(msg);
            //  /*  fprintf(stdout, "msg->arraySize :  \"%zd\"\n", msg->arraySize);
            //    fprintf(stdout, "msg->codeSize  :  \"%zd\"\n", msg->codeSize);
            //    fprintf(stdout, "msg->outputSize:  \"%zd\"\n", msg->outputSize);*/
            //    // int oned = sqrt(msg->arraySize);
            //  /*  for (int i = 0; i < msg->arraySize; i++) {
            //        fprintf(stdout, " %c.", msg->array[i]);
            //        if (i % oned == oned - 1) {
            //            fprintf(stdout, "\n");
            //        }
            //    }*/
            //    // fprintf(stdout, "\n\n");
            //   /* for (int j = 0; j < msg->codeSize; j++) {
            //        fprintf(stdout, " %c.", msg->code[j]);
            //    }*/
            //    // fprintf(stdout, "\n\n");
            //    /*for (int k = 0; k < msg->outputSize; k++) {
            //        fprintf(stdout, " %c.", msg->output[k]);
            //    }*/
            //    // fprintf(stdout, "\n\n");

            //    if (msg != NULL) {
            //        end = clock();
            //        cpu_time_used = ((double)(end - start));
            //        printf("DECODE TIME %s%f ", BRIGHT, cpu_time_used);

            //        fputs("output: \"", stdout);
            //        fwrite(msg->output, sizeof(unsigned char), msg->outputIdx, stdout);
            //        fputs("\"\n", stdout);
            //        dmtxMessageDestroy(&msg);
            //        ++successCounter;
            //        // free(msg);
            //    }
            //    dmtxRegionDestroy(&reg);
            //}
            //else {
            //    end = clock();
            //    cpu_time_used = ((double)(end - start));
            //    printf("DECODE TIME %s%f ", BRIGHT, cpu_time_used);
            //    char notFoundMsg[20] = "No region found";
            //    // Escape is : \033
            //    printf("%s%s\n",RED, notFoundMsg);
            //}
            //dmtxDecodeDestroy(&dec);
            //dmtxImageDestroy(&img);
            //free(image);

            //------ */
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

    size_t          width, height, bytesPerPixel;
    unsigned char   str[] = "30Q324343430794<OQQ";
    unsigned char* pxl;
    DmtxEncode* enc;
    DmtxImage* img;
    DmtxDecode* dec;
    DmtxRegion* reg;
    DmtxMessage* msg;

    fprintf(stdout, "input:  \"%s\"\n", str);

    /* 1) ENCODE a new Data Matrix barcode image (in memory only) */

    enc = dmtxEncodeCreate();

    /*
     dmtxEncodeSetProp( enc, DmtxPropPixelPacking, DmtxPack16bppRGB );
     dmtxEncodeSetProp( enc, DmtxPropPixelPacking, DmtxPack32bppRGB );
     dmtxEncodeSetProp( enc, DmtxPropWidth, 160 );
     dmtxEncodeSetProp( enc, DmtxPropHeight, 160 );
    */

    assert(enc != NULL);
    dmtxEncodeDataMatrix(enc, strlen((const char*)str), str);

    /* 2) COPY the new image data before releasing encoding memory */

    width = dmtxImageGetProp(enc->image, DmtxPropWidth);
    height = dmtxImageGetProp(enc->image, DmtxPropHeight);
    bytesPerPixel = dmtxImageGetProp(enc->image, DmtxPropBytesPerPixel);

    pxl = (unsigned char*)malloc(width * height * bytesPerPixel);
    assert(pxl != NULL);
    memcpy(pxl, enc->image->pxl, width * height * bytesPerPixel);

    dmtxEncodeDestroy(&enc);

    fprintf(stdout, "width:  \"%zd\"\n", width);
    fprintf(stdout, "height: \"%zd\"\n", height);
    fprintf(stdout, "bpp:    \"%zd\"\n", bytesPerPixel);

    for (int i = 0; i < width * height; i++) {
        fprintf(stdout, "%d", (pxl[i * 3]) == 0);
        if (i % width == width - 1) {
            fprintf(stdout, "\n");
        }
    }

    /* 3) DECODE the Data Matrix barcode from the copied image */

    img = dmtxImageCreate(pxl, width, height, DmtxPack24bppRGB);
    assert(img != NULL);

    dec = dmtxDecodeCreate(img, 1);
    assert(dec != NULL);

    reg = dmtxRegionFindNext(dec, NULL);
    if (reg != NULL) {
        msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);

        fprintf(stdout, "msg->arraySize :  \"%zd\"\n", msg->arraySize);
        fprintf(stdout, "msg->codeSize  :  \"%zd\"\n", msg->codeSize);
        fprintf(stdout, "msg->outputSize:  \"%zd\"\n", msg->outputSize);
        int oned = sqrt(msg->arraySize);
        for (int i = 0; i < msg->arraySize; i++) {
            fprintf(stdout, " %c.", msg->array[i]);
            if (i % oned == oned - 1) {
                fprintf(stdout, "\n");
            }
        }
        fprintf(stdout, "\n\n");
        for (int j = 0; j < msg->codeSize; j++) {
            fprintf(stdout, " %c.", msg->code[j]);
        }
        fprintf(stdout, "\n\n");
        for (int k = 0; k < msg->outputSize; k++) {
            fprintf(stdout, " %c.", msg->output[k]);
        }
        fprintf(stdout, "\n\n");

        if (msg != NULL) {
            fputs("output: \"", stdout);
            fwrite(msg->output, sizeof(unsigned char), msg->outputIdx, stdout);
            fputs("\"\n", stdout);
            dmtxMessageDestroy(&msg);
        }
        dmtxRegionDestroy(&reg);
    }

    dmtxDecodeDestroy(&dec);
    dmtxImageDestroy(&img);
    free(pxl);

    fprintf(stdout, "%d\n", getSizeIdxFromSymbolDimension(12, 12));

    char sampleImagesDir[] = "D:/Manuel/SAMPLE_IMAGES/SAMPLE_IMAGES_W_LABELS_1920x1080/";
    char sampleImagesDir2[] = "D:/Manuel/SAMPLE_IMAGES/SAMPLE_IMAGES_W_LABELS_1280x720/";
    char sampleImagesDir3[] = "D:/Manuel/SAMPLE_IMAGES/SAMPLE_IMAGES_3264x2448/";
    char sampleImagesDir4[] = "D:/Manuel/SAMPLE_IMAGES/SAMPLE_IMAGES_1440x1080/";
    char sampleImagesDir5[] = "D:/Manuel/SAMPLE_IMAGES/SAMPLE_IMAGES_960x720/";


    printf("%s\n", NORMAL_COLOR);
    int timeout_ms = 1000;
    decode_images(sampleImagesDir, timeout_ms);
    printf("%s\n", NORMAL_COLOR);

    exit(0);
}
