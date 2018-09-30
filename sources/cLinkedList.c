//
// Created by Suraj Chakravarthi Raja.
//

#include "../include/cLinkedList.h"
#include "../include/macrodef.h"
#include <stdio.h>

#ifdef _WIN32
    #include <cstddef>
    #include <cstdlib>
#else
    #include <stddef.h>
    #include <stdlib.h>
#endif


/********************************/
/* Linked list status functions */
/********************************/

extern long cListLength (cLinkedList *myList)
{
    return myList->num_elem;
}

extern int cListEmpty (cLinkedList *myList)
{
    if (myList->num_elem > 0) {
        return FALSE;
    }
    return TRUE;
}


/******************************/
/* Linked list read functions */
/******************************/

extern cListElem * cListFirstElem (cLinkedList *myList)
{
    return myList->anchor.nextElem;
}

extern cListElem * cListLastElem (cLinkedList *myList)
{
    return myList->anchor.prevElem;
}

extern cListElem * cListNextElem (cLinkedList *myList, cListElem *myElem)
{
    if ((myList->num_elem == 0) || (myList->anchor.prevElem == myElem)) {
        return NULL;
    }
    return myElem->nextElem;
}

extern cListElem * cListPrevElem (cLinkedList *myList, cListElem *myElem)
{
    if ((myList->num_elem == 0) || (myList->anchor.nextElem == myElem)) {
        return NULL;
    }
    return myElem->prevElem;
}

extern cListElem * cListFindElem (cLinkedList *myList, void *myData)
{
    cListElem *myElem = NULL;
    for (myElem = cListFirstElem(myList); myElem != NULL ; myElem = cListNextElem(myList, myElem)) {
        if (myElem->obj == myData) {
            return myElem;
        }
    }
    return myElem;
}

extern void * cListFindData (cLinkedList *myList, void *myData)
{
    return (cListFindElem(myList, myData))->obj;
}

extern void * cListFirstData (cLinkedList *myList)
{
    return (cListFirstElem(myList))->obj;
}

extern void * cListLastData (cLinkedList *myList)
{
    return (cListLastElem(myList))->obj;
}

extern void * cListNextData (cLinkedList *myList, void *myData)
{
    return cListNextElem(myList, cListFindElem(myList, myData))->obj;
}

extern void * cListPrevData (cLinkedList *myList, void *myData)
{
    return cListPrevElem(myList, cListFindElem(myList, myData))->obj;
}


/**********************************/
/* Linked list creation functions */
/**********************************/
extern int cListInit (cLinkedList *myList)
{
    if (myList == NULL) {
        return 0;
    }

    myList->num_elem        = 0;

    myList->anchor.prevElem = NULL;
    myList->anchor.nextElem = NULL;
    myList->anchor.obj      = NULL;

    return 1;
}

extern int cListInsertAfter (cLinkedList *myList, void *newData, cListElem *elem)
{
    cListElem *newElem = (cListElem *)malloc(sizeof(cListElem));
    if (cListEmpty(myList)) {
        myList->anchor.prevElem = myList->anchor.nextElem = newElem;
        newElem->prevElem       = newElem->nextElem       = &(myList->anchor);
    } else {
        newElem->prevElem           = elem;
        newElem->nextElem           = elem->nextElem;
        (elem->nextElem)->prevElem  = newElem;
         elem->nextElem             = newElem;
    }
    newElem->obj      = newData;
    myList->num_elem += 1;
    return 1;
}

extern int cListInsertBefore (cLinkedList *myList, void *newData, cListElem *elem)
{
    cListElem *newElem = (cListElem *)malloc(sizeof(cListElem));
    if (cListEmpty(myList)) {
         myList->anchor.prevElem =  myList->anchor.nextElem = newElem;
        newElem->prevElem        = newElem->nextElem        = &(myList->anchor);
    } else {
        newElem->prevElem           = elem->prevElem;
        newElem->nextElem           = elem;
        (elem->prevElem)->nextElem  = newElem;
        elem->prevElem              = newElem;
    }
    newElem->obj      = newData;
    myList->num_elem += 1;
    return 1;
}

extern int cListAppend (cLinkedList *myList, void *newData) {
    return cListInsertAfter(myList, newData, myList->anchor.prevElem);
}

extern int cListPrepend (cLinkedList *myList, void *newData) {
    return cListInsertBefore(myList, newData, myList->anchor.nextElem);
}

/********************************/
/* Linked list unlink functions */
/********************************/

extern  void cListUnlinkElem (cLinkedList *myList, cListElem *delElem)
{
    if (myList->num_elem > 0 && delElem != NULL) {
        (delElem->prevElem)->nextElem = delElem->nextElem;
        (delElem->nextElem)->prevElem = delElem->prevElem;
        myList->num_elem -= 1;

        free(delElem);
        delElem = NULL;
    } else if (delElem == NULL) {
        fprintf(ERRSTREAM, "cListUnlinkElem (cLinkedList): WARNING - Quashed attempt to de-reference a NULL pointer!\n");
    } else {
        fprintf(ERRSTREAM, "cListUnlinkElem (cLinkedList): WARNING - Quashed attempt to access empty linked list.\n");
    }

    if (cListEmpty(myList)) {
        cListInit(myList);
    }
}

extern void cListUnlinkAll (cLinkedList *myList)
{
    cListElem *elem = NULL;

    for (elem = cListFirstElem(myList); elem != NULL; elem = cListNextElem(myList, elem)) {
        free(elem);
    }
    cListInit(myList);
}

extern void cListUnlinkData (cLinkedList *delList, void *delData)
{
    cListUnlinkElem(delList, cListFindElem(delList, delData));
}