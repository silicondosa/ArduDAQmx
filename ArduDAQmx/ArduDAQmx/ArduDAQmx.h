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
 * Operational status of the ArduDAQmx library. DO NOT access this variable.
 */
extern int ArduDAQmxStatus;

/*!
 * Master list of available NI-DAQmx compatible devices. DO NOT access this variable directly.
 * The list of available NI-DAQmx devices can be printed using 'enumerateDAQmxDevices(1)'.
 * Get the pointer to tis list using 'getArduDAQmxDeviceList()'. Calling 'printDAQmxStatus' will print the meaning of this variable.
 */
extern cLinkedList *ArduDAQmxDevList;

/*!
* Device name prefix. Default value is "PXI1Slot" but can be changed.
* DO NOT access this variable directly. Acces this variable only by using the 'getArduDAQmxPrefix()' and 'setArduDAQmxPrefix(char *)' functions
*/
extern char *ArduDAQmxDevPrefix;

/*!
 * The maximum length of the DADmx device string is 8 characters. It does NOT include the NULL character.
 */
extern const unsigned MaxArduDAQmxDevPrefixLength;

/*!
 * Length of the device name prefix - used to isolate device number. DO NOT access this variable directly.
 * Access this variable onlyby using the 'getArduDAQmxPrefixLength()' function.
 * Its maximum value is defined by the 'MaxDevPrefixLength'.
 */
extern unsigned ArduDAQmxDevPrefixLength;

/*!
 * Enumerates the list of possible status modes of the ArduDAQmx library as set in 'ArduDAQmxStatus'.
 */
typedef enum _StatusMode {
	/*! Pin and task configuratoin may be altered only in the preconfigure state.*/
	STATUS_PRECONFIG	= -1,
	/*! States that the library has been configured.*/
	STATUS_CONFIG		=  0,
	/*! Sets the library as ready to use.*/
	STATUS_READY		=  1 
}StatusMode;

/*!
 * Defines the six types of I/O modes suported by this library.
 */
typedef enum _IOmode{
// pin I/O modes
	/*! Pin I/O mode: DIGITAL IN*/
	DIGITAL_IN, 
	/*! Pin I/O mode: DIGITAL OUT*/
	DIGITAL_OUT, 
	/*! Pin I/O mode: ANALOG IN*/
	ANALOG_IN,  
	/*! Pin I/O mode: ANALOG OUT*/
	ANALOG_OUT, 

// counter I/O modes
	/*! Counter I/O mode: COUNTER IN*/
	COUNTER_IN,  
	/*! Counter I/O mode: COUNTER OUT*/
	COUNTER_OUT  
}IOmode;

/*!
 * Custom data type to encompass info on pins used by ArduDAQmx.
 */
typedef struct _pin{
	/*! NI-DAQ device/slot number where the pin lives.*/
	unsigned int	SlotNum;
	/*! Pin number of the pin on the device.*/
	unsigned int	PinNum;
	/*! I/O mode of the pin as defined in ::IOmode.*/
	IOmode			pinIOmode;
} pin;

/*!
 * Custom data type encompasses info on DAQmx devices used by ArduDAQmx.
 */
typedef struct _DAQmxDevice{
	/*! DAQmx Device name - max 255 characters.*/
	char			DevName[256];
	/*! NI-DAQ device/slot number*/
	unsigned int	DevNum;
	/*! Serial number of the NI-DAQ device*/
	int				DevSerial;
	/*! This flag is set when device is simulated.*/
	int				isDevSim;
	/*! List of pins of the device used by ArduDAQmx.*/
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
