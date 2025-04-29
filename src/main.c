//Bonne pratique, pour les includes systemes toujours utiliser <> et "" pour les includes de votre projet.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "utils.h"

#include "stb_image.h"
#include "stb_image_write.h"

#define HEAP_SIZE 2048 * 2048 * 4
static uint8_t* heap = NULL;
static size_t heap_top = 0;
void* allocate(size_t size) {
	size_t old_top = heap_top;
	heap_top += size;
	assert(heap_top <= HEAP_SIZE);
	return &heap[old_top];
}

const char* img_names[] = {
	"31.bmp",
	"64.bmp",
	"128.bmp"
};

const size_t img_name_len = sizeof(img_names) / sizeof(img_names[0]);

int main(int argc, char** argv) {
	heap = (uint8_t*)malloc(HEAP_SIZE);
	assert(heap != NULL);


	//lignes 35 à 54 (nos sujets a changements) donnent la couleur
	int width, height, channels;
	unsigned char* img = stbi_load(img_names[0], &width, &height, &channels, 0);
	if (img == NULL) {
		printf("Error in loading the image\n");
		exit(1);
	}
	printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);
	for (int i = 0; i < width * height * channels; i += channels) {
		unsigned char r = img[i];
		//g et b ne sont pas nécessaire, mais on peut les mettres
		unsigned char g = img[i + 1];
		unsigned char b = img[i + 2];
		if (r == 255) {
			img[i + 1] = 0;
			img[i + 2] = 0;
		}
	}
	char filepath[_MAX_PATH] = { 0 };
	snprintf(filepath, _MAX_PATH, "solution_%s", img_names[0]);
	stbi_write_bmp(filepath, width, height, channels, img);
}