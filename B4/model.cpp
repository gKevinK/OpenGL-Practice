#include "model.h"

Mesh::~Mesh()
{

}

Mesh Mesh::LoadFile(const std::string & path)
{
    Mesh mesh;
    std::ifstream fileStream;
    std::string code;
    fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        fileStream.open(path);
        code = fileStream.getline();
        std::stringstream stream;
        stream << fileStream.rdbuf();
        fileStream.close();
        code = stream.str();
    } catch (std::ifstream::failure e) {
        std::cout << "ERROR::MESH::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        std::cout << path << std::endl;
    }
    return std::move(mesh);
}
