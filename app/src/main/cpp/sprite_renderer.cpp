//
// Created by Vladimir on 29.08.2018.
//

#include "sprite_renderer.h"
#include "linmath.h"
#include <android/log.h>

//sprite_renderer::sprite_renderer(shader& shader_item)
//{
//    m_shader_item = shader_item;
//    init_render_data();
//}

sprite_renderer::sprite_renderer(shader _shader) {
    m_shader_item = _shader;
    init_render_data();
}

void sprite_renderer::draw_sprite(texture& texture_item, vec2 position, vec2 size, const vec3& color)
{
    mat4x4 model;
    mat4x4_identity(model);
    mat4x4_translate_in_place(model, position[0], position[1], 0);

    mat4x4_scale_aniso(model, model, size[0], size[1], 1);

    m_shader_item.use();
    m_shader_item.set_matrix4f("model", model);
    m_shader_item.set_vector3f("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    texture_item.bind();

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glEnableVertexAttribArray(gvPositionHandle);
    glVertexAttribPointer(gvPositionHandle, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void sprite_renderer::init_render_data()
{
    GLfloat vertices[] = {
            // Pos      // Tex
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,

            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f
    };

    gvPositionHandle = glGetAttribLocation(m_shader_item.get_id(), "vPosition");
    __android_log_print(ANDROID_LOG_INFO, "Breakout", "glGetAttribLocation(\"vPosition\") = %d\n",
                        gvPositionHandle);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(gvPositionHandle);
    glVertexAttribPointer(gvPositionHandle, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}






