//
// Created by tyros on 9/4/2018.
//

#include "../include/garbageCollector.h"
#include <stdlib.h>

extern void addToGarbagePile(void *newElem)
{
    if (garbageInitFlag == FALSE) {
        cListInit(&garbagePile);
        garbageInitFlag = TRUE;
    }
    cListAppend(&garbagePile, newElem);
}

extern void* generateGarbageData(int datSize)
{
    void *tempElem = malloc(datSize);
    addToGarbagePile(tempElem);
    return tempElem;
}

extern void purgeGarbagePile()
{
    int i = 0, pileLen = cListLength(&garbagePile);
    void *tempElem = NULL;
    for (;  i < pileLen ; i++) {
        free( cListFirstData(&garbagePile) );
    }
}