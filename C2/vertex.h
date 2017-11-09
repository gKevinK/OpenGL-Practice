#ifndef VERTEX_H
#define VERTEX_H

#include <glm\glm.hpp>

class Vertex
{
public:
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Color;

	Vertex(float px, float py, float pz, float nx, float ny, float nz, float cr, float cg, float cb)
		: Position(px, py, pz), Normal(nx, ny, nz), Color(cr, cg, cb) {};
};

#endif // !VERTEX_H
