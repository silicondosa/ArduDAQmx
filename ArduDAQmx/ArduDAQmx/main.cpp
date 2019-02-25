// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h" //TODO: move all precompiled headers to stdafx.h
#include "ansi_c.h"
#include "macrodef.h"
#include "msunistd.h"
#include "ArduDAQmx.h"
#include <string.h>

//unsigned int enumerateTerminals();

int main()
{
	ArduDAQmxInit("PXI1Slot");
	enumerateDAQmxDevices(1);

	//enumerateDAQmxDeviceTerminals(2);
	
	printf("Counter In:\n");
	enumerateDAQmxDeviceChannels(5, COUNTER_IN, 1);

	printf("Counter Out:\n");
	enumerateDAQmxDeviceChannels(5, COUNTER_OUT, 1);
	



	//char myPin[255];
	//printf("\nTesting pin to string: %s\n\n", pin2string(myPin, 5, IOmode::ANALOG_IN, 1));

	//getchar();
	return 0;
}

/*
unsigned int enumerateTerminals()
{
	char myDev[] = "PXI1Slot5";
	const unsigned bufSize = 20000;
	char data[bufSize], data2[bufSize];
	char *rem_data;
	char *NIterm;
	
	DAQmxGetDevTerminals(myDev, data, bufSize);
	int charLength = (int)strnlen_s(data, bufSize);
	rem_data = data;
	unsigned int i = 0;
	
	for (NIterm = strtok_s(rem_data, ",", &rem_data); NIterm != NULL; NIterm = strtok_s(rem_data, ",", &rem_data), i++) {
		printf("Terminal %d: %s\n", i+1, NIterm);
	}

	printf("\nOn %s - %d Terminals (%d characters)\n\n", myDev, i, charLength);

	// get AI physical channels
	DAQmxGetDevAIPhysicalChans(myDev, data, bufSize);
	printf("Physical AI Channels (%d characters): %s\n\n", strnlen_s(data, bufSize-1), data);

	DAQmxGetDevAOPhysicalChans(myDev, data, bufSize);
	printf("Physical AO Channels (%d characters): %s\n"  , strnlen_s(data, bufSize-1), data);


	return i;
}
*/


/*
unsigned int enumerateChannels(unsigned int myDev, IOmode IOtype, unsigned int printFlag)
{
	const unsigned bufSize = 20000;
	char data[bufSize];
	char stringDev[256];
	char *remainder_data, *oneChannel_data;

	snprintf(stringDev, 1+MaxArduDAQmxDevStringLength, "%s%d", ArduDAQmxDevPrefix, myDev);

	switch (IOtype)
	{
	case ANALOG_IN:
		DAQmxGetDevAIPhysicalChans()
		break;
	case ANALOG_OUT:
		DAQmxGetDevAOPhysicalChans
		break;
	case DIGITAL_IN:
		DAQmxGetDevDIPorts
		break;
	case DIGITAL_OUT:
		DAQmxGetDevDOPorts
		break;
	case COUNTER_IN:
		DAQmxGetDevCIPhysicalChans
		break;
	case COUNTER_OUT:
		DAQmxGetDevCOPhysicalChans
		break;
	default:
		break;
	}

	remainder_data = data;
	for (oneChannel_data = strtok_s(remainder_data, ",", &remainder_data); oneChannel_data != NULL; oneChannel_data = strtok_s(remainder_data, ",", &rem_data), i++) {
		printf("Terminal %d: %s\n", i+1, NIterm);
	}

	return 0;
}
*/