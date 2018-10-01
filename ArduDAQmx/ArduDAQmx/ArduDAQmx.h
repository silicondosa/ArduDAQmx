#pragma once
#ifndef ARDUDAQMX_H

#include "ansi_c.h"
#include "NIDAQmx.h"
#include "cLinkedList.h"

extern int ArduDAQmxStatus;
extern cLinkedList *ArduDAQmxDevList;
extern unsigned DevPrefixLength;

void enumerateDAQmxDevices(int);
inline int getArduDAQmxStatus();
inline cLinkedList * getDAQmxDeviceList();

typedef enum {
	DIGITAL_IN, DIGITAL_OUT,
	 ANALOG_IN,  ANALOG_OUT,
	COUNTER_IN, COUNTER_OUT
}IOmode;

typedef struct pin{
	unsigned int	SlotNum;
	unsigned int	PinNum;
	IOmode			pinIOmode;
} pin;

typedef struct DAQmxDevice{
	char			DevName[256];
	unsigned int	DevNum;
	int				DevSerial;
	int				isDevSim;
	cLinkedList		*pinList;

} DAQmxDevice;

int ArduDAQmxInit();

pin* pinMode(unsigned int, unsigned int, IOmode);

int ArduDAQmxTerminate();
#endif // !ARDUDAQMX_H
