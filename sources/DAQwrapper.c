//
// Created by surajcha on 7/27/18.
//

#include "../include/DAQwrapper.h"
#include "../include/NIDAQmx.h"

void enumerateDAQDevices()
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

    //get the size of buffer
    buffersize = DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames, devicenames);
    devicenames = (char*)malloc(buffersize);
    remainder_devicenames = devicenames;
    //devicenames = (char*)malloc(devicenames, sizeof(char)*buffersize);
    //Get the string of devicenames in the computer
    DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames, devicenames, buffersize);
    printf("Full Device name list: %s\n\n", devicenames);
    printf("List of devices in this computer: \n \n");
    printf("FORMAT ***************************************************\n");
    printf("Device Name || Device type || Device Serial# || Simulated?\n");
    printf("**********************************************************\n");

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
        if (is_simulated = 0)
            printf("%s || %s || %d || no\n", devToken, devicetype, devicesernum);
        else
            printf("%s || %s || %d || yes\n", devToken, devicetype, devicesernum);

        //Get the next device name in the list
        devToken = strtok_s(remainder_devicenames, ", ", &remainder_devicenames);
        n++;
    }
    printf("\n");
    getchar();
}
