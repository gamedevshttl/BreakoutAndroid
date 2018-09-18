//
// Created by Vladimir on 11.08.2018.
//

#include "shader.h"

#include <iostream>

//shader::shader()
//{
//
//}

shader& shader::use()
{
    glUseProgram(m_id);
    return *this;
}

void shader::set_matrix4f(const GLchar* name, const mat4x4& matrix, GLboolean use_shader)
{
    if(use_shader)
        use();

    glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, (GLfloat*)matrix);
}

void shader::set_int(const GLchar* name, GLuint value, GLboolean use_shader)
{
    if(use_shader)
        use();

    glUniform1f(glGetUniformLocation(m_id, name), value);
}

void shader::set_vector3f(const GLchar* name, const vec3& value, GLboolean use_shader)
{
    if(use_shader)
        use();

    glUniform3f(glGetUniformLocation(m_id, name), value[0], value[1], value[2]);
}

void shader::compile(const GLchar* vertex_source,  const GLchar* fragment_source, const GLchar* geometry_source)
{
    GLuint  s_vertex, s_fragment, s_geometry;

    s_vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(s_vertex, 1, &vertex_source, NULL);
    glCompileShader(s_vertex);
    check_compile_error(s_vertex, "vertex");

    s_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(s_fragment, 1, &fragment_source, NULL);
    glCompileShader(s_fragment);
    check_compile_error(s_fragment, "vertex");

//    if (geometry_source) {
//        s_geometry = glCreateShader();
//        glShaderSource(s_geometry, 1, &geometry_source, NULL);
//        glCompileShader(s_geometry);
//        check_compile_error(s_geometry, "vertex");
//    }

    m_id = glCreateProgram();
    glAttachShader(m_id, s_vertex);
    glAttachShader(m_id, s_fragment);
    glLinkProgram(m_id);
    check_compile_error(m_id, "programm");

    glDeleteShader(s_vertex);
    glDeleteShader(s_fragment);
}

void shader::check_compile_error(GLuint object, const std::string& type)
{
    GLint success;
    GLchar info_log[1024];

    if(type != "programm"){
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(object, 1024, NULL, info_log);
            std::cout<<type <<": "<<info_log;
        }
    }
    else{
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if(!success){
            glGetProgramInfoLog(object, 1024, NULL, info_log);
            std::cout<<type <<": "<<info_log;
        }
    }
}