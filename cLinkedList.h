//
// Created by Suraj Chakravarthi Raja.
//

#ifndef CLINKEDLIST_H
#define CLINKEDLIST_H

#ifdef _WIN32
    #include <cstddef>
    #include <cstdlib>
#else
    #include <stddef.h>
    #include <stdlib.h>
#endif


typedef struct cListElemTAG {
    void    *obj;
    struct  cListElemTAG *nextElem;
    struct  cListElemTAG *prevElem;
} cListElem;

typedef struct cLinkedListTAG {
    long        num_elem;
    cListElem   anchor;
} cLinkedList;

// Linked list status functions
extern long cListLength             (cLinkedList *);
extern int  cListEmpty              (cLinkedList *);

// Linked list read functions
extern cListElem * cListFirstElem   (cLinkedList *);
extern cListElem * cListLastElem    (cLinkedList *);
extern cListElem * cListNextElem    (cLinkedList *, cListElem *);
extern cListElem * cListPrevElem    (cLinkedList *, cListElem *);

extern cListElem * cListFindElem    (cLinkedList *, void *);

extern void * cListFirstData   (cLinkedList *);
extern void * cListLastData    (cLinkedList *);
extern void * cListNextData    (cLinkedList *, void *);
extern void * cListPrevData    (cLinkedList *, void *);

// Linked list creation functions
extern int cListInit                (cLinkedList *);

extern int cListInsertAfter     (cLinkedList *, void *, cListElem *);
extern int cListInsertBefore    (cLinkedList *, void *, cListElem *);
extern int cListAppend          (cLinkedList *, void *);
extern int cListPrepend         (cLinkedList *, void *);

// Linked list unlink functions
extern void cListUnlinkElem         (cLinkedList *, cListElem *);

extern void cListUnlinkAll          (cLinkedList *);

extern void cListUnlinkData         (cLinkedList *, void *);

#endif //CLINKEDLIST_H
