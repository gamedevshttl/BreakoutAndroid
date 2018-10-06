//
// Created by Vladimir on 01.09.2018.
//

#include "game.h"

#include <GLES2/gl2.h>
#include <memory>
#include <chrono>
#include <cmath>

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
    : m_player_velocity(PLAYER_VELOCITY)
    , m_action(none)
    , m_ball_radius(BALL_RADIUS)
{
    std::copy(INITIAL_BALL_VELOCITY.begin(), INITIAL_BALL_VELOCITY.end(), m_ball_velocity);
}


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

    m_levels.push_back(one);

    vec2 ball_pos{player_pos[0] + m_player_size[0]/2 - m_ball_radius/2, player_pos[1] -m_ball_radius*2};
    m_ball = std::make_shared<ball_object>(ball_pos, m_ball_radius, m_ball_velocity, resource_manager::get_texture("fase"));
}

void game::update(GLfloat dt)
{
    m_ball->move(dt, m_width);
    do_collision();

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
}

void game::render()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if (m_levels.size() > m_current_level)
        m_levels[m_current_level].draw(*m_sprite_renderer);

    m_player->draw(*m_sprite_renderer);
    m_ball->draw(*m_sprite_renderer);
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

Direction vector_direction(vec2 target)
{
    vec2 compass[] = {
            {0.0f, 1.0f},
            {1.0f, 0.0f},
            {0.0f, -1.0f},
            {-1.0f, 0.0f},
    };

    GLfloat max = 0.0f;
    GLuint best_match = -1;

    for (GLuint i = 0; i < 4; ++i) {

        vec2 norm_target;
        vec2_norm(norm_target, target);

        GLfloat dot_product = vec2_mul_inner(norm_target, compass[i]);
        if (dot_product > max) {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}


Collision check_collision(const ball_object& lhs, const game_object& rhs)
{
    vec2 center{lhs.m_position[0] + lhs.m_radius, lhs.m_position[1] + lhs.m_radius};

    vec2 aabb_half_extension{rhs.m_size[0] / 2, rhs.m_size[1] / 2};

    vec2 aabb_center;
    vec2_add(aabb_center, rhs.m_position, aabb_half_extension);

    vec2 difference;
    vec2_sub(difference, center, aabb_center);

    vec2 max_vec{max(difference[0], -aabb_half_extension[0]), max(difference[1], -aabb_half_extension[1])};

    vec2 clamped;
    vec2_min(clamped, max_vec, aabb_half_extension);

    vec2 closest;
    vec2_add(closest, aabb_center, clamped);

    vec2_sub(difference, closest, center);

    std::array<float,2> array = {difference[0], difference[1]};

    if (vec2_len(difference) < lhs.m_radius)
        return std::make_tuple(GL_TRUE, vector_direction(difference), array);
    else
        return std::make_tuple(GL_FALSE, UP, std::array<float,2>{0.0f, 0.0f});
}

void game::do_collision()
{
    for (game_object& box : m_levels[m_current_level].m_briks) {
        if (!box.m_destroyed) {
            Collision collision = check_collision(*m_ball, box);

            if(std::get<0>(collision)) {
                if (!box.m_solid)
                    box.m_destroyed = true;

                Direction dir = std::get<1>(collision);
                std::array<float,2> diff_vector = std::get<2>(collision);

                if (dir == LEFT || dir == RIGHT) {
                    m_ball->m_velocity[0] = -m_ball->m_velocity[0];

                    GLfloat penetration = m_ball->m_radius - std::abs(diff_vector[0]);
                    if (dir == LEFT)
                        m_ball->m_position[0] += penetration;
                    else if (dir == RIGHT)
                        m_ball->m_position[0] -= penetration;
                }
                else if (dir == UP || dir == DOWN){
                    m_ball->m_velocity[1] = -m_ball->m_velocity[1];

                    GLfloat penetration = m_ball->m_radius - std::abs(diff_vector[1]);
                    if (dir == UP)
                        m_ball->m_position[1] -= penetration;
                    else if (dir == DOWN)
                        m_ball->m_position[1] += penetration;
                }
            }
        }
    }

    Collision result = check_collision(*m_ball, *m_player);
    if (!m_ball->m_stuck && std::get<0>(result)) {
        GLfloat center_board = m_player->m_position[0] + m_player->m_size[0]/2;
        GLfloat distance = (m_ball->m_position[0] + m_ball->m_radius) - center_board;
        GLfloat percentage = distance / (m_player->m_size[0]/2);

        GLfloat strenght = 2.0f;
        vec2 old_velosity;
        std::copy(m_ball->m_velocity, m_ball->m_velocity+2, old_velosity);
        m_ball->m_velocity[0] = INITIAL_BALL_VELOCITY[0] * percentage * strenght;

        vec2 norm_velocity;
        vec2_norm(norm_velocity, m_ball->m_velocity);
        vec2_scale(m_ball->m_velocity, norm_velocity, vec2_len(old_velosity));
        m_ball->m_velocity[1] = -1 * std::abs(m_ball->m_velocity[1]);
    }
}

void game::reset_level()
{
    m_levels[m_current_level].reset();
}

void game::reset_player()
{
    std::copy(m_player_size, m_player_size+2, m_player->m_size);

    m_player->m_position[0] = m_width/2 - m_player->m_size[0]/2;
    m_player->m_position[1] = m_height - m_player->m_size[1];

    vec2 ball_velocity{INITIAL_BALL_VELOCITY[0], INITIAL_BALL_VELOCITY[1]};
    m_ball->reset(vec2{m_player->m_position[0] + m_player->m_size[0]/2 - m_ball->m_radius/2, m_player->m_position[1] - m_ball_radius * 2}, ball_velocity);
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

