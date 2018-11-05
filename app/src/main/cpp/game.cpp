//
// Created by Vladimir on 01.09.2018.
//

#include "game.h"

#include <GLES2/gl2.h>
#include <memory>
#include <chrono>
#include <cmath>
#include <android/log.h>

#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_level.h"
#include "particle_generator.h"
#include "post_processor.h"



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
    : m_player_velocity(PLAYER_VELOCITY)
    , m_action(none)
    , m_ball_radius(BALL_RADIUS)
    , m_shake_time(0)
    , m_ball_velocity(INITIAL_BALL_VELOCITY)
{
}


void game::init()
{
    resource_manager::set_data_dir(m_current_dir);
    resource_manager::set_asset_manager(m_asset_manager);

    resource_manager::load_texture("textures/awesomeface.png", GL_TRUE, "fase");
    resource_manager::load_texture("textures/block.png", GL_TRUE, "block");
    resource_manager::load_texture("textures/block_solid.png", GL_TRUE, "block_solid");
    resource_manager::load_texture("textures/paddle.png", GL_TRUE, "paddle");
    resource_manager::load_texture("textures/particle.png", GL_TRUE, "particle");

    resource_manager::load_shader("shaders/particle.vs", "shaders/particle.fs", "particle");
    resource_manager::load_shader("shaders/sprite.vs", "shaders/sprite.fs", "sprite");
    resource_manager::load_shader("shaders/post_processor.vs", "shaders/post_processor.fs", "postprocessing");

    m_sprite_renderer = std::make_shared<sprite_renderer>(resource_manager::get_shader("sprite"));

    resource_manager::get_shader("sprite").use().set_int("image", 0);
    resource_manager::get_shader("particle").use().set_int("image", 0);

    m_particle_generator = std::make_shared<particle_generator>(resource_manager::get_shader("particle"),
                                                                resource_manager::get_texture("particle"),
                                                                500);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    projection = glm::ortho(0.0f, static_cast<GLfloat>(m_width), static_cast<GLfloat>(m_height), 0.0f, -1.0f, 1.0f);

    resource_manager::get_shader("sprite").use().set_matrix4f("projection", projection);
    resource_manager::get_shader("sprite").use().set_matrix4f("projection", projection);
    resource_manager::get_shader("particle").use().set_matrix4f("projection", projection);

    game_level one;
    one.load("data/level_one.json", m_width, m_height * 0.3);
    m_levels.push_back(one);

    m_player_size = glm::vec2(width / 8, height / 30);

    glm::vec2 player_pos = glm::vec2(m_width/2 - m_player_size.x/2, m_height - m_player_size.y);
    m_player = std::make_shared<game_object>(player_pos, m_player_size, resource_manager::get_texture("paddle"));

    glm::vec2 ball_pos = player_pos + glm::vec2(m_player_size.x / 2 - m_ball_radius / 2, -m_ball_radius * 2);
    m_ball = std::make_shared<ball_object>(ball_pos, m_ball_radius, m_ball_velocity, resource_manager::get_texture("fase"));

    m_post_processor = std::make_shared<post_processor>(resource_manager::get_shader("postprocessing"), m_width, m_height);
}

void game::update(GLfloat dt)
{
    if(m_ball == nullptr)
        return;

    m_ball->move(dt, m_width);
    do_collision();
    m_particle_generator->update(dt, *m_ball, 2, glm::vec2(m_ball->m_radius/2));

    GLfloat velocity = m_player_velocity * dt;
    if(m_action == left){
        if (m_player->m_position[0] > 0) {
            m_player->m_position[0] -= velocity;

            if(m_ball->m_stuck)
                m_ball->m_position[0] -= velocity;
        }
    }
    else if(m_action == right){
        if (m_player->m_position[0] < m_width - m_player->m_size[0]) {
            m_player->m_position[0] += velocity;

            if(m_ball->m_stuck)
                m_ball->m_position[0] += velocity;
        }
    }

    if (m_ball->m_position[1] >= m_height) {
        reset_level();
        reset_player();
        m_action = none;
    }

    if (m_shake_time > 0.0f) {
        m_shake_time -= dt;
        if (m_shake_time < 0.0f)
            m_post_processor->m_shake = GL_FALSE;
    }
}

void game::render()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_post_processor->begin_render();

    {
        if (m_levels.size() > m_current_level)
            m_levels[m_current_level].draw(*m_sprite_renderer);


        m_player->draw(*m_sprite_renderer);
        m_particle_generator->draw(*m_sprite_renderer);
        m_ball->draw(*m_sprite_renderer);
    }

    m_post_processor->end_render();

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration tp = now.time_since_epoch();
    std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(tp);
    std::chrono::hours h = std::chrono::duration_cast<std::chrono::hours>(tp);

    std::chrono::seconds diff_s = s - h;

    m_post_processor->render(diff_s.count());


}

float max(float x, float y)
{
    if(x > y)
        return x;
    else
        return y;
}

float min(float x, float y)
{
    if(x < y)
        return x;
    else
        return y;
}

Direction vector_direction(glm::vec2 target)
{
    glm::vec2 compass[] = {
            glm::vec2(0.0f, 1.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(0.0f, -1.0f),
            glm::vec2(-1.0f, 0.0f),
    };

    GLfloat max = 0.0f;
    GLuint best_match = -1;

    for (GLuint i = 0; i < 4; ++i) {
        GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max) {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

Collision check_collision(const ball_object& lhs, const game_object& rhs)
{
    glm::vec2 center(lhs.m_position + lhs.m_radius);

    glm::vec2 aabb_half_extension(rhs.m_size.x / 2, rhs.m_size.y / 2);
    glm::vec2 aabb_center(rhs.m_position.x + aabb_half_extension.x, rhs.m_position.y + aabb_half_extension.y);

    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extension, aabb_half_extension);

    glm::vec2 closest = aabb_center + clamped;
    difference = closest - center;

    if (glm::length(difference) < lhs.m_radius)
        return std::make_tuple(GL_TRUE, vector_direction(difference), difference);
    else
        return std::make_tuple(GL_FALSE, UP, glm::vec2(0,0));
}

void game::do_collision()
{
    for (game_object& box : m_levels[m_current_level].m_briks) {
        if (!box.m_destroyed) {
            Collision collision = check_collision(*m_ball, box);
            if(std::get<0>(collision)){
                if (!box.m_solid) {
                    box.m_destroyed = true;
                }
                else {
                    m_shake_time = 0.1;
                    m_post_processor->m_shake = GL_TRUE;
                }

                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);

                if (dir == LEFT || dir == RIGHT) {
                    m_ball->m_velocity.x = -m_ball->m_velocity.x;

                    GLfloat penetration = m_ball->m_radius - std::abs(diff_vector.x);
                    if (dir == LEFT)
                        m_ball->m_position.x += penetration;
                    else if (dir == RIGHT)
                        m_ball->m_position.x -= penetration;
                }
                else if (dir == UP || dir == DOWN) {
                    m_ball->m_velocity.y = -m_ball->m_velocity.y;

                    GLfloat penetration = m_ball->m_radius - std::abs(diff_vector.y);
                    if (dir == UP)
                        m_ball->m_position.y -= penetration;
                    else if (dir == DOWN)
                        m_ball->m_position.y += penetration;
                }

            }
        }
    }

    Collision result = check_collision(*m_ball, *m_player);
    if (!m_ball->m_stuck && std::get<0>(result)) {
        GLfloat center_board = m_player->m_position.x + m_player->m_size.x/2;
        GLfloat distance = (m_ball->m_position.x + m_ball->m_radius) - center_board;
        GLfloat percentage = distance / (m_player->m_size.x/2);

        GLfloat strenght = 2.0f;
        glm::vec2 old_velosity = m_ball->m_velocity;
        m_ball->m_velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strenght;
        m_ball->m_velocity = glm::normalize(m_ball->m_velocity) * glm::length(old_velosity);
        m_ball->m_velocity.y = -1 * std::abs(m_ball->m_velocity.y);
    }
}

void game::reset_level()
{
    m_levels[m_current_level].reset();
}

void game::reset_player()
{
    m_player->m_size = m_player_size;
    m_player->m_position = glm::vec2(m_width/2 - m_player->m_size.x/2, m_height - m_player->m_size.y);
    m_ball->reset(m_player->m_position + glm::vec2(m_player->m_size.x/2 - m_ball->m_radius/2, -m_ball_radius * 2), INITIAL_BALL_VELOCITY);
}

void game::on_touch_press(float x, float y, int idx)
{
    if (idx > 0)
        return;

    if (m_ball->m_stuck){
        m_ball->m_stuck = false;
        return;
    }


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

//    m_action = none;
}

