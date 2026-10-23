#include <math.h>
#include <stdio.h>
#include "raylib.h"

#define RGESTURES_IMPLEMENTATION
#include "rgestures.h"

static double testTime = 1.0;
static int failures = 0;
static int checks = 0;

double GetTime(void)
{
    return testTime;
}

static void ExpectGesture(const char *name, int expected)
{
    int actual = GetGestureDetected();
    checks++;
    if (actual != expected)
    {
        printf("FAIL %s: expected %d, got %d\n", name, expected, actual);
        failures++;
    }
}

static void TwoTouchEvent(int action, float leftX, float rightX)
{
    GestureEvent event = { 0 };
    event.touchAction = action;
    event.pointCount = 2;
    event.pointId[0] = 0;
    event.pointId[1] = 1;
    event.position[0] = (Vector2){ leftX, 0.5f };
    event.position[1] = (Vector2){ rightX, 0.5f };
    testTime += 0.016;
    UpdateGestures();
    ProcessGestureEvent(event);
}

static void PinchEvent(int action, float rightX)
{
    TwoTouchEvent(action, 0.1f, rightX);
}

int main(void)
{
    SetGesturesEnabled(0x3ff);

    PinchEvent(TOUCH_ACTION_DOWN, 0.35f);
    ExpectGesture("two-finger hold", GESTURE_HOLD);
    PinchEvent(TOUCH_ACTION_MOVE, 0.65f);
    ExpectGesture("initial pinch out", GESTURE_PINCH_OUT);
    PinchEvent(TOUCH_ACTION_MOVE, 0.55f);
    ExpectGesture("reverse to pinch in above initial separation", GESTURE_PINCH_IN);
    PinchEvent(TOUCH_ACTION_MOVE, 0.55f);
    ExpectGesture("stationary fingers after pinch", GESTURE_HOLD);
    PinchEvent(TOUCH_ACTION_UP, 0.55f);
    ExpectGesture("pinch release", GESTURE_NONE);

    PinchEvent(TOUCH_ACTION_DOWN, 0.75f);
    PinchEvent(TOUCH_ACTION_MOVE, 0.35f);
    ExpectGesture("initial pinch in", GESTURE_PINCH_IN);
    PinchEvent(TOUCH_ACTION_MOVE, 0.45f);
    ExpectGesture("reverse to pinch out below initial separation", GESTURE_PINCH_OUT);
    PinchEvent(TOUCH_ACTION_MOVE, 0.55f);
    ExpectGesture("continued pinch out", GESTURE_PINCH_OUT);
    PinchEvent(TOUCH_ACTION_MOVE, 0.551f);
    ExpectGesture("movement below pinch threshold", GESTURE_HOLD);
    PinchEvent(TOUCH_ACTION_UP, 0.551f);

    PinchEvent(TOUCH_ACTION_DOWN, 0.35f);
    PinchEvent(TOUCH_ACTION_MOVE, 0.351f);
    ExpectGesture("initial movement below pinch threshold", GESTURE_HOLD);
    PinchEvent(TOUCH_ACTION_MOVE, 0.55f);
    ExpectGesture("fresh pinch after release", GESTURE_PINCH_OUT);
    Vector2 pinch = GetGesturePinchVector();
    checks++;
    if ((fabsf(pinch.x - 0.45f) > 0.000001f) || (fabsf(pinch.y) > 0.000001f))
    {
        printf("FAIL pinch vector: got (%g, %g)\n", pinch.x, pinch.y);
        failures++;
    }
    SetGesturesEnabled(GESTURE_PINCH_IN);
    ExpectGesture("disabled pinch out", GESTURE_NONE);
    SetGesturesEnabled(0x3ff);
    PinchEvent(TOUCH_ACTION_UP, 0.55f);

    TwoTouchEvent(TOUCH_ACTION_DOWN, 0.3f, 0.7f);
    TwoTouchEvent(TOUCH_ACTION_MOVE, 0.2f, 0.7f);
    ExpectGesture("first finger pinch out", GESTURE_PINCH_OUT);
    TwoTouchEvent(TOUCH_ACTION_MOVE, 0.25f, 0.7f);
    ExpectGesture("first finger reversal", GESTURE_PINCH_IN);
    TwoTouchEvent(TOUCH_ACTION_MOVE, 0.25f, 0.7f);
    ExpectGesture("first finger stops", GESTURE_HOLD);
    TwoTouchEvent(TOUCH_ACTION_MOVE, 0.3f, 0.65f);
    ExpectGesture("both fingers pinch in", GESTURE_PINCH_IN);
    TwoTouchEvent(TOUCH_ACTION_MOVE, 0.2f, 0.75f);
    ExpectGesture("both fingers reverse to pinch out", GESTURE_PINCH_OUT);
    TwoTouchEvent(TOUCH_ACTION_UP, 0.2f, 0.75f);
    ExpectGesture("both fingers release", GESTURE_NONE);

    GestureEvent tap = { 0 };
    tap.touchAction = TOUCH_ACTION_DOWN;
    tap.pointCount = 1;
    tap.position[0] = (Vector2){ 0.5f, 0.5f };
    ProcessGestureEvent(tap);
    ExpectGesture("single-finger tap", GESTURE_TAP);
    UpdateGestures();
    ExpectGesture("single-finger hold", GESTURE_HOLD);

    printf("%d checks, %d failures\n", checks, failures);
    return failures? 1 : 0;
}
