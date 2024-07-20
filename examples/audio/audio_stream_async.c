/*******************************************************************************************
*
*   raylib [audio] example - Asynchronous audio streaming
*
*   Example created with raylib 5.0 on fork https://github.com/RealBitdancer/raylib
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2024 Bitdancer (@RealBitdancer)
*
********************************************************************************************/

#include "raylib.h"
#include "external/dr_mp3.h"

#include <stdlib.h> // Required for: NULL

#define MAX_SAMPLES_PER_UPDATE   4096

typedef struct AsyncAudioStream
{
    AudioStream stream;
    drmp3* ctxMp3;
} AsyncAudioStream;

//------------------------------------------------------------------------------------
// Audio callback
//------------------------------------------------------------------------------------

static unsigned int AudioInputCallback(void* buffer, unsigned int frames, void* userData)
{
    drmp3* ctxMp3 = (drmp3*)userData;
    return (unsigned int)drmp3_read_pcm_frames_f32(ctxMp3, frames, (float*)buffer);
}

//------------------------------------------------------------------------------------
// Audio callback
//------------------------------------------------------------------------------------

static AsyncAudioStream LoadAsyncAudioStream(const char* fileName)
{
    AsyncAudioStream stream = {0};
    stream.ctxMp3 = (drmp3*)RL_CALLOC(1, sizeof(drmp3));

    int result = drmp3_init_file(stream.ctxMp3, fileName, NULL);
    if (result > 0)
    {
        stream.stream = LoadAudioStream(stream.ctxMp3->sampleRate, 32, stream.ctxMp3->channels);

        SetAudioStreamLooping(stream.stream, false);
        SetAudioStreamCallback(stream.stream, AudioInputCallback, stream.ctxMp3);
    }
    return stream;
}

static void UnloadAsyncAudioStream(AsyncAudioStream stream)
{
    UnloadAudioStream(stream.stream);
    drmp3_uninit(stream.ctxMp3);
    RL_FREE(stream.ctxMp3);
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [audio] example - asynchronous audio streaming");

    InitAudioDevice();              // Initialize audio device

    SetAudioStreamBufferSizeDefault(MAX_SAMPLES_PER_UPDATE);

    AsyncAudioStream stream = LoadAsyncAudioStream("resources/country.mp3");
    PlayAudioStream(stream.stream);

    bool wasPlaying = false;        // Audio was at some point playing
    bool isPlaying = false;         // Audio is currently playing

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        isPlaying = IsAudioStreamPlaying(stream.stream);
        wasPlaying |= isPlaying;

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            if (isPlaying)
            {
                DrawText("MUSIC SHOULD BE PLAYING!", 245, 150, 20, LIGHTGRAY);

                DrawText("If this program is running on a desktop platform", 140, 200, 20, LIGHTGRAY);
                DrawText("The window can be moved around freely without interrupting the music!", 40, 230, 20, LIGHTGRAY);
            }
            else if (wasPlaying)
            {
                DrawText("MUSIC FINISHED PLAYING!", 245, 150, 20, LIGHTGRAY);
            }
            else
            {
                DrawText("SOMETHING IS WRONG!", 255, 150, 20, LIGHTGRAY);
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadAsyncAudioStream(stream);

    CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
