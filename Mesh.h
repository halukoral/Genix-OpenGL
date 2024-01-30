#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

class Shader;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int ID;
    std::string Type;
    std::string Path;
};

class Mesh {
public:

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    // render the mesh
    void Draw(Shader &Shader);

    // mesh Data
    std::vector<Vertex>       Vertices;
    std::vector<unsigned int> Indices;
    std::vector<Texture>      Textures;
    unsigned int VAO;

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void SetupMesh();
};
