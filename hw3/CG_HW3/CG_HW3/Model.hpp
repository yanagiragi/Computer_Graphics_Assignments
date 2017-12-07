#define _CRT_SECURE_NO_WARNINGS
#include <glm/glm.hpp>
#include <vector>
#include <sstream>
#include <string>
#include <iterator>
#include <iostream>
#include <stdarg.h>

struct Vertex
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
	GLfloat tangent[3];
	GLfloat bitangent[3];
};
typedef struct Vertex Vertex;

std::vector<struct Vertex> vertices;
std::vector<glm::vec3> positions;
std::vector<glm::vec2> texcoords;
std::vector<glm::vec3> normals;
std::vector<glm::vec3> tangents;
std::vector<glm::vec3> bitangents;
std::vector<glm::vec3> indices;
std::vector<unsigned int> vindices;

template<typename Out> void split(const std::string &s, char delim, Out result) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

void LoadObj(char* filename)
{
	using std::string;
	std::vector<std::string> rawData;

	FILE *fp;
	fp = fopen(filename, "r");
	char *buffer = (char *)malloc(sizeof(char) * 4096);
	while (fgets(buffer, 4096, fp) != NULL)
	{
		rawData.push_back(buffer);
	}
	free(buffer);
	fclose(fp);

	for (std::string buffer : rawData)
	{
		if (buffer[0] == 'v' && buffer[1] == ' ') {

			std::vector<std::string> v = split(buffer.substr(string("v ").size(), buffer.length() - string("v ").size()), ' ');

			positions.push_back(glm::vec3(
				std::stof(v[0], nullptr),
				std::stof(v[1], nullptr),
				std::stof(v[2], nullptr)
			));

			/*struct Vertex vert;
			vert.position[0] = std::stof(v[0], nullptr);
			vert.position[1] = std::stof(v[1], nullptr);
			vert.position[2] = std::stof(v[2], nullptr);

			vertices.push_back(vert);*/
		}
		else if (buffer[0] == 'v' && buffer[1] == 't') {

			std::vector<std::string> v = split(buffer.substr(string("vt ").size(), buffer.length() - string("vt ").size()), ' ');

			auto coord = glm::vec2(
				std::stof(v[0], nullptr),
				std::stof(v[1], nullptr)
			);

			//texcoords.push_back(coord);
		}

		else if (buffer[0] == 'v' && buffer[1] == 'n') {

			std::vector<std::string> v = split(buffer.substr(string("vn ").size(), buffer.length() - string("vn ").size()), ' ');

			auto normal = glm::vec3(
				std::stof(v[0], nullptr),
				std::stof(v[1], nullptr),
				std::stof(v[2], nullptr)
			);

			normal = glm::normalize(normal);

			normals.push_back(normal);
		}

		else if (buffer[0] == 'f' && buffer[1] == ' ') {
			std::vector<std::string> f = split(buffer.substr(string("f ").size(), buffer.length() - string("f ").size()), ' ');

			std::vector<int> v;
			std::vector<int> u;
			std::vector<int> n;

			for (int i = 0; i < 3; ++i) {
				std::vector<std::string> ff = split(f[i], '/');
				int vertexIndex = std::stod(ff[0], nullptr) - 1;
				int UVIndex = std::stod(ff[1], nullptr) - 1;
				int normalIndex = std::stod(ff[2], nullptr) - 1;

				//v.push_back(vertexIndex);
				//u.push_back(UVIndex);
				//n.push_back(normalIndex);
			
				indices.push_back(glm::vec3(vertexIndex, UVIndex, normalIndex));
			}
			
			/*glm::vec3 v0 = glm::vec3(vertices[v[0]].position[0], vertices[v[0]].position[1], vertices[v[0]].position[2]);
			glm::vec3 v1 = glm::vec3(vertices[v[1]].position[0], vertices[v[1]].position[1], vertices[v[1]].position[2]);
			glm::vec3 v2 = glm::vec3(vertices[v[2]].position[0], vertices[v[2]].position[1], vertices[v[2]].position[2]);

			glm::vec2 uv0 = texcoords[u[0]];
			glm::vec2 uv1 = texcoords[u[1]];
			glm::vec2 uv2 = texcoords[u[2]];
			
			// Edges of the triangle : postion delta
			glm::vec3 deltaPos1 = v1 - v0;
			glm::vec3 deltaPos2 = v2 - v0;

			// UV delta
			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

			for (int i = 0; i < 3; ++i) {
				tangents.push_back(tangent);
				bitangents.push_back(bitangent);
				/*vertices[v[i]].tangent[0] += tangent.x;
				vertices[v[i]].tangent[1] += tangent.y;
				vertices[v[i]].tangent[2] += tangent.z;
				vertices[v[i]].bitangent[0] += bitangent.x;
				vertices[v[i]].bitangent[1] += bitangent.y;
				vertices[v[i]].bitangent[2] += bitangent.z;
			}*/

		}
	}

	GLMmodel *model = glmReadOBJ(filename);
	auto a = model->texcoords;

	for (int i = 0; i < model->numtriangles; ++i)
	{
		int vindex[3] = { model->triangles[i].vindices[0], model->triangles[i].vindices[1], model->triangles[i].vindices[2] };
		std::vector<glm::vec3> p;
		
		for (int j = 0; j < 3; ++j) {
			glm::vec3 pos = glm::vec3(model->vertices[vindex[j] * 3 + 0], model->vertices[vindex[j] * 3 + 1], model->vertices[vindex[j] * 3 + 2]);
			p.push_back(pos);
		}

		int nindex[3] = { model->triangles[i].nindices[0], model->triangles[i].nindices[1], model->triangles[i].nindices[2] };
		std::vector<glm::vec3> n;

		for (int j = 0; j < 3; ++j) {
			glm::vec3 nor = glm::vec3(model->normals[nindex[j] * 3 + 0], model->normals[nindex[j] * 3 + 1], model->normals[nindex[j] * 3 + 2]);
			n.push_back(nor);
		}

		int tindex[3] = { model->triangles[i].tindices[0], model->triangles[i].tindices[1], model->triangles[i].tindices[2] };
		std::vector<glm::vec2> t;

		for (int j = 0; j < 3; ++j) {
			glm::vec2 tex = glm::vec2(model->texcoords[tindex[j] * 2 + 0], model->texcoords[tindex[j] * 2 + 1]);
			t.push_back(tex);
		}
	}

	/*
	for (int i = 2; i < model->numtriangles * 2 + 2 - 1; i+=2)
	{
		texcoords.push_back(
			glm::vec2(
				model->texcoords[i],
				model->texcoords[i + 1]
			)
		);
	}*/

	return;
}
