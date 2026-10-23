#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32)
#include <direct.h>
#else
#include <unistd.h>
#endif

static int failures = 0;

static bool FailSave(const char *fileName, const void *data, int dataSize)
{
    (void)fileName;
    (void)data;
    (void)dataSize;
    return false;
}

static bool SaveTruncated(const char *fileName, const void *data, int dataSize)
{
    FILE *file = fopen(fileName, "wb");
    if (file == NULL) return false;
    size_t written = fwrite(data, 1, (size_t)dataSize - 1, file);
    return (fclose(file) == 0) && (written == (size_t)dataSize - 1);
}

static void Expect(const char *name, bool passed)
{
    if (!passed)
    {
        printf("FAIL %s\n", name);
        failures++;
    }
}

int main(void)
{
    char directory[128] = { 0 };
    char source[160] = { 0 };
    char destination[160] = { 0 };
    int dataSize = 0;
    const char original[] = "source content";
    const char previous[] = "target content";

    SetTraceLogLevel(LOG_NONE);
    SetRandomSeed((unsigned int)time(NULL));
    do
    {
        snprintf(directory, sizeof(directory), "raylib-file-test-%d", GetRandomValue(0, 32767));
    } while (DirectoryExists(directory));
    if (MakeDirectory(directory) != 0) return 1;
    snprintf(source, sizeof(source), "%s/source.bin", directory);
    snprintf(destination, sizeof(destination), "%s/target.bin", directory);

    Expect("prepare source", SaveFileData(source, original, sizeof(original)));
    Expect("prepare existing destination", SaveFileData(destination, previous, sizeof(previous)));
    SetSaveFileDataCallback(FailSave);
    Expect("copy reports write failure", FileCopy(source, destination) != 0);
    Expect("move reports write failure", FileMove(source, destination) != 0);
    Expect("failed move preserves source", FileExists(source));
    SetSaveFileDataCallback(SaveTruncated);
    Expect("move reports incomplete destination", FileMove(source, destination) != 0);
    Expect("incomplete move preserves source", FileExists(source));
    SetSaveFileDataCallback(NULL);

    Expect("restore source", SaveFileData(source, original, sizeof(original)));
    Expect("copy succeeds", FileCopy(source, destination) == 0);
    unsigned char *data = LoadFileData(destination, &dataSize);
    Expect("copy preserves source", FileExists(source));
    Expect("copy writes complete content", (data != NULL) && (dataSize == sizeof(original)) &&
        (memcmp(data, original, sizeof(original)) == 0));
    UnloadFileData(data);

    Expect("move succeeds", FileMove(source, destination) == 0);
    Expect("successful move removes source", !FileExists(source));
    Expect("copy missing source reports failure", FileCopy(source, destination) != 0);
    FileRemove(destination);
#if defined(_WIN32)
    Expect("remove test directory", _rmdir(directory) == 0);
#else
    Expect("remove test directory", rmdir(directory) == 0);
#endif

    if (failures == 0) puts("PASS file copy and move failure handling");
    return failures != 0;
}
