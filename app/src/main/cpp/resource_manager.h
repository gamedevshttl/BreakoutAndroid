//
// Created by Vladimir on 25.08.2018.
//

#ifndef AIRHOCKEY_RESOURCE_MANAGER_H
#define AIRHOCKEY_RESOURCE_MANAGER_H

#include "shader.h"
#include "texture.h"

#include <GLES2/gl2.h>

#include <map>
#include <string>

#include <android/asset_manager_jni.h>

#include "png.h"

typedef struct {
    const int width;
    const int height;
    const int size;
    const GLenum gl_color_format;
    const void* data;
} RawImageData;

typedef struct {
    const png_byte* data;
    const png_size_t size;
} DataHandle;

typedef struct {
    const DataHandle data;
    png_size_t offset;
} ReadDataHandle;

typedef struct {
    const png_uint_32 width;
    const png_uint_32 height;
    const int color_type;
} PngInfo;

typedef struct {
    const long data_length;
    const void* data;
    const void* file_handle;
} FileData;

static void read_png_data_callback(
        png_structp png_ptr, png_byte* png_data, png_size_t read_length);
static PngInfo read_and_update_info(const png_structp png_ptr, const png_infop info_ptr);
static DataHandle read_entire_png_image(
        const png_structp png_ptr, const png_infop info_ptr, const png_uint_32 height);
static GLenum get_gl_color_format(const int png_color_format);

class resource_manager
{
public:
    static std::map<std::string, shader> m_shaders;
    static std::map<std::string, texture> m_textures;

    static void set_data_dir(std::string current_dir);
    static void set_asset_manager(AAssetManager* asset_manager);

    static shader load_shader(const GLchar *v_shader_file, const GLchar *f_shader_file, const std::string name);
    static shader get_shader(const std::string name);

    static texture load_texture(const GLchar* file, GLboolean aplha, const std::string name);
    static texture & get_texture(const std::string name);

    static void clear();

private:
    resource_manager(){}

    static std::string load_file(const GLchar *file_path);
    static shader load_shader_from_file(const GLchar *v_shader_file, const GLchar *f_shader_file);
    static texture load_texture_from_file(const GLchar* file, GLboolean aplha);

    static std::string m_current_dir;
    static AAssetManager* m_asset_manager;
};

#endif //AIRHOCKEY_RESOURCE_MANAGER_H
