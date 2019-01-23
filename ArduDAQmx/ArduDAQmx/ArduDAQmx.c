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
int				NIDAQmxErrorCode			= 0;
const char		*DefaultArduDAQmxDevPrefix	= "PXI1Slot";
char			*ArduDAQmxDevPrefix			= NULL;
const unsigned	MaxArduDAQmxDevPrefixLength	= 8;
unsigned		ArduDAQmxDevPrefixLength	= MaxArduDAQmxDevPrefixLength;
DAQmxDevice		*ArduDAQmxDevList			= NULL;
unsigned long	ArduDAQmxDevCount			= 0;
unsigned long	ArduDAQmxDevMaxNum			= 0;
cLinkedList		*DAQmxEnumeratedDevList		= NULL;
unsigned long	DAQmxEnumeratedDevCount		= 0;
unsigned long	DAQmxEnumeratedDevMaxNum	= 0;

// Library function definitions
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
	int				isObjInserted = 0;
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
		printf("Full Device name list: %s\n\n", DAQmxDevNameList);
		printf("******************** LIST OF DEVICES ON THIS COMPUTER *********************\n");
		printf("Device Name || Device Number || Device type || Device Serial# || Simulated?\n");
		printf("***************************************************************************\n");
	} // end print flag check

	//Get information about the DAQmx devices on the list
	for (DAQmxDevName = strtok_s(remainder_DAQmxDevNameList, ",", &remainder_DAQmxDevNameList); DAQmxDevName != NULL; DAQmxDevName = strtok_s(remainder_DAQmxDevNameList, ", ", &remainder_DAQmxDevNameList), tempDAQmxEnumeratedDevCount++) {
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
		
		// TODO: initialize list of tasks

		//set max device number
		if(DAQmxEnumeratedDevMaxNum < newDevice->DevNum)
			DAQmxEnumeratedDevMaxNum = newDevice->DevNum;

		// Sort and insert new device object into temporary linked list
		isObjInserted = 0;
		list_elem = cListLastElem(DAQmxEnumeratedDevList);
		while (isObjInserted == 0) {
			if (list_elem == NULL) { // if linked list is empty, append the new device to list
				isObjInserted = 1;
				cListAppend(DAQmxEnumeratedDevList, (void *)newDevice);
			} else if ( newDevice->DevNum == 0 ) { // if object in list has device number as 0, call FATAL error
				fprintf(ERRSTREAM, "ArduDAQmx library: FATAL: A DAQmx device has a device number as 0. All device numbers must be positive integers");
				ArduDAQmxTerminate();
				isObjInserted = 1;
				//exit(-1);
			} else if ( ((DAQmxDevice *)list_elem->obj)->DevNum == newDevice->DevNum) { // if more than one object has equal device numbers, call FATAL error
				fprintf(ERRSTREAM, "ArduDAQmx library: FATAL: Multiple DAQmx devices have the same device number. Rename devices in NI MAX.\n");
				ArduDAQmxTerminate();
				isObjInserted = 1;
				//exit(-1);
			} else if ( ((DAQmxDevice *)list_elem->obj)->DevNum <  newDevice->DevNum) { // if list object device number is greater than new device number, insert new device before the list object
				isObjInserted = 1;
				cListInsertAfter(DAQmxEnumeratedDevList, (void *)newDevice, list_elem);
			}

			list_elem = cListPrevElem(DAQmxEnumeratedDevList, list_elem);
		} // end object insertion while loop

		// Optionally print device table contents
		if (printFlag == 1) {
			if (isSimulated = 0)
				printf("%s || %d || %s || %d || no\n",  DAQmxDevName, devNUM, DAQmxDeviceType, deviceSerial);
			else
				printf("%s || %d || %s || %d || yes\n", DAQmxDevName, devNUM, DAQmxDeviceType, deviceSerial);
		} // end print flag check

		free(DAQmxDeviceType); // DYN-F free device type buffer
	} // end device enumeration for loop

	// Print end of device table, status messages
	
	if (printFlag == 1) {
		printf("***************************************************************************\n\n");
		printf("ArduDAQmx library: %d devices in internal device list\n", cListLength(DAQmxEnumeratedDevList));
		printArduDAQmxStatus();
		printf("***************************************************************************\n\n");
	} // end print flag check

	// Test for change in the number of devices since library was configured. Also checks tests for no devices.
	if (tempDAQmxEnumeratedDevCount != cListLength(DAQmxEnumeratedDevList) && ArduDAQmxFirstEnum == 0) {
		fprintf(ERRSTREAM, "ArduDAQmx library: FATAL: Length of enumerated device list not equal to enumerated device count.\n");
		fprintf(ERRSTREAM, "Temp Count: %d, Main Count (0x%p): %d", tempDAQmxEnumeratedDevCount, DAQmxEnumeratedDevList, cListLength(DAQmxEnumeratedDevList));
		ArduDAQmxTerminate();
		//getchar();
		//exit(-1);
	} else if (tempDAQmxEnumeratedDevCount == 0) { // If no new devices on the list, flash a warning and disable library
		fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: No NI-DAQmx devices detected. Terminating the library...\n");		
		setArduDAQmxLastError(ERROR_NODEVICES, 1);
		ArduDAQmxTerminate();
	} else if (DAQmxEnumeratedDevCount != 0 && DAQmxEnumeratedDevCount != tempDAQmxEnumeratedDevCount) { // if new list is different than old list, then flash warning and disable library
		fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: %d devices detected. Library had %d devices when configured and need to be reinitialized. Terminating the library now...", tempDAQmxEnumeratedDevCount, DAQmxEnumeratedDevCount);	
		setArduDAQmxLastError(ERROR_DEVCHANGE, 1);
		ArduDAQmxTerminate();
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
 * Sets the '' eror code of the ArduDAQmx library.
 * The routine also prints the meaning of the message if the printErrorMsgFlag = 1.
 * 
 * \param newErrorCode The new error code to set of type 'ArduDAQmxErrorcode'
 * \param printErrorMsgFlag
 * \return 
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
			printf("ArduDAQmx library: Status: Library uninitialized. Urgh! [STATUS_PRECONFIG]\n");
			break;
	case STATUS_CONFIG:
			printf("ArduDAQmx library: Status: Initialized, but not configured. [STATUS_CONFIG]\n");
			break;
	case STATUS_READY:
			printf("ArduDAQmx library: Status: Configured and ready. Let's Go!!! [STATUS_READY]\n");
			break;	
	default:
			break;
	}
	return ArduDAQmxStatus;
}

/*!
 * \fn inline int printArduDAQmxLastError()
 * Prints the meaning of the last library error as reflected in 'ArduDAQmxErrorCode'.
 * 
 * \return Returns 'ArduDAQmxErrorCode' as an integer.
 */
inline int printArduDAQmxLastError()
{
	switch (ArduDAQmxError)
	{
	case ERROR_DEVCHANGE:
			printf("ArduDAQmx library: Error: Number of DAQmx devices have changed during operation. [ERROR_DEVCHANGE]\n");
			break;
	case ERROR_NODEVICES:
			printf("ArduDAQmx library: Error: No DAQmx devices discovered. [ERROR_NODEVICES]\n");
			break;
	case ERROR_NOTCONFIG:
			printf("ArduDAQmx library: Error: Library is not configured. [ERROR_NOTCONFIG]\n");
			break;
	case ERROR_NONE:
			printf("ArduDAQmx library: No errors detected. [ERROR_NONE]\n");
			break;	
	default:
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
 * \fn inline unsigned getArduDAQmxDevPrefixLength()
 * Returns the length of the current DAQmx device prefix as defined in 'ArduDAQmxDevPrefixLength'.
 * 
 * \return Returns the length of current DAQmx device prefix as an unsigned integer.
 */
inline unsigned getArduDAQmxDevPrefixLength()
{
	return ArduDAQmxDevPrefixLength;
}

/*!
 * \fn int ArduDAQmxConfigure()
 * Configures the ArduDAQmx library with a list of devices and library status.
 * This function is called by 'ArduDAQmxInit' and must be called before initialization to setup the library.
 * 
 * \return Returns the status of the status of the library as defined in 'ArduDAQmxStatus'
 */
int ArduDAQmxConfigure()
{

	// If in PRECONFIG state, setup library and device list
	if (getArduDAQmxStatus() == (int)STATUS_PRECONFIG) { // if there are no issues and library is in preconfig, setup library
		ArduDAQmxDevList	= (DAQmxDevice *) malloc( sizeof(DAQmxDevice) * DAQmxEnumeratedDevMaxNum ); // DYN-M: allocate array of devices for fast access
		unsigned int		i = 0, termLoop = 0;
		cListElem *elem		= NULL;
		DAQmxDevice *cpyDev = NULL;
		unsigned int cpyInd = 0;
		
		// initialize all elements in ArduDAQmxDevList to have DevNum = 0 as it contains no valid devices yet.
		for (i = 0, cpyDev = ArduDAQmxDevList; i < DAQmxEnumeratedDevMaxNum; i++, cpyDev++) {
			cpyDev->DevNum = 0;
		}

		// copy sorted enumerated device linked list into the ArduDAQmxDevList device array that was just initialized.
		for (termLoop = 0, cpyDev = ArduDAQmxDevList, elem = cListFirstElem(DAQmxEnumeratedDevList); elem != NULL && termLoop == 0; elem = cListNextElem(DAQmxEnumeratedDevList, elem)) {
			cpyInd = ((DAQmxDevice*)(elem->obj))->DevNum - 1;
			if (cpyInd < 0) {
				fprintf(ERRSTREAM, "ArduDAQmx library: FATAL: A DAQmx device has a device number as 0. All device numbers must be positive integers");
				ArduDAQmxTerminate();
				termLoop = 1;
			}
			cpyDev[cpyInd] = *(DAQmxDevice*)(elem->obj);
		}
		ArduDAQmxDevCount  = DAQmxEnumeratedDevCount;
		ArduDAQmxDevMaxNum = DAQmxEnumeratedDevMaxNum;
		setArduDAQmxLastError(ERROR_NONE, 0);
		ArduDAQmxStatus = STATUS_CONFIG;
	} else {
		fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: Library must be in preconfig mode to configure.\n");
	}

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
		fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: Library must be in preconfig mode to initialize. Terminate library first!\n");
	}
	return ArduDAQmxStatus;
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
	//TODO: stop any active DAQmx tasks using the ArduDAQmx I/O Stop function		
	ArduDAQmxStatus				= (int)STATUS_PRECONFIG;
	ArduDAQmxError				= 0;
	NIDAQmxErrorCode			= 0;	
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
 * \fn pin * pinMode(unsigned int deviceNumer, unsigned int pinNumber, IOmode IOtype)
 * Defines the input-output mode of pins as either analog or digital pins.
 * 
 * \param deviceNumer NI-DAQ device/slot number of the pin.
 * \param pinNumber Pin number on the NI-DAQ device.
 * \param IOtype I/O type being requested on the pin as defined in ::IOmode.
 * \return Returns the 'pin' data structure used to configure and operate the pin.
 */
 
/*
pin * pinMode(unsigned int deviceNumer, unsigned int pinNumber, IOmode IOtype)
{
	DAQmxDevice myDev = ArduDAQmxDevList[deviceNumer-1];
	pin * myPin = NULL;
	if (myDev != NULL) {
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
	} // check myDev check

	return myPin;
}
*/
