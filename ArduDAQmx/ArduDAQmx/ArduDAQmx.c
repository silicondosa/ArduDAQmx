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

// initializations for global status variables that keep track of library operations
unsigned int	DAQmxMaxPinCount			= 32;
int				ArduDAQmxStatus				= (int) STATUS_PRECONFIG;
char			*ArduDAQmxDevPrefix			= "PXI1Slot";
const unsigned	MaxArduDAQmxDevPrefixLength	= 8;
unsigned		ArduDAQmxDevPrefixLength	= MaxArduDAQmxDevPrefixLength;
DAQmxDevice		*ArduDAQmxDevList			= NULL;

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
	int buffersize, loopNum = 0; // Buffer size datatypes
	int devicetype_buffersize; //devicesernum_buffersize;

	//Device Info variable initialization
	char			*devicenames, *remainder_devicenames; // device name string
	char			*devicetype; // device type string
	char			*devToken;
	DAQmxDevice		*newDevice = NULL;

	int				devicesernum;
	int				is_simulated;
	unsigned long	devNUM = 0, maxDevNum = 0;
	int				isObjInserted = 0;

	cLinkedList		*DAQmxTempDevList = NULL;
	cListElem		*list_i = NULL;
	
	// Create temporary list for DAQmx Devices
	DAQmxTempDevList = (cLinkedList *)malloc(sizeof(cLinkedList)); // DYN-M: create DAQmx master device list
	cListInit(DAQmxTempDevList);

	// obtain device names from NI-DAQmx
	buffersize = DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames, devicenames);
	devicenames = (char*)malloc(buffersize); // DYN-M: dynamically allocate devicenames
	remainder_devicenames = devicenames;

	//Get the string of devicenames in the computer
	DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames, devicenames, buffersize);

	// Optionally print device table headers
	if (printFlag == 1) {
		printf("Full Device name list: %s\n\n", devicenames);
		printf("******************** LIST OF DEVICES ON THIS COMPUTER *********************\n");
		printf("Device Name || Device Number || Device type || Device Serial# || Simulated?\n");
		printf("***************************************************************************\n");
	} // end print flag check

	//Get information about the first DAQmx device on the list
	devToken = strtok_s(remainder_devicenames, ",", &remainder_devicenames);

	while (devToken != NULL) {
		
		// Obtain some device properties
		devicetype_buffersize = DAQmxGetDeviceAttribute(devToken, DAQmx_Dev_ProductType, NULL); //Get Product Type for a DAQmx device pointed to by devToken
		devicetype = (char*)malloc(devicetype_buffersize); // DYN-M: dynamically allocate memory for devicetype
		DAQmxGetDeviceAttribute(devToken, DAQmx_Dev_ProductType, devicetype, devicetype_buffersize); //Get device type		
		DAQmxGetDeviceAttribute(devToken, DAQmx_Dev_SerialNum, &devicesernum, 1); //Get Product Serial Number for the device
		DAQmxGetDeviceAttribute(devToken, DAQmx_Dev_IsSimulated, &is_simulated, 1); //Get "Is device simulated?" flag for the device


		//If ArduDAQmx is initialized, write device properties to device struct, then write device struct to device list
		if (getArduDAQmxStatus() == (int)STATUS_PRECONFIG) {
			newDevice = (DAQmxDevice *)malloc(sizeof(DAQmxDevice)); // DYN-M: dynamically allocate newDevice for DAQmx Master List
			
			//copy datapoints
			strcpy_s(newDevice->DevName, sizeof(newDevice->DevName), devToken); // copy device name in structure
			devNUM					= strtol( &(newDevice->DevName[ArduDAQmxDevPrefixLength]), NULL, 10);
			newDevice->DevNum		= devNUM; // set device number in structure			
			newDevice->DevSerial	= devicesernum;
			newDevice->isDevSim		= is_simulated;
			newDevice->pinList		= (pin *) malloc( sizeof(pin) * DAQmxMaxPinCount ); // DYN-M: initialize pin list array
			
			// TODO: initialize list of tasks

			//set max device number
			if (devNUM > maxDevNum)
				maxDevNum = devNUM;

			// Sort and insert new device object into temporary linked list
			for (isObjInserted = 0, list_i = cListFirstElem(DAQmxTempDevList); isObjInserted != 1; list_i = cListNextElem(DAQmxTempDevList, list_i)) {
				if (list_i == NULL) {
					isObjInserted = 1;
					cListAppend(DAQmxTempDevList, (void *)newDevice); //append device struct to device list
				} else if ( ((DAQmxDevice *)list_i->obj)->DevNum > newDevice->DevNum ) {
					isObjInserted = 1;
					cListInsertAfter(DAQmxTempDevList, (void *)newDevice, list_i);
				} else if ( ((DAQmxDevice *)list_i->obj)->DevNum == newDevice->DevNum) {
					fprintf(ERRSTREAM, "ArduDAQmx library: FATAL: Multiple DAQmx devices have the same device number. Rename devices in NI MAX.\n");
					exit(-1);
				}
			} // end object insertion loop

		} // end config status check

		//free device type buffer
		free(devicetype);

		// Optionally print device table contents
		if (printFlag == 1) {
			if (is_simulated = 0)
				printf("%s || %d || %s || %d || no\n",  devToken, devNUM, devicetype, devicesernum);
			else
				printf("%s || %d || %s || %d || yes\n", devToken, devNUM, devicetype, devicesernum);
		} // end print flag check

		//Get the next DAQmx device name in the list
		devToken = strtok_s(remainder_devicenames, ", ", &remainder_devicenames);
		loopNum++;
	} // end while loop

	// Print end of device table, status messages
	if (printFlag == 1) {
		printf("***************************************************************************\n\n");
		printf("ArduDAQmx library: %d devices in internal device list\n", cListLength(DAQmxTempDevList));
		printDAQmxStatus();
	} // end print flag check

	// If in PRECONFIG state, setup library and device list
	if (getArduDAQmxStatus() == (int)STATUS_PRECONFIG) {
		ArduDAQmxDevList	= (DAQmxDevice *) malloc( sizeof(DAQmxDevice) * maxDevNum ); // DYN-M: allocate array of devices for fast access
		unsigned int		i = 0, minDevNum = 32767, devListLength = cListLength(DAQmxTempDevList);
		cListElem *elem		= NULL;
		DAQmxDevice *cpyDev = NULL;
		for (; i < maxDevNum; i++) {
			//copy sorted temprorary device list into the primary device array.
			
			//TODO: need to move some or all of the if conditions to the above sorting loop
			
			for (elem = cListFirstElem(DAQmxTempDevList) ; elem != NULL; elem = cListNextElem( DAQmxTempDevList, elem) ) {
				if ( ((DAQmxDevice *)elem->obj)->DevNum			== 0 ) {
					fprintf(ERRSTREAM, "ArduDAQmx library: FATAL: A DAQmx device has a device number as 0. All device numbers must be positive integers");
					exit(-1);
				} else if ( ((DAQmxDevice *)elem->obj)->DevNum	== minDevNum ) {
					fprintf(ERRSTREAM, "ArduDAQmx library: FATAL: More than one device has the same device number (%d)", minDevNum);
					exit(-1);
				} else if ( ((DAQmxDevice *)elem->obj)->DevNum	< minDevNum ) {
					minDevNum = ((DAQmxDevice *)elem->obj)->DevNum;
					cpyDev = (DAQmxDevice *)elem->obj;
				}
			} // linked list scan loop
			ArduDAQmxDevList[minDevNum - 1] = *cpyDev;
		} // array scan loop
	} else if (loopNum != cListLength(DAQmxTempDevList)) {
		fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: Current list of %d devices - Possible change in devices since the library was configured. Terminate the library and reconfigure!", loopNum);
	}

	// DYN-F: delete devices from DAQmxTempDevList and unlink all of it.
	cListElem *elem = NULL;
	for (elem = cListFirstElem(DAQmxTempDevList) ; elem != NULL; elem = cListNextElem( DAQmxTempDevList, elem) ) {
		// free( ((DAQmxDevice *)elem->obj)->pinList ); // DYN-F: free pinlist array
		free( (DAQmxDevice *)elem->obj ); // free DAQmxDevice object
		free(elem); // unlink linked list element
	}

	// if we are in PRECONFIG mode, move to CONFIG mode, preventing any further INIT variable changes.
	if (getArduDAQmxStatus() == (int)STATUS_PRECONFIG) {	
		ArduDAQmxStatus == STATUS_CONFIG;
	}

	// DYN-F: free DAQmxTempDevList
	free(DAQmxTempDevList);
	DAQmxTempDevList = NULL;

	free(devicenames); // DYN-F: free devicenames
} // end enumerateDAQmxDevices function

/*!
 * \fn inline int getArduDAQmxStatus()
 * Returns the current opeational status of the ArduDAQmx library as reported by the 'ArduDAQmxStatus' variable.
 * Use this function to access the 'ArduDAQmxStatus' variable.
 * 
 * \return 
 */
inline int getArduDAQmxStatus()
{
	return ArduDAQmxStatus;
}

/*!
 * \fn inline int printDAQmxStatus()
 * Prints the meaning of the library status as reflected in 'ArduDAQmxStatus'.
 * 
 * \return Returns 'ArduDAQmxStatus' as an integer.
 */
inline int printDAQmxStatus()
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
 * \fn inline void setArduDAQmxPrefix(char *newPrefix)
 * Lets users set the DAQmx device name prefix as defined in 'ArduDAQmxDevPrefix'.
 * If prefix is longer than 'MaxArduDAQmxDevPrefix', only 8 characters (not including NULL character) used - everything else is truncated.
 * 
 * \param newPrefix Pointer to a C string with the new device prefix.
 */
/*inline void setArduDAQmxPrefix(char *newPrefix)
{
	if (getArduDAQmxStatus() == STATUS_PRECONFIG) {

	}
}*/

/*!
 * \fn inline unsigned getArduDAQmxDevPrefixLength()
 * Returns the length of the current DAQmx device prefix as defined in 'ArduDAQmxDevPrefixLength'.
 * 
 * \return returns the length of current DAQmx device prefix as an unsigned integer.
 */
inline unsigned getArduDAQmxDevPrefixLength()
{
	return ArduDAQmxDevPrefixLength;
}

/*!
 * \fn int ArduDAQmxInit()
 * Performs the initialization of the ArduDAQmx library. Calling this function is mandatory before using the library.
 * If the library is not initialized and ready, calling this function will setup the library and set the library status to STATUS_READY
 * 
 * \param devicePrefix Pointer to a C string with the device name prefix.
 * \return Returns 0 if there are no errors, and an error code otherwise.
 */
int ArduDAQmxInit(char *devicePrefix)
{
	if (ArduDAQmxStatus == STATUS_PRECONFIG) {
		strncpy_s(ArduDAQmxDevPrefix, strnlen_s(ArduDAQmxDevPrefix, MaxArduDAQmxDevPrefixLength), devicePrefix, MaxArduDAQmxDevPrefixLength);
		ArduDAQmxDevPrefixLength = strnlen_s(ArduDAQmxDevPrefix, MaxArduDAQmxDevPrefixLength);
		enumerateDAQmxDevices(0);
	} else {
		fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: Library must be in preconfig mode to initialize.\n");
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
 * \return Returns the status of the library as relfected in 'ArduDAQmxStatus'.
 */
int ArduDAQmxTerminate()
{
	//TODO: stop any active DAQmx tasks using the ArduDAQmx I/O Stop function


	// DYN-F: free ArduDAQmxDevList array
	free(ArduDAQmxDevList);
	ArduDAQmxDevList = NULL;
	
	ArduDAQmxStatus = (int)STATUS_PRECONFIG;
	return ArduDAQmxStatus;
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

