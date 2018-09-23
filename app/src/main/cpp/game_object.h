#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "linmath.h"

#include "texture.h"
#include "sprite_renderer.h"

class game_object
{
public:
	game_object();
	game_object(vec2 position, vec2 size, texture sprite, vec3 color = vec3{1.0f, 1.0f, 1.0f}, vec2 velocity = vec2{0.0f, 0.0f});
	virtual void draw(sprite_renderer& renderer);

	vec2 m_position, m_size, m_velocity;
	vec3 m_color;
	GLfloat m_rotation;
	GLboolean m_solid;
	GLboolean m_destroyed;
	texture m_sprite;
};

#endif