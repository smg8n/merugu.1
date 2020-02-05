#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <dirent.h>

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

struct queue {
    char *dir;
    struct queue *next;
};
static struct queue *qp;
struct queue *tmp;
static int luser=0, llink=0, lgroup=0, lsize=0;
struct sline {
    char *mode;
    int link;
    char *user;
    char *group;
    int size;
    char *datetime;
    char *file;
};
struct sline *lines;

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

static int convertIndentStringToInteger() {
    if(indentValString != NULL) {
        return atoi(indentValString);
    }

    return -1;
}

void printHelp() {

    printf("Here's a list of options:\n");
    printf("----------\n");
    printf("\"-h\" Get Help.\n");
    printf("\"-I n\" Change indent level to n spaces.\n");
    printf("\"-L\" Follow symbolic links.\n");
    printf("\"-t\" Print file-type information.\n");
    printf("\"-p\" Print permission bits.\n");
    printf("\"-i\" Print number of links to file in inode table.\n");
    printf("\"-u\" Print UID associated with the file.\n");
    printf("\"-g\" Print GID associated with the file.\n");
    printf("\"-s\" Print size of file in bytes.\n");
    printf("\"-d\" Show time of last modification.\n");
    printf("\"-l\" Enable -t -p -i -u -g -s option flags.\n\n");
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
                printHelp();
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


int main(int argc, char* argv[])
{
    setFlags(argc,argv);
    char *root;
    DIR *dp;

    if (1 < argc) {
        root = argv[1];
    } else {
        root = malloc(sizeof(char) * 2);
        strcpy(root, ".");
    }

    // print out
    int cline;
    printf("--- stat info of dir '%s' ---\n", qp->dir);
    for(int i=0; i<cline; i++) {
        printf("%s ", lines[i].mode);
        printf("%*d ", llink+1, lines[i].link);
        printf("%*s ", luser+1, lines[i].user);
        printf("%*s ", lgroup+1, lines[i].group);
        printf("%*d ", lsize+1, lines[i].size);
        printf("%s ", lines[i].datetime);
        printf("%s \n", lines[i].file);
    }
    printf("\n");

    tmp = qp;
    qp = qp->next;
    free(tmp);
    free(lines);
    closedir(dp);

    return 1;
}