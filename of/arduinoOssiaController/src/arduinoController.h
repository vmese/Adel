#ifndef ARDUINOCONTROLLER_H
#define ARDUINOCONTROLLER_H

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

class arduinoController
{
public:
    //arduinoController(const std::string &name, const int &rate);
    arduinoController();
    void initialize(const int &version);
    void update();
    bool connect();
    void disconnect();
    static arduinoController* getInstance();
    static void initializeCallback(const int &version);
    bool isInitialized();
    void setup();
    void loadConfiguration(const std::string &fileName);
    void sendServoAngle(int pinNb,int angle);

private :
    ofArduino	*arduino;
    std::string fPortName;
    int fRate;
    bool xInitialized;
    static arduinoController *fInstance;
    ofEvent <const int> arduinoInitiliazedEvent;

    boost::mutex fMutex;
    ofxXmlSettings fXMLReader;

    std::vector<std::string> fServosNames;
    std::vector<int> fServosPins;
    std::vector<int> fServosInitialPositions;

};

#endif // ARDUINOCONTROLLER_H
