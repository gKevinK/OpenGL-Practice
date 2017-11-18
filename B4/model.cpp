#include "model.h"

Mesh::~Mesh()
{
    for (Vertex * & v : vertexes)
        delete v;
    for (HalfEdge * & e : edges)
        delete e;
    for (Face * & f : faces)
        delete f;
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::SetMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexD), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexD), (void *)(offsetof(VertexD, position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexD), (void *)(offsetof(VertexD, normal)));
    glEnableVertexAttribArray(1);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

void Mesh::SetData(bool interp)
{
    vertices.clear();
    if (interp) {
        for (Face * & f : faces) {
            Vertex * v1 = f->edge->vertex;
            Vertex * v2 = f->edge->next->vertex;
            Vertex * v3 = f->edge->next->next->vertex;
            glm::vec3 norm = glm::cross(v3->position - v2->position, v2->position - v1->position);
            v1->normal += norm;
            v2->normal += norm;
            v3->normal += norm;
        }
        for (Vertex * & v : vertexes) {
            v->normal = glm::normalize(v->normal);
        }
        for (Face * & f : faces) {
            std::array<Vertex *, 3> vs;
            vs[0] = f->edge->vertex;
            vs[1] = f->edge->next->vertex;
            vs[2] = f->edge->next->next->vertex;
            for (int i = 0; i < 3; i++) {
                VertexD vertd;
                vertd.position = vs[i]->position;
                vertd.normal = vs[i]->normal;
                vertices.push_back(vertd);
            }
        }
    } else {
        for (Face * & f : faces) {
            Vertex * v1 = f->edge->vertex;
            Vertex * v2 = f->edge->next->vertex;
            Vertex * v3 = f->edge->next->next->vertex;
            glm::vec3 norm = glm::normalize(glm::cross(v2->position - v1->position, v3->position - v2->position));
            VertexD vertd;
            vertd.normal = norm;
            vertd.position = v1->position;
            vertices.push_back(vertd);
            vertd.position = v2->position;
            vertices.push_back(vertd);
            vertd.position = v3->position;
            vertices.push_back(vertd);
        }
    }
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexD), &vertices[0], GL_STATIC_DRAW);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

void Mesh::addNewEdge(HalfEdge * e, unsigned int from, unsigned int to)
{
    Vertex * v1 = vertexes[from];
    Vertex * v2 = vertexes[to];
    e->vertex = v1;
    v1->edges.push_back(e);
    for (HalfEdge * & ec : v2->edges) {
        if (ec->next->vertex == v1) {
            e->twin = ec;
            ec->twin = e;
        }
    }
    edges.push_back(e);
}

void Mesh::LoadFile(const std::string & path)
{
    vertexes.push_back(new Vertex());
    std::ifstream fileStream;
    std::string file;
    try {
        fileStream.open(path);
        std::string line;
        while (std::getline(fileStream, line)) {
            if (line.empty()) continue;
            if (line[0] == '#') continue;
            std::stringstream stream(line);
            char t;
            stream >> t;
            if (t == 'v') {
                float x, y, z;
                stream >> x >> y >> z;
                Vertex * vert = new Vertex();
                vert->position = glm::vec3(x, y, z);
                vertexes.push_back(vert);
            }
            else if (t == 'f') {
                unsigned int a, b, c;
                stream >> a >> b >> c;
                Face * face = new Face();
                HalfEdge * e1 = new HalfEdge();
                HalfEdge * e2 = new HalfEdge();
                HalfEdge * e3 = new HalfEdge();
                e1->next = e2;
                e2->next = e3;
                e3->next = e1;
                e1->face = face;
                e2->face = face;
                e3->face = face;
                addNewEdge(e1, a, b);
                addNewEdge(e2, b, c);
                addNewEdge(e3, c, a);
                face->edge = e1;
                faces.push_back(face);
            }
        }
        fileStream.close();
    } catch (std::ifstream::failure e) {
        std::cout << "ERROR::MESH::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        std::cout << path << std::endl;
    }
}

void Mesh::Draw(GLenum type)
{
    glDrawArrays(type, 0, vertices.size() * sizeof(VertexD));
}
