
#include "Arguments.h"

int helpFlag;
int setIndentFlag;
int symLinkFlag;
int printInfoFlag;
int permissionsFlag;
int numLinksFlag;
int uidFlag;
int gidFlag;
int fileSizeFlag;
int lastModTimeFlag;
int tpiugsFlag;

char* indentValString;
int convertedIndentVal;
char* dirName;

void initializeFlags() {
    
    helpFlag = 0;       // -h
    setIndentFlag = 0;  // -I n
    symLinkFlag = 0;    // -L
    printInfoFlag = 0;  // -t
    permissionsFlag = 0;// -p
    numLinksFlag = 0;   // -i
    uidFlag = 0;        // -u
    gidFlag = 0;        // -g
    fileSizeFlag = 0;   // -s
    lastModTimeFlag = 0;// -d
    tpiugsFlag = 0;     // -l (lowercase L)

    indentValString = NULL;   // -I n
    dirName= NULL;            // [dirname]
}

static void tpiugsEnable() {
    printInfoFlag = 1;
    permissionsFlag = 1;
    numLinksFlag = 1;
    uidFlag = 1;
    gidFlag = 1;
    fileSizeFlag = 1;
}

static void printArgs(int argc, char** argv) {
    int i;
    for(i = 0; i < argc; i++){
        printf("%d: %s\n", i, argv[i]);
    }
}

static int convertIndentStringToInteger() { 
    if(indentValString != NULL) {
        return atoi(indentValString);
    }

    return -1;
}

void setFlags(int argc, char** argv) {

    //Stuff for getopts
    int i;
    int c;
    int opterr = 0;

    //Iterate through users passed parameters and set flags accordingly.
    while((c = getopt(argc, argv, "hI:Ltpiugsdl")) != -1) {
        switch(c) {
            case 'h':
                helpFlag = 1;
                break;
            
            case 'I':
                setIndentFlag = 1;
                indentValString = optarg;
                convertedIndentVal = convertIndentStringToInteger();
                break;
            
            case 'L':
                symLinkFlag = 1;
                break;

            case 't':
                printInfoFlag = 1;
                break;
            
            case 'p':
                permissionsFlag = 1;
                break;

            case 'i':
                numLinksFlag = 1;
                break;

            case 'u':
                uidFlag = 1;
                break;

            case 'g':
                gidFlag = 1;
                break;

            case 's':
                fileSizeFlag = 1;
                break;

            case 'd':
                lastModTimeFlag = 1;
                break;

            case 'l':
                tpiugsFlag = 1;
                tpiugsEnable();
                break;
                
            case '?':
                if(optopt = 'I') {
                    printf("Option -%c requires an argument.\n", optopt);
                    exit(0);
                }
                break;
        }
        
    }

    //Set dirname to directory argument.
    for(i = optind; i < argc; i++) {
        if(argv[i] != NULL)
            dirName = argv[i];
    }

    if(dirName == NULL) {
        dirName = ".";
    }
    //DEBUGprintArgs(argc, argv);
}

void printFlags() {
    
    printf("-h = %d\n", helpFlag);
    printf("-I = %d\t%s\n", setIndentFlag, indentValString); 
    printf("-L = %d\n", symLinkFlag); 
    printf("-t = %d\n", printInfoFlag);
    printf("-p = %d\n", permissionsFlag);
    printf("-i = %d\n", numLinksFlag);
    printf("-u = %d\n", uidFlag);
    printf("-g = %d\n", gidFlag);
    printf("-s = %d\n", fileSizeFlag);
    printf("-d = %d\n", lastModTimeFlag);
    printf("-l = %d\n", tpiugsFlag);
    printf("dirname = %s\n", dirName);
}
