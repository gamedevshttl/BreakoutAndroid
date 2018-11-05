#include "game_level.h"
#include "resource_manager.h"

#include <fstream>
#include <sstream>
#include <string>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

game_level::game_level()
{}

void game_level::load(const GLchar* file, GLuint level_width, GLuint level_height)
{
	std::string file_content = resource_manager::load_file(file);

	std::stringstream ss(file_content.c_str());
	std::string line;
    GLuint tile_code;

    std::vector<std::vector<GLuint>> tile_data;

	while(std::getline(ss,line,'\n')){


        std::istringstream sstream(line);
        std::vector<GLuint> row;
        while(sstream >> tile_code)
            row.push_back(tile_code);
        tile_data.push_back(row);
	}

    if(tile_data.size() > 0)
        init(tile_data, level_width, level_height);
}

void game_level::draw(sprite_renderer& renderer)
{
	for (auto& brick : m_briks)
		if (!brick.m_destroyed)
			brick.draw(renderer);
}

GLboolean game_level::is_completed()
{
	return false;
}

void game_level::init(const std::vector<std::vector<GLuint>>& tile_data, GLuint level_width, GLuint level_height)
{
	if (tile_data.empty())
		return;

	GLuint height = tile_data.size();
	GLuint width = tile_data[0].size();

	GLfloat unit_width = level_width / static_cast<GLfloat>(width);
	GLfloat unit_height = level_height / static_cast<GLfloat>(height);

	for (GLuint y = 0; y < height; ++y) {
		for (GLuint x = 0; x < width; ++x) {
			glm::vec2 pos(unit_width * x, unit_height * y);
			glm::vec2 size(unit_width, unit_height);

			if (tile_data[y][x] == 1) {
				game_object obj(pos, size, resource_manager::get_texture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
				obj.m_solid = true;
				m_briks.push_back(obj);
			}
			else if(tile_data[y][x] > 1){
				glm::vec3 color(1.0f);
				if (tile_data[y][x] == 2)
					color = glm::vec3(0.2f, 0.6f, 1.0f);
				else if (tile_data[y][x] == 3)
					color = glm::vec3(0.0f, 0.7f, 0.0f);
				else if (tile_data[y][x] == 4)
					color = glm::vec3(0.8f, 0.8f, 0.4f);
				else if (tile_data[y][x] == 5)
					color = glm::vec3(1.0f, 0.5f, 0.0f);

				m_briks.push_back(game_object(pos, size, resource_manager::get_texture("block"), color));
			}
		}
	}
}

void game_level::reset()
{
	for (auto& brick : m_briks)
		brick.m_destroyed = false;
}
