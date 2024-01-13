#pragma once

#include <string>
#include <GL/glew.h>

class Shader
{
public:
	unsigned int ID;

	// constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* InVertexPath, const char* InFragmentPath);

	// activate the shader
    // ------------------------------------------------------------------------
    void Use() const { glUseProgram(ID); }

	// utility uniform functions
    // ------------------------------------------------------------------------
    void SetBool(const std::string &InName, bool InValue) const;
	void SetInt(const std::string &InName, int InValue) const;
	void SetFloat(const std::string &InName, float InValue) const;

private:
	
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void CheckCompileErrors(unsigned int InShader, const std::string& Type);
};
