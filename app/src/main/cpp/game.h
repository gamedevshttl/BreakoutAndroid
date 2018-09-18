//
// Created by Vladimir on 01.09.2018.
//

#ifndef BREAKOUT_GAME_H
#define BREAKOUT_GAME_H

#include <android/asset_manager_jni.h>

void on_surface_created();
void on_surface_changed(int width, int height);
void on_draw_frame();
void set_data_dir(const char *dir);

void set_asset_manager(AAssetManager* asset_manager);

#endif //BREAKOUT_GAME_H
