#ifndef MODEL_H
#define MODEL_H

#include <glm\glm.hpp>
#include <vector>
#include <iostream>
#include <fstream>

struct Vertex;
struct HalfEdge;
struct Face;
class Mesh;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    Mesh * mesh;
    HalfEdge * edge;
};

struct HalfEdge
{
    Vertex * vertex;
    Face * face;
    HalfEdge * twin;
    HalfEdge * next;
};

struct Face
{
    HalfEdge * edge;
};

class Mesh
{
public:
    std::vector<Vertex *> vertices;
    std::vector<HalfEdge *> edges;
    std::vector<Face *> faces;

    Mesh() {};
    ~Mesh() {};

    static Mesh LoadFile(const std::string & path);
};

#endif // !MODEL_H
