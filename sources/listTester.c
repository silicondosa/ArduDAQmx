//
// Created by surajcha on 7/24/18.
//

#include "testUtils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "cLinkedList.h"

int alt_main(int argc, const char * argv[])
{
    int				length, bufIndex, errorCode;
    struct stat		errStatus;
    unsigned long	temp;
    FILE			*fp = NULL;
    cLinkedList		*list = (cList *)malloc(sizeof(cLinkedList));
    cListInit(list);
    char			filebuf[1024];
    trans			buf;

    bufIndex = 0;

    if ( argc == 2 || argc == 3 ) {//argument count matches
        if ( strcmp("sort", argv[1]) != 0 ) {//check sort arg
            malformed("");
            closePgm(1, fp, list);
        }

            //-------------------------
            //sort argument received!!!
            //-------------------------
        else {
            //select input vector based on last argument.
            if ( argc == 2 ) {
                fp = stdin;
            } else {
                //check file i/o issues
                stat(argv[2], &errStatus);
                fp = fopen(argv[2], "r");
                if ( fp == NULL ) {
                    errorCode = errno;
                    perror("warmup1: File I/O error");
                    if (argv[2][0] == '-') fprintf(stderr, "It may also be a malformed command\n");
                    //fprintf(stderr, "Error Code (%d)\n",errorCode); //debug
                    closePgm(-1, fp, list);
                } else if(S_ISDIR(errStatus.st_mode)) {
                    fprintf(stderr, "warmup1: File I/O error: This is directory and not a file.\n");
                    closePgm(-1, fp, list);
                }
            }

            //begin file ops
            while( fgets(filebuf, 1025, fp)!= NULL )
            {
                length =  (int)strlen(filebuf);
                //transaction type check
                if ( filebuf[length-1] != '\n' ) {
                    ferr("Line length exceeds 1,024 characters.");
                    closePgm(1, fp, list);
                } else if( filebuf[0] != '+' && filebuf[0] != '-' ) {
                    ferr("A Valid transaction type has not specified.");
                    closePgm(1, fp, list);
                } else { // load "transaction type" and move on
                    buf.t = filebuf[0];
                    //check \t and then check unix time, then \t and amt, finally \t and descr[25] format with ... when formatting
                }

                //transaction time check
                if ( filebuf[1] == '\t' ) {
                    //call istime here
                    bufIndex = 2;
                    //printf("%s\n",filebuf); //debug
                    temp = istime(filebuf, &bufIndex, length, &buf);
                    //bufIndex+=1;
                    if ( temp == -1 ) {
                        ferr("Invalid transaction time.");
                        closePgm(1, fp, list);
                    }
                } else {
                    ferr("Tab spacing incorrect.");
                    closePgm(1, fp, list);
                }

                //transaction amount check
                if ( filebuf[bufIndex] == '\t' ) {
                    bufIndex+=1;
                    temp = isamt(filebuf, &bufIndex, length, &buf);
                    if ( temp != 0 ) {
                        ferr("Invalid transaction amount.");
                        closePgm(1, fp, list);
                    }
                } else {
                    ferr("Tab spacing incorrect.");
                    closePgm(1, fp, list);
                }

                //description check
                if ( filebuf[bufIndex] == '\t' ) {
                    bufIndex+=1;
                    temp = getdesc(filebuf, &bufIndex, length, &buf); //rework it
                    if ( temp== -1 ) {
                        ferr("Invalid Description");
                        closePgm(1, fp, list);
                    }
                    //file parsing done!
                } else {
                    ferr("Tab spacing incorrect.");
                    closePgm(1, fp, list);
                }

                //Now build and sort the list
                temp = buildlist(list, &buf);
                if (temp == -1) {
                    serr("Invalid Transaction Entry");
                    closePgm(1, fp, list);
                }
            }//end of file ops while loop

            //finally, we get to print the list
            printlist(list);

        }//end sort received

    } else { //argument count doesn't match
        malformed("");
        closePgm(1, fp, list);
    }//end else

    closePgm(0, fp, list);
    return 0;
}//end main function
