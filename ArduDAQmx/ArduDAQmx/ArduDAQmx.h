/*!
 * \file ArduDAQmx.h
 * \brief ArduDAQmx: A simple Arduino-esque wrapper for the National Instruments NI-DAQmx C/C++ API
 * \author Suraj Chakravarthi Raja
 */

#pragma once
#ifndef ARDUDAQMX_H

#include "ansi_c.h"
#include "NIDAQmx.h"
#include "cLinkedList.h"

// ArduDAQmx uses some global status variables to keep track of library operations
/*!
 * \def ArduDAQmxStatus
 * \brief Operational status of the ArduDAQmx library. DO NOT access this variable.
 */
extern int ArduDAQmxStatus;

/*!
 * \def ArduDAQmxDevList
 * \brief Master list of available NI-DAQmx compatible devices. DO NOT access this variable directly.
 * The list of available NI-DAQmx devices can be printed using 'enumerateDAQmxDevices(1)'.
 * Get the pointer to tis list using 'getArduDAQmxDeviceList()'. Calling 'printDAQmxStatus' will print the meaning of this variable.
 */
extern cLinkedList *ArduDAQmxDevList;

/*!
* \def ArduDAQmxDevPrefix
* \brief Device name prefix. Default value is "PXI1Slot" but can be changed.
* DO NOT access this variable directly. Acces this variable only by using the 'getArduDAQmxPrefix()' and 'setArduDAQmxPrefix(char *)' functions
*/
extern char *ArduDAQmxDevPrefix;

/*!
 * \def MaxArduDAQmxDevPrefixLength
 * The maximum length of the DADmx device string is 8 characters. It does NOT include the NULL character.
 * 
 */
extern const unsigned MaxArduDAQmxDevPrefixLength;

/*!
 * \def ArduDAQmxDevPrefixLength
 * \brief Length of the device name prefix - used to isolate device number. DO NOT access this variable directly.
 * Access this variable onlyby using the 'getArduDAQmxPrefixLength()' function.
 * Its maximum value is defined by the 'MaxDevPrefixLength'.
 */
extern unsigned ArduDAQmxDevPrefixLength;

/*!
 * \def StatusModes
 * \brief Enumerates the list of possible status modes of the ArduDAQmx library as set in 'ArduDAQmxStatus'.
 */
typedef enum {
	STATUS_PRECONFIG	= -1,
	STATUS_CONFIG		=  0,
	STATUS_READY		=  1
}StatusModes;

/*!
 * \def IOmode
 * \brief Defines the six types of I/O modes suported by this library.
 */
typedef enum {
	// pin I/O modes
	DIGITAL_IN, DIGITAL_OUT,
	 ANALOG_IN,  ANALOG_OUT,
	// counter I/O modes
	COUNTER_IN, COUNTER_OUT
}IOmode;

/*!
 * \def pin
 * \brief Custom data type to encompass info on pins used by ArduDAQmx.
 * 
 */
typedef struct pin{
	unsigned int	SlotNum;
	unsigned int	PinNum;
	IOmode			pinIOmode;
} pin;

/*!
 * \def DAQmxDevice
 * \brief Custom data type encompasses info on DAQmx devices used by ArduDAQmx.
 * 
 */
typedef struct DAQmxDevice{
	char			DevName[256];
	unsigned int	DevNum;
	int				DevSerial;
	int				isDevSim;
	cLinkedList		*pinList;

} DAQmxDevice;

// library function declarations
	// configuration functions
void enumerateDAQmxDevices(int printFlag);
inline int getArduDAQmxStatus();
inline int printDAQmxStatus();
inline cLinkedList * getDAQmxDeviceList();
inline char * getArduDAQmxPrefix();
inline void setArduDAQmxPrefix(char *newPrefix);
inline unsigned getArduDAQmxDevPrefixLength();

	// initialization and termination functions
int ArduDAQmxInit();
int ArduDAQmxTerminate();

	// mode selection functions
pin* pinMode(unsigned int, unsigned int, IOmode);


	// timing and trigger selection functions


	// I/O Run/Pause/Stop functions


	// read/write functions


#endif // !ARDUDAQMX_H
