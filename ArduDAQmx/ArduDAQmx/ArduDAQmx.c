/*!
 * \file ArduDAQmx.c
 * \brief ArduDAQmx: A simple Arduino-esque wrapper for the National Instruments NI-DAQmx C/C++ API
 * \author Suraj Chakravarthi Raja
 */

//#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include "ArduDAQmx.h"
#include "ansi_c.h"
#include "macrodef.h"
#include "NIDAQmx.h"
#include <string.h>

int ArduDAQmxFirstEnum = 1; // Flag to indicate that this is the first device enumeration call hasn't been invoked.

// initializations for global status variables that keep track of library operations
	// EXTERN declarations
int				ArduDAQmxStatus				= (int) STATUS_PRECONFIG;
int				ArduDAQmxError				= 0;
long			NIDAQmxErrorCode			= 0;
const char		*DefaultArduDAQmxDevPrefix	= "PXI1Slot";
char			*ArduDAQmxDevPrefix			= NULL;
const unsigned	MaxArduDAQmxDevPrefixLength	= 8;
const unsigned	MaxArduDAQmxDevStringLength = MaxArduDAQmxDevPrefixLength + 2;
const unsigned	MaxNIstringLength			= 255;
unsigned		ArduDAQmxDevPrefixLength	= MaxArduDAQmxDevPrefixLength;
DAQmxDevice		*ArduDAQmxDevList			= NULL;
unsigned long	ArduDAQmxDevCount			= 0;
unsigned long	ArduDAQmxDevMaxNum			= 0;
cLinkedList		*DAQmxEnumeratedDevList		= NULL;
unsigned long	DAQmxEnumeratedDevCount		= 0;
unsigned long	DAQmxEnumeratedDevMaxNum	= 0;
cLinkedList		*ArduDAQmxTaskList			= NULL;
unsigned long	ArduDAQmxTaskCount			= 0;

sampleClock		ArduDAQmxSampleClock;

// Library support function definitions
char* dev2string(char *strBuf, unsigned int devNum)
{
	snprintf(strBuf, 1 + MaxArduDAQmxDevStringLength, "%s%d", ArduDAQmxDevPrefix, devNum);
	return strBuf;
}

char* pin2string(char *strbuf, unsigned int devNum, IOmode ioMode, unsigned int pinNum)
{
	const int pinTypeLength = 10;
	char pinType[pinTypeLength];
	switch (ioMode)
	{
	case IOmode::ANALOG_IN:
		snprintf(pinType, pinTypeLength, "ai");
		break;
	case IOmode::ANALOG_OUT:
		snprintf(pinType, pinTypeLength, "ao");
		break;
	case IOmode::DIGITAL_IN:
		snprintf(pinType, pinTypeLength, "port");
		break;
	case IOmode::DIGITAL_OUT:
		snprintf(pinType, pinTypeLength, "port");
		break;
	case IOmode::COUNTER_IN:
		snprintf(pinType, pinTypeLength, "ctr");
		break;
	case IOmode::COUNTER_OUT:
		snprintf(pinType, pinTypeLength, "ctr");
		break;
	default:
		fprintf(ERRSTREAM, "ArduDAQmx library: FATAL: Invalid I/O type requested.\n");
		break;
	}
	snprintf(strbuf, MaxNIstringLength, "%s%d/%s%d", ArduDAQmxDevPrefix, devNum, pinType, pinNum);
	return strbuf;
}

/*!
 * \fn int32 DAQmxErrChk(int32 NIerrCode)
 * Checks if passed value represents a valid NI-DAQmx error. If DAQmx error is detected it sets 'NIDAQmxErrorCode.'
 * Then, it shuts down all NI-DAQmx tasks and terminates ArduDAQmx library immediately.
 * 
 * \param NIerrCode the INT32 error code from NI-DAQmx is passed here.
 * \return Returns the NI-DAQmx error code as an INT32 value.
 */
inline int32 DAQmxErrChk(int32 NIerrCode)
{
	if (NIerrCode < 0) {
		char DAQmxErrChkStr[2048];
		DAQmxGetExtendedErrorInfo(DAQmxErrChkStr, 2048);
		NIDAQmxErrorCode = NIerrCode;
		setArduDAQmxLastError(ERROR_NIDAQMX, 1);
		ArduDAQmxTerminate();
	}
	return NIerrCode;
}

// Library configuration funcion definitions
/*!
 * \fn void enumerateDAQmxDevices(int printFlag)
 * enumerates all accessible DAQmx devices and populates the 'ArduDAQmxDevList' list along with some status messages.
 * This function can also optionally prints a table with the same list of DAQmx devices.
 * 
 * \param printFlag When set to TRUE(1), the function prints enumerates the devices in a table.
 */
void enumerateDAQmxDevices(int printFlag)
{
	int				buffersize = 0; // Buffer size datatypes
	int				devicetype_buffersize = 0; //devicesernum_buffersize;
	int				tempDAQmxEnumeratedDevCount  = 0; // temp DAQmx dev count that is used to crosscheck

	//Device Info variable initialization
	char			*DAQmxDevNameList = NULL; // primary device name string
	char			*remainder_DAQmxDevNameList = NULL; // remainder device name string
	char			*DAQmxDeviceType = NULL; // device type string
	char			*DAQmxDevName = NULL;
	DAQmxDevice		*newDevice = NULL;

	int				deviceSerial;
	int				isSimulated;
	unsigned long	devNUM = 0;
	int				isObjInserted = 0, DEVenumERR = 0;
	cListElem		*list_elem = NULL;
	
	// Create temporary linked list to enumerate and sort DAQmx devices
	ArduDAQmxClearEnumeratedDevices();
	DAQmxEnumeratedDevList = (cLinkedList *) malloc(sizeof(cLinkedList)); // DYN-M: create DAQmx master device list. Cleared in ArduDAQmxClearEnumeratedDevices()
	cListInit(DAQmxEnumeratedDevList);
	tempDAQmxEnumeratedDevCount = 0;

	// obtain device names from NI-DAQmx
	buffersize = DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames, (void *)DAQmxDevNameList);
	DAQmxDevNameList = (char*) malloc(buffersize); // DYN-M: dynamically allocate devicenames. Cleared at the end of this function.
	remainder_DAQmxDevNameList = DAQmxDevNameList;

	//Get the string of devicenames from NI-DAQmx
	DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames, DAQmxDevNameList, buffersize);

	// Optionally print device table headers
	if (printFlag == 1) {
		fprintf(LOGSTREAM, "Full Device name list: %s\n\n", DAQmxDevNameList);
		fprintf(LOGSTREAM, "*** LIST OF DEVICES ON THIS COMPUTER ******************************************************************\n");
		fprintf(LOGSTREAM, "| Dev# || Dev Type || Dev Serial# || Sim? || AI Ch# || AO Ch# || DI Ch# || DO Ch# || CI Ch# || CO Ch# |\n");
		fprintf(LOGSTREAM, "*******************************************************************************************************\n");
	} // end print flag check

	//Get information about the DAQmx devices on the list
	for (DAQmxDevName = strtok_s(remainder_DAQmxDevNameList, ",", &remainder_DAQmxDevNameList); DAQmxDevName != NULL && DEVenumERR == 0; DAQmxDevName = strtok_s(remainder_DAQmxDevNameList, ", ", &remainder_DAQmxDevNameList), tempDAQmxEnumeratedDevCount++) {
		// Obtain some device properties
		devicetype_buffersize = DAQmxGetDeviceAttribute(DAQmxDevName, DAQmx_Dev_ProductType, NULL); //Get Product Type for a DAQmx device pointed to by devToken
		DAQmxDeviceType = (char*) malloc(devicetype_buffersize); // DYN-M: dynamically allocate memory for devicetype. Memory is cleared later in this function.
		DAQmxGetDeviceAttribute(DAQmxDevName, DAQmx_Dev_ProductType, DAQmxDeviceType, devicetype_buffersize); //Get device type		
		DAQmxGetDeviceAttribute(DAQmxDevName, DAQmx_Dev_SerialNum  , &deviceSerial  , 1); //Get Product Serial Number for the device
		DAQmxGetDeviceAttribute(DAQmxDevName, DAQmx_Dev_IsSimulated, &isSimulated  , 1); //Get "Is device simulated?" flag for the device

		newDevice = (DAQmxDevice *) malloc(sizeof(DAQmxDevice)); // DYN-M: dynamically allocate newDevice for DAQmx device. Freed recursively in ArduDAQmxClearEnumeratedDevices().
			
		//copy retrieved datapoints to the newly created DAQmx device object
		strcpy_s(newDevice->DevName, sizeof(newDevice->DevName), DAQmxDevName); // copy device name in structure
		devNUM					= strtol( &(newDevice->DevName[ArduDAQmxDevPrefixLength]), NULL, 10);
		newDevice->DevNum		= devNUM; // set device number in structure			
		newDevice->DevSerial	= deviceSerial;
		newDevice->isDevSim		= isSimulated;
		//newDevice->pinList		= (pin *) malloc( sizeof(pin) * DAQmxMaxPinCount ); // DYN-M: initialize pin list array
		
		//initialize numbers of available physical channels
		newDevice->numAIch = enumerateDAQmxDeviceChannels(newDevice->DevNum, ANALOG_IN  , 0);
		newDevice->numAOch = enumerateDAQmxDeviceChannels(newDevice->DevNum, ANALOG_OUT , 0);
		newDevice->numDIch = enumerateDAQmxDeviceChannels(newDevice->DevNum, DIGITAL_IN , 0);
		newDevice->numDOch = enumerateDAQmxDeviceChannels(newDevice->DevNum, DIGITAL_OUT, 0);
		newDevice->numCIch = enumerateDAQmxDeviceChannels(newDevice->DevNum, COUNTER_IN , 0);
		newDevice->numCOch = enumerateDAQmxDeviceChannels(newDevice->DevNum, COUNTER_OUT, 0);
		
		//set max device number
		if(DAQmxEnumeratedDevMaxNum < newDevice->DevNum)
			DAQmxEnumeratedDevMaxNum = newDevice->DevNum;

		// Sanity check new device
		if ( newDevice->DevNum == 0 ) { // if object in list has device number as 0, call FATAL error
			fprintf(ERRSTREAM, "ArduDAQmx library: FATAL: A DAQmx device has a device number as 0. All device numbers must be positive integers");
			ArduDAQmxTerminate();
			setArduDAQmxLastError(ERROR_NODEVICES, 1);
			DEVenumERR = -1;
		} else if (newDevice->numDIch != newDevice->numDOch) {
			fprintf(ERRSTREAM, "ArduDAQmx library: FATAL: A DAQmx device has unequal number of digital input and output ports - not supported.");
			ArduDAQmxTerminate();
			setArduDAQmxLastError(ERROR_UNSUPPORTED,1);
			DEVenumERR = -1;
		} else if (newDevice->numDIch != newDevice->numDOch) {
			fprintf(ERRSTREAM, "ArduDAQmx library: FATAL: A DAQmx device has unequal number of counter input and output ports - not supported.");
			ArduDAQmxTerminate();
			setArduDAQmxLastError(ERROR_UNSUPPORTED, 1);
			DEVenumERR = -1;
		}
		// Sort and insert new device object into temporary linked list
		isObjInserted = 0;
		list_elem = cListLastElem(DAQmxEnumeratedDevList);
		while (isObjInserted == 0) {
			if (list_elem == NULL) { // if linked list is empty, append the new device to list
				isObjInserted = 1;
				cListAppend(DAQmxEnumeratedDevList, (void *)newDevice);
			} else if ( ((DAQmxDevice *)list_elem->obj)->DevNum == newDevice->DevNum) { // if more than one object has equal device numbers, call FATAL error
				fprintf(ERRSTREAM, "ArduDAQmx library: FATAL: Multiple DAQmx devices have the same device number. Rename devices in NI MAX.\n");
				ArduDAQmxTerminate();
				setArduDAQmxLastError(ERROR_UNSUPPORTED, 1);
				isObjInserted = 1;
				DEVenumERR = -1;
				//exit(-1);
			} else if ( ((DAQmxDevice *)list_elem->obj)->DevNum <  newDevice->DevNum) { // if list object device number is greater than new device number, insert new device before the list object
				isObjInserted = 1;
				cListInsertAfter(DAQmxEnumeratedDevList, (void *)newDevice, list_elem);
			}

			list_elem = cListPrevElem(DAQmxEnumeratedDevList, list_elem);
		} // end object insertion while loop

		// Optionally print device table contents
		if (printFlag == 1) {
			fprintf(LOGSTREAM, "| %4d || %s || %11d ||", devNUM, DAQmxDeviceType, deviceSerial);
			if (isSimulated = 0)	fprintf(LOGSTREAM, "   NO ||");
			else					fprintf(LOGSTREAM, "  YES ||");
			fprintf(LOGSTREAM, " %6d || %6d || %6d || %6d || %6d || %6d |\n", newDevice->numAIch, newDevice->numAOch, newDevice->numDIch, newDevice->numDOch, newDevice->numCIch, newDevice->numCOch);
		} // end print flag check

		free(DAQmxDeviceType); // DYN-F free device type buffer
	} // end device enumeration for loop

	// Print end of device table, status messages
	
	if (printFlag == 1) {
		if (DEVenumERR != 0) {
			fprintf(LOGSTREAM, "*******************************************************************************************************\n");
			fprintf(LOGSTREAM, "ArduDAQmx library: The last device printed above encountered an error. Terminating library.\n");
			printArduDAQmxStatus();
			fprintf(LOGSTREAM, "*******************************************************************************************************\n");

		} else {
			fprintf(LOGSTREAM, "*******************************************************************************************************\n");
			fprintf(LOGSTREAM, "ArduDAQmx library: %d devices in internal device list\n", cListLength(DAQmxEnumeratedDevList));
			printArduDAQmxStatus();
			fprintf(LOGSTREAM, "*******************************************************************************************************\n");
		}
	} // end print flag check

	//Sanity check comparison from previous enumeration
	// Test for change in the number of devices since library was configured. Also checks tests for no devices.
	if (tempDAQmxEnumeratedDevCount != cListLength(DAQmxEnumeratedDevList) && ArduDAQmxFirstEnum == 0) {
		fprintf(ERRSTREAM, "ArduDAQmx library: FATAL: Length of enumerated device list not equal to enumerated device count.\n");
		fprintf(ERRSTREAM, "Temp Count: %d, Main Count (0x%p): %d", tempDAQmxEnumeratedDevCount, DAQmxEnumeratedDevList, cListLength(DAQmxEnumeratedDevList));
		setArduDAQmxLastError(ERROR_DEVCHANGE, 1);
		ArduDAQmxTerminate();
		DEVenumERR = -2;
		//getchar();
		//exit(-1);
	} else if (tempDAQmxEnumeratedDevCount == 0) { // If no new devices on the list, flash a warning and disable library
		fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: No NI-DAQmx devices detected. Terminating the library...\n");		
		setArduDAQmxLastError(ERROR_NODEVICES, 1);
		ArduDAQmxTerminate();
		DEVenumERR = -2;
	} else if (DAQmxEnumeratedDevCount != 0 && DAQmxEnumeratedDevCount != tempDAQmxEnumeratedDevCount) { // if new list is different than old list, then flash warning and disable library
		fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: %d devices detected. Library had %d devices when configured and need to be reinitialized. Terminating the library now...", tempDAQmxEnumeratedDevCount, DAQmxEnumeratedDevCount);	
		setArduDAQmxLastError(ERROR_DEVCHANGE, 1);
		ArduDAQmxTerminate();
		DEVenumERR = -2;
	}

	DAQmxEnumeratedDevCount = tempDAQmxEnumeratedDevCount;
	ArduDAQmxFirstEnum = 0;
	
	// Free up memory
	free(DAQmxDevNameList); // DYN-F: free devicenames
	DAQmxDevNameList = NULL;
	remainder_DAQmxDevNameList = NULL;
} // end enumerateDAQmxDevices function

/*!
 * \fn inline int getArduDAQmxStatus()
 * Returns the current operational status of the ArduDAQmx library as reported by the 'ArduDAQmxStatus' variable.
 * Use this function to access the 'ArduDAQmxStatus' variable.
 * 
 * \return Returns an integer value of the status of the ArduDAQmx library as defined in the 'StatusMode' enumeration.
 */
inline int getArduDAQmxStatus()
{
	return ArduDAQmxStatus;
}

/*!
 * \fn inline int getArduDAQmxLastError()
 * Returns the last error code of the ArduDAQmx library as reported by the 'ArduDAQmxErrorCode' variable.
 * Use this function to access the 'ArduDAQmxErrorCode' variable.
 * 
 * \return Returns an integer value of the status of the ArduDAQmx library as defined in the 'ArduDAQmxErrorCode' enumeration.
 */
inline int getArduDAQmxLastError()
{
	return ArduDAQmxError;
}

/*!
 * \fn inline int setArduDAQmxLastError(ErrorCode newErrorCode, unsigned printErrorMsgFlag)
 * Sets the 'ArduDAQmxError' error code of the ArduDAQmx library.
 * The routine also prints the meaning of the message if the printErrorMsgFlag = 1.
 * 
 * \param newErrorCode The new error code to set of type 'ArduDAQmxErrorcode'
 * \param printErrorMsgFlag
 * \return Returns the error code in 'ArduDAQmxError'.
 */
inline int setArduDAQmxLastError(ArduDAQmxErrorCode newErrorCode, unsigned printErrorMsgFlag)
{
	ArduDAQmxError = newErrorCode;
	if (printErrorMsgFlag == 1)
		printArduDAQmxLastError();
	return ArduDAQmxError;
}


/*!
 * \fn inline int printDAQmxStatus()
 * Prints the meaning of the library status as reflected in 'ArduDAQmxStatus'.
 * 
 * \return Returns 'ArduDAQmxStatus' as an integer.
 */
inline int printArduDAQmxStatus()
{
	switch (ArduDAQmxStatus)
	{
	case STATUS_PRECONFIG:
		fprintf(LOGSTREAM, "ArduDAQmx library: Status: Library uninitialized. Urgh! [STATUS_PRECONFIG]\n");
		break;
	case STATUS_CONFIG:
		fprintf(LOGSTREAM, "ArduDAQmx library: Status: Initialized, but not configured. [STATUS_CONFIG]\n");
		break;
	case STATUS_READY:
		fprintf(LOGSTREAM, "ArduDAQmx library: Status: Configured and ready. Let's Go!!! [STATUS_READY]\n");
		break;	
	case STATUS_RUN:
		fprintf(LOGSTREAM, "ArduDAQmx library: Status: DAQmx Running - data being collected [STATUS_RUN]\n");
		break;
	default:
		fprintf(LOGSTREAM, "ArduDAQmx library: Status: ArduDAQmx has an unknown status [Status code: %d]\n", ArduDAQmxStatus);
		break;
	}
	return ArduDAQmxStatus;
}

/*!
 * \fn inline int printArduDAQmxLastError()
 * Prints the meaning of the last library error as reflected in 'ArduDAQmxErrorCode'.
 * Unknown errors are detected as such and their integer error codes are printed.
 * 
 * \return Returns 'ArduDAQmxErrorCode' as an integer.
 */
inline int printArduDAQmxLastError()
{
	switch (ArduDAQmxError)
	{
	case ERROR_UNSUPPORTED:
		fprintf(ERRSTREAM, "ArduDAQmx library: Error: A feature/functionality that is unsupported by ArduDAQmx requested. [ERROR_UNSUPPORTED]\n");
		break;
	case ERROR_INVIO:
		fprintf(ERRSTREAM, "ArduDAQmx library: Error: An unsupported/invalid I/O type was selected. [ERROR_INVIO]\n");
		break;
	case ERROR_NIDAQMX:
		fprintf(ERRSTREAM, "ArduDAQmx library: Error: NI-DAQmx has encountered an error (NI Error Code: %d). [ERROR_NIDAQMX]\n", NIDAQmxErrorCode);
		char NIerrorString[1000];
		DAQmxGetErrorString(NIDAQmxErrorCode, NIerrorString, 1000);
		fprintf(ERRSTREAM, "ArduDAQmx library: NI-DAQmx Error: %s\n", NIerrorString);
		break;
	case ERROR_DEVCHANGE:
		fprintf(ERRSTREAM, "ArduDAQmx library: Error: Number of DAQmx devices have changed during operation. [ERROR_DEVCHANGE]\n");
		break;
	case ERROR_NODEVICES:
		fprintf(ERRSTREAM, "ArduDAQmx library: Error: No DAQmx devices discovered. [ERROR_NODEVICES]\n");
		break;
	case ERROR_NOTCONFIG:
		fprintf(ERRSTREAM, "ArduDAQmx library: Error: Library is not configured. [ERROR_NOTCONFIG]\n");
		break;
	case ERROR_NONE:
		fprintf(ERRSTREAM, "ArduDAQmx library: No errors detected. [ERROR_NONE]\n");
		break;	
	default:
		fprintf(ERRSTREAM, "ArduDAQmx library: An unknown error was detected. [ArduDAQmx error code: %d]\n", ArduDAQmxError);
		break;
	}
	return (int)ArduDAQmxError;
}

/*!
 * \fn inline cLinkedList * getDAQmxDeviceList()
 * Returns a pointer to the array of active DAQmx devices.
 * 
 * \return Returns a pointer to the array of active DAQmx devices.
 */
inline DAQmxDevice * getDAQmxDeviceList()
{
	return ArduDAQmxDevList;
}

/*!
 * \fn inline unsigned long getDAQmxDeviceCount()
 * Returns the numver of NI-DAQmx devices detected by the ArduDAQmx library.
 * 
 * \return numver of NI-DAQmx devices detected by the ArduDAQmx library.
 */
inline unsigned long getDAQmxDeviceCount()
{
	return ArduDAQmxDevCount;
}


/*!
 * \fn inline char * getArduDAQmxPrefix()
 * Returns a pointer to the DAQmx device prefix string
 * 
 * \return returns a character pointer to the DAQmx device prefix string
 */
inline char * getArduDAQmxPrefix()
{
	return ArduDAQmxDevPrefix;
}

/*!
 * \fn inline int unsigned getArduDAQmxDevPrefixLength()
 * Returns the length of the current DAQmx device prefix as defined in 'ArduDAQmxDevPrefixLength'.
 * 
 * \return Returns the length of current DAQmx device prefix as an unsigned integer.
 */
inline unsigned int getArduDAQmxDevPrefixLength()
{
	return ArduDAQmxDevPrefixLength;
}

/*!
 * \fn unsigned int enumerateDAQmxDeviceTerminals(unsigned int deviceNumber)
 * Detects, enumerates and prints all the terminals on a specified DAQmx device.
 * 
 * \return Returns the total number of terminals on the device.
 */
unsigned int enumerateDAQmxDeviceTerminals(unsigned int deviceNumber)
{
	char myDev[1+MaxNIstringLength];
	const unsigned bufSize = 20000;
	char data[bufSize], data2[bufSize];
	char *rem_data;
	char *oneCh_data;
	
	dev2string(myDev, deviceNumber);
	NIDAQmxErrorCode = DAQmxGetDevTerminals(myDev, data, bufSize);
	int charLength = (int)strnlen_s(data, bufSize);
	rem_data = data;
	unsigned int i = 0;
	
	for (oneCh_data = strtok_s(rem_data, ",", &rem_data); oneCh_data != NULL; oneCh_data = strtok_s(rem_data, ",", &rem_data), i++) {
		fprintf(LOGSTREAM, "Terminal %d: %s\n", i+1, oneCh_data);
	}

	fprintf(LOGSTREAM, "\nOn %s - %d Terminals (%d characters)\n\n", myDev, i, charLength);

	return i;
}

/*!
 * \fn unsigned int enumerateDAQmxDeviceChannels(unsigned int myDev, IOmode IOtype, unsigned int printFlag)
 * Returns the number of physical channels of a particular I/O type available in a specified device.
 * A printFlag also allows users to optionally have the function print the list of available channels.
 * 
 * \param myDev Device ID numver of the NI-DAQmx device as specified by ArduDAQmx
 * \param IOtype The supported 'IOmode' I/O types for which number of channels must be enumerated.
 * \param printFlag Set to 1 to print the list of channels of the specified I/O types available with the device.
 * \return Returns the number of physical channels of the specified I/O type that is available in the device.
 */
unsigned int enumerateDAQmxDeviceChannels(unsigned int myDev, IOmode IOtype, unsigned int printFlag)
{
	const unsigned bufSize = 20000;
	char data[bufSize];
	char DevIDstring[1+MaxArduDAQmxDevStringLength];
	char *rem_data, *oneCh_data;

	dev2string(DevIDstring, myDev);

	switch (IOtype)
	{
	case ANALOG_IN:		// ENUM value 0
		DAQmxGetDevAIPhysicalChans(DevIDstring, data, bufSize);
		break;
	case ANALOG_OUT:	// ENUM value 1
		DAQmxGetDevAOPhysicalChans(DevIDstring, data, bufSize);
		break;
	case DIGITAL_IN:	// ENUM value 2
		DAQmxGetDevDIPorts(DevIDstring, data, bufSize);
		break;
	case DIGITAL_OUT:	// ENUM value 3
		DAQmxGetDevDOPorts(DevIDstring, data, bufSize);
		break;
	case COUNTER_IN:	// ENUM value 4
		DAQmxGetDevCIPhysicalChans(DevIDstring, data, bufSize);
		break;
	case COUNTER_OUT:	// ENUM value 5
		DAQmxGetDevCOPhysicalChans(DevIDstring, data, bufSize);
		break;
	default:
		setArduDAQmxLastError(ArduDAQmxErrorCode::ERROR_INVIO, 1);
		ArduDAQmxTerminate();
		break;
	}

	rem_data = data;
	int i = 0;
	for (oneCh_data = strtok_s(rem_data, ",", &rem_data); oneCh_data != NULL; oneCh_data = strtok_s(rem_data, ",", &rem_data), i++) {
		if (printFlag == 1) {
			fprintf(LOGSTREAM, "Terminal %d: %s\n", i + 1, oneCh_data);	
		} // end printflag if block
		
		// Check and omit counting of frequency scalers
		if (IOtype == COUNTER_OUT && strstr(oneCh_data, "freqout") != NULL) {
			i--;
		}
	}// end channel counting for loop

	return i; // returns the number of termninals of a certain I/O type avaiable in a particular device.
}

/*!
 * \fn int ArduDAQmxConfigure()
 * Configures the ArduDAQmx library with a list of devices, tasks and library status.
 * This function is called by 'ArduDAQmxInit' and must be called before initialization to setup the library.
 * 
 * \return Returns the status of the status of the library as defined in 'ArduDAQmxStatus'
 */
int ArduDAQmxConfigure()
{

	// If in PRECONFIG state, setup library and device list
	if (getArduDAQmxStatus() == (int)STATUS_PRECONFIG) { // if there are no issues and library is in preconfig, setup library
		ArduDAQmxDevList	= (DAQmxDevice *) malloc( sizeof(DAQmxDevice) * DAQmxEnumeratedDevMaxNum ); // DYN-M: allocate array of devices for fast access
		unsigned int		i = 0, j = 0;
		cListElem *elem		= NULL;
		DAQmxDevice *cpyDev = NULL;
		unsigned int cpyInd = 0;
		
		if (cListLength(DAQmxEnumeratedDevList) > 0 && ArduDAQmxTaskList != NULL) {
			ArduDAQmxTaskList = (cLinkedList *)malloc(sizeof(cLinkedList));
			cListInit(ArduDAQmxTaskList);
		}
		// initialize all elements in ArduDAQmxDevList to have DevNum = 0 as it contains no valid devices yet.
		for (i = 0, cpyDev = ArduDAQmxDevList; i < DAQmxEnumeratedDevMaxNum; i++, cpyDev++) {
			cpyDev->DevNum = 0;
		}

		// copy sorted enumerated device linked list into the ArduDAQmxDevList device array that was just initialized.
		for (cpyDev = ArduDAQmxDevList, elem = cListFirstElem(DAQmxEnumeratedDevList); elem != NULL && getArduDAQmxLastError() == ERROR_NONE; elem = cListNextElem(DAQmxEnumeratedDevList, elem)) {
			cpyInd = ((DAQmxDevice*)(elem->obj))->DevNum - 1;
			if (cpyInd < 0) {
				fprintf(ERRSTREAM, "ArduDAQmx library: FATAL: A DAQmx device has a device number as 0. All device numbers must be positive integers");
				ArduDAQmxTerminate();
				setArduDAQmxLastError(ERROR_NODEVICES, 1);
				return getArduDAQmxStatus();
			}
			cpyDev[cpyInd] = *(DAQmxDevice*)(elem->obj);

			// if pins of an IO type are present, initialize task, convert realtime errors to warnings and pinlist for that IO type

			// analog inputs present
			if (cpyDev[cpyInd].numAIch > 0) { 
					// initialize AI task
				cpyDev[cpyInd].AItask.DevNum		= cpyDev[cpyInd].DevNum;
				cpyDev[cpyInd].AItask.taskIOmode	= ANALOG_IN;
				cpyDev[cpyInd].AItask.activePinList = (cLinkedList *)malloc(sizeof(cLinkedList));
				cListInit(cpyDev[cpyInd].AItask.activePinList);
				cpyDev[cpyInd].AItask.activePinCnt	= 0;
				cpyDev[cpyInd].AItask.ioDataSize	= sizeof(float64);
				cpyDev[cpyInd].AItask.ioBuffer		= NULL;
				DAQmxErrChk(DAQmxCreateTask("", &(cpyDev[cpyInd].AItask.Handler)));
					cListAppend(ArduDAQmxTaskList, (void *) &(cpyDev[cpyInd].AItask));
					ArduDAQmxTaskCount = cListLength(ArduDAQmxTaskList);
				cpyDev[cpyInd].AItask.clockHandler	= NULL;
					// convert AI RT errors to warnings
				DAQmxErrChk(DAQmxSetRealTimeConvLateErrorsToWarnings( cpyDev[cpyInd].AItask.Handler, 1));
					// initialize AI pin list
				cpyDev[cpyInd].AIpins = (pin *) malloc(cpyDev[cpyInd].numAIch * sizeof(pin));
				for (j = 0; j < cpyDev[cpyInd].numAIch; j++) {
					cpyDev[cpyInd].AIpins[j].DevNum			= cpyDev[cpyInd].DevNum;
					cpyDev[cpyInd].AIpins[j].PinNum			= j;
					cpyDev[cpyInd].AIpins[j].pinTask		= &(cpyDev[cpyInd].AItask);
					cpyDev[cpyInd].AIpins[j].pinAssignFlag	= 0;
					cpyDev[cpyInd].AIpins[j].pinIOmode		= INVALID_IO;
					cpyDev[cpyInd].AIpins[j].pinValue		= 0;
				}
			}

			// analog outputs present
			if (cpyDev[cpyInd].numAOch > 0) { 
					// initialize AO task
				cpyDev[cpyInd].AOtask.DevNum		= cpyDev[cpyInd].DevNum;
				cpyDev[cpyInd].AOtask.taskIOmode	= ANALOG_OUT;
				cpyDev[cpyInd].AOtask.activePinList = (cLinkedList *)malloc(sizeof(cLinkedList));
				cListInit(cpyDev[cpyInd].AOtask.activePinList);
				cpyDev[cpyInd].AOtask.activePinCnt	= 0;
				cpyDev[cpyInd].AOtask.ioDataSize	= sizeof(float64);
				cpyDev[cpyInd].AOtask.ioBuffer		= NULL;
				DAQmxErrChk(DAQmxCreateTask("", &(cpyDev[cpyInd].AOtask.Handler)));
					cListAppend(ArduDAQmxTaskList, (void *) &(cpyDev[cpyInd].AOtask));
					ArduDAQmxTaskCount = cListLength(ArduDAQmxTaskList);
				cpyDev[cpyInd].AOtask.clockHandler	= NULL;
					// convert AO RT errors to warnings
				DAQmxErrChk(DAQmxSetRealTimeConvLateErrorsToWarnings( cpyDev[cpyInd].AOtask.Handler, 1));
					// initialize AO pin list
				cpyDev[cpyInd].AOpins = (pin *) malloc(cpyDev[cpyInd].numAOch * sizeof(pin));
				for (j = 0; j < cpyDev[cpyInd].numAOch; j++) {
					cpyDev[cpyInd].AOpins[j].DevNum			= cpyDev[cpyInd].DevNum;
					cpyDev[cpyInd].AOpins[j].PinNum			= j;
					cpyDev[cpyInd].AOpins[j].pinTask		= &(cpyDev[cpyInd].AOtask);
					cpyDev[cpyInd].AOpins[j].pinAssignFlag	= 0;
					cpyDev[cpyInd].AOpins[j].pinIOmode		= INVALID_IO;
					cpyDev[cpyInd].AOpins[j].pinValue		= 0;
				}
			}


			// digital in/outs presents
			if (cpyDev[cpyInd].numDIch > 0 || cpyDev[cpyInd].numDOch > 0) { 
				
				// digital in pins are present
				if (cpyDev[cpyInd].numDIch > 0) { 
							// initialize DI task
					cpyDev[cpyInd].DItask.DevNum		= cpyDev[cpyInd].DevNum;
					cpyDev[cpyInd].DItask.taskIOmode	= DIGITAL_IN;
					cpyDev[cpyInd].DItask.activePinList = (cLinkedList *)malloc(sizeof(cLinkedList));
					cListInit(cpyDev[cpyInd].DItask.activePinList);
					cpyDev[cpyInd].DItask.activePinCnt	= 0;
					cpyDev[cpyInd].DItask.ioDataSize	= cpyDev[cpyInd].numDIch <= 8 ? sizeof(uInt8) : cpyDev[cpyInd].numDIch <= 16 ? sizeof(uInt16) : sizeof(uInt32);
					cpyDev[cpyInd].DItask.ioBuffer		= NULL;
					DAQmxErrChk(DAQmxCreateTask("", &(cpyDev[cpyInd].DItask.Handler)));
						cListAppend(ArduDAQmxTaskList, (void *) &(cpyDev[cpyInd].DItask));
						ArduDAQmxTaskCount = cListLength(ArduDAQmxTaskList);
					cpyDev[cpyInd].DItask.clockHandler	= NULL;
						// convert DI RT errors to warnings
					DAQmxErrChk(DAQmxSetRealTimeConvLateErrorsToWarnings( cpyDev[cpyInd].DItask.Handler, 1));
				}
				
				// digital outputs present
				if (cpyDev[cpyInd].numDOch > 0) { 
						// initialize DO task
					cpyDev[cpyInd].DOtask.DevNum		= cpyDev[cpyInd].DevNum;
					cpyDev[cpyInd].DOtask.taskIOmode	= DIGITAL_OUT;
					cpyDev[cpyInd].DOtask.activePinList = (cLinkedList *)malloc(sizeof(cLinkedList));
					cListInit(cpyDev[cpyInd].DOtask.activePinList);
					cpyDev[cpyInd].DOtask.activePinCnt	= 0;
					cpyDev[cpyInd].DItask.ioDataSize	= cpyDev[cpyInd].numDOch <= 8 ? sizeof(uInt8) : cpyDev[cpyInd].numDOch <= 16 ? sizeof(uInt16) : sizeof(uInt32);
					cpyDev[cpyInd].DOtask.ioBuffer		= NULL;
					DAQmxErrChk(DAQmxCreateTask("", &(cpyDev[cpyInd].DOtask.Handler)));
						cListAppend(ArduDAQmxTaskList, (void *) &(cpyDev[cpyInd].DOtask));
						ArduDAQmxTaskCount = cListLength(ArduDAQmxTaskList);
					cpyDev[cpyInd].DOtask.clockHandler	= NULL;
					// convert DO RT errors to warnings
					DAQmxErrChk(DAQmxSetRealTimeConvLateErrorsToWarnings( cpyDev[cpyInd].DOtask.Handler, 1));
				}
					// initialize common Digital IO pin list
				cpyDev[cpyInd].DIpins = (pin *) malloc(cpyDev[cpyInd].numDIch * sizeof(pin));
				cpyDev[cpyInd].DOpins = cpyDev[cpyInd].DIpins;
				int maxJ = (cpyDev[cpyInd].numDIch > cpyDev[cpyInd].numDOch) ? cpyDev[cpyInd].numDIch: cpyDev[cpyInd].numDOch;
				for (j = 0; j < maxJ; j++) {
					cpyDev[cpyInd].DIpins[j].DevNum			= cpyDev[cpyInd].DevNum;
					cpyDev[cpyInd].DIpins[j].PinNum			= j;
					cpyDev[cpyInd].DIpins[j].pinTask		= NULL;
					cpyDev[cpyInd].DIpins[j].pinAssignFlag	= 0;
					cpyDev[cpyInd].DIpins[j].pinIOmode		= INVALID_IO;
					cpyDev[cpyInd].DIpins[j].pinValue		= 0;
				}
			}


			// counters in/out present
			if (cpyDev[cpyInd].numCIch > 0 || cpyDev[cpyInd].numCOch > 0) { 
				// initialize 1 task per counter IO available.
				cpyDev[cpyInd].CTRtask = (ArduDAQmxTask *)malloc(cpyDev[cpyInd].numCIch * sizeof(ArduDAQmxTask));
				for (i = 0; i < cpyDev[cpyInd].numCIch; i++) { // if counter inputs present, create counter tasks for each pin
					cpyDev[cpyInd].CTRtask[i].DevNum		= cpyDev[cpyInd].DevNum;
					cpyDev[cpyInd].CTRtask[i].taskIOmode	= COUNTER_IN;
					cpyDev[cpyInd].CTRtask[i].activePinList = (cLinkedList *)malloc(sizeof(cLinkedList));
					cListInit(cpyDev[cpyInd].CTRtask[i].activePinList);
					cpyDev[cpyInd].CTRtask[i].activePinCnt	= 0;
					cpyDev[cpyInd].CTRtask[i].ioDataSize	= sizeof(float64);
					cpyDev[cpyInd].CTRtask[i].ioBuffer		= NULL;
					DAQmxErrChk(DAQmxCreateTask("", &(cpyDev[cpyInd].CTRtask[i].Handler)));
						cListAppend(ArduDAQmxTaskList, (void *) &(cpyDev[cpyInd].CTRtask[i]));
						ArduDAQmxTaskCount = cListLength(ArduDAQmxTaskList);
					cpyDev[cpyInd].CTRtask[i].clockHandler = NULL;
						// convert CTR RT errors to warnings
					DAQmxErrChk(DAQmxSetRealTimeConvLateErrorsToWarnings( cpyDev[cpyInd].CTRtask[i].Handler, 1));
				}
					// initialize common CTR pin list
				cpyDev[cpyInd].CIpins = (pin *) malloc(cpyDev[cpyInd].numCIch * sizeof(pin));
				cpyDev[cpyInd].COpins = cpyDev[cpyInd].CIpins;
				int maxJ = (cpyDev[cpyInd].numCIch > cpyDev[cpyInd].numCOch) ? cpyDev[cpyInd].numCIch: cpyDev[cpyInd].numCOch;
				for (j = 0; j < maxJ; j++) {
					cpyDev[cpyInd].CIpins[j].DevNum			= cpyDev[cpyInd].DevNum;
					cpyDev[cpyInd].CIpins[j].PinNum			= j;
					cpyDev[cpyInd].CIpins[j].pinTask		= &(cpyDev[cpyInd].CTRtask[j]);
					cpyDev[cpyInd].CIpins[j].pinAssignFlag	= 0;
					cpyDev[cpyInd].CIpins[j].pinIOmode		= INVALID_IO;
					cpyDev[cpyInd].CIpins[j].pinValue		= 0;
				}
			}
		} // end device copy for loop
		ArduDAQmxDevCount  = DAQmxEnumeratedDevCount;
		ArduDAQmxDevMaxNum = DAQmxEnumeratedDevMaxNum;
		if (ArduDAQmxDevCount > 0) { // if NI devices present, library is ready, else library fails to initialize.
			setArduDAQmxLastError(ERROR_NONE, 0);
			NIDAQmxErrorCode	= 0;	
			ArduDAQmxStatus		= STATUS_CONFIG;
		} else {
			setArduDAQmxLastError(ERROR_NODEVICES, 1);
		}
	} else { // if library not in preconfig mode, it has already been initialized
		fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: Library must be in preconfig mode to configure.\n");
	}

	if (getArduDAQmxLastError() != ERROR_NONE) ArduDAQmxTerminate();
	return getArduDAQmxStatus();
}

/*!
 * \fn int ArduDAQmxInit()
 * Performs the initialization of the ArduDAQmx library. Calling this function is mandatory before using the library.
 * If the library is not initialized and ready, calling this function will setup the library and set the library status to STATUS_CONFIG.
 * 
 * \param devicePrefix Pointer to a C string with the device name prefix.
 * \return Returns 0 if there are no errors, and an error code otherwise.
 */
int ArduDAQmxInit(char *devicePrefix)
{
	if (ArduDAQmxStatus == STATUS_PRECONFIG) {
		// copy string passed as parameter as device prefix and set prefix length
		ArduDAQmxDevPrefix = (char *)malloc(sizeof(char)*(1+MaxArduDAQmxDevPrefixLength)); // DYN-M: allocate memory for the NI-DAQmx prefix
		strncpy_s(ArduDAQmxDevPrefix, 1+strnlen_s(devicePrefix, MaxArduDAQmxDevPrefixLength), devicePrefix, MaxArduDAQmxDevPrefixLength);		
		ArduDAQmxDevPrefixLength = strnlen_s(ArduDAQmxDevPrefix, MaxArduDAQmxDevPrefixLength);
		// enumerate and configure devices
		enumerateDAQmxDevices(0);
		ArduDAQmxConfigure();
	} else {
		fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: Library must be in preconfig mode to initialize. Terminating library!\n");
		ArduDAQmxTerminate();
		setArduDAQmxLastError(ERROR_NOTCONFIG,1);
	}
	return getArduDAQmxStatus();
}

/*!
 * \fn int ArduDAQmxTerminate()
 * Terminates the ArduDAQmx library and sets the status to STATUS_PRECONFIG.
 * It also cleans up any dynamic memory allocated by the library with some garbage collection.
 * Calling this function is mandatory before exitting the program.
 * But the function may also be called if the library needs to be reconfigured.
 * 
 * \param newStatus The error/status that the library must take after program terminates
 * \return Returns the status of the library as relfected in 'ArduDAQmxStatus'.
 */
int ArduDAQmxTerminate()
{
	//stop any active DAQmx tasks using the ArduDAQmx I/O Stop function		
	int i, j;
	for (i = 0; i < ArduDAQmxDevMaxNum; i++) { // for all NI-DAQmx devices
		if (ArduDAQmxDevList[i].numAIch > 0) { 
				// terminate NI AI task handler
			DAQmxStopTask(ArduDAQmxDevList[i].AItask.Handler);
			DAQmxClearTask(ArduDAQmxDevList[i].AItask.Handler);
				// clear AI pin list
			free(ArduDAQmxDevList[i].AIpins);
			ArduDAQmxDevList[i].AIpins = NULL;
				// clear AI task
			ArduDAQmxDevList[i].AItask.taskIOmode = INVALID_IO;
			if (ArduDAQmxDevList[i].AItask.activePinList != NULL) {
				cListUnlinkAll(ArduDAQmxDevList[i].AItask.activePinList);
				free(ArduDAQmxDevList[i].AItask.activePinList);
				ArduDAQmxDevList[i].AItask.activePinList = NULL;
			}
		}
		if (ArduDAQmxDevList[i].numAOch > 0) {
				// terminate NI AO task handler
			DAQmxStopTask(ArduDAQmxDevList[i].AOtask.Handler);
			DAQmxClearTask(ArduDAQmxDevList[i].AOtask.Handler);
				// clear AO pin list
			free(ArduDAQmxDevList[i].AOpins);
			ArduDAQmxDevList[i].AOpins = NULL;
				// clear AO task
			ArduDAQmxDevList[i].AOtask.taskIOmode = INVALID_IO;
			if (ArduDAQmxDevList[i].AOtask.activePinList != NULL) {
				cListUnlinkAll(ArduDAQmxDevList[i].AOtask.activePinList);
				free(ArduDAQmxDevList[i].AOtask.activePinList);
				ArduDAQmxDevList[i].AOtask.activePinList = NULL;
			}
		}
		if (ArduDAQmxDevList[i].numDIch > 0 || ArduDAQmxDevList[i].numDOch > 0) {
			if (ArduDAQmxDevList[i].numDIch > 0) { // if DI pins present
					// terminate NI DI task handler
				DAQmxStopTask(ArduDAQmxDevList[i].DItask.Handler);
				DAQmxClearTask(ArduDAQmxDevList[i].DItask.Handler);
					// clear DI task
				ArduDAQmxDevList[i].DItask.taskIOmode = INVALID_IO;
				if (ArduDAQmxDevList[i].DItask.activePinList != NULL) {
					cListUnlinkAll(ArduDAQmxDevList[i].DItask.activePinList);
					free(ArduDAQmxDevList[i].DItask.activePinList);
					ArduDAQmxDevList[i].DItask.activePinList = NULL;
				}
			}
			if (ArduDAQmxDevList[i].numDOch > 0) { // if DO pins present
					// terminate NI DO task handler
				DAQmxStopTask(ArduDAQmxDevList[i].DOtask.Handler);
				DAQmxClearTask(ArduDAQmxDevList[i].DOtask.Handler);
					// clear DO task
				ArduDAQmxDevList[i].DOtask.taskIOmode = INVALID_IO;
				if (ArduDAQmxDevList[i].DOtask.activePinList != NULL) {
					cListUnlinkAll(ArduDAQmxDevList[i].DOtask.activePinList);
					free(ArduDAQmxDevList[i].DOtask.activePinList);
					ArduDAQmxDevList[i].DOtask.activePinList = NULL;
				}
			}
				// Clear common digital pin list
			free(ArduDAQmxDevList[i].DIpins);
			ArduDAQmxDevList[i].DIpins = NULL;
			ArduDAQmxDevList[i].DOpins = NULL;
		}
		if (ArduDAQmxDevList[i].numCIch > 0 || ArduDAQmxDevList[i].numCOch > 0) {
			for (j = 0; j < ArduDAQmxDevList[i].numCIch; j++) { // if any CTR in tasks are present
					// terminate all NI CTR task handlers
				DAQmxStopTask (ArduDAQmxDevList[i].CTRtask[j].Handler);
				DAQmxClearTask(ArduDAQmxDevList[i].CTRtask[j].Handler);
					// clear all CTR tasks
				ArduDAQmxDevList[i].CTRtask[j].taskIOmode = INVALID_IO;
				if (ArduDAQmxDevList[i].CTRtask[j].activePinList != NULL) {
					cListUnlinkAll(ArduDAQmxDevList[i].CTRtask[j].activePinList);
					free(ArduDAQmxDevList[i].CTRtask[j].activePinList);
					ArduDAQmxDevList[i].CTRtask[j].activePinList = NULL;
				}
			}
			free(ArduDAQmxDevList[i].CTRtask);
				// clear common CTR pin list
			free(ArduDAQmxDevList[i].CIpins);
			ArduDAQmxDevList[i].CIpins = NULL;
			ArduDAQmxDevList[i].COpins = NULL;
		}

	} // end for loop for task, pin clearing
	cListUnlinkAll(ArduDAQmxTaskList);
	ArduDAQmxTaskCount = cListLength(ArduDAQmxTaskList);

	// Resetting sample clock to invalid
	ArduDAQmxSampleClock.sourceDevNum = 0;
	ArduDAQmxSampleClock.sourceIOmode = INVALID_IO;

	// print any errors
	if (getArduDAQmxLastError() != 0)
		printArduDAQmxLastError();

	// dynamic memory housekeeping
	ArduDAQmxStatus				= (int)STATUS_PRECONFIG;
	free(ArduDAQmxTaskList);
		ArduDAQmxTaskList		= NULL;
	free(ArduDAQmxDevPrefix); // DYN-F: free ArduDAQmxDevPrefix allocation
		ArduDAQmxDevPrefix		= NULL;	
	ArduDAQmxDevPrefixLength	= MaxArduDAQmxDevPrefixLength;
	free(ArduDAQmxDevList); // DYN-F: free ArduDAQmxDevList array
		ArduDAQmxDevList		= NULL;	
	ArduDAQmxDevCount			= 0;
	ArduDAQmxDevMaxNum			= 0;
	ArduDAQmxClearEnumeratedDevices(); // clear enumerated device list

	ArduDAQmxFirstEnum = 1;

	return ArduDAQmxStatus;
}

/*!
 * \fn void ArduDAQmxClearEnumeratedDevices()
 * Clears the linked list of enumerated devices. Automatically called by 'enumerateDAQmxDevices'.
 * Not to be used by the user. 
 */
void ArduDAQmxClearEnumeratedDevices()
{
	// DYN-F: delete devices from DAQmxEnumeratedDevList and unlink all of it.
	if (DAQmxEnumeratedDevList != NULL) {
		cListElem *myElem = cListFirstElem(DAQmxEnumeratedDevList);
		cListElem *delElem = NULL;
		while (myElem != NULL) {			
			delElem = myElem;
			myElem = cListNextElem(DAQmxEnumeratedDevList, myElem);
			free(((DAQmxDevice *)delElem->obj)); // DYN-F: free DAQmxDevice object
			cListUnlinkElem(DAQmxEnumeratedDevList, delElem); // unlink linked list element
		}

		free(DAQmxEnumeratedDevList); // DYN-F: free DAQmxEnumeratedDevList

		DAQmxEnumeratedDevList = NULL;
		DAQmxEnumeratedDevCount = 0;
		DAQmxEnumeratedDevMaxNum = 0;
	}
}

/*!
 * \fn int pinMode(unsigned int devNum, unsigned int pinNum, IOmode IOtype)
 * Defines the input-output mode of pins as either analog or digital pins.
 * If sample clock information is undefined, the function will set it to default values. Then sets up device of this pin as clock source.
 * 
 * \param devNum NI-DAQ device/slot number of the pin.
 * \param pinNum Pin number on the NI-DAQ device.
 * \param IOtype I/O type being requested on the pin as defined in ::IOmode.
 * \return Returns the last error code of ArduDAQmx.
 */
int pinMode(unsigned int devNum, unsigned int pinNum, IOmode IOtype)
{
	DAQmxDevice *myDev  = NULL;
	pin			*myPin	= NULL;
	ArduDAQmxTask	*myTask = NULL;
	if (ArduDAQmxStatus == STATUS_CONFIG || ArduDAQmxStatus == STATUS_READY) { // if library is in CONFIG mode
		char pinIDstr[256], pinName[256];
		if (devNum > 0 && devNum < ArduDAQmxDevCount) {
			myDev = &(ArduDAQmxDevList[devNum - 1]);
			if (myDev->DevNum != 0) {
				switch (IOtype) {

				case ANALOG_IN: // ANALOG IN
					if (pinNum >= 0 && pinNum < myDev->numAIch) { // pinNum validity check
						myPin = &(myDev->AIpins[pinNum]);
						// setup task
						myTask = &(myDev->AItask);
						cListAppend(myTask->activePinList, (void *)myPin);
						myTask->activePinCnt = cListLength(myTask->activePinList);
						// warn if pin assigned, and setup pin
						if (myPin->pinAssignFlag != 0) {
							fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: Dev %d - AI pin%d being reasigned.\n", devNum, pinNum);
							cListUnlinkElem(myPin->pinTask->activePinList, cListFindElem(myPin->pinTask->activePinList, (void *)myPin));
							myPin->pinTask->activePinCnt = cListLength(myPin->pinTask->activePinList);
						}
						myPin->pinTask = myTask;
						myPin->pinAssignFlag = 1;
						myPin->pinIOmode = ANALOG_IN;
						
						snprintf(pinName, 256, "dev%dAI%d", devNum, pinNum);
						DAQmxErrChk(DAQmxCreateAIVoltageChan(myDev->AItask.Handler, pin2string(pinIDstr, devNum, IOtype, pinNum), pinName, NIdef.NItermConf, NIdef.AImin, NIdef.AImax, NIdef.NIanlgUnits, NULL));
					} else {
						fprintf(ERRSTREAM, "ArduDAQmx library: ERROR: On Dev %d - Attempt to access invalid AI pin%d. Terminating library.\n", devNum, pinNum);
						ArduDAQmxTerminate();
						setArduDAQmxLastError(ArduDAQmxErrorCode::ERROR_INVIO, 1);
						myPin = NULL;
					}
					break;

				case ANALOG_OUT: // ANALOG OUT
					if (pinNum >= 0 && pinNum < myDev->numAOch) { // pinNum validity check
						myPin = &(myDev->AOpins[pinNum]);
						// setup task
						myTask = &(myDev->AOtask);
						cListAppend(myTask->activePinList, (void *)myPin);
						myTask->activePinCnt = cListLength(myTask->activePinList);
						// warn if pin assigned, and setup pin
						if (myPin->pinAssignFlag != 0) {
							fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: Dev %d - AO pin%d being reasigned.\n", devNum, pinNum);
							cListUnlinkElem(myPin->pinTask->activePinList, cListFindElem(myPin->pinTask->activePinList, (void *)myPin));
							myPin->pinTask->activePinCnt = cListLength(myPin->pinTask->activePinList);
						}
						myPin->pinTask = myTask;
						myPin->pinAssignFlag = 1;
						myPin->pinIOmode = ANALOG_OUT;

						snprintf(pinName, 256, "dev%dAO%d", devNum, pinNum);
						DAQmxErrChk(DAQmxCreateAOVoltageChan(myDev->AOtask.Handler, pin2string(pinIDstr, devNum, IOtype, pinNum), pinName, NIdef.AOmin, NIdef.AOmax, NIdef.NIanlgUnits, NULL));
					} else {
						fprintf(ERRSTREAM, "ArduDAQmx library: ERROR: On Dev %d - Attempt to access invalid AO pin%d. Terminating library.\n", devNum, pinNum);
						ArduDAQmxTerminate();
						setArduDAQmxLastError(ArduDAQmxErrorCode::ERROR_INVIO, 1);
						myPin = NULL;
					}
					break;

				case DIGITAL_IN: // DIGITAL IN
					if (pinNum >= 0 && pinNum < myDev->numDIch) { // pinNum validity check
						myPin = &(myDev->DIpins[pinNum]);
						// setup task
						myTask = &(myDev->DItask);
						cListAppend(myTask->activePinList, (void *)myPin);
						myTask->activePinCnt = cListLength(myTask->activePinList);
						// warn if pin assigned, and setup pin
						if (myPin->pinAssignFlag != 0) {
							fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: Dev %d - DI port%d being reasigned.\n", devNum, pinNum);
							cListUnlinkElem(myPin->pinTask->activePinList, cListFindElem(myPin->pinTask->activePinList, (void *)myPin));
							myPin->pinTask->activePinCnt = cListLength(myPin->pinTask->activePinList);
						}
						myPin->pinTask = myTask;
						myPin->pinAssignFlag = 1;
						myPin->pinIOmode = DIGITAL_IN;

						snprintf(pinName, 256, "dev%dDI%d", devNum, pinNum);
						DAQmxErrChk(DAQmxCreateDIChan(myDev->DItask.Handler, pin2string(pinIDstr, devNum, IOtype, pinNum), pinName, DAQmx_Val_ChanForAllLines));
					} else {
						fprintf(ERRSTREAM, "ArduDAQmx library: ERROR: On Dev %d - Attempt to access invalid DI port%d. Terminating library.\n", devNum, pinNum);
						ArduDAQmxTerminate();
						setArduDAQmxLastError(ArduDAQmxErrorCode::ERROR_INVIO, 1);
						myPin = NULL;
					}
					break;

				case DIGITAL_OUT: // DIGITAL OUT
					if (pinNum >= 0 && pinNum < myDev->numDOch) { // pinNum validity check
						myPin = &(myDev->DOpins[pinNum]);
						// setup task
						myTask = &(myDev->DOtask);
						cListAppend(myTask->activePinList, (void *)myPin);
						myTask->activePinCnt = cListLength(myTask->activePinList);
						// warn if pin assigned, and setup pin
						if (myPin->pinAssignFlag != 0) {
							fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: Dev %d - DO port%d being reasigned.\n", devNum, pinNum);
							cListUnlinkElem(myPin->pinTask->activePinList, cListFindElem(myPin->pinTask->activePinList, (void *)myPin));
							myPin->pinTask->activePinCnt = cListLength(myPin->pinTask->activePinList);
						}
						myPin->pinTask = myTask;
						myPin->pinAssignFlag = 1;
						myPin->pinIOmode = DIGITAL_OUT;

						snprintf(pinName, 256, "dev%dDO%d", devNum, pinNum);
						DAQmxErrChk(DAQmxCreateDOChan(myDev->DOtask.Handler, pin2string(pinIDstr, devNum, IOtype, pinNum), pinName, NIdef.NIdigiGrp));
					} else {
						fprintf(ERRSTREAM, "ArduDAQmx library: ERROR: On Dev %d - Attempt to access invalid DO port%d. Terminating library.\n", devNum, pinNum);
						ArduDAQmxTerminate();
						setArduDAQmxLastError(ArduDAQmxErrorCode::ERROR_INVIO, 1);
						myPin = NULL;
					}
					break;

				case COUNTER_IN: // COUNTER IN
					if (pinNum >= 0 && pinNum < myDev->numCIch) { // pinNum validity check
						myPin = &(myDev->CIpins[pinNum]);
						// setup task
						myTask = &(myDev->CTRtask[pinNum]);
						myTask->taskIOmode = IOtype;
						cListAppend(myTask->activePinList, (void *)myPin);
						myTask->activePinCnt = cListLength(myTask->activePinList);
						// warn if pin assigned, and setup pin
						if (myPin->pinAssignFlag != 0) {
							fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: Dev %d - CTR%d being reasigned.\n", devNum, pinNum);
							cListUnlinkElem(myPin->pinTask->activePinList, cListFindElem(myPin->pinTask->activePinList, (void *)myPin));
							myPin->pinTask->activePinCnt = cListLength(myPin->pinTask->activePinList);
						}
						myPin->pinTask = myTask;
						myPin->pinAssignFlag = 1;
						myPin->pinIOmode = COUNTER_IN;

						snprintf(pinName, 256, "dev%dCTR%d", devNum, pinNum);
						DAQmxErrChk(DAQmxCreateCIAngEncoderChan(myDev->CTRtask[pinNum].Handler, pin2string(pinIDstr, devNum, IOtype, pinNum), pinName, NIdef.NIctrMode, NIdef.ZEN, NIdef.Zval, NIdef.Zphase, NIdef.NIctrunits, NIdef.encoderPPR, NIdef.angleInit, ""));
					} else {
						fprintf(ERRSTREAM, "ArduDAQmx library: ERROR: On Dev %d - Attempt to access invalid CTR input pin%d. Terminating library.\n", devNum, pinNum);
						ArduDAQmxTerminate();
						setArduDAQmxLastError(ArduDAQmxErrorCode::ERROR_INVIO, 1);
						myPin = NULL;
					}
					break;

				case COUNTER_OUT: // COUNTER OUT
					if (pinNum >= 0 && pinNum < myDev->numCOch) { // pinNum validity check
						myPin = &(myDev->COpins[pinNum]);
						// setup task
						myTask = &(myDev->CTRtask[pinNum]);
						myTask->taskIOmode = IOtype;
						cListAppend(myTask->activePinList, (void *)myPin);
						myTask->activePinCnt = cListLength(myTask->activePinList);
						// warn if pin assigned, and setup pin
						if (myPin->pinAssignFlag != 0) {
							fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: Dev %d - CTR%d being reasigned.\n", devNum, pinNum);
							cListUnlinkElem(myPin->pinTask->activePinList, cListFindElem(myPin->pinTask->activePinList, (void *)myPin));
							myPin->pinTask->activePinCnt = cListLength(myPin->pinTask->activePinList);
						}
						myPin->pinTask = myTask;
						myPin->pinAssignFlag = 1;
						myPin->pinIOmode = COUNTER_OUT;

						snprintf(pinName, 256, "dev%dCTR%d", devNum, pinNum);
						DAQmxErrChk(DAQmxCreateCOPulseChanTicks(myDev->CTRtask[pinNum].Handler, pin2string(pinIDstr, devNum, IOtype, pinNum), pinName, "OnboardClock", NIdef.plsIdleSt, NIdef.plsInitDel, NIdef.plsLoTick, NIdef.plsHiTick));
					} else {
						fprintf(ERRSTREAM, "ArduDAQmx library: ERROR: On Dev %d - Attempt to access invalid CTR input pin%d. Terminating library.\n", devNum, pinNum);
						ArduDAQmxTerminate();
						setArduDAQmxLastError(ArduDAQmxErrorCode::ERROR_INVIO, 1);
						myPin = NULL;
					}
					break;

				default:					 
					fprintf(ERRSTREAM, "ArduDAQmx library: ERROR: Dev %d - pin%d given invalid IO type\n");
					ArduDAQmxTerminate();
					setArduDAQmxLastError(ArduDAQmxErrorCode::ERROR_INVIO, 1);
					myPin = NULL;
					break;
				}
				// every type of IO mode supported by the device gets it's own task.

				// Search through them by IO type and pin number
				//if task found
					// if pin found
						// reset pin mode if possible, else return NULL
					//if pin not found, attach pin to the task of 

				// ELSE if task not found, create task and add pin number to task
					//when creating task, check sync order. If new order is less than current order,
					//reset sync order for all tasks.

				//TODO: reorder IO modes to match sync order. !!!!!!!!!!!!!!!!!!!!!!
			} else { // myDev devNum check
				fprintf(ERRSTREAM, "ArduDAQmx library: ERROR: A device in library has invalid device number %d. Terminating library.\n", myDev->DevNum);
				ArduDAQmxTerminate();
				setArduDAQmxLastError(ArduDAQmxErrorCode::ERROR_UNSUPPORTED, 1);
				myPin = NULL;
			}
		} else {// devNum check
			fprintf(ERRSTREAM, "ArduDAQmx library: ERROR: Invalid device number requested. Terminating library.\n");
			ArduDAQmxTerminate();
			setArduDAQmxLastError(ArduDAQmxErrorCode::ERROR_UNSUPPORTED, 1);
			myPin = NULL;
		}

		// Set sample clock information for the task
		//DAQmxErrChk(DAQmxCfgSampClkTiming(loadCelltaskHandle, "", controlFreq, DAQmx_Val_Rising, DAQmx_Val_HWTimedSinglePoint, 1));
	} else {
		fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: pinMode may be called only when library is in CONFIG mode.\n");
		ArduDAQmxTerminate();
		setArduDAQmxLastError(ArduDAQmxErrorCode::ERROR_NOTCONFIG, 1);
		myPin = NULL;
	}
	if (myPin != NULL) {
		if (ArduDAQmxSampleClock.sourceDevNum == 0) {
			setSampleClock(devNum, IOtype, pinNum, ArduDAQmxSampleClock.samplingRate);
			fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: Sample clock not set explicity. Implicitly setting it to Dev%d with sampling rate of %f.\n", devNum, ArduDAQmxSampleClock.samplingRate);
		}
		if (myTask->clockHandler == NULL) {
			if (setTaskClock(myTask, &(ArduDAQmxSampleClock)) != (int32)ERROR_NONE) {
				ArduDAQmxTerminate();
				myPin = NULL;
			}
		}
	}
	return getArduDAQmxLastError();
}

/*!
 * \fn inline bool isSampleClock()
 * The function is used to check if sample clock has been set.
 * 
 * \return Returns TRUE(1) if sample clock has been set. Else, returns FALSE (0).
 */
inline bool isSampleClock()
{
	if (ArduDAQmxSampleClock.sourceDevNum == 0)
		return FALSE;
	return TRUE;
}

inline int32 setTaskClock(ArduDAQmxTask *NItask, sampleClock *sampClk) {
	return DAQmxErrChk(DAQmxCfgSampClkTiming(NItask->Handler, "", sampClk->samplingRate, sampClk->ActiveEdgTrg, sampClk->NIsampleMode, 1));
}

int setSampleClock(unsigned int sourceDevNum, IOmode sourceIOmode, unsigned int sourcePinNum, double samplingRate)
{
	if (getArduDAQmxStatus() == STATUS_CONFIG || getArduDAQmxStatus() == STATUS_READY) {
		ArduDAQmxStatus = STATUS_READY;
		ArduDAQmxSampleClock.sourceDevNum = sourceDevNum;
		ArduDAQmxSampleClock.sourceIOmode = sourceIOmode;
		ArduDAQmxSampleClock.samplingRate = samplingRate;
		ArduDAQmxSampleClock.ActiveEdgTrg = DAQmx_Val_Rising;
		ArduDAQmxSampleClock.NIsampleMode = DAQmx_Val_HWTimedSinglePoint;
		ArduDAQmxSampleClock.numberSample = 1;
		
		//set sample clock source ID string
		switch (sourceIOmode) {
		case ANALOG_IN:
			snprintf(ArduDAQmxSampleClock.sampClkSrcID, 1 + MaxNIstringLength, "%s%d/ai/SampleClock", ArduDAQmxDevPrefix, sourceDevNum);
			ArduDAQmxSampleClock.sampClkTask = &(ArduDAQmxDevList[sourceDevNum].AItask.Handler);
			break;
		case ANALOG_OUT:
			snprintf(ArduDAQmxSampleClock.sampClkSrcID, 1 + MaxNIstringLength, "%s%d/ao/SampleClock", ArduDAQmxDevPrefix, sourceDevNum);
			ArduDAQmxSampleClock.sampClkTask = &(ArduDAQmxDevList[sourceDevNum].AOtask.Handler);
			break;
		case DIGITAL_IN:
			snprintf(ArduDAQmxSampleClock.sampClkSrcID, 1 + MaxNIstringLength, "%s%d/di/SampleClock", ArduDAQmxDevPrefix, sourceDevNum);
			ArduDAQmxSampleClock.sampClkTask = &(ArduDAQmxDevList[sourceDevNum].DItask.Handler);
			break;
		case DIGITAL_OUT:
			snprintf(ArduDAQmxSampleClock.sampClkSrcID, 1 + MaxNIstringLength, "%s%d/do/SampleClock", ArduDAQmxDevPrefix, sourceDevNum);
			ArduDAQmxSampleClock.sampClkTask = &(ArduDAQmxDevList[sourceDevNum].DOtask.Handler);
			break;
		case COUNTER_IN:
			snprintf(ArduDAQmxSampleClock.sampClkSrcID, 1 + MaxNIstringLength, "%s%d/Ctr%dSource",    ArduDAQmxDevPrefix, sourceDevNum, sourcePinNum);
			ArduDAQmxSampleClock.sampClkTask = &(ArduDAQmxDevList[sourceDevNum].CTRtask[sourcePinNum].Handler);
			break;
		case COUNTER_OUT:
			snprintf(ArduDAQmxSampleClock.sampClkSrcID, 1 + MaxNIstringLength, "%s%d/Ctr%dSource",    ArduDAQmxDevPrefix, sourceDevNum, sourcePinNum);
			ArduDAQmxSampleClock.sampClkTask = &(ArduDAQmxDevList[sourceDevNum].CTRtask[sourcePinNum].Handler);
			break;
		default:
			setArduDAQmxLastError(ERROR_INVIO, 1);
			break;
		}
		return getArduDAQmxLastError();
	}// end if block that checked config status of the library
	fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: pinMode may be called only when library is in CONFIG mode.\n");
	return setArduDAQmxLastError(ERROR_NOTCONFIG, 1);
}


inline void setSamplingRate(float64 samplingRate)
{
	if (getArduDAQmxStatus() == STATUS_CONFIG) {
		ArduDAQmxSampleClock.samplingRate = samplingRate;
	}
}

void waitSampleClock(float64 waitSeconds)
{
	bool32 isLate;
	DAQmxErrChk(DAQmxWaitForNextSampleClock(&(ArduDAQmxSampleClock.sampClkTask), waitSeconds, &isLate));
}

int ArduDAQmxStart()
{
	cListElem *elem = NULL;
	ArduDAQmxTask *task = NULL;
	if (getArduDAQmxLastError() == ERROR_NONE) {
		if (getArduDAQmxStatus() == STATUS_READY) {
			ArduDAQmxStatus = STATUS_RUN;
			for (elem = cListFirstElem(ArduDAQmxTaskList); elem != NULL && getArduDAQmxLastError() == (int)ERROR_NONE; elem = cListNextElem(ArduDAQmxTaskList, elem)) {
				task = (ArduDAQmxTask *)elem->obj;
				//create I/O buffer
				task->ioBuffer = malloc(task->ioDataSize * task->activePinCnt);

				//start NI task
				DAQmxErrChk(DAQmxStartTask( task->Handler ));
			}
		}
		else {
			fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: ArduDAQmx may be started only when library is in READY [1] status. Current status code: [%d].\n", getArduDAQmxStatus());
		}
	}
	return getArduDAQmxLastError();
}

int ArduDAQmxStop()
{
	cListElem *elem = NULL;
	ArduDAQmxTask *task = NULL;
	if (getArduDAQmxLastError() == ERROR_NONE) {
		if (getArduDAQmxStatus() == STATUS_RUN) {
			ArduDAQmxStatus = STATUS_READY;
			for (elem = cListFirstElem(ArduDAQmxTaskList); elem != NULL && getArduDAQmxLastError() == (int)ERROR_NONE; elem = cListNextElem(ArduDAQmxTaskList, elem)) {
				task = (ArduDAQmxTask *)elem->obj;
				//free I/O buffer
				free(task->ioBuffer);

				//stop and clear NI task
				DAQmxErrChk(DAQmxStopTask ( task->Handler ));
				DAQmxErrChk(DAQmxClearTask( task->Handler ));
			}
		}
		else {
			fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: ArduDAQmx may be stopped only when library is in RUN [2] status. Current status code: [%d].\n", getArduDAQmxStatus());
		}
	}
	return getArduDAQmxLastError();
}


