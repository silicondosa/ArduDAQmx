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
#include "macrodef.h"
#include "cLinkedList.h"
#include "testUtils.h"

extern int errNo;

void malformed(char *errSpecs) //printing command syntax errors
{
    fprintf(stderr, "warmup1: Malformed Command. %s\nCommand syntax is 'warmup1 sort [tfile]'\n\tNote that the file input is optional\n", errSpecs);
}

void ferr(char *err) //printing input errors
{
    fprintf(stderr, "warmup1: Input file is incorrectly Formatted: %s\n", err);
}

void serr(char *err) //printing input errors
{
    fprintf(stderr, "warmup1: Sort Error: %s\n", err);
}

int isnum(char c) //what is this character?
{
    switch (c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':	return 1;
            break;
        case '\t':	return 0;
            break;
        case '.':	return 2;
            break;
        default:	return -1;
            break;
    }
}

int istime(char c[], int *index, int length, trans *buf)
{
    int i, o, rhead, flag, limit;
    time_t currtime;
    rhead = *index; flag = 0;
    limit = rhead + 11;
    for (i = rhead; i<limit && i<length && (flag != -1 || flag !=2); i+=1) {
        o = isnum(c[i]);
        if ( o == 1 ) flag = 1;
        else if ( o == 0 && flag == 1 ) {
            flag = 2;
            *index = i;
        } else flag = -1;
        //printf("%c : %d\n",c[i],flag);//debug
    }//end isnum checking loop

    //printf("%c : %d\n",c[i-1],flag);//debug
    if (  flag == -1 || i == length || (c[i-1] != '\t' /*&& i == limit*/) ) return -1;
    long long temp = strtoul( &c[rhead], NULL, 0 );
    //printf("%f\n",temp);//debug
    if ( temp <= 4294967295LL ){
        buf->ut = (unsigned int)temp;

        currtime = 0;
        currtime = time(&currtime);
        //printf("%lld - %lu\n", temp, currtime); //debug
        if (temp > (long)currtime)
            return -1;
        //printf("%lu - %u\n",temp, buf->ut); //debug
        return 0;
    }
    return -1;
}

int isamt(char c[], int *index, int length, trans *obj)
{
    int i, j;
    char wbuf[9];
    i = *index; j = 0;
    if ( isnum(c[i]) == 1 ) {
        wbuf[j] = c[i];
        j = 1;
        //we use counter j as a means to check for amt<10,000,000 also
        for (i += 1; isnum(c[i]) == 1 && j<7 && i<length; i+=1) {
            wbuf[j] = c[i];
            j+=1;
        }
        if ( i == length || c[i] != '.' ) return -1;
        wbuf[j] = ' ';
        wbuf[j+1] = '\0';
        if ( isnum(c[i+1]) == 1 ) obj->d[0] = c[i+1];
        else return -1;
        if ( isnum(c[i+2]) == 1 ) obj->d[1] = c[i+2];
        else return -1;
        obj->d[3] = '\0';
        i+=3;
        if ( i == length || c[i] != '\t' ) return -1;
        *index = i;
        obj->whole = atoi(wbuf);
        //printf("%s.%s - %d.%d - %f\n", wbuf,obj->d, obj->whole,atoi(obj->d), getamt(obj)/100.0); //debug
    } else return -1;
    if ( obj->whole < 0 ) return -1;
    return 0;
}

int getdesc(char c[], int *index, int length, trans *obj)
{
    if ( c[*index] == '\n' || c[*index] == '\0' )
        return -1;
    int i, j, flag;
    char o;
    //printf("c[index] = %c\n",c[*index]); //debug
    i = (*index);
    for (j = 0, flag = 0; i < length && j < 24 && c[i] != '\n'; i+=1, j+=1) {
        o = c[i];
        if ( o == ' ' && flag == 0 ) j--;
        else {
            if ( o == '\t' ) return -1;
            else obj->dsc[j] = o;
            flag = 1;
        }
        //printf("(%d) %s : %c , %c\n",i, obj->dsc, o, c[i]);//debug
    }//end for loop
    if( i==length || flag == 0 )return -1;
    obj->dsc[j] = '\0';
    return 1;
}

/*
void makelist  (cList *list, trans *obj)
{
	trans *newObj = (trans *)malloc(sizeof(trans));
	*newObj = *obj;
	cListAppend(list, newObj);
	//return 0;
}
int insertelem (cList *newlist, cListElem *elem, cListElem *newelem)
{
	if ( (*(trans *)newelem->obj).ut == (*(trans *)elem->obj).ut ) {
		return -1;
	}
	else if ( (*(trans *)newelem->obj).ut < (*(trans *)elem->obj).ut ) {
		cListInsertBefore(newlist, elem->obj, newelem);
		return 1;
	}
	return 0;
}
void sortlist (cList *baselist)
{
	cListElem *c, *d, *n;
	trans t;
	n = cListLast(baselist)->prev;
	for (c = cListFirst(baselist)->next ; c <= n; c++) {
		d = c;

		while ( d->prev != NULL && (*(trans *)d->obj).ut < (*(trans *)d->prev->obj).ut) {
			t						= *(trans *)d->obj;
			*(trans *)d->obj		= *(trans *)d->prev->obj;
			*(trans *)d->prev->obj	= t;

			d = cListPrev(baselist, d);
		}
	}
}*/

/*insertion sort
for (c = 1 ; c <= n - 1; c++) {
	d = c;

	while ( d > 0 && array[d] < array[d-1]) {
		t          = array[d];
		array[d]   = array[d-1];
		array[d-1] = t;

		d--;
	}
}
 */

int buildlist (cLinkedList *list, trans *obj)
{
    int flag = 0;
    cListElem *p = cListFirstElem(list);
    trans *newObj = (trans *)malloc(sizeof(trans));
    *newObj = *obj;
    //printf("%d - %d",newObj->ut, (*(trans *)p->obj).ut); //debug - causes seg fault
    if (cListLength(list) == 0) {
        cListAppend( list, newObj );
        //printf(" : new list\n"); //debug
        flag = 1;
    } else if (cListLength(list) == 1) {
        if ( newObj->ut < (*(trans *)p->obj).ut ) {
            cListInsertBefore(list, newObj, p);
            flag = 1;
        } else if ( newObj->ut > (*(trans *)p->obj).ut ) {
            cListInsertAfter(list, newObj, p);
            flag = 1;
        } else return -1;
    }//1 element condition
    else {//if list > 2
        //list traversal
        //printf(" : old list\n"); //debug
        for (p = cListFirstElem(list); cListNextElem(list, p) != NULL && flag == 0; p = cListNextElem(list, p) ) {

            //object equal check
            //printf("%d - %d",newObj->ut, (*(trans *)p->obj).ut);//debug
            if ( (*(trans *)p->obj).ut == newObj->ut) {
                //printf("equal \n");//debug
                return -1;//ut's equal - invalid entry
            }

                //ready to insert now
            else if ( newObj->ut < (*(trans *)p->obj).ut ) {
                //printf(" : lesser\n");//debug
                cListInsertBefore(list, newObj, p);
                flag = 1;
            } //end insert condition checks
            //printf("loop end\n");//debug
        }//list traversal loop
        //commented to check for insertion error
        if ( flag == 0 ) {
            //printf(" : append\n");//debug
            if (newObj->ut < (*(trans *)p->obj).ut) {
                cListInsertBefore(list, newObj, p);
                flag = 1;
            }else if (newObj->ut > (*(trans *)p->obj).ut) {
                cListAppend(list, newObj);
                flag = 1;
            } else return -1;
        }
        //printf("\n\n"); // debug
    }//end else (non-empty)
    //sortlist(list);
    //printlist(list); //debug
    //printf("\n\n\n"); //debug

    return 0;
}

int destlist (cLinkedList *list)
{
    cListElem *elem;
    for (elem = cListFirstElem(list); elem != NULL; elem = cListNextElem(list, elem))
        free(elem->obj); //free the object from memory
    cListUnlinkAll(list);//unlink and destory the list
    return 0;
}

int getamt (trans *obj)
{
    return ( (obj->whole*100) + atoi(obj->d) );
}

void printlist (cLinkedList *list)
{
    int i;
    time_t currtime;
    char *htime;
    int total, amt;
    cListElem *elem;
    setlocale(LC_NUMERIC, "en_US");
    total = 0; amt = 0;

    /*
     Print Format - Just for reference

     00000000011111111112222222222333333333344444444445555555555666666666677777777778
     12345678901234567890123456789012345678901234567890123456789012345678901234567890

     +-----------------+--------------------------+----------------+----------------+
     |       Date      | Description              |         Amount |        Balance |
     +-----------------+--------------------------+----------------+----------------+
     | Thu Aug 21 2008 | ...                      |      1,723.00  |      1,723.00  |
     | Wed Dec 31 2008 | ...                      | (       45.33) |      1,677.67  |
     | Mon Jul 13 2009 | ...                      |     10,388.07  |     12,065.74  |
     | Sun Jan 10 2010 | ...                      | (      654.32) |     11,411.42  |
     +-----------------+--------------------------+----------------+----------------+
     */

    printf("+-----------------+--------------------------+----------------+----------------+\n");
    printf("|       Date      | Description              |         Amount |        Balance |\n");
    printf("+-----------------+--------------------------+----------------+----------------+\n");
    //let's start printing the list NOW
    for (elem = cListFirstElem(list); elem != NULL; elem = cListNextElem(list, elem)) {
        //this prints the (i)th transaction
        currtime = (*(trans *)elem->obj).ut;
        htime = ctime(&currtime);

        //print Date
        printf("| ");
        for (i =  0; i < 11; i+=1) printf("%c", htime[i]);
        for (i = 20; i < 24; i+=1) printf("%c", htime[i]);
        //printf("%15u",(*(trans *)elem->obj).ut); //debug

        //print description
        printf(" | %-24.24s | ",(*(trans *)elem->obj).dsc);

        //print amount
        amt = getamt( (trans *)elem->obj );
        if( (*(trans *)elem->obj).t == '-' ) printf("(%'12.2f) | ",amt/100.0);
        else printf(" %'12.2f  | ",amt/100.0);

        //print total
        if( (*(trans *)elem->obj).t == '+' ) total += amt;
        else total -= amt;
        if(total < 0 ) {
            if ( (total/100.0) > -10000000 ) printf("(%'12.2f) |\n",(float)fabs((double)(total/100.0)));
            else {
                //printf split to prevent trigraphs
                printf("(?,???,???.??");
                printf(") |\n");
            }
        } else if(total >= 0 ) {
            if ( (total/100.0) <  10000000 ) printf(" %'12.2f  |\n",total/100.0);
            else printf(" ?,???,???.??  |\n");
        }
    }//end list print loop
    printf("+-----------------+--------------------------+----------------+----------------+\n");
}

void closePgm(int errCode, FILE *fp, cLinkedList *list) //close the program
{
    fclose(fp);
    destlist(list);
    free(list);
    if( errCode != 0 ) exit(errCode);
}