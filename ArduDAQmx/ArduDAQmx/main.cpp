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

	/*
	for (int i = 6; i < 7; i++) {
		setSampleClock(2, (_IOmode)i, 9, 1);
		printf("%s\n", ArduDAQmxSampleClock.sampClkSrcID);
	}
	*/



	//char myPin[255];
	//printf("\nTesting pin to string: %s\n\n", pin2string(myPin, 5, IOmode::ANALOG_IN, 1));

	//getchar();
	return 0;
}
