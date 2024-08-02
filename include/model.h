#ifndef MODEL_H
#define MODEL_H

#include <includes.h>

struct Model {
	float x, y, z;
	float* vertices;
	size_t vertexCount;
	unsigned int* indices;
	size_t indexCount;
	float* normals;
	size_t normalCount; 
};

extern struct Model* models;
extern size_t modelCount;

void appendModel(struct Model* model); 

void render(struct Model *model);

struct Model initModel (char* path, float xpos, float ypos, float zpos, float divisionFactor);

void perspective (float *vertices, size_t size);

#endif
