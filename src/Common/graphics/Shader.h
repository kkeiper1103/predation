//
// Created by kkeiper1103 on 11/6/22.
//

#ifndef MODULETWO_SHADER_H
#define MODULETWO_SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <easyloggingpp/easylogging++.h>

#include <memory>
#include <string>
#include <vector>

// used to resolve glsl includes
#include <stb/stb_include.h>

class Shader {
GLuint programId = 0;

std::vector<GLuint> shaders;

public:
    using Ptr = std::shared_ptr<Shader>;

    Shader(const char* vs, const char* fs, const char* gs = nullptr);
    Shader(const std::string& vs, const std::string& fs);
    Shader(const std::string& vs, const std::string& fs, const std::string& gs);

    ~Shader();

    inline void use() const { glUseProgram(programId); }

    static Ptr FromFiles(const std::string& vsFile, const std::string& fsFile);
    static Ptr FromFiles(const std::string& vsFile, const std::string& fsFile, const std::string& gsFile);

    // uniform stuff
    void setMat4(const std::string& name, const glm::mat4& value) const;

    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;

    void setInt(const std::string& name, const int& value) const;
    void setFloat(const std::string& name, const float& value) const;

    GLuint getUniformLocation(const std::string& name);

private:
    GLuint createShaderFragment(GLuint type, const char* source);
};


#endif //MODULETWO_SHADER_H
