#include "raylib.h"

#include <math.h>
#include <stdio.h>

static int CheckPose(Model model, float translation, float rotation, float scale, const char *name)
{
    const Transform pose = model.currentPose[0];
    const float tolerance = 0.0001f;
    int failed = !(fabsf(pose.translation.x - translation) <= tolerance) ||
        !(fabsf(pose.rotation.z - sinf(rotation*0.5f)) <= tolerance) ||
        !(fabsf(pose.rotation.w - cosf(rotation*0.5f)) <= tolerance) ||
        !(fabsf(pose.scale.x - scale) <= tolerance);

    if (failed)
    {
        printf("FAIL %s: translation=%f rotation=(%f, %f) scale=%f\n",
            name, pose.translation.x, pose.rotation.z, pose.rotation.w, pose.scale.x);
    }

    return failed;
}

int main(void)
{
    BoneInfo bone = { 0 };
    Transform bindPose = { { 0 }, { 0, 0, 0, 1 }, { 1, 1, 1 } };
    Transform currentPose = { 0 };
    Matrix boneMatrix = { 0 };
    Model model = { 0 };
    model.skeleton.boneCount = 1;
    model.skeleton.bones = &bone;
    model.skeleton.bindPose = &bindPose;
    model.currentPose = &currentPose;
    model.boneMatrices = &boneMatrix;

    Transform framesA[2] = {
        { { 0, 0, 0 }, { 0, 0, 0, 1 }, { 1, 1, 1 } },
        { { 10, 0, 0 }, { 0, 0, 0.7071067812f, 0.7071067812f }, { 3, 3, 3 } }
    };
    Transform framesB[3] = {
        { { 20, 0, 0 }, { 0, 0, 0, 1 }, { 3, 3, 3 } },
        { { 30, 0, 0 }, { 0, 0, 0.7071067812f, 0.7071067812f }, { 5, 5, 5 } },
        { { 40, 0, 0 }, { 0, 0, 0.7071067812f, 0.7071067812f }, { 7, 7, 7 } }
    };
    ModelAnimPose posesA[] = { &framesA[0], &framesA[1] };
    ModelAnimPose posesB[] = { &framesB[0], &framesB[1], &framesB[2] };
    ModelAnimation animA = { 0 };
    animA.boneCount = 1;
    animA.keyframeCount = 2;
    animA.keyframePoses = posesA;
    ModelAnimation animB = { 0 };
    animB.boneCount = 1;
    animB.keyframeCount = 3;
    animB.keyframePoses = posesB;

    int failures = 0;
    UpdateModelAnimationEx(model, animA, 0.25f, animA, 0.25f, 0.0f);
    failures += CheckPose(model, 2.5f, PI/8.0f, 1.5f, "first cycle");
    UpdateModelAnimationEx(model, animA, 2.25f, animA, 2.25f, 0.0f);
    failures += CheckPose(model, 2.5f, PI/8.0f, 1.5f, "wrapped first animation");
    UpdateModelAnimationEx(model, animA, 3.25f, animA, 3.25f, 0.0f);
    failures += CheckPose(model, 7.5f, 3.0f*PI/8.0f, 2.5f, "wrapped last-to-first interpolation");
    UpdateModelAnimationEx(model, animA, 2.25f, animB, 3.25f, 0.25f);
    failures += CheckPose(model, 7.5f, PI/8.0f, 2.0f, "different animation lengths");
    UpdateModelAnimationEx(model, animA, 2.0f, animB, 3.0f, 0.5f);
    failures += CheckPose(model, 10.0f, 0.0f, 2.0f, "wrapped integer frames");
    UpdateModelAnimationEx(model, animA, 0.0f, animB, 3.5f, 1.0f);
    failures += CheckPose(model, 25.0f, PI/4.0f, 4.0f, "wrapped second animation");

    animA.keyframeCount = 1;
    UpdateModelAnimationEx(model, animA, 5.75f, animA, 5.75f, 0.5f);
    failures += CheckPose(model, 0.0f, 0.0f, 1.0f, "single keyframe");

    if (failures == 0) puts("PASS model animation interpolation");
    return failures != 0;
}
