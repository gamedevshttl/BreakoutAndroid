//
// Created by Vladimir on 01.09.2018.
//

#ifndef BREAKOUT_GAME_H
#define BREAKOUT_GAME_H

#include <android/asset_manager_jni.h>

#include <memory>
#include <tuple>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_level.h"
#include "ball_object.h"
#include "reward.h"

class particle_generator;
class post_processor;

void on_surface_created();
void on_surface_changed(int width, int height);
void on_update();
void set_data_dir(const char *dir);
void set_asset_manager(AAssetManager* asset_manager);
void on_touch_press(float x, float y, int idx);
void on_touch_drag(float x, float y, int idx);
void on_touch_release(float x, float y, int idx);


enum game_state {
    game_active,
    game_menu,
    game_win
};

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(200.0f, -700.0f);



class game
{
public:
    game();
    void init();
    void set_asset_manager(AAssetManager* asset_manager);
    void set_data_dir(std::string current_dir);
    void on_surface_changed(int width, int height);

    void update(GLfloat dt);
    void render();
    void do_collision();
    void reset_level();
    void reset_player();
    void spawn_rewards(const glm::vec2& position, GLuint index);
    void update_reward(GLfloat dt);
    void activate_reward(const reward& reward_item);

    void on_touch_press(float x, float y, int idx);
    void on_touch_drag(float x, float y, int idx);
    void on_touch_release(float x, float y, int idx);

    std::shared_ptr<sprite_renderer> m_sprite_renderer;
    std::string m_current_dir;
    AAssetManager* m_asset_manager;

    int m_width, m_height;
    glm::mat4x4 projection;
    std::vector<game_level> m_levels;
    GLuint m_current_level = 0;

    std::shared_ptr<game_object> m_player;
    glm::vec2 m_player_size;
    GLfloat m_player_velocity;

    std::shared_ptr<ball_object> m_ball;
    GLfloat m_ball_radius;
    glm::vec2 m_ball_velocity;

    std::shared_ptr<particle_generator> m_particle_generator;
    std::shared_ptr<post_processor> m_post_processor;
    GLfloat m_shake_time;

    std::vector<reward> m_rewards;

    GLuint m_game_screen_top;
    GLuint m_game_screen_low;
    GLint m_game_screen_height_diff;

private:
    GLfloat m_mouse_x, m_prev_mouse_x, m_mouse_y;
    GLfloat m_diff_pos;
    GLfloat m_move_time;
};


#endif //BREAKOUT_GAME_H
