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
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
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
        VertexCode = vShaderStream.str();
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

Shader::Shader(const char* InVertexPath, const char* InFragmentPath, const char* InGeometryPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string VertexCode;
    std::string GeometryCode;
    std::string FragmentCode;
    std::ifstream vShaderFile;
    std::ifstream gShaderFile;
    std::ifstream fShaderFile;

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(InVertexPath);
        gShaderFile.open(InGeometryPath);
        fShaderFile.open(InFragmentPath);
        std::stringstream vShaderStream, gShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        gShaderStream << gShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        gShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        VertexCode = vShaderStream.str();
        GeometryCode = gShaderStream.str();
        FragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = VertexCode.c_str();
    const char* gShaderCode = GeometryCode.c_str();
    const char* fShaderCode = FragmentCode.c_str();

    // 2. compile shaders
    unsigned int Vertex, Geometry, Fragment;

    // vertex shader
    Vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(Vertex, 1, &vShaderCode, nullptr);
    glCompileShader(Vertex);
    CheckCompileErrors(Vertex, "VERTEX");

    // geometry shader
    Geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(Geometry, 1, &gShaderCode, nullptr);
    glCompileShader(Geometry);
    CheckCompileErrors(Geometry, "GEOMETRY");
    
    // fragment Shader
    Fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(Fragment, 1, &fShaderCode, nullptr);
    glCompileShader(Fragment);
    CheckCompileErrors(Fragment, "FRAGMENT");

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, Vertex);
    glAttachShader(ID, Geometry);
    glAttachShader(ID, Fragment);
    glLinkProgram(ID);
    CheckCompileErrors(ID, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(Vertex);
    glDeleteShader(Geometry);
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

void Shader::SetFloat(const std::string& InName, float InValue) const
{
    glUniform1f(glGetUniformLocation(ID, InName.c_str()), InValue);
}

void Shader::SetVec2(const std::string& InName, glm::vec2& Value) const
{
    glUniform2fv(glGetUniformLocation(ID, InName.c_str()), 1, &Value[0]);
}

void Shader::SetVec2(const std::string& InName, const float X, const float Y) const
{
    glUniform2f(glGetUniformLocation(ID, InName.c_str()), X, Y);
}

void Shader::SetVec3(const std::string& InName, glm::vec3& Value) const
{
    glUniform3fv(glGetUniformLocation(ID, InName.c_str()), 1, &Value[0]);
}

void Shader::SetVec3(const std::string& InName, const float X, const float Y, const float Z) const
{
    glUniform3f(glGetUniformLocation(ID, InName.c_str()), X, Y, Z);
}

void Shader::SetVec4(const std::string& InName, glm::vec4& Value) const
{
    glUniform4fv(glGetUniformLocation(ID, InName.c_str()), 1, &Value[0]);
}

void Shader::SetVec4(const std::string& InName, const float X, const float Y, const float Z, const float W) const
{
    glUniform4f(glGetUniformLocation(ID, InName.c_str()), X, Y, Z, W);
}

void Shader::SetMat2(const std::string& InName, glm::mat2& Mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, InName.c_str()), 1, GL_FALSE, &Mat[0][0]);
}

void Shader::SetMat3(const std::string& InName, glm::mat3& Mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, InName.c_str()), 1, GL_FALSE, &Mat[0][0]);
}

void Shader::SetMat4(const std::string& InName, glm::mat4& Mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, InName.c_str()), 1, GL_FALSE, &Mat[0][0]);
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
