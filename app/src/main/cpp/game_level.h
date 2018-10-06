#ifndef GAMELEVEL_H
#define GAMELEVEL_H


#include "sprite_renderer.h"
#include "game_object.h"
#include <vector>

class game_level
{
public:
	game_level();
	void load(const GLchar* file, GLuint level_width, GLuint level_height);
	void draw(sprite_renderer& renderer);
	GLboolean is_completed();
	void reset();
	
	std::vector<game_object> m_briks;

private:
	void init(const std::vector<std::vector<GLuint>>& tile_data, GLuint level_width, GLuint level_height);
};

#endif