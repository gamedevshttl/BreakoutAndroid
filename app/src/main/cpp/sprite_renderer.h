//
// Created by Vladimir on 29.08.2018.
//

#ifndef AIRHOCKEY_SPRITE_RENDERER_H
#define AIRHOCKEY_SPRITE_RENDERER_H

#include "shader.h"
#include "texture.h"

class sprite_renderer
{
public:
//    sprite_renderer(shader& shader_item);

    sprite_renderer(shader _shader);

    void draw_sprite(texture& texture_item, vec2 position, vec2 size, const vec3& color);
private:

    shader m_shader_item;

    //GLuint m_VAO;
    GLuint m_VBO;
    void init_render_data();

    GLuint gvPositionHandle;
};


#endif //AIRHOCKEY_SPRITE_RENDERER_H
