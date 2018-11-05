//
// Created by Vladimir on 01.09.2018.
//

#include "game.h"

#include <GLES2/gl2.h>
#include <memory>
#include <chrono>
#include <cmath>
#include <android/log.h>
#include <algorithm>

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

    reward_manager::init();

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

    update_reward(dt);
}

void game::render()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_post_processor->begin_render();

    {
        if (m_levels.size() > m_current_level)
            m_levels[m_current_level].draw(*m_sprite_renderer);

        m_player->draw(*m_sprite_renderer);

        for (auto& reward_item : m_rewards) {
            if (!reward_item.m_destroyed)
                reward_item.draw(*m_sprite_renderer);
        }

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

GLboolean check_collision(const game_object& lhs, const game_object& rhs)
{
    bool collision_x = lhs.m_position.x + lhs.m_size.x >= rhs.m_position.x &&
                       rhs.m_position.x + rhs.m_size.x >= lhs.m_position.x;

    bool collision_y = lhs.m_position.y + lhs.m_size.y >= rhs.m_position.y &&
                       rhs.m_position.y + rhs.m_size.y >= lhs.m_position.y;

    return collision_x && collision_y;
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
    for (brick_object& box : m_levels[m_current_level].m_briks) {
        if (!box.m_destroyed) {
            Collision collision = check_collision(*m_ball, box);
            if(std::get<0>(collision)){
                if (!box.m_solid) {
                    box.m_destroyed = true;
                    spawn_rewards(box.m_position, box.m_index);
                }
                else {
                    m_shake_time = 0.1;
                    m_post_processor->m_shake = GL_TRUE;
                }

                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);

                if (!(m_ball->m_pass_through && !box.m_solid)) {
                    if (dir == LEFT || dir == RIGHT) {
                        m_ball->m_velocity.x = -m_ball->m_velocity.x;

                        GLfloat penetration = m_ball->m_radius - std::abs(diff_vector.x);
                        if (dir == LEFT)
                            m_ball->m_position.x += penetration;
                        else if (dir == RIGHT)
                            m_ball->m_position.x -= penetration;
                    } else if (dir == UP || dir == DOWN) {
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
    }

    for (auto& reward_item : m_rewards) {
        if (!reward_item.m_destroyed) {

            if (reward_item.m_position.y >= m_height)
                reward_item.m_destroyed = GL_TRUE;

            if (check_collision(*m_player, reward_item)) {
                activate_reward(reward_item);
                reward_item.m_destroyed = GL_TRUE;
                reward_item.m_activated = GL_TRUE;
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

        m_ball->m_stuck = m_ball->m_sticky;
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
    m_ball->m_color = glm::vec3(1.0f);
    m_player->m_color = glm::vec3(1.0f);
    m_post_processor->m_chaos = GL_FALSE;
    m_post_processor->m_confise = GL_FALSE;
    m_ball->m_pass_through = GL_FALSE;
}

void game::spawn_rewards(const glm::vec2& position, GLuint index)
{
    std::string reward_type = m_levels[m_current_level].get_reward(index);

    if (!reward_type.empty())
        m_rewards.push_back(reward(reward_type,
                                   reward_manager::get_color(reward_type),
                                   reward_manager::get_duration(reward_type),
                                   position,
                                   glm::vec2(m_width / 8, m_height / 30),
                                   reward_manager::get_texture(reward_type),
                                   glm::vec2(0.0f, 150.0f)));
}

GLboolean is_other_reward_active(std::vector<reward> rewards, const std::string type)
{
    for (const auto& reward_item : rewards)
        if (reward_item.m_type == type && reward_item.m_activated)
            return GL_TRUE;
    return GL_FALSE;
}

void game::update_reward(GLfloat dt)
{
    for (auto& reward_item : m_rewards) {
        reward_item.m_position += reward_item.m_velocity * dt;

        if (reward_item.m_activated) {
            reward_item.m_duration -= dt;

            if (reward_item.m_duration <= 0.0) {
                reward_item.m_activated = GL_FALSE;

                if (reward_item.m_type == "sticky") {
                    if (!is_other_reward_active(m_rewards, "sticky")) {
                        m_ball->m_sticky = GL_FALSE;
                        m_player->m_color = glm::vec3(1.0f);
                    }
                }
                else if(reward_item.m_type == "pass-through") {
                    if (!is_other_reward_active(m_rewards, "pass-through")) {
                        m_ball->m_pass_through = GL_FALSE;
                        m_player->m_color = glm::vec3(1.0f);
                        m_ball->m_color = glm::vec3(1.0f);
                    }
                }
                else if(reward_item.m_type == "confuse") {
                    if (!is_other_reward_active(m_rewards, "confuse")) {
                        m_post_processor->m_confise = GL_FALSE;
                    }
                }
                else if(reward_item.m_type == "chaos") {
                    if (!is_other_reward_active(m_rewards, "chaos")) {
                        m_post_processor->m_chaos = GL_FALSE;
                    }
                }
            }
        }
    }

    m_rewards.erase(
            std::remove_if(m_rewards.begin(), m_rewards.end(), [](const reward& reward_item)
            { return reward_item.m_destroyed && !reward_item.m_activated; }),
            m_rewards.end());
}

void game::activate_reward(const reward& reward_item)
{
    if (reward_item.m_type == "speed") {
        m_ball->m_velocity *= 1.2;
    }
    else if (reward_item.m_type == "sticky") {
        m_ball->m_sticky = GL_TRUE;
        m_player->m_color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (reward_item.m_type == "pass-through") {
        m_ball->m_pass_through = GL_TRUE;
        m_ball->m_color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (reward_item.m_type == "pad-size-increase") {
        m_player->m_size.x += 50;
    }
    else if (reward_item.m_type == "confuse") {
        if (!m_post_processor->m_chaos)
            m_post_processor->m_confise = GL_TRUE;
    }
    else if (reward_item.m_type == "chaos") {
        if (!m_post_processor->m_confise)
            m_post_processor->m_chaos = GL_TRUE;
    }
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

