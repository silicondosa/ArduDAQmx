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
	enumerateDAQmxDeviceChannels(5, CTR_ANGLE_IN, 1);

	printf("Counter Out:\n");
	enumerateDAQmxDeviceChannels(5, CTR_TICK_OUT, 1);

	printf("----------------------------------------------------------------------------------\n\n");

	setSampleClock(5, ANALOG_IN, 0, 1000.0);
	pinMode(2, 0, ANALOG_OUT, 0);

	ArduDAQmxStart();

	// PUT IO functions here.

	ArduDAQmxStop();

	/*
	for (int i = 6; i < 7; i++) {
		setSampleClock(2, (_IOmode)i, 9, 1);
		printf("%s\n", ArduDAQmxSampleClock.sampClkSrcID);
	}
	*/



	//char myPin[255];
	//printf("\nTesting pin to string: %s\n\n", pin2string(myPin, 5, IOmode::ANALOG_IN, 1));

	//getchar();

	ArduDAQmxTerminate();
	printf("End of main function - bye bye!\n");
	return 0;
}
