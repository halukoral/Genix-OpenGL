#include "Model.h"

#include <iostream>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "stb_image.h"

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
	std::string Filename = std::string(path);
	Filename = directory + '/' + Filename;

	unsigned int TextureId;
	glGenTextures(1, &TextureId);

	int Width, Height, NrComponents;
	unsigned char *Data = stbi_load(Filename.c_str(), &Width, &Height, &NrComponents, 0);
	if (Data)
	{
		GLenum Format;
		if (NrComponents == 1)
		{
			Format = GL_RED;
		}
		else if (NrComponents == 3)
		{
			Format = GL_RGB;
		}
		else if (NrComponents == 4)
		{
			Format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, TextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(Data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(Data);
	}

	return TextureId;
}

Model::Model(std::string const& path, bool gamma): GammaCorrection(gamma)
{
	LoadModel(path);
}

void Model::Draw(Shader& InShader)
{
	for (auto& Mesh : Meshes)
	{
		Mesh.Draw(InShader);
	}
}

void Model::LoadModel(std::string const& path)
{
	// read file via ASSIMP
	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if(!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << Importer.GetErrorString() << std::endl;
		return;
	}
	// retrieve the directory path of the filepath
	Directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	ProcessNode(Scene->mRootNode, Scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	// process each mesh located at the current node
	for(unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* Mesh = scene->mMeshes[node->mMeshes[i]];
		Meshes.push_back(ProcessMesh(Mesh, scene));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for(unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}

}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	// data to fill
	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;
	std::vector<Texture> Textures;

	// walk through each of the mesh's vertices
	for(unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex Vertex;
		glm::vec3 Vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		Vector.x = mesh->mVertices[i].x;
		Vector.y = mesh->mVertices[i].y;
		Vector.z = mesh->mVertices[i].z;
		Vertex.Position = Vector;
		// normals
		if (mesh->HasNormals())
		{
			Vector.x = mesh->mNormals[i].x;
			Vector.y = mesh->mNormals[i].y;
			Vector.z = mesh->mNormals[i].z;
			Vertex.Normal = Vector;
		}
		// texture coordinates
		if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x; 
			vec.y = mesh->mTextureCoords[0][i].y;
			Vertex.TexCoords = vec;
			// tangent
			Vector.x = mesh->mTangents[i].x;
			Vector.y = mesh->mTangents[i].y;
			Vector.z = mesh->mTangents[i].z;
			Vertex.Tangent = Vector;
			// bitangent
			Vector.x = mesh->mBitangents[i].x;
			Vector.y = mesh->mBitangents[i].y;
			Vector.z = mesh->mBitangents[i].z;
			Vertex.Bitangent = Vector;
		}
		else
			Vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		Vertices.push_back(Vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for(unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace Face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for(unsigned int j = 0; j < Face.mNumIndices; j++)
		{
			Indices.push_back(Face.mIndices[j]);        
		}
	}
	// process materials
	aiMaterial* Material = scene->mMaterials[mesh->mMaterialIndex];    
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	std::vector<Texture> DiffuseMaps = LoadMaterialTextures(Material, aiTextureType_DIFFUSE, "texture_diffuse");
	Textures.insert(Textures.end(), DiffuseMaps.begin(), DiffuseMaps.end());
	// 2. specular maps
	std::vector<Texture> SpecularMaps = LoadMaterialTextures(Material, aiTextureType_SPECULAR, "texture_specular");
	Textures.insert(Textures.end(), SpecularMaps.begin(), SpecularMaps.end());
	// 3. normal maps
	std::vector<Texture> NormalMaps = LoadMaterialTextures(Material, aiTextureType_HEIGHT, "texture_normal");
	Textures.insert(Textures.end(), NormalMaps.begin(), NormalMaps.end());
	// 4. height maps
	std::vector<Texture> HeightMaps = LoadMaterialTextures(Material, aiTextureType_AMBIENT, "texture_height");
	Textures.insert(Textures.end(), HeightMaps.begin(), HeightMaps.end());
        
	// return a mesh object created from the extracted mesh data
	return Mesh(Vertices, Indices, Textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> Textures;
	for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool Skip = false;
		for(unsigned int j = 0; j < TexturesLoaded.size(); j++)
		{
			if(std::strcmp(TexturesLoaded[j].Path.data(), str.C_Str()) == 0)
			{
				Textures.push_back(TexturesLoaded[j]);
				Skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if(!Skip)
		{   // if texture hasn't been loaded already, load it
			Texture Texture;
			Texture.ID = TextureFromFile(str.C_Str(), this->Directory);
			Texture.Type = typeName;
			Texture.Path = str.C_Str();
			Textures.push_back(Texture);
			TexturesLoaded.push_back(Texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
		}
	}
	return Textures;
}
