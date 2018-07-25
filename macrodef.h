//
// Created by surajcha on 7/24/18.
//

#ifndef CLINKEDLIST_MACRODEF_H
#define CLINKEDLIST_MACRODEF_H

#ifndef NULL
    #define NULL 0L
#endif /* ~NULL */

#ifndef TRUE
    #define FALSE 0
    #define TRUE 1
#endif /* ~TRUE */

#ifdef _WIN32
    #define DIR_SEP '\\'
#else /* ~_WIN32 */
    #define DIR_SEP '/'
#endif /* ~_WIN32 */

#ifndef max
    #define max(A,B) (((A)>(B)) ? (A) : (B))
#endif /* ~max */

#ifndef  min
    #define min(A,B) (((A)>(B)) ? (B) : (A))
#endif /* ~min */

#ifndef round
    #define round(X) (((X) >= 0) ? (int)((X)+0.5) : (int)((X)-0.5))
#endif /* ~round */

#ifndef MAXPATHLENGTH
    #define MAXPATHLENGTH 256
#endif /* ~MAXPATHLENGTH */


#endif //CLINKEDLIST_MACRODEF_H
