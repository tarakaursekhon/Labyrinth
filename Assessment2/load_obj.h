#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <vector>
#include <windows.h>
#include <wingdi.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


using namespace std;

#define PIXEL_W 1920
#define PIXEL_H 1080

float colour_buffer[PIXEL_W * PIXEL_H * 3];
float depth_buffer[PIXEL_W * PIXEL_H];

struct vertex
{
	glm::vec3 pos;
	glm::vec3 col;
	glm::vec3 nor;
};

struct triangle
{
public:
	vertex v1, v2, v3;
	int mtlID;
	bool reflect;
	int primID;
};

int obj_parse(const char* objFile, const char* mtlDir, std::vector<triangle>* io_tris)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objFile, mtlDir))
	{
		throw std::runtime_error(warn + err);
	}

	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;

	for (const auto& shape : shapes)
	{
		size_t index_offset = 0;
		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
		{
			int material_id = shape.mesh.material_ids[f];
			float r = 1.0f, g = 1.0f, b = 1.0f;

			if (material_id >= 0 && material_id < materials.size())
			{
				r = materials[material_id].diffuse[0];
				g = materials[material_id].diffuse[1];
				b = materials[material_id].diffuse[2];
			}

			int fv = shape.mesh.num_face_vertices[f];

			for (int v = 0; v < fv; v++)
			{
				tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

				vertex vert{};
				vert.pos =
				{
					attrib.vertices[3 * idx.vertex_index + 0],
					attrib.vertices[3 * idx.vertex_index + 1],
					attrib.vertices[3 * idx.vertex_index + 2],
				};

				if (idx.normal_index >= 0) {
					vert.nor =
					{
						attrib.normals[3 * idx.normal_index + 0],
						attrib.normals[3 * idx.normal_index + 1],
						attrib.normals[3 * idx.normal_index + 2]
					};
				}

				vert.col = { r, g, b };

				vertices.push_back(vert);
				indices.push_back(indices.size());
			}

			index_offset += fv;
		}
	}

	for (int i = 0; i < indices.size() / 3; i++)
	{
		triangle tri;
		tri.v1 = vertices[indices[(i * 3) + 0]];
		tri.v2 = vertices[indices[(i * 3) + 1]];
		tri.v3 = vertices[indices[(i * 3) + 2]];
		tri.reflect = false;
		tri.primID = i;
		io_tris->push_back(tri);
	}

	printf("successfully parsed %s and read %d triangles \n", objFile, io_tris->size());
	return io_tris->size();
}

float* trisToVertices(std::vector<triangle> io_tris)
{
	float* verts = (float*) malloc(io_tris.size() * 3 * 10 * sizeof(float));
	int i = 0;

	for (triangle& t : io_tris)
	{
		verts[i] = t.v1.pos[0];
		verts[i + 1] = t.v1.pos[1];
		verts[i + 2] = t.v1.pos[2];
		verts[i + 3] = t.v1.col[0];
		verts[i + 4] = t.v1.col[1];
		verts[i + 5] = t.v1.col[2];
		verts[i + 6] = 1.f;
		verts[i + 7] = t.v1.nor[0];
		verts[i + 8] = t.v1.nor[1];
		verts[i + 9] = t.v1.nor[2];

		i += 10;

		verts[i] = t.v2.pos[0];
		verts[i + 1] = t.v2.pos[1];
		verts[i + 2] = t.v2.pos[2];
		verts[i + 3] = t.v2.col[0];
		verts[i + 4] = t.v2.col[1];
		verts[i + 5] = t.v2.col[2];
		verts[i + 6] = 1.f;
		verts[i + 7] = t.v2.nor[0];
		verts[i + 8] = t.v2.nor[1];
		verts[i + 9] = t.v2.nor[2];

		i += 10;

		verts[i] = t.v3.pos[0];
		verts[i + 1] = t.v3.pos[1];
		verts[i + 2] = t.v3.pos[2];
		verts[i + 3] = t.v3.col[0];
		verts[i + 4] = t.v3.col[1];
		verts[i + 5] = t.v3.col[2];
		verts[i + 6] = 1.f;
		verts[i + 7] = t.v3.nor[0];
		verts[i + 8] = t.v3.nor[1];
		verts[i + 9] = t.v3.nor[2];

		i += 10;
	}

	return verts;
}

void CounterEndAndPrint(LARGE_INTEGER StartingTime, LARGE_INTEGER* EndingTime, LARGE_INTEGER Frequency)
{
	QueryPerformanceCounter(EndingTime);

	LARGE_INTEGER ElapsedMicroseconds;
	ElapsedMicroseconds.QuadPart = (*EndingTime).QuadPart - StartingTime.QuadPart;
	ElapsedMicroseconds.QuadPart *= 1000000;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
	std::cout << (float)ElapsedMicroseconds.QuadPart / (float)1000000 << std::endl;
}

float linear_to_gamma(float linear_component)
{
	if (linear_component > 0)
	{
		return std::sqrt(linear_component);
	}

	return 0;
}

void writeColToDisplayBuffer(glm::vec3 col, int pixel_x, int pixel_y)
{
	float r = linear_to_gamma(col.x);
	float g = linear_to_gamma(col.y);
	float b = linear_to_gamma(col.z);


	float rc = std::clamp(r, .0f, 1.f);
	float gc = std::clamp(g, .0f, 1.f);
	float bc = std::clamp(b, .0f, 1.f);

	float pixel_r = rc * 255.f;
	float pixel_g = gc * 255.f;
	float pixel_b = bc * 255.f;

	colour_buffer[(pixel_y * PIXEL_W * 3) + (pixel_x * 3) + 0] = pixel_r;
	colour_buffer[(pixel_y * PIXEL_W * 3) + (pixel_x * 3) + 1] = pixel_g;
	colour_buffer[(pixel_y * PIXEL_W * 3) + (pixel_x * 3) + 2] = pixel_b;

}

int savebitmap(const char* filename, float* pixelBuffer, int w, int h)
{
	char* charBuffer = (char*)malloc(sizeof(unsigned char) * w * h * 3);

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			charBuffer[(y * w * 3) + (x * 3) + 0] = pixelBuffer[((h - y) * w * 3) + (x * 3) + 2];
			charBuffer[(y * w * 3) + (x * 3) + 1] = pixelBuffer[((h - y) * w * 3) + (x * 3) + 1];
			charBuffer[(y * w * 3) + (x * 3) + 2] = pixelBuffer[((h - y) * w * 3) + (x * 3) + 0];
		}
	}

	BITMAPINFOHEADER infoHdr;
	infoHdr.biSize = 40;
	infoHdr.biWidth = w;
	infoHdr.biHeight = h;
	infoHdr.biPlanes = 1;
	infoHdr.biBitCount = 24;
	infoHdr.biCompression = 0;
	infoHdr.biSizeImage = sizeof(unsigned char) * w * h * 3;
	infoHdr.biXPelsPerMeter = 0;
	infoHdr.biYPelsPerMeter = 0;
	infoHdr.biClrUsed = 0;
	infoHdr.biClrImportant = 0;

	BITMAPFILEHEADER fileHdr;
	fileHdr.bfType = 19778;
	fileHdr.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(unsigned char) * w * h * 3);
	fileHdr.bfReserved1 = 0;
	fileHdr.bfReserved2 = 0;
	fileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	FILE* bitmapFile;

	errno_t err = fopen_s(&bitmapFile, filename, "wb");
	if (err != 0 || bitmapFile == NULL)
	{
		printf("savebitmap - open failed for %s\n", filename);
		return NULL;
	}

	fwrite(&fileHdr, sizeof(BITMAPFILEHEADER), 1, bitmapFile);

	fwrite(&infoHdr, sizeof(BITMAPINFOHEADER), 1, bitmapFile);

	fseek(bitmapFile, fileHdr.bfOffBits, SEEK_SET);

	int nBytes = infoHdr.biWidth * infoHdr.biHeight * 3;
	fwrite(charBuffer, sizeof(unsigned char), nBytes, bitmapFile);

	fclose(bitmapFile);

	free(charBuffer);

	printf("savebitmap - saved %s w=%d h=%d bits=%d\n", filename, infoHdr.biWidth, infoHdr.biHeight, infoHdr.biBitCount);
}
