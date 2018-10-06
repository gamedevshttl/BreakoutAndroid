#include "ball_object.h"

ball_object::ball_object()
	: game_object()
	, m_radius(12.5f)
	, m_stuck(true)
{}

ball_object::ball_object(vec2 pos, GLfloat radius, vec2 velocity, texture sprite)
	: game_object(pos, vec2{radius* 2, radius * 2}, sprite, vec3{1.0f, 1.0f, 1.0f}, velocity)
	, m_radius(radius)
	, m_stuck(true)
{}

void ball_object::move(GLfloat dt, GLuint window_width)
{
	if (!m_stuck) {
		m_position[0] += m_velocity[0] * dt;
		m_position[1] += m_velocity[1] * dt;

		if (m_position[0] < 0.0f){
			m_velocity[0] = -m_velocity[0];
			m_position[0] = 0.0f;
		}
		else if (m_position[0] + m_size[0] >= window_width) {
			m_velocity[0] = -m_velocity[0];
			m_position[0] = window_width - m_size[0];
		}
		else if (m_position[1] <= 0.0f) {
			m_velocity[1] = -m_velocity[1];
			m_position[1] = 0.0f;
		}
	}
}

void ball_object::reset(vec2 position, vec2 velocity)
{
	std::copy(position, position+2, m_position);
	std::copy(velocity, velocity+2, m_velocity);

	m_stuck = true;
}
