#ifndef MODEL_H
#define MODEL_H

#include <glad\glad.h>
#include <glm\glm.hpp>
#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <sstream>

struct Vertex;
struct VertexD;
struct HalfEdge;
struct Face;
class Mesh;

struct VertexD
{
    glm::vec3 position;
    glm::vec3 normal;
};

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    std::vector<HalfEdge *> edges;
};

struct HalfEdge
{
    //unsigned int vertexId;
    Vertex * vertex;
    //unsigned int faceId;
    Face * face;
    //unsigned int twinId;
    HalfEdge * twin;
    //unsigned int nextId;
    HalfEdge * next;
};

struct Face
{
    //unsigned int edgeId;
    HalfEdge * edge;
};

class Mesh
{
public:
    std::vector<Vertex *> vertexes;
    std::vector<HalfEdge *> edges;
    std::vector<Face *> faces;
    unsigned int VAO, VBO, EBO;
    std::vector<VertexD> vertices;
    //std::vector<unsigned int> indices;

    Mesh() {};
    ~Mesh();
    void SetMesh();
    void SetData(bool interp = false);
    void LoadFile(const std::string & path);
    void Draw(GLenum type);

private:
    void addNewEdge(HalfEdge * e, unsigned int from, unsigned int to);
};

#endif // !MODEL_H
