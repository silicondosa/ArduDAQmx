//
// Created by surajcha on 7/27/18.
//

#ifndef CLINKEDLIST_DAQWRAPPER_H
#define CLINKEDLIST_DAQWRAPPER_H

#include "cLinkedList.h"
#include "NIDAQmx.h"

typedef enum    taskStatesTAG   {CONFIG, READY, RUNNING, STOPPED} taskStates;
typedef enum    pinTypesTAG     {AI, AO, DI, DO, CTR} pinTypes;

typedef struct  DAQdeviceTAG {
    // Device properties
    unsigned long   pdtNum;
    unsigned int    chassisNum, slotNum;
    char            devAddr[256];
    unsigned int    numAI, numAO, numDI, numDO, numCTR;

    // Task and pin list
    cLinkedList     taskList, usedPinList;
} DAQdevice;

typedef struct  DAQtaskTAG {
    // Task properties
    taskStates      taskState;
    DAQdevice       *homeDevice;

    // Pin list
    cLinkedList     pinList;
} DAQtask;


typedef struct  DAQpinTAG {
    // Pin properties
    pinTypes        pinType;
    char            signalName[32];
    char            pinAddr[256];
    DAQdevice       *homeTask;
    DAQdevice       *homeDevice;

    // Pin linear signal scaling
    double          signalSlope;
    double          signalIntercept;
} DAQpin;


void setPin(DAQpin *, pinTypes);
void * readPin(DAQpin *);
void writePin(DAQpin *, void *);



#endif //CLINKEDLIST_DAQWRAPPER_H
