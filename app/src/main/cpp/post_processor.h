#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include "shader.h"
#include "texture.h"

class post_processor
{
public:
	shader m_post_processor_shader;
	texture m_texture;
	GLuint m_width, m_height;

	GLboolean m_confise, m_chaos, m_shake;

	post_processor(shader shader, GLuint width, GLuint height);

	void begin_render();
	void end_render();
	void render(GLfloat time);

private:
	GLuint FBO;
	GLuint RBO;
	void init_render_data();

    GLuint m_VBO;
    GLuint m_vertex_handle;

	GLuint FTextureID;

	GLint default_frame_buffer;
};


#endif // POST_PROCESSOR


