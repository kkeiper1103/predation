//
// Created by kkeiper1103 on 12/17/22.
//

#include "Application.h"
#include "utils/Timer.h"

#include <memory>

ApplicationPtr application_ = nullptr;
ApplicationPtr& app() {
    if(application_ != nullptr) return application_;

    application_ = std::make_unique<Application>();

    return application_;
}

// debug callback, in case the user has requested a debug version of opengl
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length,
                                     const GLchar *msg, const void *data)
{
    char* _source;
    char* _type;
    char* _severity;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            _source = "API";
            break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            _source = "WINDOW SYSTEM";
            break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            _source = "SHADER COMPILER";
            break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
            _source = "THIRD PARTY";
            break;

        case GL_DEBUG_SOURCE_APPLICATION:
            _source = "APPLICATION";
            break;

        case GL_DEBUG_SOURCE_OTHER:
            _source = "UNKNOWN";
            break;

        default:
            _source = "UNKNOWN";
            break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            _type = "ERROR";
            break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            _type = "DEPRECATED BEHAVIOR";
            break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            _type = "UDEFINED BEHAVIOR";
            break;

        case GL_DEBUG_TYPE_PORTABILITY:
            _type = "PORTABILITY";
            break;

        case GL_DEBUG_TYPE_PERFORMANCE:
            _type = "PERFORMANCE";
            break;

        case GL_DEBUG_TYPE_OTHER:
            _type = "OTHER";
            break;

        case GL_DEBUG_TYPE_MARKER:
            _type = "MARKER";
            break;

        default:
            _type = "UNKNOWN";
            break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            _severity = "HIGH";
            break;

        case GL_DEBUG_SEVERITY_MEDIUM:
            _severity = "MEDIUM";
            break;

        case GL_DEBUG_SEVERITY_LOW:
            _severity = "LOW";
            break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
            _severity = "NOTIFICATION";
            break;

        default:
            _severity = "UNKNOWN";
            break;
    }

    printf("%d: %s of %s severity, raised from %s: %s\n",
           id, _type, _severity, _source, msg);
}

Application::Application() {
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow("Default Title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);

    if(window == nullptr)
        LOG(ERROR) << "Unable to Create Window";

    //
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    glContext = SDL_GL_CreateContext(window);

    if(glContext == nullptr)
        LOG(ERROR) << "Unable to Obtain GLContext";

    SDL_GL_MakeCurrent(window, glContext);

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    SDL_GL_SetSwapInterval(1);

    // resize and set titles and everything
    configure(config);

    // set relative mouse mode
    // SDL_SetWindowGrab(window, SDL_TRUE);

    // @todo use real rng
    srand(time(NULL));
}

Application::~Application() {
    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(glContext);

    nk_sdl_shutdown();

    SDL_Quit();
}

void Application::run(AppLogic* logic) {
    if(logic == nullptr) {
        LOG(ERROR) << "Please Specify an AppLogic* for Application::run()!";
        exit(EXIT_FAILURE);
    }

    Timer t;
    while(running) {
        SDL_Event e;

        nk_input_begin(nkContext);
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = false;


            if(e.type == SDL_KEYUP && e.key.keysym.scancode == SDL_SCANCODE_F11) {
                int x = 0, y = 0;
                int w = config.width, h = config.height;

                Uint32 rmask, gmask, bmask, amask;

                rmask = 0x000000ff;
                gmask = 0x0000ff00;
                bmask = 0x00ff0000;
                amask = 0xff000000;

                auto pixels = std::vector<uint8_t>(w * h * 4);
                glReadPixels(x,y,w, h, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

                for(int line = 0; line != h/2; ++line) {
                    std::swap_ranges(
                            pixels.begin() + 4 * w * line,
                            pixels.begin() + 4 * w * (line+1),
                            pixels.begin() + 4 * w * (h-line-1));
                }

                SDL_Surface * surf = SDL_CreateRGBSurfaceFrom(pixels.data(), w, h, 8*4, w*4, rmask, gmask, bmask, amask);
                SDL_SaveBMP(surf, "screenshot.bmp");

                SDL_FreeSurface(surf);
            }


            logic->input(&e);
            nk_sdl_handle_event(&e);
        }
        nk_input_end(nkContext);

        t.tick([&](double dt) {
            logic->update(dt);
        });

        logic->gui(nkContext);
        render(logic);
    }
}

Application &Application::configure(ApplicationConfig config) {
    this->config = config;

    SDL_SetWindowSize(window, config.width, config.height);
    SDL_SetWindowFullscreen(window, config.fullscreen);
    SDL_SetWindowTitle(window, config.title.data());
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);


    if(nkContext == nullptr) {
        nkContext = nk_sdl_init(window);

        nk_sdl_font_stash_begin(&atlas);
        nk_sdl_font_stash_end();
    }

    return *this;
}




void Application::render(AppLogic* logic) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1000.f); // make this configurable, something like 1000.f * GameSettings::activeConfig()->view_range
    glClearColor(.5, .5, .5, 1);
    glViewport(0, 0, config.width, config.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // run render calls from world
    logic->render();

    if(renderGui) nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

    SDL_GL_SwapWindow(window);
}
