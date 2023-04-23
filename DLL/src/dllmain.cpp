// dllmain.cpp : Defines the entry point for the DLL application.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <foo/config.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <jemalloc/jemalloc.h>

#define STBI_NO_GIF

#define STBI_ASSERT(x) static_cast<void*>(0)
#define STBI_MALLOC je_malloc
#define STBI_REALLOC je_realloc
#define STBI_FREE je_free

#define STB_IMAGE_IMPLEMENTATION
#pragma warning (push, 0)
#pragma warning( disable: 26451 )
#include "stb_image.h"
#pragma warning (pop)


#ifdef __cplusplus
extern "C" {
#endif

    FOO_API bool load_image_info(const char* path, int* x, int* y, int* comp) {
        return stbi_info(path, x, y, comp);
    }

    FOO_API uint8_t* load_image(const char* path, int* x, int* y, int* comp, int req_comp) {
        return stbi_load(path, x, y, comp, req_comp);
    }

    FOO_API void destroy_image(uint8_t* image) {
        stbi_image_free(image);
    }

#ifdef __cplusplus
}
#endif


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     ) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        return gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return true;
}

