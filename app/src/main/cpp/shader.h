//
// Created by Vladimir on 11.08.2018.
//

#ifndef AIRHOCKEY_SHADER_H
#define AIRHOCKEY_SHADER_H


#include <GLES2/gl2.h>
#include <string>

#include "linmath.h"

class shader {

public:
//    shader();
    shader& use();
    void compile(const GLchar* vertex_source,  const GLchar* fragment_source, const GLchar* geometry_shader = nullptr);
    GLuint get_id(){return m_id;}

    void set_matrix4f(const GLchar* name, const mat4x4& matrix, GLboolean use_shader = false);
    void set_int(const GLchar* name, GLuint value, GLboolean use_shader = false);
    void set_vector3f(const GLchar* name, const vec3& value, GLboolean use_shader = false);
    void set_vector2f(const GLchar* name, const vec2& value, GLboolean use_shader = false);
    void set_vector4f(const GLchar* name, const vec4& value, GLboolean use_shader = false);
private:
    void check_compile_error(GLuint object, const std::string& type);

    GLuint m_id;
};


#endif //AIRHOCKEY_SHADER_H
