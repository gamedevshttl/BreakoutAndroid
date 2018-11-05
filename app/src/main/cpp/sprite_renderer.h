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
    sprite_renderer(shader _shader);
    void draw_sprite(texture& texture_item, glm::vec2 position, glm::vec2 size = glm::vec2(10,10), glm::vec3 color = glm::vec3(1.0f));
private:
    shader m_shader_item;

    GLuint m_VBO;
    void init_render_data();

    GLuint gvPositionHandle;
};


#endif //AIRHOCKEY_SPRITE_RENDERER_H
