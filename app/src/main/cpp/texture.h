//
// Created by Vladimir on 26.08.2018.
//

#ifndef AIRHOCKEY_TEXTURE_H
#define AIRHOCKEY_TEXTURE_H

#include <GLES2/gl2.h>

class texture
{
public:
    GLuint m_id;
    GLuint m_width, m_height;
    GLuint m_internal_format;
    GLuint m_image_format;

    GLuint m_wrap_t;
    GLuint m_wrap_s;
    GLuint m_filter_min;
    GLuint m_filter_max;

    texture();

    void generate(GLuint width, GLuint height, unsigned char* data);

    void bind();

};

#endif //AIRHOCKEY_TEXTURE_H
