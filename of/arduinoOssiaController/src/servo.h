#ifndef SERVO_H
#define SERVO_H


#include "ofMain.h"
#include "ofxOssia.h"
#include "arduinoController.h"

class servo
{
public:
    servo();
    void setOssiaParams(ossia::ParameterGroup ossiaParentNode, string name);
    void update();
    void draw();
    void setController(arduinoController *controller) {fController = controller;};
    void setup(int pinNb, int min, int max);
    void setAngle(float angle) { fAngle = angle;}

private:
    arduinoController *fController;
    float fAngle;
    ossia::Parameter <int>  fMin;
    ossia::Parameter <int>  fMax;
    int fPinNb;
    bool fIsInitialized;

    // OSSIA parameters
    ossia::ParameterGroup fOssiaServoControl;
    ossia::ParameterGroup fOssiaMinMaxControl;
};

#endif // SERVO_H
