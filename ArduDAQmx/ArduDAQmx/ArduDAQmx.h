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
	/*! Library is not ready to run. Configure library, sample clock and pin mode first!*/
	ERROR_NOTREADY			= -7,
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
	/*! Counter I/O mode: COUNTER IN - reads angular position*/
	CTR_ANGLE_IN	= 4,
	/*! Counter I/O mode: COUNTER OUT - writes ticks*/
	CTR_TICK_OUT	= 5
}IOmode;

/*!
 * Possible I/O directions - Input and Output, defined for easy programming. 
 */
typedef enum _IO_DIRECTION {
	INPUT	= 0,
	OUTPUT	= 1
}IO_DIRECTION;

/*!
 * Some default values for NI-DAQmx.
 */
typedef struct _NIdefaults {
	float64 AImin		= -10;
	float64 AImax		= 10;
	int32	NItermConf	= DAQmx_Val_RSE;

	float64 AOmin		= -10;
	float64 AOmax		= 10;	
	int32	NIanlgUnits	= DAQmx_Val_Volts;

	int32	NIdigiGrp	= DAQmx_Val_ChanForAllLines;

	int32	NIctrMode	= DAQmx_Val_X4;
	bool32  ZEN			= 0;
	float64 Zval		= 0.0;
	int32	Zphase		= DAQmx_Val_AHighBHigh;
	int32	NIctrunits	= DAQmx_Val_Degrees;
	float64 angleInit	= 0.0;

	uInt32	encoderPPR	= 2048; // For CUI AMT103 encoder

	int32	plsIdleSt		= DAQmx_Val_Low;
	int32	plsInitDel		= 0;
	int32	plsLoTick		= 1;
	int32	plsHiTick		= 1;

 }NIdefaults;

	// Sample Clock and Trigger selection functions
typedef struct _sampleClock {
	unsigned int	sourceDevNum = 0;
	IOmode			sourceIOmode = INVALID_IO;
	float64			samplingRate = 1000;
	int32			ActiveEdgTrg = DAQmx_Val_Rising;
	int32			NIsampleMode = DAQmx_Val_HWTimedSinglePoint;
	uInt64			numberSample = 1;
	char			sampClkSrcID[256];
	TaskHandle		*sampClkTask = NULL;
} sampleClock;

/*!
 * Structure to hold information regarding the tasks of a device. 
 */
typedef struct _ArduDAQmxTask {
	/*! Device number of the task.*/
	unsigned int	DevNum = 0;
	/*!	The I/O type of the task.*/
	IOmode			taskIOmode = INVALID_IO;
	/*! List of pins associated with the task. Dynamically allocated when task starts, deleted during termination.*/
	cLinkedList		*activePinList;
	/*! Number of active pins associated with the task.*/
	unsigned int	activePinCnt = 0;
	/*! Size of each element of the data array I/O buffer. Set based on I/O type and pin count.*/
	size_t			ioDataSize = 0;
	/*! Buffer of data to write/read from the pins of the task. Created when task starts, removed when task stops.*/
	void			*ioBuffer = NULL;
	/*! NI-DAQmx task handler for the task.*/
	TaskHandle		Handler;
	/*! Sample Clock handler*/
	sampleClock *clockHandler = NULL;
} ArduDAQmxTask;


/*!
 * Custom data type to encompass info on pins used by ArduDAQmx.
 */
typedef struct _pin {
	/*! NI-DAQ device/slot number where the pin lives.*/
	unsigned int	DevNum		= 0;
	/*! Pin number of the pin on the device.*/
	unsigned int	PinNum		= 0;
	/*! Task to which the pin is associated.*/
	ArduDAQmxTask		*pinTask;
	/*!  Set to 1 iff the pin has been assigned to a task.*/
	bool			pinAssignFlag	= 0;
	/*! I/O mode of the pin as defined in ::IOmode.*/
	IOmode			pinIOmode = INVALID_IO;
	/*! Value that is set to or received from the pin.*/
	double			pinValue = 0;
} pin;

/*!
 * Custom data type encompasses info on DAQmx devices used by ArduDAQmx.
 */
typedef struct _DAQmxDevice{
	/*! DAQmx Device name - max 255 characters.*/
	char			DevName[16];
	/*! NI-DAQ device/slot number*/
	unsigned int	DevNum		= 0;
	/*! Serial number of the NI-DAQ device*/
	int				DevSerial	= 0;
	/*! This flag is set when device is simulated.*/
	int				isDevSim	= 0;

	//task list and pin list may not be needed
	/*! Pointer to the array of pins of the device.*/
	pin				*pinList	= NULL;;
	/*! List of tasks associated with the device.*/
	cLinkedList		*taskList	= NULL;


	// Number of NI-DAQmx tasks based on this NI article: https://knowledge.ni.com/KnowledgeArticleDetails?id=kA00Z0000019KWYSA2&l=en-US
		// Task handlers are dynamically initialized in the configure function and cleared in the terminate functiion
	ArduDAQmxTask		AItask;
	ArduDAQmxTask		AOtask;
	ArduDAQmxTask		DItask;
	ArduDAQmxTask		DOtask;
	ArduDAQmxTask		*CTRtask;

	/*
	TaskHandle		  AItaskHandler;
	TaskHandle		  AOtaskHandler;
	TaskHandle		  DItaskHandler;
	TaskHandle		  DOtaskHandler;
	TaskHandle		*CTRtaskHandler;
	*/
	
	// NI-DAQmx pin management
		// The pin lists are dynamically initialized in the configure function and cleared in the terminate function.
	/*!	Number of Analong Input channels available in the device.*/
	unsigned int	numAIch = 0;
	/*! List of AI pins on the device.*/
	pin				*AIpins = NULL;

	/*!	Number of Analong Output channels available in the device.*/
	unsigned int	numAOch = 0;
	/*! List of AO pins on the device.*/
	pin				*AOpins = NULL;

	/*!	Number of Digital Input channels available in the device.*/
	unsigned int	numDIch = 0;
	/*! List of DI pins on the device.*/
	pin				*DIpins = NULL;

	/*!	Number of Digital Output channels available in the device.*/
	unsigned int	numDOch = 0;
	/*! List of DO pins on the device.*/
	pin				*DOpins = NULL;

	/*!	Number of Counter Input channels available in the device.*/
	unsigned int	numCIch = 0;
	/*! List of CI pins on the device.*/
	pin				*CIpins = NULL;

	/*!	Number of Counter Output channels available in the device.*/
	unsigned int	numCOch = 0;
	/*! List of CO pins on the device.*/
	pin				*COpins = NULL;
} DAQmxDevice;

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

// MASTER TASK LIST
extern cLinkedList	*ArduDAQmxTaskList;
extern unsigned long ArduDAQmxTaskCount;

/*!
 * The global sanple clock configuration structure that is used by the ArduDAQmx library
 */
extern sampleClock ArduDAQmxSampleClock;

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
inline unsigned int getArduDAQmxDevPrefixLength();

unsigned int enumerateDAQmxDeviceTerminals(unsigned int deviceNumber);
unsigned int enumerateDAQmxDeviceChannels(unsigned int myDev, IOmode IOtype, unsigned int printFlag);

// Library initialization and termination function declarations
int ArduDAQmxConfigure();
int ArduDAQmxInit(char *devicePrefix);
int ArduDAQmxTerminate();
void ArduDAQmxClearEnumeratedDevices();

	// Get the DAQmx device
//DAQmxDevice * findDAQmxDeviceData(unsigned int deviceNumber);

	// mode selection and timing configuration functions
int pinMode(unsigned int, unsigned int, IOmode, bool pinRst);

inline bool isSampleClock();
inline int32 setTaskClock(ArduDAQmxTask *NItask, sampleClock *sampClk);
int setSampleClock(unsigned int sourceDevNum, IOmode sourceIOmode, unsigned int sourcePinNum, double samplingRate);
inline void setSamplingRate(float64 samplingRate);
void waitSampleClock(float64 waitSeconds);

	// I/O Run/Pause/Stop functions
int ArduDAQmxStart();
int ArduDAQmxStop();

	// read functions
int analogReadPin (unsigned int devNum, unsigned int pinNum, double  readData);
int analogReadTask(unsigned int devNum,                      double *readData);
int analogReadEnum(unsigned int devNum);

int digitalReadPort(unsigned int devNum, unsigned int portNum, uInt32 *readData);

int counterAngleRead(unsigned int devNum, unsigned int ctrNum, float64 readData);

	// write functions
int analogWritePin (unsigned int devNum, unsigned int pinNum, double  writeData);
int analogWriteTask(unsigned int devNum,                      double *writeData);
int analogWriteEnum(unsigned int devNum);

int digitalWritePort(unsigned int devNum, unsigned int portNum, uInt32 *writeData);

int counterTickWrite(unsigned int devNum, unsigned int ctrNum);

#endif // !ARDUDAQMX_H
