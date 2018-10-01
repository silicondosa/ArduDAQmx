//#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include "ArduDAQmx.h"
#include "ansi_c.h"
#include "NIDAQmx.h"
#include <string.h>

int ArduDAQmxStatus = 0;
cLinkedList *ArduDAQmxDevList = NULL;
unsigned DevPrefixLength = 8;

void enumerateDAQmxDevices(int printFlag)
{
	int buffersize, n = 1;
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
		if (getArduDAQmxStatus() == 1) {
			//Create device struct
			DAQmxDevice *newDevice = (DAQmxDevice *)malloc(sizeof(DAQmxDevice));
			
			//copy datapoints
			strcpy_s(newDevice->DevName, sizeof(newDevice->DevName), devToken);
			devNUM = strtol( &(newDevice->DevName[8]), NULL, 10);
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
		n++;
	}
	if(printFlag == 1)
		printf("***************************************************************************\n\n");

	printf("ArduDAQmx Status Code: %d [%d devices in device list]\n", getArduDAQmxStatus(),cListLength(ArduDAQmxDevList));
}

inline int getArduDAQmxStatus()
{
	return ArduDAQmxStatus;
}

inline cLinkedList * getDAQmxDeviceList()
{
	return ArduDAQmxDevList;
}

int ArduDAQmxInit()
{
	//initialize device list
	ArduDAQmxDevList = (cLinkedList *)malloc(sizeof(cLinkedList));
	cListInit(ArduDAQmxDevList);

	//set library status as ready
	ArduDAQmxStatus = 1;
	return ArduDAQmxStatus;
}

int ArduDAQmxTerminate()
{
	//TODO: delete all device structs
	//TODO: delete devicelist linkedlist
	ArduDAQmxStatus = 0;
	return ArduDAQmxStatus;
}
