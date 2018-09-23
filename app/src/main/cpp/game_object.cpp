#include "game_object.h"
#include <algorithm>

game_object::game_object()
	: m_position{0.0f,0.0f}
	, m_size{1.0f,1.0f}
	, m_velocity{0.0f, 0.0f}
	, m_color{1.0f, 1.0f, 1.0f}
	, m_rotation(0.0f)
	, m_sprite()
	, m_solid(false)
	, m_destroyed(false)
{}

game_object::game_object(vec2 position, vec2 size, texture sprite, vec3 color, vec2 velocity)
	: m_rotation(0.0f)
	, m_sprite(sprite)
	, m_solid(false)
	, m_destroyed(false)
{
    std::copy(position, position+2, m_position);
    std::copy(size, size+2, m_size);
    std::copy(color, color+3, m_color);
    std::copy(velocity, velocity+2, m_velocity);
}

void game_object::draw(sprite_renderer& renderer)
{
	renderer.draw_sprite(m_sprite, m_position, m_size, m_color);
}
