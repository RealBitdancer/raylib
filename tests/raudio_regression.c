#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EXTERNAL_CONFIG_FLAGS
#define SUPPORT_MODULE_RAUDIO 1
#include "raudio.c"

unsigned char *LoadFileData(const char *fileName, int *dataSize) { (void)fileName; *dataSize = 0; return NULL; }
void UnloadFileData(unsigned char *data) { free(data); }
bool SaveFileData(const char *fileName, const void *data, int dataSize) { (void)fileName; (void)data; (void)dataSize; return false; }
bool SaveFileText(const char *fileName, const char *text) { (void)fileName; (void)text; return false; }
bool IsFileExtension(const char *fileName, const char *ext) { (void)fileName; (void)ext; return false; }
const char *GetFileExtension(const char *fileName) { (void)fileName; return ""; }
const char *GetFileNameWithoutExt(const char *filePath) { (void)filePath; return ""; }

static int CheckSamples(const void *data, unsigned int sampleSize, unsigned int count, int signal, const char *stage)
{
    for (unsigned int i = 0; i < count; i++)
    {
        float value = 0.0f;
        if (sampleSize == 8) value = ((const unsigned char *)data)[i];
        else if (sampleSize == 16) value = ((const short *)data)[i];
        else value = ((const float *)data)[i];
        float expected = (sampleSize == 8)? (signal? 160.0f : 128.0f) : (signal? ((sampleSize == 16)? 8192.0f : 0.25f) : 0.0f);
        if (value != expected)
        {
            printf("FAIL %s: %u-bit sample %u expected %.2f, got %.2f\n", stage, sampleSize, i, expected, value);
            return 0;
        }
    }
    return 1;
}

static int TestStream(unsigned int sampleSize, unsigned int channels)
{
    AudioStream stream = LoadAudioStream(48000, sampleSize, channels);
    if (!IsAudioStreamValid(stream)) return 0;
    union { float f32[16]; short s16[32]; unsigned char u8[64]; } signal = { 0 }, output = { 0 };
    for (unsigned int i = 0; i < 2*channels; i++)
    {
        if (sampleSize == 8) signal.u8[i] = 160;
        else if (sampleSize == 16) signal.s16[i] = 8192;
        else signal.f32[i] = 0.25f;
    }
    unsigned int bytesPerFrame = channels*(sampleSize/8);
    UpdateAudioStream(stream, &signal, 2);
    if (!CheckSamples(stream.buffer->data, sampleSize, 2*channels, 1, "queued signal")) return 0;
    if (!CheckSamples(stream.buffer->data + 2*bytesPerFrame, sampleSize, 2*channels, 0, "partial-update silence")) return 0;
    if (!CheckSamples(stream.buffer->data + 4*bytesPerFrame, sampleSize, 4*channels, 0, "initial-buffer silence")) return 0;
    PlayAudioStream(stream);
    UpdateAudioStream(stream, &signal, 2);
    if (ReadAudioBufferFramesInInternalFormat(stream.buffer, &output, 4) != 4) return 0;
    if (!CheckSamples(&output, sampleSize, 2*channels, 1, "read signal")) return 0;
    if (!CheckSamples((unsigned char *)&output + 2*bytesPerFrame, sampleSize, 2*channels, 0, "read padding")) return 0;
    if (ReadAudioBufferFramesInInternalFormat(stream.buffer, &output, 4) != 4) return 0;
    if (!CheckSamples(&output, sampleSize, 4*channels, 0, "underrun silence")) return 0;
    StopAudioStream(stream);
    PlayAudioStream(stream);
    ma_silence_pcm_frames(&signal, 2, stream.buffer->converter.formatIn, channels);
    UpdateAudioStream(stream, &signal, 2);
    float mixed[128] = { 0 };
    OnSendAudioDataToDevice(&AUDIO.System.device, mixed, NULL, 64);
    for (unsigned int i = 0; i < 128; i++)
    {
        if (fabsf(mixed[i]) > ((sampleSize == 8)? 0.003f : 0.00001f))
        {
            printf("FAIL mixer silence: %u-bit %u-channel sample %u got %.6f\n", sampleSize, channels, i, mixed[i]);
            return 0;
        }
    }
    memset(mixed, 0, sizeof(mixed));
    const float input[] = { 0.25f, -0.25f, 0.5f, -0.5f };
    MixAudioFrames(mixed, input, 2, stream.buffer);
    for (unsigned int i = 0; i < 4; i++) if (fabsf(mixed[i] - input[i]*0.6875f) > 0.00001f) return 0;
    UnloadAudioStream(stream);
    printf("PASS %u-bit %u-channel: padding, initialization, underrun, signal preservation, stereo mixing\n", sampleSize, channels);
    return 1;
}

int main(void)
{
    if (ma_mutex_init(&AUDIO.System.lock) != MA_SUCCESS) return 2;
    AUDIO.System.device.sampleRate = 48000;
    AUDIO.System.device.playback.format = ma_format_f32;
    AUDIO.System.device.playback.channels = 2;
    SetAudioStreamBufferSizeDefault(4);
    const unsigned int sizes[] = { 8, 16, 32 };
    for (unsigned int s = 0; s < 3; s++)
        for (unsigned int channels = 1; channels <= 2; channels++)
            if (!TestStream(sizes[s], channels)) return 1;
    ma_mutex_uninit(&AUDIO.System.lock);
    puts("PASS raudio regression (no audio device required)");
    return 0;
}
