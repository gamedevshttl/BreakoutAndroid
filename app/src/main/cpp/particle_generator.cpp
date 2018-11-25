#include "particle_generator.h"
#include <android/log.h>

#include <cstdlib>

particle_generator::particle_generator(shader _shader, texture _texture, GLuint amount, GLfloat scale)

	: m_amount(amount)
	, m_last_used_particle(0)
    , m_scale(scale)
{
    m_shader = _shader;
    m_texture = _texture;
	init();
}

void particle_generator::update(GLfloat dt, const game_object &object, GLuint new_particle, glm::vec2 offset)
{
    if (dt < 0)
        return;

    if (dt > 10)
        return;

	for (GLuint i = 0; i < new_particle; ++i) {
		int unused_particle = first_unsed_particle();
        if(unused_particle<m_particles.size())
			respawn_particle(m_particles[unused_particle], object, offset);
	}


	for (auto &item : m_particles) {
		item.m_life -= dt;
		if (item.m_life > 0.0f) {
			for(int i = 0; i<2; ++i)
				item.m_position[i] -= item.m_velosity[i] * dt;

			item.m_color[3] -= dt * 1.5;
		}
	}
}

void particle_generator::draw(sprite_renderer& renderer)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	for (const auto &item : m_particles) {
		if (item.m_life > 0.0f) {

            m_shader.use();
			m_shader.set_vector_2f("offset", item.m_position);
			m_shader.set_vector_4f("color", item.m_color);
			m_shader.set_float("scale", m_scale);

			glActiveTexture(GL_TEXTURE0);
			m_texture.bind();

			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
			glEnableVertexAttribArray(m_vertex_handle);
			glVertexAttribPointer(m_vertex_handle, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

			glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void particle_generator::init()
{

	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	m_vertex_handle = glGetAttribLocation(m_shader.get_id(), "vertex");
	__android_log_print(ANDROID_LOG_INFO, "Breakout", "glGetAttribLocation(\"m_vertex_handle\") = %d\n",
                        m_vertex_handle);


	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(m_vertex_handle);
	glVertexAttribPointer(m_vertex_handle, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (GLuint i = 0; i < m_amount; ++i)
		m_particles.push_back(particle());
}

GLuint particle_generator::first_unsed_particle()
{
	for (GLuint i = m_last_used_particle; i < m_amount; ++i) {
		if (m_particles[i].m_life <= 0.0f) {
			m_last_used_particle = i;
			return i;
		}
	}

	m_last_used_particle = 0;
	return 0;
}

void particle_generator::respawn_particle(particle &particle_item, const game_object &object, glm::vec2 offset)
{
	GLfloat random = ((rand() % 100) - 50) / offset.x * 4;
	GLfloat color = 0.5 + ((rand() % 100) / 100.0f);

    particle_item.m_position = object.m_position + random + offset;
    particle_item.m_color = glm::vec4(color, color, color, 1.0f);
    particle_item.m_life = 0.5f;
    particle_item.m_velosity = object.m_velocity * 0.1f;
}