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
 * Maximum number of pins in any given device 
 */
const unsigned int DAQmxMaxPinCount = 32;


/*!
 * Operational status of the ArduDAQmx library. DO NOT access this variable.
 */
extern int ArduDAQmxStatus;

/*!
 * Error code of the ArduDAQmx library. DO NOT access this variable.
 */
extern int ArduDAQmxError;

/*!
 * Error code of the NI-DAQmx library. DO NOT access this variable.
 */
extern long NIDAQmxErrorCode;

/*!
* Default device name prefix as "PXI1Slot".
* DO NOT access this variable directly. Acces this variable only by using the 'getArduDAQmxPrefix()' and 'setArduDAQmxPrefix(char *)' functions
*/
extern const char *DefaultArduDAQmxDevPrefix;

/*!
* Device name prefix. Default value is set as defined in 'DefaultArduDAQmxDevPrefix'.
* DO NOT access this variable directly. Acces this variable only by using the 'getArduDAQmxPrefix()' and 'setArduDAQmxPrefix(char *)' functions.
* It can only be set by calling the 'ArduDAQmxInit' function.
*/
extern char *ArduDAQmxDevPrefix;

/*!
 * The maximum length of the DADmx device string prefix is 8 characters. It does NOT include the NULL character.
 */
extern const unsigned MaxArduDAQmxDevPrefixLength;

/*!
 * The maximum length of the DAQmx device string is 10 characters. It does NOT include the NULL character.
 * 8 characters are for the 'ArduDAQmxDevPrefix', same as 'MaxArduDAQmxDevPrefixLength'.
 * The last 2 characrers are for the device ID - with a maximum value of 99.
 */
extern const unsigned MaxArduDAQmxDevStringLength;

/*!
 * The maximum ID string length permitted by NI-DAQmx is 255 characters. 
 */
extern const unsigned MaxNIstringLength;

/*!
 * Length of the device name prefix - used to isolate device number. DO NOT access this variable directly.
 * Access this variable onlyby using the 'getArduDAQmxPrefixLength()' function.
 * Its maximum value is defined by the 'MaxDevPrefixLength'.
 */
extern unsigned ArduDAQmxDevPrefixLength;

/*!
 * Enumerates the list of possible status modes of the ArduDAQmx library as set in 'ArduDAQmxStatus'.
 */
typedef enum _ArduDAQmxStatusMode {
	/*! ArduDAQmx configuration may be altered only in the preconfigure state.*/
	STATUS_PRECONFIG	= -1,
	/*! States that the ArduDAQmx library has been configured. NI-DAQmx may now be setup.*/
	STATUS_CONFIG		=  0,
	/*! The NI-DAQmx synchronization, clocks and triggers are setup and is ready to run.*/
	STATUS_READY		=  1,
	/*!	The library is in operation and running. Data is being collected using it now.*/
	STATUS_RUN			=  2
}ArduDAQmxStatusMode;

/*!
 * Enumerates the list of error codes of the ArduDAQmx library as set in 'ArduDAQmxErrorCode'.
 */
typedef enum _ArduDAQmxErrorCode {
	/*! A feature or functionality that is unsupported by ArduDAQmx requested.*/
	ERROR_UNSUPPORTED		= -6,
	/*!	An invalid or unsupported I/O type has been selected.*/
	ERROR_INVIO				= -5,
	/*! NI-DAQmx has generated an error. Need to check 'NIDAQmxErrorCode' for details.*/
	ERROR_NIDAQMX			= -4,
	/*! List of NI-DAQmx devices detected by ArduDAQmx library has changed.*/
	ERROR_DEVCHANGE			= -3,
	/*! No NI-DAQmx devices detected by ArduDAQmx library.*/
	ERROR_NODEVICES			= -2,
	/*! Pin and task configuratoin may be altered only in the preconfigure state.*/
	ERROR_NOTCONFIG			= -1,
	/*! States that the library has been configured.*/
	ERROR_NONE				=  0,
}ArduDAQmxErrorCode;


/*!
 * Defines the six types of I/O modes suported by this library.
 */
typedef enum _IOmode{
// Invalid I/O mode
	/*! Pin I/O mode: INVALID IO mode*/
	INVALID_IO		= 32767,

// pin I/O modes
	/*! Pin I/O mode: ANALOG IN*/
	ANALOG_IN		= 0,  
	/*! Pin I/O mode: DIGITAL IN*/
	DIGITAL_IN		= 1,
	/*! Pin I/O mode: ANALOG OUT*/
	ANALOG_OUT		= 2,
	/*! Pin I/O mode: DIGITAL OUT*/
	DIGITAL_OUT		= 3,
	
// counter I/O modes
	/*! Counter I/O mode: COUNTER IN*/
	COUNTER_IN		= 4,
	/*! Counter I/O mode: COUNTER OUT*/
	COUNTER_OUT		= 5
}IOmode;

/*!
 * Custom data type to encompass info on pins used by ArduDAQmx.
 */
typedef struct _pin {
	/*! NI-DAQ device/slot number where the pin lives.*/
	unsigned int	DevNum		= 0;
	/*! Pin number of the pin on the device.*/
	unsigned int	PinNum		= 0;
	/*! Pin active use flag*/
	bool			activeFlag	= 0;
	/*! I/O mode of the pin as defined in ::IOmode.*/
	IOmode			pinIOmode = INVALID_IO;
} pin;

/*!
 * Custom data type encompasses info on DAQmx devices used by ArduDAQmx.
 */
typedef struct _DAQmxDevice{
	/*! DAQmx Device name - max 255 characters.*/
	char			DevName[16];
	/*! NI-DAQ device/slot number*/
	unsigned int	DevNum;
	/*! Serial number of the NI-DAQ device*/
	int				DevSerial;
	/*! This flag is set when device is simulated.*/
	int				isDevSim;

	//task list and pin list may not be needed
	/*! Pointer to the array of pins of the device.*/
	pin				*pinList;
	/*! List of tasks associated with the device.*/
	cLinkedList		*taskList;

	// Number of NI-DAQmx tasks based on this NI article: https://knowledge.ni.com/KnowledgeArticleDetails?id=kA00Z0000019KWYSA2&l=en-US
	
	TaskHandle		  AItaskHandler;
	TaskHandle		  AOtaskHandler;
	TaskHandle		  DItaskHandler;
	TaskHandle		  DOtaskHandler;
	TaskHandle		*CTRtaskHandler;

	/*!	Number of Analong Input channels available in the device.*/
	unsigned int	numAIch;
	/*!	Number of Analong Output channels available in the device.*/
	unsigned int	numAOch;
	/*!	Number of Digital Input channels available in the device.*/
	unsigned int	numDIch;
	/*!	Number of Digital Output channels available in the device.*/
	unsigned int	numDOch;
	/*!	Number of Counter Input channels available in the device.*/
	unsigned int	numCIch;
	/*!	Number of Counter Output channels available in the device.*/
	unsigned int	numCOch;
} DAQmxDevice;

typedef struct _DAQmxTask {
	/*! Device number of the task.*/
	unsigned int	DevNum;
	/*! Pointer to the device object of the task.*/
	DAQmxDevice		*myDev;
	/*!	The I/O type of the task.*/
	IOmode			taskIOmode;
	/*! List of pins associated with the task.*/
	pin				pinList[DAQmxMaxPinCount];
} DAQmxTask;


// MASTER ARRAY OF DEVICES
/*!
 * Pointer to master array of available NI-DAQmx compatible devices. DO NOT access this variable directly.
 * The list of available NI-DAQmx devices can be printed using 'enumerateDAQmxDevices(1)'.
 * Get the pointer to tis list using 'getArduDAQmxDeviceList()'. Calling 'printDAQmxStatus' will print the meaning of this variable.
 */
extern DAQmxDevice	*ArduDAQmxDevList;

/*!
 * Number of available DAQmx devices detected by the library as in the 'ArduDAQmxDevList' list pointer.
 */
extern unsigned long ArduDAQmxDevCount;

/*!
 * Highest device number of all NI-DAQmx devices detected by the library as in the 'ArduDAQmxDevList' list pointer.
 */
extern unsigned long ArduDAQmxDevMaxNum;


// TEMPORARY LINKED LIST OF DEVICES
/*!
 * Linked list of available DAQmx devices as obtained by the 'enumerateDAQmxDevices' function.
 * The list of available NI-DAQmx devices can be printed using 'enumerateDAQmxDevices(1)'.
 */
extern cLinkedList *DAQmxEnumeratedDevList;

/*!
 * Number of available DAQmx devices enumerated by the 'enumerateDAQmxDevices' function.
 */
extern unsigned long DAQmxEnumeratedDevCount;

/*!
 * Highest device number of all NI-DAQmx devices enumerated by the 'enumerateDAQmxDevices' function.
 */
extern unsigned long DAQmxEnumeratedDevMaxNum;



// Library support function declarations
char* dev2string(char *strBuf, unsigned int devNum);
char* pin2string(char *strbuf, unsigned int devNum, IOmode ioMode, unsigned int pinNum);
inline int32 DAQmxErrChk(int32 NIerrCode);

// Library configuration function declarations
//void deleteEnumeratedDevices();
void enumerateDAQmxDevices(int printFlag);
inline int getArduDAQmxStatus();
inline int getArduDAQmxLastError();
inline int setArduDAQmxLastError(ArduDAQmxErrorCode newErrorCode, unsigned printErrorMsgFlag);
inline int printArduDAQmxStatus();
inline int printArduDAQmxLastError();
inline DAQmxDevice * getDAQmxDeviceList();
inline unsigned long getDAQmxDeviceCount();
inline char * getArduDAQmxPrefix();
inline unsigned getArduDAQmxDevPrefixLength();

unsigned int enumerateDAQmxDeviceTerminals(unsigned int deviceNumber);
unsigned int enumerateDAQmxDeviceChannels(unsigned int myDev, IOmode IOtype, unsigned int printFlag);

// Library initialization and termination function declarations
int ArduDAQmxConfigure();
int ArduDAQmxInit(char *devicePrefix);
int ArduDAQmxTerminate();
void ArduDAQmxClearEnumeratedDevices();

	// Get the DAQmx device
//DAQmxDevice * findDAQmxDeviceData(unsigned int deviceNumber);

	// mode selection functions
pin* pinMode(unsigned int, unsigned int, IOmode);


	// Sample Clock and Trigger selection functions


	// I/O Run/Pause/Stop functions


	// read/write functions


#endif // !ARDUDAQMX_H
