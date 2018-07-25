//
// Created by surajcha on 7/24/18.
//

#ifndef CLINKEDLIST_TESTUTILS_H
#define CLINKEDLIST_TESTUTILS_H

#include <stdio.h>
#include "cLinkedList.h"

typedef struct trans {
    char            t;			//transaction type
    unsigned int    ut;			//unix time
    int             whole;		//transaction amount
    char            d[4];		//decimal part of transaction
    char            dsc[25];	//transaction decription
} trans;

int isnum       (char);
int istime      (char *, int *, int, trans *);
int isamt       (char *, int *, int, trans *);
int getdesc     (char *, int *, int, trans *);
int buildlist   (cLinkedList *, trans *);
int destlist    (cLinkedList *);
int getamt      (trans *);
void printlist  (cLinkedList *);
void malformed  (char *);
void closePgm   (int, FILE *, cLinkedList *);
void ferr       (char *);
void serr       (char *);
void sortlist   (cLinkedList *);

#endif //CLINKEDLIST_TESTUTILS_H
