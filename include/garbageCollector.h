//
// Created by tyros on 9/4/2018.
//

#ifndef GARBAGECOLLECTOR_H
#define GARBAGECOLLECTOR_H

#include "cLinkedList.h"
#include <stdlib.h>

extern cLinkedList garbagePile;
extern int garbageInitFlag = FALSE;

extern void addToGarbagePile(void *);
extern void* generateGarbageData(int);
extern void purgeGarbagePile();


#endif //GARBAGECOLLECTOR_H
