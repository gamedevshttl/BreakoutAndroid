#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "texture.h"
#include "sprite_renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class game_object
{
public:
	game_object();
	game_object(glm::vec2 position, glm::vec2 size, texture sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	virtual void draw(sprite_renderer& renderer);

	glm::vec2 m_position, m_size, m_velocity;
	glm::vec3 m_color;

	GLfloat m_rotation;
	GLboolean m_solid;
	GLboolean m_destroyed;
	texture m_sprite;
};

#endif