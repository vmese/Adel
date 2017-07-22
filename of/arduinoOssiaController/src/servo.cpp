#include "servo.h"

servo::servo():
fIsInitialized(false)
{

}

void servo::setup(int pinNb, int min, int max)
{
    fMin.setup(fOssiaMinMaxControl, "min", min, 0, 180);
    fMax.setup(fOssiaMinMaxControl, "max", max, 0, 180);

    fPinNb = pinNb;
    fIsInitialized = true;
}

void servo::setOssiaParams(ossia::ParameterGroup ossiaParentNode, string name)
{
    fOssiaServoControl.setup(ossiaParentNode, name);
    fOssiaMinMaxControl.setup(fOssiaServoControl, "MIN/MAX");
    //_gl.setup(_joint, "my3d");
}

void servo::update()
{

    if (fIsInitialized)
    {
        int angle = (int)ofMap(fAngle, 0., 1., fMin, fMax);
        if (fController==NULL)
        {
         ofLogError("Servo Controller not set");
        }
        else
        {
            fController->sendServoAngle(fPinNb, angle);
        }
    }
}

void servo::draw()
{

}

