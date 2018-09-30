//
// Created by surajcha on 7/24/18.
//

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <locale.h>
#include "../include/macrodef.h"
#include "../include/cLinkedList.h"
#include "../include/listTestUtils.h"

typedef int dataFormat;

void printList(cLinkedList *myList)
{
    int i = 0, listLength = cListLength(myList);
    cListElem *currElem;
    for (i = 0, currElem = cListFirstElem(myList); i < listLength; ++i, currElem = cListNextElem(myList, currElem) ) {
        printf("%d, ", *(dataFormat *)(currElem->obj) );
    }
    printf("\n");
}

int listTest()
{
    int dataLength = 10, i = 0, j = 0;
    dataFormat *data = (dataFormat *) malloc(dataLength*sizeof(dataFormat));
    for (i = 0; i < dataLength; ++i) {
        data[i] = i+1;
    }
    printf("Length of data: %d\n", dataLength);

    cLinkedList *dataList = (cList *)malloc(sizeof(cList));
    cListInit  ( dataList);
    //getchar();
    for (i = 0; i < dataLength; ++i) {
        printf("%d, ", data[i]);
        cListAppend(dataList, (void *) &(data[i]) );
    }

    // test list status functions
    printf("\n\nData ingested into a linked list. List length = %ld\n", cListLength(dataList));
    char yes[] = "YES!";
    char  no[] = "NO!";
    printf("Is the list empty? - %s\n", cListEmpty(dataList)==TRUE?yes:no);

    // test list read functions
    printf("First Data: %d\n", *((dataFormat *)cListFirstData(dataList)) );
        printf("\tNext Data: %d\n", *((dataFormat *)cListNextData(dataList, &(data[0]) )) );
        //printf("\tPrev Data: %d\n", *((dataFormat *)cListPrevData(dataList, &(data[0]) )) );
    printf("Last Data: %d\n", *((dataFormat *)cListLastData(dataList)) );
        //printf("\tNext Data: %d\n", *((dataFormat *)cListNextData(dataList, &(data[dataLength-1]) )) );
        printf("\tPrev Data: %d\n", *((dataFormat *)cListPrevData(dataList, &(data[dataLength-1]) )) );

    // test list creation functions (cListInit, cListAppend already tested when creating the linked list)
    dataFormat *afterData = (dataFormat *)malloc(sizeof(dataFormat));
    *afterData = -1;
    dataFormat *insertLocData = &(data[4]);
    cListInsertAfter(dataList, (void *) afterData, cListFindElem(dataList,  (void *)insertLocData) );
    dataLength++;
    printf("Inserting %d after %d: ", *afterData, *insertLocData);
    printList(dataList);

    dataFormat *prependData = (dataFormat *)malloc(sizeof(dataFormat));
    *prependData = 0;
    cListPrepend(dataList, (void *) prependData);
    dataLength++;
    printf("List after prepending %d: ", *prependData);
    printList(dataList);

    dataFormat *beforeData = (dataFormat *)malloc(sizeof(dataFormat));
    *beforeData = -2;
    insertLocData = &(data[8]);
    cListInsertBefore(dataList, (void *) beforeData, cListFindElem(dataList,  (void *)insertLocData) );
    dataLength++;
    printf("Inserting %d before %d: ", *beforeData, *insertLocData);
    printList(dataList);


    // memory consolidation, test list unlink functions
    cListUnlinkData(dataList, (void *)afterData); dataLength--;
    cListUnlinkData(dataList, (void *)prependData); dataLength--;
    cListUnlinkData(dataList, (void *)beforeData); dataLength--;

    free(afterData);
    free(prependData);
    free(beforeData);

    cListUnlinkAll(dataList);
    cListUnlinkData( dataList, &(data[0]) ); //test spurious unlink
    free(dataList);
    free(data);
}

