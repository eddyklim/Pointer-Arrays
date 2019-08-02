/*
 * eduardk_hw4.c
 *
 *  Created on: Feb 3, 2018
 *  Author: Eduard Klimenko
 *
 *  This program takes in two images and produces 2 new images.
 *  One image is a blend or mixture of the two input images,
 *  the other is an 8x8 checker board. Input files must be in
 *  .bmp extension and pixel width/height must be multiples of 8.
 *  This program unlike assignment 2, uses no explicit arrays or
 *  indexing.
 */

/* Headers. */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define HEADER_LENGTH 54

/* Prototypes. */
void processFileData(FILE *f, unsigned long filesize, FILE *f2);
unsigned long get_file_size(char *s);
void processImages(void *data, void *data2, unsigned long filesize);
void blendImages(void *data, void *data2, unsigned long filesize);
void checkerImages(void *data, void *data2, unsigned long filesize, int* width,
        int* height);

/*
 * This is the main function. Accepts command line arguments of two valid file
 * names. No other input will be accepted. Checks if they are not empty and if
 * they are bmp files. The file size is also obtained here.
 */
int main(int argc, char **argv) {
    /* Eclipse compatibility for this program. */
    setvbuf(stdout, NULL, _IONBF, 0);
    FILE *f;
    FILE *f2;
    int goForIt = 0;
    int goForIt2 = 0;

    unsigned long filesize;
    unsigned long filesize2;
    if (argc == 3) {
        if ((goForIt = (long) (filesize = get_file_size(*(argv + 1))) >= 0)
                && (goForIt2 = (long) (filesize2 = get_file_size(*(argv + 1)))
                        >= 0)) {
            f = fopen(*(argv + 1), "rb");
            f2 = fopen(*(argv + 2), "rb");
        } else
            printf("%s or %s cannot be accessed!\n", *(argv + 1), *(argv + 2));
    }
    if (goForIt && goForIt2) {
        processFileData(f, filesize, f2);
    }
    fclose(f);
    fclose(f2);
    return 0;
}

/*
 * This function takes in two files and their file size. Allocates
 * enough memory to fit in all data from the two files. Checks the
 * first two bytes to see if the files are in bmp format. If they
 * are, process the files.
 */
void processFileData(FILE *f, unsigned long filesize, FILE *f2) {
    void *fileData;
    unsigned char *fileType;
    void *fileData2;
    unsigned char *fileType2;

    fileData = (void *) malloc(filesize);
    fread(fileData, filesize, 1, f);
    fileType = (unsigned char *) fileData;

    fileData2 = (void *) malloc(filesize);
    fread(fileData2, filesize, 1, f2);
    fileType2 = (unsigned char *) fileData2;

    //if both are bmp files, process them
    if (*fileType == 'B' && *(fileType + 1) == 'M' && *fileType2 == 'B'
            && *(fileType2 + 1) == 'M') {
        processImages(fileData, fileData2, filesize);
    } else {
        printf("Uknown file format or mismatch!\n");
    }
    free(fileData);
    free(fileData2);
}

/*
 * This function takes in two memory locations of the files loaded in memory
 * including the size. Grabs the width and height of the file which is stored
 * at the 18th and 22nd byte respectively (only applies to bmp file headers).
 * Compares if the images are the same dimensions. If so, create the blended
 * image and the checker image.
 */
void processImages(void *data, void *data2, unsigned long filesize) {
    int *width;
    int *height;
    width = (int *) (data + 18);  // location of width
    height = (int *) (data + 22); // location of height

    int *width2;
    int *height2;
    width2 = (int *) (data2 + 18);
    height2 = (int *) (data2 + 22);

    // if they are the same dimensions and square then process
    if (*width == *width2 && *height == *height2 && *width == *height
            && *width2 == *height2) {
        blendImages(data, data2, filesize);
        checkerImages(data, data2, filesize, width, height);
    } else {
        printf("Images are not the same dimensions!\n");
    }
}

/*
 * This function takes in two memory locations of the files loaded in memory
 * including the size. Creates a file for output, allocates enough memory
 * for the new array. Copies the bmp header (first 54 bytes), and then puts
 * the average of each of the chars for the remainder of the file into the
 * new array. Writes the new array to the output file, and frees the memory
 * and closes the file.
 */
void blendImages(void *data, void *data2, unsigned long filesize) {
    FILE *outfile1 = fopen("blend.bmp", "wb");
    void *blend;
    unsigned char *fileType;
    blend = (void *) malloc(filesize);
    fileType = (unsigned char *) blend;

    unsigned char *c1 = data;
    unsigned char *c2 = data2;

    for (int i = 0; i < HEADER_LENGTH; i++) {
        *(fileType + i) = *(c1 + i);
    }
    for (int i = HEADER_LENGTH; i < filesize; i++) {
        *(fileType + i) = ((*(c1 + i) + *(c2 + i)) / 2);
    }

    fwrite(fileType, filesize, 1, outfile1);
    free(blend);
    fclose(outfile1);

}

/*
 * This function takes in two memory locations of the files loaded in memory
 * including the size, pixel width, and pixel height. Creates a file for output,
 * allocates enough memory for the new array. Copies the bmp header (first 54 bytes),
 * and then uses an algorithm that prints an 8x8 checker board mix of the two images.
 * Writes the new array to the output file, and frees the memory and closes the file.
 */
void checkerImages(void *data, void *data2, unsigned long filesize, int* width,
        int* height) {
    FILE *outfile2 = fopen("checker.bmp", "wb");
    void *checker;
    unsigned char *fileType;
    checker = (void *) malloc(filesize);
    fileType = (unsigned char *) checker;

    unsigned char *c1 = data;
    unsigned char *c2 = data2;

    for (int i = 0; i < HEADER_LENGTH; i++) {
        *(fileType + i) = *(c1 + i);
    }

    /* The pointer, used to switch between the two arrays. */
    unsigned char *Ptr;
    Ptr = ((c2 + 0) + HEADER_LENGTH);
    int cols = *width * 3;
    int offset = 0;
    int index = 0;
    for (int i = 0; i < *height; i++) {
        index = 0;
        offset = 0;
        /* Sets the offset every 1/8th of the image. */
        if (i % (*height / 4) >= *height / 8) {
            offset = 1;
        }
        for (int j = 0; j < 8; j++) {
            /* Sets the pointer to some element in the first array. */
            if (j % 2 == offset) {
                Ptr = ((c1 + i * cols) + index + HEADER_LENGTH);
            }
            for (int k = 0; k < *width * 3 / 8; k++) {
                unsigned char average = *Ptr++;
                *((fileType + i * cols) + index + HEADER_LENGTH) = average;
                index++;
            }
            /* Sets the pointer to some element in the second array. */
            Ptr = ((c2 + i * cols) + index + HEADER_LENGTH);
        }
    }

    fwrite(fileType, filesize, 1, outfile2);
    free(checker);
    fclose(outfile2);

}

/*
 * This function takes in pointer to a file and creates a structure which contains
 * the size of the file. Returns the size as an unsigned long.
 */
unsigned long get_file_size(char *s) {
    long result = -1;
    struct stat file_info;
    if (stat(s, &file_info) > -1) {
        result = (unsigned long) (file_info.st_size);
    }
    return (unsigned long) result;
}
