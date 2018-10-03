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
int ArduDAQmxStatus = 0;
cLinkedList *ArduDAQmxDevList = NULL;
char *ArduDAQmxDevPrefix = "PXI1Slot";
const unsigned MaxArduDAQmxDevPrefixLength = 8;
unsigned ArduDAQmxDevPrefixLength = MaxArduDAQmxDevPrefixLength;

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
	int buffersize, loopNum = 0;
	//Buffer size datatypes
	int devicetype_buffersize;
	//int devicesernum_buffersize;

	//Device Info variable initialization
	char* devicenames = NULL;
	char* remainder_devicenames;
	char* devicetype;
	int devicesernum;
	int is_simulated;
	char * devToken;
	unsigned long devNUM = 0;

	//get the size of buffer
	buffersize = DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames, devicenames);
	devicenames = (char*)malloc(buffersize);
	remainder_devicenames = devicenames;
	//devicenames = (char*)malloc(devicenames, sizeof(char)*buffersize);
	//Get the string of devicenames in the computer
	DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames, devicenames, buffersize);

	// Optionally print device table
	if (printFlag == 1) {
		printf("Full Device name list: %s\n\n", devicenames);
		printf("******************** LIST OF DEVICES ON THIS COMPUTER *********************\n");
		printf("Device Name || Device Number || Device type || Device Serial# || Simulated?\n");
		printf("***************************************************************************\n");
	} // end print flag check

	//Get information about the device
	devToken = strtok_s(remainder_devicenames, ",", &remainder_devicenames);

	while (devToken != NULL)
	{
		//Get Product Type for a device
		devicetype_buffersize = DAQmxGetDeviceAttribute(devToken, DAQmx_Dev_ProductType, NULL);
		devicetype = (char*)malloc(devicetype_buffersize);
		DAQmxGetDeviceAttribute(devToken, DAQmx_Dev_ProductType, devicetype, devicetype_buffersize);

		//Get Product Serial Number for the device
		DAQmxGetDeviceAttribute(devToken, DAQmx_Dev_SerialNum, &devicesernum, 1);


		//Get Is device simulated? for the device
		DAQmxGetDeviceAttribute(devToken, DAQmx_Dev_IsSimulated, &is_simulated, 1);

		//If ArduDAQmx is initialized, write device properties to device struct
		//Then write device struct to device list
		if (getArduDAQmxStatus() == (int)STATUS_PRECONFIG) {
			//Create device struct
			DAQmxDevice *newDevice = (DAQmxDevice *)malloc(sizeof(DAQmxDevice));
			
			//copy datapoints
			strcpy_s(newDevice->DevName, sizeof(newDevice->DevName), devToken);
			devNUM = strtol( &(newDevice->DevName[ArduDAQmxDevPrefixLength]), NULL, 10);
			newDevice->DevNum = devNUM;
			newDevice->DevSerial = devicesernum;
			newDevice->isDevSim = is_simulated;
			
			//append device struct to device list
			cListAppend(ArduDAQmxDevList, (void *)newDevice);
		}

		// Optionally print device table
		if (printFlag == 1) {
			if (is_simulated = 0)
				printf("%s || %d || %s || %d || no\n",  devToken, devNUM, devicetype, devicesernum);
			else
				printf("%s || %d || %s || %d || yes\n", devToken, devNUM, devicetype, devicesernum);
		}

		//Get the next device name in the list
		devToken = strtok_s(remainder_devicenames, ", ", &remainder_devicenames);
		loopNum++;
	}
	if (printFlag == 1) {
		printf("***************************************************************************\n\n");
		printf("ArduDAQmx library: %d devices in internal device list\n", cListLength(ArduDAQmxDevList));
		if (getArduDAQmxStatus() == (int)STATUS_PRECONFIG) {
			ArduDAQmxStatus = (int)STATUS_CONFIG;
		} else if (loopNum != cListLength(ArduDAQmxDevList)) {
			fprintf(ERRSTREAM, "ArduDAQmx library: WARNING: Current list of %d devices - Possible change in devices since the library was configured. Terminate the library and reconfigure!", loopNum);
		}
		printDAQmxStatus();
	}
}

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
			printf("ArduDAQmx library: Status: Not yet configured. Urgh :-( [STATUS_PRECONFIG]\n");
			break;
	case STATUS_CONFIG:
			printf("ArduDAQmx library: Status: Configured but not initialized. [STATUS_CONFIG]\n");
			break;
	case STATUS_READY:
			printf("ArduDAQmx library: Status: Initialized and Ready. Let's Go! [STATUS_READY]\n");
			break;	
	default:
			break;
	}
	return ArduDAQmxStatus;
}

/*!
 * \fn inline cLinkedList * getDAQmxDeviceList()
 * Returns a pointer to the linked list of active DAQmx devices.
 * 
 * \return Returns a pointer to the linked list of active DAQmx devices.
 */
inline cLinkedList * getDAQmxDeviceList()
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
inline void setArduDAQmxPrefix(char *newPrefix)
{
	if (getArduDAQmxStatus() == STATUS_PRECONFIG) {
		strncpy_s(ArduDAQmxDevPrefix, strnlen_s(ArduDAQmxDevPrefix, MaxArduDAQmxDevPrefixLength), newPrefix, MaxArduDAQmxDevPrefixLength);
		ArduDAQmxDevPrefixLength = strnlen_s(ArduDAQmxDevPrefix, MaxArduDAQmxDevPrefixLength);
	}
	ArduDAQmxStatus = STATUS_CONFIG;
}

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
 * \return Returns 0 if there are no errors, and an error code otherwise.
 */
int ArduDAQmxInit()
{
	//initialize device list
	ArduDAQmxDevList = (cLinkedList *)malloc(sizeof(cLinkedList));
	cListInit(ArduDAQmxDevList);

	//set library status as ready
	ArduDAQmxStatus = 1;
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
	//TODO: delete all device structs
	//TODO: delete devicelist linkedlist	
	ArduDAQmxStatus = (int)STATUS_PRECONFIG;
	return ArduDAQmxStatus;
}

pin * pinMode(unsigned int, unsigned int, IOmode)
{
	return NULL;
}

