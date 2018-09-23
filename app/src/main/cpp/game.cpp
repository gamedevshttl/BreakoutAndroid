//
// Created by Vladimir on 01.09.2018.
//

#include "game.h"

#include <GLES2/gl2.h>
#include <memory>

#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_level.h"
#include "linmath.h"

std::shared_ptr<sprite_renderer> m_sprite_renderer;
std::string m_current_dir;
AAssetManager* m_asset_manager;

int m_width, m_height;
mat4x4 projection;
std::vector<game_level> m_game_levels;
GLuint level_idx = 0;

void on_surface_created() {
    resource_manager::set_data_dir(m_current_dir);
    resource_manager::set_asset_manager(m_asset_manager);

    resource_manager::load_texture("textures/awesomeface.png", GL_TRUE, "fase");
    resource_manager::load_texture("textures/block.png", GL_TRUE, "block");
    resource_manager::load_texture("textures/block_solid.png", GL_TRUE, "block_solid");
    resource_manager::load_texture("textures/paddle.png", GL_TRUE, "paddle");

    resource_manager::load_shader("shaders/sprite.vs", "shaders/sprite.fs", "sprite");

    m_sprite_renderer = std::make_shared<sprite_renderer>(resource_manager::get_shader("sprite"));

    mat4x4_ortho(projection, 0, m_width, m_height, 0, -1.0f, 1.0f);

    resource_manager::get_shader("sprite").use().set_matrix4f("projection", projection);
    resource_manager::get_shader("sprite").use().set_int("image", 0);

}

void on_surface_changed(int width, int height)
{
    glViewport(0, 0, width, height);

    m_width = width, m_height = height;

    mat4x4_ortho(projection, 0, m_width, m_height, 0, -1.0f, 1.0f);

    resource_manager::get_shader("sprite").use().set_matrix4f("projection", projection);

    game_level one;
    one.load("data/level_one.lvl", m_width, m_height * 0.3);

    m_game_levels.push_back(one);
}

void on_draw_frame()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    vec2 position = {10, 10};
    vec2 size = {100, 100};
    vec3 color = {1.0, 0.0, 0.0};

    m_sprite_renderer->draw_sprite(resource_manager::get_texture("fase"),  position, size, color);

    if (m_game_levels.size() > level_idx)
        m_game_levels[level_idx].draw(*m_sprite_renderer);
}

void set_data_dir(const char *dir)
{
    m_current_dir = dir;
}

void set_asset_manager(AAssetManager* asset_manager)
{
    m_asset_manager = asset_manager;
}