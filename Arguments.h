
#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

                    /* OPTION FLAG DECLARATIONS */

extern int helpFlag;        // -h
extern int setIndentFlag;   // -I n
extern int symLinkFlag;     // -L
extern int printInfoFlag;   // -t
extern int permissionsFlag; // -p
extern int numLinksFlag;    // -i
extern int uidFlag;         // -u
extern int gidFlag;         // -g
extern int fileSizeFlag;    // -s
extern int lastModTimeFlag; // -d
extern int tpiugsFlag;      // -l (lowercase L)

extern char* indentValString;
extern int convertedIndentVal;
extern char* dirName;

                    /* FUNCTION DECLARATIONS */

//Function initalizes all variables that store user set and parameters that
//pertain to those options to 0 or NULL.
void initializeFlags();

//Function reads user set options from the command line and sets flags and
//flag parameters accordingly.
void setFlags(int argc, char** argv);

//Function prints status of all flags. (Used for debugging)
void printFlags();

#endif
