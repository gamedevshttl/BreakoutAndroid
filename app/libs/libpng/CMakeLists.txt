cmake_minimum_required( VERSION 3.6 )

set( project_name png )
project( ${project_name} )

set( source_dir jni )

set( headers
    ${source_dir}/pnginfo.h
    ${source_dir}/config.h
    ${source_dir}/png.h
    ${source_dir}/pngconf.h
    ${source_dir}/pngdebug.h
    ${source_dir}/pnglibconf.h
    ${source_dir}/pngprefix.h
    ${source_dir}/pngpriv.h
    ${source_dir}/pngstruct.h
)

set( sources
    ${source_dir}/arm/arm_init.c
    ${source_dir}/arm/filter_neon.S
    ${source_dir}/arm/filter_neon_intrinsics.c
    ${source_dir}/png.c
    ${source_dir}/pngerror.c
    ${source_dir}/pngget.c
    ${source_dir}/pngmem.c
    ${source_dir}/pngpread.c
    ${source_dir}/pngread.c
    ${source_dir}/pngrio.c
    ${source_dir}/pngrtran.c
    ${source_dir}/pngrutil.c
    ${source_dir}/pngset.c
    ${source_dir}/pngtest.c
    ${source_dir}/pngtrans.c
    ${source_dir}/pngwio.c
    ${source_dir}/pngwrite.c
    ${source_dir}/pngwtran.c
    ${source_dir}/pngwutil.c
)

find_package( zlib REQUIRED )

# If you want a shared library instead of static, specify the BUILD_SHARED_LIBS
# variable (which is a built-in CMake option).
add_library( ${project_name} ${sources} ${headers} )
