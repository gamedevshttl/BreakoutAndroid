//
// Created by Vladimir on 01.09.2018.
//

#include "game.h"

#include <GLES2/gl2.h>
#include <memory>
#include <chrono>

#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_level.h"
#include "linmath.h"

std::chrono::time_point<std::chrono::system_clock> last_frame;
game m_game;

void on_surface_created() {
    m_game.init();
}

void on_surface_changed(int width, int height)
{
    m_game.on_surface_changed(width, height);
}

void on_update()
{
    std::chrono::time_point<std::chrono::system_clock> current_frame = std::chrono::system_clock::now();
    int delta_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_frame - last_frame).count();
    last_frame = current_frame;
    float dt = (GLfloat)delta_time/1000;

    m_game.update(dt);

    m_game.render();
}

void set_data_dir(const char *dir)
{
    m_game.set_data_dir(dir);
}

void set_asset_manager(AAssetManager* asset_manager)
{
    m_game.set_asset_manager(asset_manager);
}

void on_touch_press(float x, float y, int idx)
{
    m_game.on_touch_press(x, y, idx);

}

void on_touch_drag(float x, float y, int idx)
{
    m_game.on_touch_drag(x, y, idx);
}

void on_touch_release(float x, float y, int idx)
{
    m_game.on_touch_release(x, y, idx);
}

game::game()
    : m_player_velocity(500.0f)
    , m_action(none)

{}


void game::init()
{
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

void game::set_asset_manager(AAssetManager* asset_manager)
{
    m_asset_manager = asset_manager;
}

void game::set_data_dir(std::string current_dir)
{
    m_current_dir = current_dir;
}

void game::on_surface_changed(int width, int height)
{
    glViewport(0, 0, width, height);

    m_width = width, m_height = height;

    mat4x4_ortho(projection, 0, m_width, m_height, 0, -1.0f, 1.0f);

    resource_manager::get_shader("sprite").use().set_matrix4f("projection", projection);

    game_level one;
    one.load("data/level_one.lvl", m_width, m_height * 0.3);

    m_player_size[0] = m_width / 8;
    m_player_size[1] = m_height / 30;

    vec2 player_pos{m_width/2 - m_player_size[0]/2, m_height - m_player_size[1]};

    m_player = std::make_shared<game_object>(player_pos, m_player_size, resource_manager::get_texture("paddle"));

    m_game_levels.push_back(one);
}

void game::update(GLfloat dt)
{
    GLfloat velocity = m_player_velocity * dt;
    if(m_action == left){
        if (m_player->m_position[0] > 0) {
            m_player->m_position[0] -= velocity;
        }
    }
    else if(m_action == right){
        if (m_player->m_position[0] < m_width - m_player->m_size[0]) {
            m_player->m_position[0] += velocity;
        }
    }
}

void game::render()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if (m_game_levels.size() > level_idx)
        m_game_levels[level_idx].draw(*m_sprite_renderer);

    m_player->draw(*m_sprite_renderer);
}

void game::on_touch_press(float x, float y, int idx)
{
    if (idx > 0)
        return;

    if(x > 0.5)
        m_action = right;
    else
        m_action = left;
}

void game::on_touch_drag(float x, float y, int idx)
{
    if (idx > 0)
        return;
}

void game::on_touch_release(float x, float y, int idx)
{
    if (idx > 0)
        return;

    m_action = none;
}

