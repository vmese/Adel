#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofBackground(0,0,0); // black
    ofBackground(255,255,255); // white

    // setup gui from ofxGui
    _gui.setup("Gui");
    _gui.setPosition(0 , 0);
    _gui.minimizeAll();

    //setup Ossia
    fOssia.setup();
    fOssiaAngleControl.setup(fOssia.get_root_node(), "Control");
    fOssiaAngleServo1.setup(fOssiaAngleControl, "servo1", 0.5, 0., 1.);
    fOssiaAngleServo2.setup(fOssiaAngleControl, "servo2", 0.5, 0., 1.);

    // create a control zone named servo 1 with root node as parent
    servo1.setOssiaParams(fOssia.get_root_node(), "servo1");
    servo2.setOssiaParams(fOssia.get_root_node(), "servo2");

    servo1.setup( 9, 0, 180);
    servo2.setup(10, 70, 110);

    ofSetLogLevel(OF_LOG_VERBOSE);

    arduino = new arduinoController();
    arduino->loadConfiguration("arduinoConfig.xml");
    servo1.setController(arduino);
    servo2.setController(arduino);

    arduino->connect();



//    while(!arduino->isInitialized())
//    {
//        usleep(10000);
//    }
    //arduino->setup();


    // Connect with direct method
//    ard.connect("/dev/ttyACM0", 57600);
//    printf("wait for ready event\n");
//    ofAddListener(ard.EInitialized,this,&ofApp::setupArduino);

    _gui.add (fOssia.get_root_node());
}

void ofApp::setupArduino(const int &version)
{
    ofRemoveListener(ard.EInitialized, this, &ofApp::setupArduino);
    printf("received\n");
    printf("get infos\n");
    ofLogNotice() << ard.getFirmwareName();
    ofLogNotice() << "firmata v" << ard.getMajorFirmwareVersion() << "." << ard.getMinorFirmwareVersion();
    printf("done\n");

    printf("Setup done\n");

}

//--------------------------------------------------------------
void ofApp::update(){

    servo1.setAngle(fOssiaAngleServo1);
    servo2.setAngle(fOssiaAngleServo2);

    //Rq : need to be called after arduino->connect
    arduino->update();

    if (arduino->isInitialized()) {
        servo1.update();
        servo2.update();
     }


}

//--------------------------------------------------------------
void ofApp::draw(){
 _gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::exit()
{
    ofLogNotice() << "Exit app";
    arduino->disconnect();
}
