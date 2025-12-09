/**********************************************************************************************
*
*   rcore_desktop_win32 - Functions to manage window, graphics device and inputs
*
*   PLATFORM: DESKTOP: WIN32
*       - Windows (Win32, Win64)
*
*   LIMITATIONS:
*       - Initial development stage, lot of functionality missing
*       - No support for MOUSE_BUTTON_FORWARD/MOUSE_BUTTON_BACK
*
*   POSSIBLE IMPROVEMENTS:
*       - Improvement 01
*       - Improvement 02
*
*   ADDITIONAL NOTES:
*       - TRACELOG() function is located in raylib [utils] module
*
*   CONFIGURATION:
*       #define RCORE_PLATFORM_CUSTOM_FLAG
*           Custom flag for rcore on target platform -not used-
*
*   DEPENDENCIES:
*       - Win32 API (windows.h)
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2013-2025 Ramon Santamaria (@raysan5) and contributors
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

// Move windows.h symbols to new names to avoid redefining the same names as raylib
#define CloseWindow CloseWindowWin32
#define Rectangle RectangleWin32
#define ShowCursor ShowCursorWin32
#define DrawTextA DrawTextAWin32
#define DrawTextW DrawTextWin32
#define DrawTextExA DrawTextExAWin32
#define DrawTextExW DrawTextExWin32

#include "../src/platforms/windows/sal_win32.c"

#undef CloseWindow      // raylib symbol collision
#undef Rectangle        // raylib symbol collision
#undef ShowCursor       // raylib symbol collision
#undef LoadImage        // raylib symbol collision
#undef DrawText         // raylib symbol collision
#undef DrawTextA
#undef DrawTextW
#undef DrawTextEx       // raylib symbol collision
#undef DrawTextExA
#undef DrawTextExW

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

typedef struct {
    SalContext* context;
} PlatformData;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
extern CoreData CORE;                   // Global CORE state context
static PlatformData platform = { 0 };   // Platform specific data

static void salKeyCB(SalContext* context, SalKey key, int scancode, SalKeyAction action, SalKeyMods mods, void* userData)
{
    if (key < 0)
    {
        return;    // Security check, macOS fn key generates -1
    }

    // WARNING: GLFW could return GLFW_REPEAT, we need to consider it as 1
    // to work properly with our implementation (IsKeyDown/IsKeyUp checks)
    if (action == SAL_KEY_ACTION_RELEASE)
    {
        CORE.Input.Keyboard.currentKeyState[key] = 0;
    }
    else if (action == SAL_KEY_ACTION_PRESS)
    {
        CORE.Input.Keyboard.currentKeyState[key] = 1;
    }
    else if (action == SAL_KEY_ACTION_REPEAT)
    {
        CORE.Input.Keyboard.keyRepeatInFrame[key] = 1;
    }

    // WARNING: Check if CAPS/NUM key modifiers are enabled and force down state for those keys
    if (((key == SAL_KEY_CAPS_LOCK) && (FLAG_IS_SET(mods, SAL_KEY_MOD_CAPS_LOCK))) ||
        ((key == SAL_KEY_NUM_LOCK) && (FLAG_IS_SET(mods, SAL_KEY_MOD_NUM_LOCK))))
    {
        CORE.Input.Keyboard.currentKeyState[key] = 1;
    }

    // Check if there is space available in the key queue
    if ((CORE.Input.Keyboard.keyPressedQueueCount < MAX_KEY_PRESSED_QUEUE) && (action == SAL_KEY_ACTION_PRESS))
    {
        // Add character to the queue
        CORE.Input.Keyboard.keyPressedQueue[CORE.Input.Keyboard.keyPressedQueueCount] = key;
        CORE.Input.Keyboard.keyPressedQueueCount++;
        CORE.Input.Keyboard.keyPressedQueueCount++;
    }

    // Check the exit key to set close window
    if ((key == CORE.Input.Keyboard.exitKey) && (action == SAL_KEY_ACTION_RELEASE))
    {
        salQuit(platform.context);
    }
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Window and Graphics Device
//----------------------------------------------------------------------------------

// Check if application should close
bool WindowShouldClose(void)
{
    return !salContinue(platform.context);
}

// Toggle fullscreen mode
void ToggleFullscreen(void)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: Toggle full screen functionality not implemented");
}

// Toggle borderless windowed mode
void ToggleBorderlessWindowed(void)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: Toggle borderless windowed functionality not implemented");
}

// Set window state: maximized, if resizable
void MaximizeWindow(void)
{
    salSetWindowState(platform.context, SAL_WINDOW_STATE_MAXIMIZED);
}

// Set window state: minimized
void MinimizeWindow(void)
{
    salSetWindowState(platform.context, SAL_WINDOW_STATE_MINIMIZED);
}

// Restore window from being minimized/maximized
void RestoreWindow(void)
{
    salClearWindowState(platform.context, SAL_WINDOW_STATE_MAXIMIZED);
    salClearWindowState(platform.context, SAL_WINDOW_STATE_MINIMIZED);
}

// Set window configuration state using flags
void SetWindowState(unsigned int flags)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: Set window state functionality not implemented");
}

// Clear window configuration state flags
void ClearWindowState(unsigned int flags)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: Clear window state functionality not implemented");
}

// Set icon for window
void SetWindowIcon(Image image)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: Set window icon functionality not implemented");
}

// Set icon for window
void SetWindowIcons(Image *images, int count)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: Set window icons functionality not implemented");
}

void SetWindowTitle(const char *title)
{
    salSetWindowTitle(platform.context, title);
}

// Set window position on screen (windowed mode)
void SetWindowPosition(int x, int y)
{
    salSetWindowPosition(platform.context, x, y);
}

// Set monitor for the current window
void SetWindowMonitor(int monitor)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: SetWindowMonitor not implemented");
}

// Set window minimum dimensions (FLAG_WINDOW_RESIZABLE)
void SetWindowMinSize(int width, int height)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: SetWindowMinSize not implemented");

    CORE.Window.screenMin.width = width;
    CORE.Window.screenMin.height = height;
}

// Set window maximum dimensions (FLAG_WINDOW_RESIZABLE)
void SetWindowMaxSize(int width, int height)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: SetWindowMaxSize not implemented");

    CORE.Window.screenMax.width = width;
    CORE.Window.screenMax.height = height;
}

// Set window dimensions
void SetWindowSize(int width, int height)
{
    salSetWindowSize(platform.context, width, height);
}

// Set window opacity, value opacity is between 0.0 and 1.0
void SetWindowOpacity(float opacity)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: SetWindowOpacity not implemented");
}

// Set window focused
void SetWindowFocused(void)
{
    salSetWindowState(platform.context, SAL_WINDOW_STATE_FOCUSED);
}

// Get native window handle
void *GetWindowHandle(void)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: GetWindowHandle not implemented");
    return NULL;
}

int GetMonitorCount(void)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: GetMonitorCount not implemented");
    return 0;
}

// Get current monitor where window is placed
int GetCurrentMonitor(void)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: GetCurrentMonitor not implemented");
    return -1;
}

// Get selected monitor position
Vector2 GetMonitorPosition(int monitor)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: GetMonitorPosition not implemented");
    return (Vector2){ 0, 0 };
}

// Get selected monitor width (currently used by monitor)
int GetMonitorWidth(int monitor)
{
    return salGetMonitorSize(platform.context, monitor).width;
}

// Get selected monitor height (currently used by monitor)
int GetMonitorHeight(int monitor)
{
    return salGetMonitorSize(platform.context, monitor).height;
}

// Get selected monitor physical width in millimetres
int GetMonitorPhysicalWidth(int monitor)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: GetMonitorPhysicalWidth not implemented");
    return 0;
}

// Get selected monitor physical height in millimetres
int GetMonitorPhysicalHeight(int monitor)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: GetMonitorPhysicalHeight not implemented");
    return 0;
}

// Get selected monitor refresh rate
int GetMonitorRefreshRate(int monitor)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: GetMonitorRefreshRate not implemented");
    return 0;
}

// Get the human-readable, UTF-8 encoded name of the selected monitor
const char *GetMonitorName(int monitor)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: GetMonitorName not implemented");
    return 0;
}

// Get window position XY on monitor
Vector2 GetWindowPosition(void)
{
    const SalPoint position = salGetWindowPosition(platform.context);
    return (Vector2){ (float)position.x, (float)position.y };
}

// Get window scale DPI factor for current monitor
Vector2 GetWindowScaleDPI(void)
{
    const SalDpiScale scale = salGetWindowDpiScale(platform.context);
    return (Vector2){ scale.xdpi, scale.ydpi };
}

// Set clipboard text content
void SetClipboardText(const char *text)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: SetClipboardText not implemented");
}

// Get clipboard text content
const char *GetClipboardText(void)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: GetClipboardText not implemented");
    return NULL;
}

// Get clipboard image
Image GetClipboardImage(void)
{
    Image image = { 0 };

    TRACELOG(LOG_WARNING, "SAL-WIN32: GetClipboardImage not implemented");

    return image;
}

// Show mouse cursor
void ShowCursor(void)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: ShowCursor not implemented");
    CORE.Input.Mouse.cursorHidden = false;
}

// Hides mouse cursor
void HideCursor(void)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: HideCursor not implemented");
    CORE.Input.Mouse.cursorHidden = true;
}

// Enables cursor (unlock cursor)
void EnableCursor(void)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: EnableCursor not implemented");
}

// Disables cursor (lock cursor)
void DisableCursor(void)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: DisableCursor not implemented");
}

// Swap back buffer with front buffer (screen drawing)
void SwapScreenBuffer(void)
{
    salSwapBuffers(platform.context);
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Misc
//----------------------------------------------------------------------------------

// Get elapsed time measure in seconds
double GetTime(void)
{
    // Elapsed time since salInitialize()
    return salGetTime(platform.context);
}

// Open URL with default system browser (if available)
// NOTE: This function is only safe to use if you control the URL given
// A user could craft a malicious string performing another action
// Only call this function yourself not with user input or make sure to check the string yourself
// Ref: https://github.com/raysan5/raylib/issues/686
void OpenURL(const char *url)
{
    // Security check to (partially) avoid malicious code on target platform
    if (strchr(url, '\'') != NULL) TRACELOG(LOG_WARNING, "SYSTEM: Provided URL could be potentially malicious, avoid [\'] character");
    else
    {
        char *cmd = (char *)RL_CALLOC(strlen(url) + 32, sizeof(char));
        sprintf(cmd, "explorer \"%s\"", url);
        int result = system(cmd);
        if (result == -1) TRACELOG(LOG_WARNING, "OpenURL() child process could not be created");
        RL_FREE(cmd);
    }
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Inputs
//----------------------------------------------------------------------------------

// Set internal gamepad mappings
int SetGamepadMappings(const char *mappings)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: SetGamepadMappings not implemented");
    return -1;
}

// Set gamepad vibration
void SetGamepadVibration(int gamepad, float leftMotor, float rightMotor, float duration)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: SetGamepadVibration not implemented");
}

// Set mouse position XY
void SetMousePosition(int x, int y)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: SetMousePosition not implemented");
}

// Set mouse cursor
void SetMouseCursor(int cursor)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: SetMouseCursor not implemented");
    CORE.Input.Mouse.cursorHidden = false;
}

// Get physical key name
const char* GetKeyName(int key)
{
    TRACELOG(LOG_WARNING, "SAL-WIN32: GetKeyName not implemented");
    return NULL;
}

// Register all input events
void PollInputEvents(void)
{
    salPollEvents(platform.context);
}

//----------------------------------------------------------------------------------
// Module Internal Functions Definition
//----------------------------------------------------------------------------------

int InitPlatform(void)
{
    SalContext* context = salInitialize(CORE.Window.title, CORE.Window.screen.width, CORE.Window.screen.height, 0);
    if (context == NULL)
    {
        TRACELOG(LOG_FATAL, "PLATFORM: Failed to initialize SAL (Simple Abstraction Layer)");
        return -1;
    }

    platform.context = context;
    CORE.Window.ready = true;

    const SalSize monitorSize = salGetMonitorSize(platform.context, 0);
    CORE.Window.display.width = monitorSize.width;
    CORE.Window.display.height = monitorSize.height;

    CORE.Window.render.width = CORE.Window.screen.width;
    CORE.Window.render.height = CORE.Window.screen.height;
    CORE.Window.currentFbo.width = CORE.Window.render.width;
    CORE.Window.currentFbo.height = CORE.Window.render.height;

    CORE.Window.title = salGetWindowTitle(platform.context);
    CORE.Storage.basePath = GetWorkingDirectory();

    TRACELOG(LOG_INFO, "DISPLAY: Device initialized successfully");
    TRACELOG(LOG_INFO, "    > Display size: %i x %i", CORE.Window.display.width, CORE.Window.display.height);
    TRACELOG(LOG_INFO, "    > Screen size:  %i x %i", CORE.Window.screen.width, CORE.Window.screen.height);
    TRACELOG(LOG_INFO, "    > Render size:  %i x %i", CORE.Window.render.width, CORE.Window.render.height);
    TRACELOG(LOG_INFO, "    > Viewport offsets: %i, %i", CORE.Window.renderOffset.x, CORE.Window.renderOffset.y);

    rlLoadExtensions(salGetProcLoadFunction(platform.context));

    InitTimer();

    salSetKeyCallback(platform.context, salKeyCB, NULL);

    return 0;
}

// Close platform
void ClosePlatform(void)
{
    salFinalize(platform.context);
}
