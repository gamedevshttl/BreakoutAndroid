#include <jni.h>
#include <string>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "game.h"

extern "C" JNIEXPORT jstring

JNICALL
Java_breakout_yellowball_com_newbreakout_Breakout_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


extern "C"
JNIEXPORT void JNICALL
Java_breakout_yellowball_com_newbreakout_RenderWrapper_on_1suface_1createrd(JNIEnv *env,
                                                                            jobject instance) {

    // TODO
    on_surface_created();
}

extern "C"
JNIEXPORT void JNICALL
Java_breakout_yellowball_com_newbreakout_RenderWrapper_on_1serface_1changed(JNIEnv *env,
                                                                            jobject instance,
                                                                            jint width,
                                                                            jint height) {

    // TODO
    on_surface_changed(width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_breakout_yellowball_com_newbreakout_RenderWrapper_on_1draw_1frame(JNIEnv *env,
                                                                       jobject instance) {

    // TODO
    on_draw_frame();
}

extern "C"
JNIEXPORT void JNICALL
Java_breakout_yellowball_com_newbreakout_Breakout_init_1asset_1manager(JNIEnv *env,
                                                                       jobject instance,
                                                                       jobject assetManager) {

    // TODO
    AAssetManager* asset_manager;
    asset_manager = AAssetManager_fromJava(env, assetManager);

    set_asset_manager(asset_manager);
}

extern "C"
JNIEXPORT void JNICALL
Java_breakout_yellowball_com_newbreakout_Breakout_on_1set_1data_1dir(JNIEnv *env, jobject instance,
                                                                     jstring dir_) {
    const char *dir = env->GetStringUTFChars(dir_, 0);

    // TODO
    set_data_dir(dir );
    env->ReleaseStringUTFChars(dir_, dir);
}