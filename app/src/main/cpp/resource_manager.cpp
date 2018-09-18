
#include "resource_manager.h"

#include <android/asset_manager_jni.h>

#include <iostream>
#include <vector>

#include <jni.h>
#include <string.h>
#include <malloc.h>

#include "png.h"
#include "pngconf.h"
#include "assert.h"

std::map<std::string, shader> resource_manager::m_shaders;
std::map<std::string, texture> resource_manager::m_textures;
std::string resource_manager::m_current_dir;
AAssetManager* resource_manager::m_asset_manager;

void resource_manager::set_data_dir(std::string current_dir)
{
    m_current_dir = current_dir;
}

void resource_manager::set_asset_manager(AAssetManager* asset_manager)
{
    m_asset_manager = asset_manager;
}

shader resource_manager::load_shader(const GLchar *v_shader_file, const GLchar *f_shader_file,
                                     const std::string name) {

    m_shaders[name] = load_shader_from_file(v_shader_file, f_shader_file);
    return m_shaders[name];
}

shader resource_manager::get_shader(const std::string name)
{
    return m_shaders[name];
}

texture resource_manager::load_texture(const GLchar* file, GLboolean aplha, const std::string name)
{
    m_textures[name] = load_texture_from_file(file, aplha);
    return m_textures[name];
}

texture& resource_manager::get_texture(const std::string name)
{
    return  m_textures[name];
}

void resource_manager::clear()
{
    for(auto iter: m_shaders)
        glDeleteProgram(iter.second.get_id());

    for(auto iter: m_textures)
        glDeleteTextures(1, &iter.second.m_id);
}

std::string resource_manager::load_file(const GLchar *file_path)
{
    std::string file_content;
    AAsset* assetFile = AAssetManager_open(m_asset_manager, file_path, AASSET_MODE_STREAMING);

    uint8_t* data = (uint8_t*)AAsset_getBuffer(assetFile);
    int32_t size = AAsset_getLength(assetFile);
    if (data == NULL) {
        AAsset_close(assetFile);
    }
    else{
        std::vector<uint8_t> buffer_ref;
        buffer_ref.reserve(size);
        buffer_ref.assign(data, data + size);
        AAsset_close(assetFile);
        file_content.assign(buffer_ref.begin(), buffer_ref.end());
    }

    return file_content;
}


shader resource_manager::load_shader_from_file(const GLchar *v_shader_file,
                                               const GLchar *f_shader_file) {

    std::string vertex_code = load_file(v_shader_file);
    std::string fragment_code = load_file(f_shader_file);

    shader shader_value;
    shader_value.compile(vertex_code.c_str(), fragment_code.c_str());
    return shader_value;

}

static void read_png_data_callback(png_structp png_ptr, png_byte* png_data, png_size_t read_length)
{
    ReadDataHandle* handle = (ReadDataHandle*)png_get_io_ptr(png_ptr);
    const png_byte* png_src = handle->data.data + handle->offset;

    memcpy(png_data, png_src, read_length);
    handle->offset += read_length;
}

static PngInfo read_and_update_info(const png_structp png_ptr, const png_infop info_ptr)
{
    png_uint_32 width, height;
    int bit_depth, color_type;

    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(
            png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

    // Convert transparency to full alpha
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    // Convert grayscale, if needed.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    // Convert paletted images, if needed.
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    // Add alpha channel, if there is none.
    // Rationale: GL_RGBA is faster than GL_RGB on many GPUs)
    if (color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_RGB)
        png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);

    // Ensure 8-bit packing
    if (bit_depth < 8)
        png_set_packing(png_ptr);
    else if (bit_depth == 16)
        png_set_scale_16(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    // Read the new color type after updates have been made.
    color_type = png_get_color_type(png_ptr, info_ptr);

    return (PngInfo) {width, height, color_type};
}

static DataHandle read_entire_png_image(const png_structp png_ptr, const png_infop info_ptr, const png_uint_32 height)
{
    const png_size_t row_size = png_get_rowbytes(png_ptr, info_ptr);
    png_size_t data_length = row_size * height;

    assert(row_size > 0);

    png_byte* raw_image = (png_byte*)malloc(data_length);
    assert(raw_image != NULL);

    png_byte* row_ptrs[height];

    png_uint_32 i;
    for (i = 0; i < height; i++) {
        row_ptrs[i] = raw_image + i * row_size;
    }

    png_read_image(png_ptr, &row_ptrs[0]);

    return (DataHandle) {raw_image, data_length};
}

GLenum get_gl_color_format(const int png_color_format)
{
    assert(png_color_format == PNG_COLOR_TYPE_GRAY
           || png_color_format == PNG_COLOR_TYPE_RGB_ALPHA
           || png_color_format == PNG_COLOR_TYPE_GRAY_ALPHA);

    switch (png_color_format) {
        case PNG_COLOR_TYPE_GRAY:
            return GL_LUMINANCE;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            return GL_RGBA;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            return GL_LUMINANCE_ALPHA;
    }

    return 0;
}

RawImageData get_raw_image_data_from_png(const void* png_data, const int png_data_size) {
    assert(png_data != NULL && png_data_size > 8);

    png_structp png_ptr = png_create_read_struct(
            PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    assert(png_ptr != NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    assert(info_ptr != NULL);

    ReadDataHandle png_data_handle = (ReadDataHandle) {{(png_byte*)png_data, (png_size_t)png_data_size}, 0};
    png_set_read_fn(png_ptr, &png_data_handle, read_png_data_callback);

    if (setjmp(png_jmpbuf(png_ptr))) {
//        CRASH("Error reading PNG file!");
    }

    const PngInfo png_info = read_and_update_info(png_ptr, info_ptr);
    const DataHandle raw_image = read_entire_png_image(
            png_ptr, info_ptr, png_info.height);

    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    return (RawImageData) {
            (int)png_info.width,
            (int)png_info.height,
            (int)raw_image.size,
            get_gl_color_format(png_info.color_type),
            raw_image.data};
}

void release_raw_image_data(const RawImageData* data) {
    assert(data != NULL);
    free((void*)data->data);
}

FileData resource_manager::get_asset_data(const char* relative_path) {
    assert(relative_path != NULL);
    AAsset* asset = AAssetManager_open(m_asset_manager, relative_path, AASSET_MODE_STREAMING);
    assert(asset != NULL);

    return (FileData) { AAsset_getLength(asset), AAsset_getBuffer(asset), asset };
}

void resource_manager::release_asset_data(const FileData* file_data) {
    assert(file_data != NULL);
    assert(file_data->file_handle != NULL);
    AAsset_close((AAsset*)file_data->file_handle);
}

texture resource_manager::load_texture_from_file(const GLchar *file, GLboolean alpha) {

    texture texture_value;
    if (alpha) {
        texture_value.m_internal_format = GL_RGBA;
        texture_value.m_image_format = GL_RGBA;
    }

    const FileData png_file = get_asset_data(file);
    const RawImageData raw_image_data = get_raw_image_data_from_png(png_file.data, png_file.data_length);

    if (raw_image_data.data) {
        texture_value.m_image_format = raw_image_data.gl_color_format;

        texture_value.generate(raw_image_data.width, raw_image_data.height,
                               (unsigned char *) raw_image_data.data);

        release_raw_image_data(&raw_image_data);
        release_asset_data(&png_file);
    }

    return texture_value;
}
