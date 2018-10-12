#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include <vector>

#include "shader.h"
#include "texture.h"
#include "game_object.h"

struct particle {
	particle():m_position{0.0f, 0.0f}, m_velosity{0.0f, 0.0f}, m_color{1.0f, 1.0f, 1.0f, 1.0f}, m_life(0.0f)
	{}

	vec2 m_position, m_velosity;
	vec4 m_color;
	GLfloat m_life;
};


class particle_generator
{
public:
	particle_generator(shader _shader, texture _texture, GLuint amount);
	void update(GLfloat dt, const game_object &object, GLuint new_particle, vec2 offset);
	void draw(sprite_renderer& renderer);
private:
	void init();
	GLuint first_unsed_particle();
	void respawn_particle(particle &particle_item, const game_object &object, vec2 offset);

	std::vector<particle> m_particles;
	GLuint m_amount;
	shader m_shader;
	texture m_texture;
	GLuint m_VBO;
	GLuint m_vertex_handle;

	GLuint m_last_used_particle;
};
#endif