#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>

class Shader
{
public:
    unsigned int ID;

    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* InVertexPath, const char* InFragmentPath);
    Shader(const char* InVertexPath, const char* InFragmentPath, const char* InGeometryPath);

    // activate the shader
    // ------------------------------------------------------------------------
    void Use() const { glUseProgram(ID); }

    // utility uniform functions
    // ------------------------------------------------------------------------
    void SetBool(const std::string& InName, bool InValue) const;
    void SetInt(const std::string& InName, int InValue) const;
    void SetFloat(const std::string& InName, float InValue) const;
    void SetVec2(const std::string& InName, glm::vec2& Value) const;
    void SetVec2(const std::string& InName, const float X, const float Y) const;
    void SetVec3(const std::string& InName, glm::vec3& Value) const;
    void SetVec3(const std::string& InName, const float X, const float Y, const float Z) const;
    void SetVec4(const std::string& InName, glm::vec4& Value) const;
    void SetVec4(const std::string& InName, const float X, const float Y, const float Z, const float W) const;
    void SetMat2(const std::string& InName, glm::mat2& Mat) const;
    void SetMat3(const std::string& InName, glm::mat3& Mat) const;
    void SetMat4(const std::string& InName, glm::mat4& Mat) const;

private:

    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void CheckCompileErrors(unsigned int InShader, const std::string& Type);
};
