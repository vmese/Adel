#include "arduinoController.h"


arduinoController * arduinoController::fInstance = 0;


void arduinoController::initializeCallback(const int &version)
{
    ofLogNotice() << "Entering into init callback";
    arduinoController *tmp = arduinoController::getInstance();
    tmp->initialize(version);
}

arduinoController::arduinoController():
xInitialized(false),
fPortName(""),
fRate(0)
{
  printf("arduinoController::arduinoController\n");
  fInstance = this;
  xInitialized = false;
  arduino = new ofArduino();
  if (arduino == NULL)
  {
      ofLogError("Error while contructing arduino");
  }
}

bool arduinoController::connect()
{
    printf("arduinoController::connect\n");
    if (fPortName == "" || fRate == 0)
    {
        ofLogError() << "name of the port and baud rate not specified";
    }

    ofLogNotice() << "Try to connect to " << fPortName << " at " << fRate;
    bool connected = arduino->connect(fPortName,fRate);
    printf("Wait for ready Event\n");
    ofAddListener(arduino->EInitialized,&initializeCallback);
}

void arduinoController::initialize(const int &version)
{
    //boost::mutex::scoped_lock lock(fMutex);
    ofLogNotice() << "Entering into init";
    ofRemoveListener(arduino->EInitialized,&initializeCallback);
    printf("getFirmwareNames\n");
    ofLogNotice() << arduino->getFirmwareName();
    ofLogNotice() << "firmata v" << arduino->getMajorFirmwareVersion() << "." << arduino->getMinorFirmwareVersion();

    xInitialized = true;

    setup();

    printf("done\n");

}

void arduinoController::setup()
{
    //boost::mutex::scoped_lock lock(fMutex);
    printf("arduinoController::setup()\n");
    if (xInitialized)
    {
        for (int i=0;i<fServosNames.size();i++)
        {
            arduino->sendServoAttach(fServosPins[i]);
            arduino->sendServo(fServosPins[i], fServosInitialPositions[i], false);
        }
    }
    printf("done::setup()\n");
}

void arduinoController::sendServoAngle(int pinNb,int angle)
{
    arduino->sendServo(pinNb, angle, false);
}

void arduinoController::disconnect()
{
   arduino->disconnect();
   xInitialized = false;

}

void arduinoController::update()
{

    arduino->update();
}

arduinoController *arduinoController::getInstance()
{
    return fInstance;
}

bool arduinoController::isInitialized()
{
    return xInitialized;
}

void arduinoController::loadConfiguration(const std::string &fileName)
{
    std::string message;
    if( fXMLReader.loadFile(fileName) ){
        message = fileName + " loaded!";
        ofLogNotice() << message;
    }else{
        message = "unable to load " + fileName + " - check data/ folder";
        ofLogError() << message;
    }

    // find port name and baudrate
    std::string portName = fXMLReader.getValue("port::name", "");
    int rate = fXMLReader.getValue("port::rate", 0);

    if (portName == "" || rate == 0)
    {
        ofLogError() << "name of the port and baud rate not find in xml";
    }
    else
    {
        fPortName = portName ;
        fRate = rate;
    }

    // find servos names
    fServosNames.clear();
    fServosPins.clear();
    fServosInitialPositions.clear();

    int nbServos = fXMLReader.getNumTags("servo");
    if (nbServos>0)
    {
        for(int i = 0; i < nbServos; i++)
        {
              fXMLReader.pushTag("servo", i);
              std::string servoName = fXMLReader.getValue("name","");
              int pinNb = fXMLReader.getValue("pinNb",0);
              int pos = fXMLReader.getValue("initialPos",0);
              ofLogNotice()<< "Servo " << servoName << "attached to pin " << pinNb;
              if (servoName!="" && pinNb!=0)
              {
                  fServosNames.push_back(servoName);
                  fServosPins.push_back(pinNb);
                  fServosInitialPositions.push_back(pos);
              }
              fXMLReader.popTag();
         }

    }
}
