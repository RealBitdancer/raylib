#include "raylib.h"
#include <stdio.h>
#include <string.h>

static int checks = 0;
static int failures = 0;

static void CheckDraw(int sourceFormat, int destinationFormat, Color tint, int variant)
{
    Image source = GenImageColor(3, 2, WHITE);
    for (int y = 0; y < source.height; y++)
    {
        for (int x = 0; x < source.width; x++)
        {
            ImageDrawPixel(&source, x, y, ((x == 0) && (y == 0))? WHITE : (Color){
                (unsigned char)(240 - 40*x), (unsigned char)(40 + 70*y),
                (unsigned char)(80 + 30*x), (unsigned char)(60 + 70*x) });
        }
    }
    ImageFormat(&source, sourceFormat);

    Image actual = GenImageColor(6, 5, (Color){ 37, 91, 143, 89 });
    ImageFormat(&actual, destinationFormat);
    Image expected = ImageCopy(actual);
    Image sampled = ImageCopy(source);
    int posX = (variant == 1)? -1 : 1;
    int posY = (variant == 1)? -1 : 1;
    if (variant == 2) ImageResize(&sampled, 4, 3);

    int sourceBytes = GetPixelDataSize(1, 1, sampled.format);
    int destinationBytes = GetPixelDataSize(1, 1, expected.format);
    int opaqueSource = (sourceFormat != PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA) &&
        (sourceFormat != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8) &&
        (sourceFormat != PIXELFORMAT_UNCOMPRESSED_R4G4B4A4);
    int identityTint = (tint.r == 255) && (tint.g == 255) && (tint.b == 255) && (tint.a == 255);
    for (int y = 0; y < sampled.height; y++)
    {
        for (int x = 0; x < sampled.width; x++)
        {
            int dx = x + posX;
            int dy = y + posY;
            if ((dx < 0) || (dy < 0) || (dx >= expected.width) || (dy >= expected.height)) continue;
            Color src = GetPixelColor((unsigned char *)sampled.data + (y*sampled.width + x)*sourceBytes, sampled.format);
            void *dst = (unsigned char *)expected.data + (dy*expected.width + dx)*destinationBytes;
            if (opaqueSource && identityTint && (sampled.format == expected.format))
            {
                memcpy(dst, (unsigned char *)sampled.data + (y*sampled.width + x)*sourceBytes, sourceBytes);
            }
            else SetPixelColor(dst, ColorAlphaBlend(GetPixelColor(dst, expected.format), src, tint), expected.format);
        }
    }

    Rectangle sourceRect = { 0, 0, (float)source.width, (float)source.height };
    Rectangle destinationRect = { (float)posX, (float)posY, (float)sampled.width, (float)sampled.height };
    ImageDrawImagePro(&actual, source, sourceRect, destinationRect, (Vector2){ 0 }, 0, tint);
    checks++;
    if (memcmp(actual.data, expected.data, GetPixelDataSize(actual.width, actual.height, actual.format)) != 0)
    {
        if (failures < 5)
        {
            Color got = GetPixelColor((unsigned char *)actual.data + ((posY < 0)? 0 : (posY*actual.width + posX))*destinationBytes, actual.format);
            Color want = GetPixelColor((unsigned char *)expected.data + ((posY < 0)? 0 : (posY*expected.width + posX))*destinationBytes, expected.format);
            printf("FAIL src=%d dst=%d tint=(%u,%u,%u,%u) variant=%d got=(%u,%u,%u,%u) expected=(%u,%u,%u,%u)\n",
                sourceFormat, destinationFormat, tint.r, tint.g, tint.b, tint.a, variant,
                got.r, got.g, got.b, got.a, want.r, want.g, want.b, want.a);
        }
        failures++;
    }
    UnloadImage(sampled);
    UnloadImage(expected);
    UnloadImage(actual);
    UnloadImage(source);
}

int main(void)
{
    const int sourceFormats[] = {
        PIXELFORMAT_UNCOMPRESSED_GRAYSCALE,
        PIXELFORMAT_UNCOMPRESSED_R5G6B5,
        PIXELFORMAT_UNCOMPRESSED_R8G8B8,
        PIXELFORMAT_UNCOMPRESSED_R32,
        PIXELFORMAT_UNCOMPRESSED_R32G32B32,
        PIXELFORMAT_UNCOMPRESSED_R16,
        PIXELFORMAT_UNCOMPRESSED_R16G16B16,
        PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA,
        PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        PIXELFORMAT_UNCOMPRESSED_R4G4B4A4
    };
    const Color tints[] = { WHITE, RED, { 128, 200, 64, 255 }, { 128, 200, 64, 128 }, { 128, 200, 64, 0 } };
    for (int s = 0; s < (int)(sizeof(sourceFormats)/sizeof(sourceFormats[0])); s++)
    {
        for (int d = 0; d < 2; d++)
        {
            for (int t = 0; t < (int)(sizeof(tints)/sizeof(tints[0])); t++)
            {
                for (int v = 0; v < 3; v++)
                {
                    CheckDraw(sourceFormats[s], d? PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 : sourceFormats[s], tints[t], v);
                }
            }
        }
    }
    printf("ImageDrawImagePro tint regression: %d checks, %d failures\n", checks, failures);
    return failures? 1 : 0;
}
