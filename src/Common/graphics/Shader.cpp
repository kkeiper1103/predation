//
// Created by kkeiper1103 on 11/6/22.
//

#define STB_INCLUDE_IMPLEMENTATION
#define STB_INCLUDE_LINE_GLSL

#include "Shader.h"

#include <iostream>
#include <iterator>
#include <memory>
#include <map>

#include <glm/gtc/type_ptr.hpp>

std::map<GLuint, const char*> types = {
    {GL_VERTEX_SHADER, "Vertex Shader"},
    {GL_FRAGMENT_SHADER, "Fragment Shader"},
    {GL_GEOMETRY_SHADER, "Geometry Shader"},
};

Shader::Shader(const char *vs, const char *fs, const char *gs) {
    programId = glCreateProgram();

    if(!vs) throw std::runtime_error("Can't Create Shader without VertexShaderSource!");
    if(!fs) throw std::runtime_error("Can't Create Shader without FragmentShaderSource!");

    shaders.push_back(createShaderFragment(GL_VERTEX_SHADER, vs));
    shaders.push_back(createShaderFragment(GL_FRAGMENT_SHADER, fs));

    if(gs) shaders.push_back(createShaderFragment(GL_GEOMETRY_SHADER, gs));

    for(auto& id: shaders)
        glAttachShader(programId, id);

    glLinkProgram(programId);

    //
    int linked;
    glGetProgramiv(programId, GL_LINK_STATUS, &linked);
    if(!linked) {
        char buffer[255];
        glGetProgramInfoLog(programId, sizeof(buffer), nullptr, buffer);

        std::cerr << "Error Linking Program: " << std::endl << buffer << std::endl;
    }

    // no idea
    glUniformBlockBinding(programId, glGetUniformBlockIndex(programId, "matrices"), 0);
}

// delegate constructors
Shader::Shader(const std::string &vs, const std::string &fs) : Shader(vs.c_str(), fs.c_str()){}
Shader::Shader(const std::string &vs, const std::string &fs, const std::string &gs) : Shader(vs.c_str(), fs.c_str(), gs.c_str()) {}

Shader::~Shader() {

    // can't use LOG(INFO) here because the logger is gone when this is called. One of the weaknesses of easylogginpp
    std::cout << "[debug] deleting " << shaders.size() << " shader fragments\n";

    for(auto& shaderId: shaders)
        glDeleteShader(shaderId);

    glDeleteProgram(programId);
}

GLuint Shader::createShaderFragment(GLuint type, const char *source) {
    GLuint id = glCreateShader(type);

    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);

    // error checking
    int compiled;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        char buffer[255];
        glGetShaderInfoLog(id, sizeof(buffer), nullptr, buffer);

        std::cerr << "Error Compiling " << types[type] << ": " << std::endl << buffer << std::endl;
    }

    return id;
}

Shader::Ptr Shader::FromFiles(const std::string &vsFile, const std::string &fsFile) {
    // read vertex source
    char errors[255];
    char* vsSource = stb_include_file((char*) vsFile.data(), "", "resources/shaders", errors);

    // read frag source
    char* fsSource = stb_include_file((char*) fsFile.data(), "", "resources/shaders", errors);

    // make shader
    auto s= std::make_shared<Shader>(vsSource, fsSource);

    // clean up resources
    free(vsSource);
    free(fsSource);

    return s;
}

Shader::Ptr Shader::FromFiles(const std::string &vsFile, const std::string &fsFile, const std::string &gsFile) {
    // read vertex source
    char errors[255];
    char* vsSource = stb_include_file((char*) vsFile.data(), "", "resources/shaders", errors);
    if(strlen(errors) > 0)
        LOG(ERROR) << errors;

    // read frag source
    char* fsSource = stb_include_file((char*) fsFile.data(), "", "resources/shaders", errors);
    if(strlen(errors) > 0)
        LOG(ERROR) << errors;

    // read geometry source
    char* gsSource = stb_include_file((char*) gsFile.data(), "", "resources/shaders", errors);
    if(strlen(errors) > 0)
        LOG(ERROR) << errors;

    // make shader
    auto s= std::make_shared<Shader>(vsSource, fsSource, gsSource);

    // clean up resources
    free(vsSource);
    free(fsSource);
    free(gsSource);

    return s;
}

void Shader::setMat4(const std::string &name, const glm::mat4 &value) const {
    use();
    glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), 1, false, glm::value_ptr(value));
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
    use();
    glUniform2f(glGetUniformLocation(programId, name.c_str()), value.x, value.y);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    use();
    glUniform3f(glGetUniformLocation(programId, name.c_str()), value.x, value.y, value.z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    use();
    glUniform4f(glGetUniformLocation(programId, name.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::setInt(const std::string &name, const int &value) const {
    use();
    glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, const float &value) const {
    use();
    glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
}

GLuint Shader::getUniformLocation(const std::string &name) {
    return glGetUniformLocation(programId, name.c_str());
}
