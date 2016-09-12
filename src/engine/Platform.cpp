#include "Platform.h"
#include "utils/Utils.h"
#include <bgfx/bgfxplatform.h>
#include <thread>

//#include <bx/bx.h>
//#include <bgfx/bgfx.h>
//#include <bx/string.h>
//#include <bx/crtimpl.h>

//#include <time.h>

//#include <bx/thread.h>
//#include <bx/os.h>
//#include <bx/handlealloc.h>
//#include <string.h> // memset
//#include <string>

//#include <fcntl.h>

//#if BX_PLATFORM_EMSCRIPTEN
//#	include <emscripten.h>
//#endif // BX_PLATFORM_EMSCRIPTEN

//#include "entry_p.h"
//#include "cmd.h"
//#include "input.h"

//#define RMT_ENABLED ENTRY_CONFIG_PROFILER
//#include <remotery/lib/Remotery.h>

using namespace Engine;

Platform::Platform()
    : isRunning(true)
    , window(nullptr)
{
}

int32_t Platform::run(int argc, char** argv)
{
    constexpr int width = 1280;
    constexpr int height = 720;

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    //glfwMakeContextCurrent(window);

    // Initialize bgfx
    bgfx::glfwSetWindow(window);

    glfwSetKeyCallback(window, keyEvent);
    glfwSetMouseButtonCallback(window, mouseButtonEvent);
    glfwSetCursorPosCallback(window, mouseMoveEvent);
    glfwSetScrollCallback(window, scrollEvent);

    glfwSetWindowSizeCallback(window, windowSizeEvent);
    windowSizeEvent(window, width, height);

    setMouseLockCallback([&window](bool lock)
    {
        if(lock)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    });

    // TODO read bindings from file

//    bindKey(GLFW_KEY_W, ActionType::DebugSkySpeed, false);

    // Camera Switch Keys

    bindKey(GLFW_KEY_F1, ActionType::CameraFirstPerson, false);
    bindKey(GLFW_KEY_F2, ActionType::CameraThirdPerson, false);
    bindKey(GLFW_KEY_F3, ActionType::CameraFree, false);
    bindKey(GLFW_KEY_F4, ActionType::CameraViewer, false);

    // FirstPerson Camera Controller

    bindKey(GLFW_KEY_W, ActionType::FirstPersonMoveForward, true, false);
    bindKey(GLFW_KEY_S, ActionType::FirstPersonMoveForward, true, true);
    bindKey(GLFW_KEY_D, ActionType::FirstPersonMoveRight, true, false);
    bindKey(GLFW_KEY_A, ActionType::FirstPersonMoveRight, true, true);

    bindMouseAxis(MouseAxis::CursorX, ActionType::FirstPersonLookHorizontal, true, false);
    bindMouseAxis(MouseAxis::CursorY, ActionType::FirstPersonLookVertical, true, true);

    bindKey(GLFW_KEY_UP, ActionType::FirstPersonLookVertical, true, false);
    bindKey(GLFW_KEY_DOWN, ActionType::FirstPersonLookVertical, true, true);
    bindKey(GLFW_KEY_RIGHT, ActionType::FirstPersonLookHorizontal, true, false);
    bindKey(GLFW_KEY_LEFT, ActionType::FirstPersonLookHorizontal, true, true);

    // Free Camera Controller

    bindKey(GLFW_KEY_W, ActionType::FreeMoveForward, true, false);
    bindKey(GLFW_KEY_S, ActionType::FreeMoveForward, true, true);
    bindKey(GLFW_KEY_D, ActionType::FreeMoveRight, true, false);
    bindKey(GLFW_KEY_A, ActionType::FreeMoveRight, true, true);
    bindKey(GLFW_KEY_Q, ActionType::FreeMoveUp, true, true);
    bindKey(GLFW_KEY_E, ActionType::FreeMoveUp, true, false);

    bindMouseAxis(MouseAxis::CursorX, ActionType::FreeLookHorizontal, true, false);
    bindMouseAxis(MouseAxis::CursorY, ActionType::FreeLookVertical, true, true);

    bindKey(GLFW_KEY_UP, ActionType::FreeLookVertical, true, false);
    bindKey(GLFW_KEY_DOWN, ActionType::FreeLookVertical, true, true);
    bindKey(GLFW_KEY_RIGHT, ActionType::FreeLookHorizontal, true, false);
    bindKey(GLFW_KEY_LEFT, ActionType::FreeLookHorizontal, true, true);

    // Viewer Camera Controller
    bindMouseAxis(MouseAxis::CursorX, ActionType::ViewerHorizontal, true);
    bindMouseAxis(MouseAxis::CursorY, ActionType::ViewerVertical, true);
    bindMouseButton(GLFW_MOUSE_BUTTON_RIGHT, ActionType::ViewerRotate, true);
    bindMouseButton(GLFW_MOUSE_BUTTON_LEFT, ActionType::ViewerClick, false);
    bindKey(GLFW_KEY_LEFT_SHIFT, ActionType::ViewerPan, true);
    bindKey(GLFW_KEY_LEFT_CONTROL, ActionType::ViewerZoom, true);
    bindMouseAxis(MouseAxis::ScrollY, ActionType::ViewerMouseWheel, false);

    // Player Controller
    bindKey(GLFW_KEY_W, ActionType::PlayerForward, true);
    bindKey(GLFW_KEY_S, ActionType::PlayerBackward, true);
    bindKey(GLFW_KEY_LEFT, ActionType::PlayerTurnLeft, true);
    bindKey(GLFW_KEY_RIGHT, ActionType::PlayerTurnRight, true);
    bindKey(GLFW_KEY_A, ActionType::PlayerStrafeLeft, true);
    bindKey(GLFW_KEY_D, ActionType::PlayerStrafeRight, true);
    bindKey(GLFW_KEY_Q, ActionType::PlayerAttackFist, true);

    bindKey(GLFW_KEY_A, ActionType::WeaponAnimationAttackLeft, true);
    bindKey(GLFW_KEY_D, ActionType::WeaponAnimationAttackRight, true);
    bindKey(GLFW_KEY_W, ActionType::WeaponAnimationRun, true);
    bindKey(GLFW_KEY_S, ActionType::WeaponAnimationBackpedal, true);
    bindKey(GLFW_KEY_Q, ActionType::WeaponAnimationAttack, true);

    bindKey(GLFW_KEY_SPACE, ActionType::DebugMoveSpeed, true);

//    // special keys test
//    bindKey(GLFW_KEY_LEFT_SHIFT, ActionType::PlayerForward, true);
//    bindKey(GLFW_KEY_RIGHT_SHIFT, ActionType::PlayerBackward, true);
//    bindKey(GLFW_KEY_LEFT_CONTROL, ActionType::PlayerTurnLeft, true);
//    bindKey(GLFW_KEY_RIGHT_CONTROL, ActionType::PlayerTurnRight, true);
//    bindKey(GLFW_KEY_LEFT_ALT, ActionType::PlayerStrafeLeft, true);
//    bindKey(GLFW_KEY_RIGHT_ALT, ActionType::PlayerStrafeRight, true);

    // Start logic thread
    std::promise<int32_t> returnValue;
    auto future = returnValue.get_future();
    std::thread thread(&Platform::mainLoop, this, std::move(returnValue), argc, argv);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        /* Poll for and process events */
        //glfwPollEvents();
        glfwWaitEvents();
    }
    isRunning = false;

    thread.join();
    glfwTerminate();
    return future.get();
}

void Platform::mainLoop(std::promise<int32_t> && returnValue, int argc, char** argv)
{
    //DBG(BX_COMPILER_NAME " / " BX_CPU_NAME " / " BX_ARCH_NAME " / " BX_PLATFORM_NAME);

//    if (BX_ENABLED(ENTRY_CONFIG_PROFILER) )
//    {
//        rmtSettings* settings = rmt_Settings();
//        BX_WARN(NULL != settings, "Remotery is not enabled.");
//        if (NULL != settings)
//        {
//            settings->malloc  = rmtMalloc;
//            settings->realloc = rmtRealloc;
//            settings->free    = rmtFree;

//            rmtError err = rmt_CreateGlobalInstance(&s_rmt);
//            BX_WARN(RMT_ERROR_NONE != err, "Remotery failed to create global instance.");
//            if (RMT_ERROR_NONE == err)
//            {
//                rmt_SetCurrentThreadName("Main");
//            }
//            else
//            {
//                s_rmt = NULL;
//            }
//        }
//    }

    Utils::initializeFileReaderWriter();

//#if BX_CONFIG_CRT_FILE_READER_WRITER
//    s_fileReader = new bx::CrtFileReader;
//    s_fileWriter = new bx::CrtFileWriter;
//#endif // BX_CONFIG_CRT_FILE_READER_WRITER

    //cmdInit();
    //cmdAdd("mouselock", cmdMouseLock);
    //cmdAdd("graphics",  cmdGraphics );
    //cmdAdd("exit",      cmdExit     );

//    inputInit();
//    inputAddBindings("bindings", s_bindings);

//    entry::WindowHandle defaultWindow = { 0 };
//    entry::setWindowTitle(defaultWindow, bx::baseName(_argv[0]) );
//    setWindowSize(defaultWindow, ENTRY_DEFAULT_WIDTH, ENTRY_DEFAULT_HEIGHT);

    init(argc, argv);
    bgfx::frame();

//    WindowHandle defaultWindow = { 0 };
//    setWindowSize(defaultWindow, ENTRY_DEFAULT_WIDTH, ENTRY_DEFAULT_HEIGHT);

//#if BX_PLATFORM_EMSCRIPTEN
//    s_app = _app;
//    emscripten_set_main_loop(&updateApp, -1, 1);
//#else
    while ( update() );
//#endif // BX_PLATFORM_EMSCRIPTEN

    returnValue.set_value( shutdown() );

//    inputRemoveBindings("bindings");
//    inputShutdown();

//    cmdShutdown();

    Utils::destroyFileReaderWriter();

//#if BX_CONFIG_CRT_FILE_READER_WRITER
//    delete s_fileReader;
//    s_fileReader = NULL;

//    delete s_fileWriter;
//    s_fileWriter = NULL;
//#endif // BX_CONFIG_CRT_FILE_READER_WRITER

//    if (BX_ENABLED(ENTRY_CONFIG_PROFILER)
//    &&  NULL != s_rmt)
//    {
//        rmt_DestroyGlobalInstance(s_rmt);
//    }

//    return result;
}
