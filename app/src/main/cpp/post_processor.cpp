#include "post_processor.h"

#include <android/log.h>

#include <iostream>
#include <GLES2/gl2.h>

post_processor::post_processor(shader shader, GLuint width, GLuint height)
	: m_post_processor_shader(shader)
	, m_texture()
	, m_width(width)
	, m_height(height)
	, m_confise(GL_FALSE)
	, m_chaos(GL_FALSE)
	, m_shake(GL_FALSE)
    , default_frame_buffer(0)
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &FTextureID);
    glBindTexture(GL_TEXTURE_2D, FTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FTextureID, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	init_render_data();

	m_post_processor_shader.set_int("scene", 0, GL_TRUE);
	GLfloat offset = 1.0f / 300.0f;
	GLfloat offsets[9][2] = {
		{ -offset,  offset },  // top-left
		{ 0.0f,    offset },  // top-center
		{ offset,  offset },  // top-right
		{ -offset,  0.0f },  // center-left
		{ 0.0f,    0.0f },  // center-center
		{ offset,  0.0f },  // center - right
		{ -offset, -offset },  // bottom-left
		{ 0.0f,   -offset },  // bottom-center
		{ offset, -offset }   // bottom-right
	};
	glUniform2fv(glGetUniformLocation(m_post_processor_shader.get_id(), "offsets"), 9, (GLfloat*)offsets);

	GLfloat edge_kernel[9] = {
		-1.0, -1.0, -1.0,
		-1.0,  8.0, -1.0,
		-1.0, -1.0, -1.0
	};
	glUniform1fv(glGetUniformLocation(m_post_processor_shader.get_id(), "edge_kernel"), 9, edge_kernel);

	GLfloat blur_kernel[9] = {
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16
	};
	glUniform1fv(glGetUniformLocation(m_post_processor_shader.get_id(), "blur_kernel"), 9, blur_kernel);
}

void post_processor::begin_render()
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &default_frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void post_processor::end_render()
{
   glBindFramebuffer(GL_FRAMEBUFFER, default_frame_buffer);
}

void post_processor::render(GLfloat time)
{
	m_post_processor_shader.use();
	m_post_processor_shader.set_float("time", time);
	m_post_processor_shader.set_int("confuse", m_confise);
	m_post_processor_shader.set_int("chaos", m_chaos);
	m_post_processor_shader.set_int("shake", m_shake);

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, FTextureID);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glEnableVertexAttribArray(m_vertex_handle);
    glVertexAttribPointer(m_vertex_handle, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void post_processor::init_render_data()
{
	GLfloat vertices[] = {
		// Pos        // Tex
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f
	};

    m_vertex_handle = glGetAttribLocation(m_post_processor_shader.get_id(), "vertex");
    __android_log_print(ANDROID_LOG_INFO, "[Breakout][post_processor]", "glGetAttribLocation(\"vertex\") = %d\n",
                        m_vertex_handle);

    glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glEnableVertexAttribArray(m_vertex_handle);
	glVertexAttribPointer(m_vertex_handle, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}