
#include "StdAfx.h"
#include "ofSerialWin.h"
#include "ofTypes.h"

#ifdef TARGET_WIN32

#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <algorithm>

#pragma comment (lib, "Setupapi.lib")

//------------------------------------
   // needed for serial bus enumeration:
   //4d36e978-e325-11ce-bfc1-08002be10318}
   DEFINE_GUID (GUID_SERENUM_BUS_ENUMERATOR, 0x4D36E978, 0xE325,
   0x11CE, 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18);
//------------------------------------

void ofSerial::enumerateWin32Ports(){

    // thanks joerg for fixes...

	if (bPortsEnumerated == true) return;

	HDEVINFO hDevInfo = NULL;
	SP_DEVINFO_DATA DeviceInterfaceData;
	int i = 0;
	DWORD dataType, actualSize = 0;
	unsigned char dataBuf[MAX_PATH + 1];

	// Reset Port List
	nPorts = 0;
	// Search device set
	hDevInfo = SetupDiGetClassDevs((struct _GUID *)&GUID_SERENUM_BUS_ENUMERATOR,0,0,DIGCF_PRESENT);
	if ( hDevInfo ){
      while (TRUE){
         ZeroMemory(&DeviceInterfaceData, sizeof(DeviceInterfaceData));
         DeviceInterfaceData.cbSize = sizeof(DeviceInterfaceData);
         if (!SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInterfaceData)){
             // SetupDiEnumDeviceInfo failed
             break;
         }

         if (SetupDiGetDeviceRegistryPropertyA(hDevInfo,
             &DeviceInterfaceData,
             SPDRP_FRIENDLYNAME,
             &dataType,
             dataBuf,
             sizeof(dataBuf),
             &actualSize)){

			sprintf(portNamesFriendly[nPorts], "%s", dataBuf);
			portNamesShort[nPorts][0] = 0;

			// turn blahblahblah(COM4) into COM4

            char *   begin    = NULL;
            char *   end    = NULL;
            begin          = strstr((char *)dataBuf, "COM");


            if (begin)
                {
                end          = strstr(begin, ")");
                if (end)
                    {
                      *end = 0;   // get rid of the )...
                      strcpy(portNamesShort[nPorts], begin);
                }
                if (nPorts++ > MAX_SERIAL_PORTS)
                        break;
            }
         }
            i++;
      }
   }
   SetupDiDestroyDeviceInfoList(hDevInfo);

   bPortsEnumerated = false;
}



//----------------------------------------------------------------
ofSerial::ofSerial(){

	nPorts 				= 0;
	bPortsEnumerated 	= false;

	portNamesShort = new char * [MAX_SERIAL_PORTS];
	portNamesFriendly = new char * [MAX_SERIAL_PORTS];
	for (int i = 0; i < MAX_SERIAL_PORTS; i++){
		portNamesShort[i] = new char[10];
		portNamesFriendly[i] = new char[MAX_PATH];
	}

	bInited = false;
}

//----------------------------------------------------------------
ofSerial::~ofSerial(){

	close();



	nPorts 				= 0;
	bPortsEnumerated 	= false;

	for (int i = 0; i < MAX_SERIAL_PORTS; i++) {
		delete [] portNamesShort[i];
		delete [] portNamesFriendly[i];
	}
	delete [] portNamesShort;
	delete [] portNamesFriendly;

	bInited = false;
}

//----------------------------------------------------------------
static bool isDeviceArduino( ofSerialDeviceInfo & A ){
	//TODO - this should be ofStringInString
	return ( strstr(A.getDeviceName().c_str(), "usbserial") != NULL ||
		 strstr(A.getDeviceName().c_str(), "usbmodem") != NULL );
}

//----------------------------------------------------------------
void ofSerial::buildDeviceList(){

	deviceType = "serial";
	devices.clear();

	std::vector <std::string> prefixMatch;

	enumerateWin32Ports();
	std::cout << "found " << nPorts << " devices" << "\r\n";
	for (int i = 0; i < nPorts; i++){
		//NOTE: we give the short port name for both as that is what the user should pass and the short name is more friendly
		devices.push_back(ofSerialDeviceInfo(std::string(portNamesShort[i]), std::string(portNamesShort[i]), i));
	}

	//here we sort the device to have the aruino ones first.
	partition(devices.begin(), devices.end(), isDeviceArduino);
	//we are reordering the device ids. too!
	for(int k = 0; k < (int)devices.size(); k++){
		devices[k].deviceID = k;
	}

	bHaveEnumeratedDevices = true;
}


//----------------------------------------------------------------
void ofSerial::listDevices(){
	buildDeviceList();
	for(int k = 0; k < (int)devices.size(); k++){
		std::cout << "[" << devices[k].getDeviceID() << "] = "<< devices[k].getDeviceName().c_str() << "\r\n";
	}
}

//----------------------------------------------------------------
std::vector <ofSerialDeviceInfo> ofSerial::getDeviceList(){
	buildDeviceList();
	return devices;
}

//----------------------------------------------------------------
void ofSerial::enumerateDevices(){
	listDevices();
}

//----------------------------------------------------------------
void ofSerial::close(){

	if (bInited){
		std::cout << "closing serial" << "\r\n";
		SetCommTimeouts(hComm,&oldTimeout);
		CloseHandle(hComm);
		hComm 		= INVALID_HANDLE_VALUE;
		bInited 	= false;
	}

}

//----------------------------------------------------------------
bool ofSerial::setup(){
	return setup(0,9600);		// the first one, at 9600 is a good choice...
}

//----------------------------------------------------------------
bool ofSerial::setup(int deviceNumber, int baud){

	buildDeviceList();
	if( deviceNumber < (int)devices.size() ){
		return setup(devices[deviceNumber].devicePath, baud);
	}else{
		std::cout << "couldn't find device " << deviceNumber << ", only " << devices.size() << " devices found" << "\r\n";
		return false;
	}

}

//----------------------------------------------------------------
bool ofSerial::setup(std::string portName, int baud){

	std::cout << "setup serial" << "\r\n";
	bInited = false;

	char pn[sizeof(portName)];
	int num;
	if (sscanf(portName.c_str(), "COM%d", &num) == 1) {
		// Microsoft KB115831 a.k.a if COM > COM9 you have to use a different
		// syntax
		sprintf(pn, "\\\\.\\COM%d", num);
	} else {
		strncpy(pn, (const char *)portName.c_str(), sizeof(portName)-1);
	}

	// open the serial port:
	// "COM4", etc...

	hComm=CreateFileA(pn,GENERIC_READ|GENERIC_WRITE,0,0,
					OPEN_EXISTING,0,0);

	if(hComm==INVALID_HANDLE_VALUE){
		std::cout << "setup(): unable to open " << portName << "\r\n";
		return false;
	}


	// now try the settings:
	COMMCONFIG cfg;
	DWORD cfgSize;
	char  buf[80];

	cfgSize=sizeof(cfg);
	GetCommConfig(hComm,&cfg,&cfgSize);
	int bps = baud;
	sprintf(buf,"baud=%d parity=N data=8 stop=1",bps);

	#if (_MSC_VER)       // microsoft visual studio
		// msvc doesn't like BuildCommDCB,
		//so we need to use this version: BuildCommDCBA
		if(!BuildCommDCBA(buf,&cfg.dcb)){
			std::cout << "setup(): unable to build comm dcb, (" << buf << ")" << "\r\n";
		}
	#else
		if(!BuildCommDCB(buf,&cfg.dcb)){
			std::cout << "setup(): unable to build comm dcb, (" << buf << ")" << "\r\n";
		}
	#endif


	// Set baudrate and bits etc.
	// Note that BuildCommDCB() clears XON/XOFF and hardware control by default

	if(!SetCommState(hComm,&cfg.dcb)){
		std::cout << "setup(): couldn't set comm state: " << cfg.dcb.BaudRate << " bps, xio " << cfg.dcb.fInX << "/" << cfg.dcb.fOutX << "\r\n";
	}
	//std::cout << "bps=" << cfg.dcb.BaudRate << ", xio=" << cfg.dcb.fInX << "/" << cfg.dcb.fOutX << "\r\n";

	// Set communication timeouts (NT)
	COMMTIMEOUTS tOut;
	GetCommTimeouts(hComm,&oldTimeout);
	tOut = oldTimeout;
	// Make timeout so that:
	// - return immediately with buffered characters
	tOut.ReadIntervalTimeout=MAXDWORD;
	tOut.ReadTotalTimeoutMultiplier=0;
	tOut.ReadTotalTimeoutConstant=0;
	SetCommTimeouts(hComm,&tOut);

	bInited = true;
	return true;
}


//----------------------------------------------------------------
int ofSerial::writeBytes(unsigned char * buffer, int length){

	if (!bInited){
		std::cout << "writeBytes(): serial not inited" << "\r\n";
		return OF_SERIAL_ERROR;
	}

	DWORD written;
	if(!WriteFile(hComm, buffer, length, &written,0)){
			//std::cout << "writeBytes(): couldn't write to port" << "\r\n";
			return OF_SERIAL_ERROR;
	}
	//std::cout <<  "wrote " << (int) written << " bytes" << "\r\n";
	return (int)written;
}

//----------------------------------------------------------------
int ofSerial::readBytes(unsigned char * buffer, int length){

	if (!bInited){
		std::cout << "readBytes(): serial not inited" << "\r\n";
		return OF_SERIAL_ERROR;
	}

	DWORD nRead = 0;
	if (!ReadFile(hComm,buffer,length,&nRead,0)){
		//std::cout << "readBytes(): couldn't read from port" << "\r\n";
		return OF_SERIAL_ERROR;
	}
	return (int)nRead;
}

//----------------------------------------------------------------
bool ofSerial::writeByte(unsigned char singleByte){

	if (!bInited){
		std::cout << "writeByte(): serial not inited" << "\r\n";
		//return OF_SERIAL_ERROR; // this looks wrong.
		return false;
	}

	unsigned char tmpByte[1];
	tmpByte[0] = singleByte;

	DWORD written = 0;
	if(!WriteFile(hComm, tmpByte, 1, &written,0)){
			//std::cout << "writeByte(): couldn't write to port" << "\r\n";
			//return OF_SERIAL_ERROR; // this looks wrong.
			return false;
	}

	//std::cout << "wrote byte" << "\r\n";

	return ((int)written > 0 ? true : false);
}

//----------------------------------------------------------------
int ofSerial::readByte(){

	if (!bInited){
		std::cout << "readByte(): serial not inited" << "\r\n";
		return OF_SERIAL_ERROR;
	}

	unsigned char tmpByte[1];
	memset(tmpByte, 0, 1);

	DWORD nRead;
	if (!ReadFile(hComm, tmpByte, 1, &nRead, 0)){
		std::cout << "readByte(): couldn't read from port" << "\r\n";
		return OF_SERIAL_ERROR;
	}

	return (int)(tmpByte[0]);
}


//----------------------------------------------------------------
void ofSerial::flush(bool flushIn, bool flushOut){

	if (!bInited){
		std::cout << "flush(): serial not inited" << "\r\n";
		return;
	}

	int flushType = 0;

	if( flushIn && flushOut) flushType = PURGE_TXCLEAR | PURGE_RXCLEAR;
	else if(flushIn) flushType = PURGE_RXCLEAR;
	else if(flushOut) flushType = PURGE_TXCLEAR;
	else return;

	PurgeComm(hComm, flushType);
}

void ofSerial::drain(){
    if (!bInited){
		std::cout << "drain(): serial not inited" << "\r\n";
		return;
    }
}

//-------------------------------------------------------------
int ofSerial::available(){

	if (!bInited){
		std::cout << "available(): serial not inited" << "\r\n";
		return OF_SERIAL_ERROR;
	}

	int numBytes = 0;

	COMSTAT stat;
    DWORD err;
    if(hComm!=INVALID_HANDLE_VALUE){
        if(!ClearCommError(hComm, &err, &stat)){
            numBytes = 0;
        } else {
            numBytes = stat.cbInQue;
        }
    } else {
        numBytes = 0;
    }

	return numBytes;
}

bool ofSerial::isInitialized() const{
	return bInited;
}

#endif