#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(const char* InVertexPath, const char* InFragmentPath)
{
        // 1. retrieve the vertex/fragment source code from filePath
        std::string VertexCode;
        std::string FragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

		// ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try 
        {
            // open files
            vShaderFile.open(InVertexPath);
            fShaderFile.open(InFragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            VertexCode   = vShaderStream.str();
            FragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = VertexCode.c_str();
        const char* fShaderCode = FragmentCode.c_str();

		// 2. compile shaders
        unsigned int Vertex, Fragment;

		// vertex shader
        Vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(Vertex, 1, &vShaderCode, nullptr);
        glCompileShader(Vertex);
        CheckCompileErrors(Vertex, "VERTEX");

		// fragment Shader
        Fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(Fragment, 1, &fShaderCode, nullptr);
        glCompileShader(Fragment);
        CheckCompileErrors(Fragment, "FRAGMENT");

		// shader Program
        ID = glCreateProgram();
        glAttachShader(ID, Vertex);
        glAttachShader(ID, Fragment);
        glLinkProgram(ID);
        CheckCompileErrors(ID, "PROGRAM");

		// delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(Vertex);
        glDeleteShader(Fragment);
}

void Shader::SetBool(const std::string& InName, const bool InValue) const
{         
	glUniform1i(glGetUniformLocation(ID, InName.c_str()), (int)InValue); 
}

void Shader::SetInt(const std::string& InName, const int InValue) const
{ 
	glUniform1i(glGetUniformLocation(ID, InName.c_str()), InValue); 
}

void Shader::SetFloat(const std::string& InName, const float InValue) const
{ 
	glUniform1f(glGetUniformLocation(ID, InName.c_str()), InValue); 
}

void Shader::CheckCompileErrors(unsigned InShader, const std::string& Type)
{
	int Success;
	char InfoLog[1024];
	if (Type != "PROGRAM")
	{
		glGetShaderiv(InShader, GL_COMPILE_STATUS, &Success);
		if (!Success)
		{
			glGetShaderInfoLog(InShader, 1024, nullptr, InfoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << Type << "\n" << InfoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(InShader, GL_LINK_STATUS, &Success);
		if (!Success)
		{
			glGetProgramInfoLog(InShader, 1024, nullptr, InfoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << Type << "\n" << InfoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}
