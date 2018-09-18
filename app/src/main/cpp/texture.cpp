//
// Created by Vladimir on 26.08.2018.
//

#include "texture.h"

texture::texture()
    : m_width(0)
    , m_height(0)
    , m_internal_format(GL_RGB)
    , m_image_format(GL_RGB)
    , m_wrap_t(GL_REPEAT)
    , m_wrap_s(GL_REPEAT)
    , m_filter_min(GL_LINEAR)
    , m_filter_max(GL_LINEAR)
{
    glGenTextures(1, &m_id);
}


void texture::generate(GLuint width, GLuint height, unsigned char* data)
{
    m_width = width;
    m_height = height;

    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, m_image_format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filter_min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filter_max);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, m_id);
}


