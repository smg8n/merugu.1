
#include "Print.h"

#define MAX_ENTRY_SIZE 1024
char currentEntry[MAX_ENTRY_SIZE] = "";

static void concatToCurrentEntry(int* ip, const char* s){
    int startIndex = *ip;
    for(;*ip < startIndex + strlen(s); ++*ip) {
        currentEntry[*ip] = s[*ip - startIndex];
    }
    currentEntry[*ip + 1] = '\0';
}

static char* fileSizeStringGenerator(const int sizeInBytes) {
    
    char* result = (char*) malloc(1024);

    //GB
    if(sizeInBytes / 1073741824) {
        sprintf(result, "%dG", sizeInBytes / 1073741824);
        return result;
    }

    //MB
    if(sizeInBytes / 1048576 > 0) {
        sprintf(result, "%dM", sizeInBytes / 1048576);
        return result;
    }

    //KB
    if(sizeInBytes / 1024 > 0) {
        sprintf(result, "%dK", sizeInBytes / 1024);
        return result;
    }

    // < 1 KB
    sprintf(result, "%d", sizeInBytes);

    return result;
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

void buildEntryString(const char* path, const char* name, int indent) {

    //Ignore current and parent directory.
    if(path == "." || path == "..")
        return;

    //Index to be passed to concatToCurrentEntry func.
    int i;

    //Get the current files stat struct.
    struct stat fileStat;
    if(lstat(path, &fileStat) == -1) {
        perror("dt: error: ");
        exit(EXIT_FAILURE);
    }    

    //Add indent.
    for(i = 0; i < indent; ++i) {
        currentEntry[i] = ' ';
    }


    //Append File name.
    for(;i < indent + strlen(name); ++i){
        currentEntry[i] = name[i - indent];
    }
    currentEntry[i] = '\0';

                            /* Append File Info */

    //Follow symlinks if file is a sym link.
    if((fileStat.st_mode & S_IFMT) == S_IFLNK) {
        char target_path[256];
        const char* link_path = name;

        /* Attempt to read the target of the symbolic link. */
        int len = readlink (link_path, target_path, sizeof (target_path));

        if (len == -1) {
            perror ("readlink");
        }
        else {
            /* NUL-terminate the target path. */
            target_path[len] = '\0';

            /* Print it. */
            concatToCurrentEntry(&i, "-->slink:");
            concatToCurrentEntry(&i, target_path);
        }
    }

    //Add spaces between file name info.
    int j = i;
    for(; j < 50; ++j) {
        concatToCurrentEntry(&i, " ");
    }

    //Permission bits.
    if(permissionsFlag) {
        S_ISDIR(fileStat.st_mode) ? concatToCurrentEntry(&i, "d") : concatToCurrentEntry(&i, "-");
        fileStat.st_mode & S_IRUSR ? concatToCurrentEntry(&i, "r") : concatToCurrentEntry(&i, "-");
        fileStat.st_mode & S_IWUSR ? concatToCurrentEntry(&i, "w") : concatToCurrentEntry(&i, "-");
        fileStat.st_mode & S_IXUSR ? concatToCurrentEntry(&i, "x") : concatToCurrentEntry(&i, "-");
        fileStat.st_mode & S_IRGRP ? concatToCurrentEntry(&i, "r") : concatToCurrentEntry(&i, "-");
        fileStat.st_mode & S_IWGRP ? concatToCurrentEntry(&i, "w") : concatToCurrentEntry(&i, "-");
        fileStat.st_mode & S_IXGRP ? concatToCurrentEntry(&i, "x") : concatToCurrentEntry(&i, "-");
        fileStat.st_mode & S_IROTH ? concatToCurrentEntry(&i, "r") : concatToCurrentEntry(&i, "-");
        fileStat.st_mode & S_IWOTH ? concatToCurrentEntry(&i, "w") : concatToCurrentEntry(&i, "-");
        fileStat.st_mode & S_IXOTH ? concatToCurrentEntry(&i, "x") : concatToCurrentEntry(&i, "-");

        concatToCurrentEntry(&i, "\t");
    }


    //File type.
    if(printInfoFlag) {
        switch(fileStat.st_mode & S_IFMT) {
            case S_IFREG:
                concatToCurrentEntry(&i, "RegularFile");
                break;

            case S_IFDIR:
                concatToCurrentEntry(&i, "Directory");
                break;

            case S_IFBLK:
                concatToCurrentEntry(&i, "Block");
                break;

            case S_IFCHR:
                concatToCurrentEntry(&i, "CharDevice");
                break;

            case S_IFIFO:
                concatToCurrentEntry(&i, "FIFOpipe");
                break;

            case S_IFLNK:
                concatToCurrentEntry(&i, "SymLink\t");
                break;

            case S_IFSOCK:
                concatToCurrentEntry(&i, "Socket");
                break;

            default:
                concatToCurrentEntry(&i, "Unknown");
                break;
        }

        concatToCurrentEntry(&i, "\t");
    }

    //Number of links.
    if(numLinksFlag) {
        char temp[255];

        sprintf(temp, "%ld", fileStat.st_nlink);
        concatToCurrentEntry(&i, temp);

        concatToCurrentEntry(&i, "\t");
    }

    //UID.
    if(uidFlag) {
        struct passwd* userInfo;
        userInfo = getpwuid(fileStat.st_uid);
        concatToCurrentEntry(&i, userInfo->pw_name);

        concatToCurrentEntry(&i, "\t");
    }

    //GID.
    if(gidFlag) {
        struct group* groupInfo;
        groupInfo = getgrgid(fileStat.st_gid);
        concatToCurrentEntry(&i, groupInfo->gr_name);

        concatToCurrentEntry(&i, "\t");
    }

    //File size.
    if(fileSizeFlag) {
        concatToCurrentEntry(&i, fileSizeStringGenerator(fileStat.st_size));

        concatToCurrentEntry(&i, "\t");
    }

    //Last Modification Time.
    if(lastModTimeFlag) {
        int j;
        char* lastModTime = (char*) malloc(255);
        lastModTime = ctime(&fileStat.st_mtime);

        //Remove the pointlessly forced \n character from the end of the 
        //returned string from ctime.
        for(j = 0; j < strlen(lastModTime); ++j) {
            if(lastModTime[j] == '\n')
                 lastModTime[j] = '\0';
        }

        concatToCurrentEntry(&i, lastModTime);

        concatToCurrentEntry(&i, "\t");

        //Won't let me free(lastModTime) here for some reason?
    }
}

void printEntry() {

    printf("%s\n", currentEntry);
    
    //Reset currentEntry.
    int i;
    for(i = 0; i < MAX_ENTRY_SIZE; ++i)
        currentEntry[i] = 0;
}

