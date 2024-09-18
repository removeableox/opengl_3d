#include <model.h>

#define FAST_OBJ_IMPLEMENTATION
#include <fast_obj.h>

struct Model* models = NULL;
size_t modelCount = 0;

void appendModel(struct Model* model) {
	modelCount++;
	models = (struct Model*)realloc(models, modelCount*sizeof(struct Model));
	models[modelCount-1] = *model;
}

void render(struct Model *model) {
	// apply perspective to vertices
	float perspectiveVertices[model->vertexCount];
	for (int i = 0; i < model->vertexCount; i++) {
		perspectiveVertices[i] = model->vertices[i];
	}
	perspective(perspectiveVertices, model->vertexCount);

	// render
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, model->vertexCount * sizeof(float), perspectiveVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, model->normalCount * sizeof(float), model->normals, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->indexCount * sizeof(unsigned int), model->indices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, model->indexCount, GL_UNSIGNED_INT, 0);
}

struct Model initModel (char* path, float xpos, float ypos, float zpos, float divisionFactor) {
	fastObjMesh* mesh = fast_obj_read(path);
	struct Model model;

	model.x = xpos;
	model.y = ypos;
	model.z = zpos;

	model.vertexCount = mesh->position_count*3;

	model.vertices = (float*)malloc(model.vertexCount*sizeof(float));
	for (int i = 0; i < model.vertexCount; i++) {
		model.vertices[i] = mesh->positions[i];
	}

	model.indexCount = mesh->index_count;

	model.indices = (unsigned int*)malloc(model.indexCount*sizeof(float));
	for (int i = 0; i < model.indexCount; i++) {
		model.indices[i] = mesh->indices[i].p;
	}

	if (divisionFactor != 1) {
		for (int i = 0; i < model.vertexCount; i++) {
			model.vertices[i] /= divisionFactor;
		}
	}

	fast_obj_destroy(mesh);

	// Allocate space for normals (initialize to 0 for accumulation)
	model.normals = (float*)calloc(model.vertexCount * 3, sizeof(float));

	// Loop through each triangle (assuming indexCount is a multiple of 3)
	for (int i = 0; i < model.indexCount; i += 3) {
		int idx1 = model.indices[i];
		int idx2 = model.indices[i+1];
		int idx3 = model.indices[i+2];

		// Get the vertices of the triangle
		vec3 v1 = {model.vertices[idx1 * 3], model.vertices[idx1 * 3 + 1], model.vertices[idx1 * 3 + 2]};
		vec3 v2 = {model.vertices[idx2 * 3], model.vertices[idx2 * 3 + 1], model.vertices[idx2 * 3 + 2]};
		vec3 v3 = {model.vertices[idx3 * 3], model.vertices[idx3 * 3 + 1], model.vertices[idx3 * 3 + 2]};

		// Create vectors for edges
		vec3 edge1, edge2, normal;

		// Compute edges
		glm_vec3_sub(v2, v1, edge1); // edge1 = v2 - v1
		glm_vec3_sub(v3, v1, edge2); // edge2 = v3 - v1

		// Compute the cross product to get the normal
		glm_vec3_cross(edge1, edge2, normal);

		// Normalize the normal
		glm_vec3_normalize(normal);

		// Accumulate the normal to each vertex of the triangle
		model.normals[idx1 * 3]     += normal[0];
		model.normals[idx1 * 3 + 1] += normal[1];
		model.normals[idx1 * 3 + 2] += normal[2];

		model.normals[idx2 * 3]     += normal[0];
		model.normals[idx2 * 3 + 1] += normal[1];
		model.normals[idx2 * 3 + 2] += normal[2];

		model.normals[idx3 * 3]     += normal[0];
		model.normals[idx3 * 3 + 1] += normal[1];
		model.normals[idx3 * 3 + 2] += normal[2];
	}

	// Normalize accumulated normals for each vertex
	for (int i = 0; i < model.vertexCount; i++) {
		vec3 normal = {model.normals[i * 3], model.normals[i * 3 + 1], model.normals[i * 3 + 2]};
		glm_vec3_normalize(normal);

		model.normals[i * 3]     = normal[0];
		model.normals[i * 3 + 1] = normal[1];
		model.normals[i * 3 + 2] = normal[2];
	}

	model.normalCount = model.vertexCount * 3;


	appendModel(&model);

	return model; 
}

void perspective (float *vertices, size_t size) {
	for (int i = 0; i < size; i += 3) {
		float distance = vertices[i+2] + 1;
		float scale = 1/distance;
		vertices[i] *= scale;
		vertices[i+1] *= scale;
		vertices[i+2] *= scale;
	}
}
