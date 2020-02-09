#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#define msize 1024
#define bsize 256

struct queue
{
    char* name;
    struct queue* next;
};


int opth; // help
int optL; // symbolic links
int optt; // print info
int optp; // print permission bits
int opti; // print number of links
int optu; // print file UID
int optg; // print file GID
int opts; // print file size
int optd; // show time of last mod
int optl; // print tpiugs info

char* spaces;
char* directory_name;
char* queuedlist;
char actfsys[msize] = "";
struct queue* rear;


void initialiseall();//Initialization of all global variables and structures
void enqueue(char* name);//adding values to queue structure
int dequeue(char* name);//deleting values to queue structure
void firstinline(void);//check for remaining queues
void *getcwdir(void);//returns current working directory
void bfs(char* node, char* fname, int indent, char* data);//for breath first search
int isdir(char* path);//check if its a directory
void concatinate(int* marker, char* datum);//appends all directory file information
void printinfo(char* path, char* fname, int indent, char* data);//outputs all details of files and directory
void printentry();//adjust print information
void filesystem(char* path, int indent, char* root);//walks through the directory

void help();//prints help
void printerror(char* massage);//prints errors

void initialiseall()
{
    opth = 0;
    optL = 0;
    optt = 0;
    optp = 0;
    opti = 0;
    optu = 0;
    optg = 0;
    opts = 0;
    optd = 0;
    optl = 0;
    spaces = NULL;
    directory_name = NULL;
    queuedlist = NULL;
    rear = NULL;
}


/* Print Help */

void help()
{
    printf("\n|HELP|MENU|");
    printf("\t-h : display help menu\n");
    printf("\t-L : follow symbolic links, if any. default does not follow symbolic links\n");
    printf("\t-t : print information on file type\n");
    printf("\t-p : print permission bits\n");
    printf("\t-i : print the number of links to file in inode table\n");
    printf("\t-u : print the UID associated with the file\n");
    printf("\t-g : print the GID associated with the file\n");
    printf("\t-s : print the size of file in bytes\n");
    printf("\t-d : show the time of last modification\n");
    printf("\t-l : print information on file as if options t p i u g s are all specified\n");
}


void enqueue(char* name)
{
    struct queue* ptr;
    char* cptr;

    if((ptr = (struct queue *) malloc (sizeof (struct queue))) == NULL)
	{
        printerror("malloc");
	}

    if((cptr = (char *) malloc (strlen (name) + 1)) == NULL)
	{
		printerror("malloc");
	}

    strcpy(cptr, name);
    ptr -> name = cptr;

    if(rear == NULL)
	{
        ptr -> next = ptr;
    }
    else {
        ptr -> next = rear -> next;
        rear -> next = ptr;
    }
    rear = ptr;
}

int dequeue(char* name)
{
    struct queue *ptr;
    char *cptr;

    if(!rear)
    {
        fprintf(stderr, "dt: warning: queue is empty.\n");
        return -1;
    }

    ptr = rear -> next;
    cptr = ptr -> name;

    if(ptr == rear)
	{
        rear = NULL;
	} else {
        rear -> next = ptr -> next;
	}

    free(ptr);
    strcpy(name, cptr);
    free(cptr);
    return 0;
}

void firstinline(void)
{
    struct queue *ptr;

    if(!rear)
    {
        fprintf(stderr, "dt: warning: queue is empty\n");
        return;
    }

    ptr = rear -> next;

    /* printf("%s\n", ptr -> name); */
    queuedlist = ptr -> name;
}

void printerror(char* massage)
{
    perror(massage);
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
    initialiseall();
    int input;

    while( (input = getopt(argc,argv,"hI:Ltpiugsdl")) != -1 )
    {
        switch(input)
        {
            case 'h':
                help();
                return 0;
            case 'L':
                optL = 1;
                break;
            case 't':
                optt = 1;
                break;
            case 'p':
                optp = 1;
                break;
            case 'i':
                opti = 1;
                break;
            case 'u':
                optu = 1;
                break;
            case 'g':
                optg = 1;
                break;
            case 's':
                opts = 1;
                break;
            case 'd':
                optd = 1;
                break;
            case 'l':
                optp = 1;
                opti = 1;
                optu = 1;
                optg = 1;
                opts = 1;
                optd = 1;
                optt = 1;
                break;
            case '?':
                printerror("dt: error: invalid argument. refer to option -h.\n");
                return 0;
        }
    }

    char cwd[PATH_MAX];
    char *root = getcwdir();

    if(argv[optind] != NULL)
    {
        directory_name = argv[optind];
    }
    if(directory_name == NULL)
    {
       if(getcwd(cwd, sizeof(cwd)) != NULL)
       {
            directory_name = cwd;
       } else {
           perror("dt: error");
           exit(EXIT_FAILURE);
       }
    }

    /* prints the root directory    */
    printinfo(directory_name, directory_name, 0, root);
    printentry();

    /* traverses and queues */
    filesystem(directory_name, 0, root);

    while(rear)
    {
        firstinline();
        filesystem(queuedlist, 0, root);
        dequeue(queuedlist);
    }


    free(root);
    return 0;
}


/* knowing current working directory */

void *getcwdir(void)
{
    char *cwdir, *token;
    char buffer[PATH_MAX + 1];
    char *directory;
    size_t length;

    cwdir = getcwd(buffer, PATH_MAX + 1);
    token = strrchr(cwdir, '/');

    if(cwdir == NULL)
    {
        perror("dt: error");
        exit(EXIT_FAILURE);
    }

    if (token == NULL)
    {
        perror("dt: error");
        exit(EXIT_FAILURE);
    }

    length = strlen(token);
    directory = malloc(length);
    memcpy(directory, token+1, length);

    return directory;
}


/* Bfs */

void bfs(char* node, char* fname, int indent, char* data)
{
    printf("BFS calls with: %s\n", node);


}
/*check if it is directory*/

int isdir(char* path)
{
    struct stat stats;
    if(stat (path, &stats) == -1)
    {
        return 0;
    } else {
        return S_ISDIR(stats.st_mode);
    }
}

/* concatinate Data */
void concatinate(int* marker, char* datum)
{
    int kickoff = *marker;
    for(; *marker < kickoff+strlen(datum); ++*marker)
    {
        actfsys[*marker] = datum[*marker - kickoff];
    }
    actfsys[*marker + 1] = '\0';
}


void printinfo(char *path, char *fname, int indent, char* root)
{
    int index;

    /* knowing current and parent dirs */
    if(path == "." || path == "..")
    {
        return;
    }

    struct stat fstats;

    if(lstat (path, &fstats) == -1)
    {
        perror("dt: error");
        exit(EXIT_FAILURE);
    }

    if(optL == 1)
    {
        /* S_IFMT bit mask for the file type bitfield. S_IFLNK symbolic link       */
        if( (fstats.st_mode & S_IFMT) == S_IFLNK)
        {
            char symlink_ref[256];
            const char* symlink_path = fname;
            int bufsiz_len = readlink(symlink_path, symlink_ref, sizeof(symlink_ref));
            if(bufsiz_len == -1)
            {
                perror("dt: error");
            } else {
                symlink_ref[bufsiz_len] = '\0';
                concatinate(&index, "symlink:");
                concatinate(&index, symlink_ref);
            }
        }
    }
    int i;
    for(i = index; i < 0; ++i)
    {
        concatinate(&index, " ");
    }
    if(optp == 1)
    {
        int fmode = fstats.st_mode;

        if(S_ISDIR(fmode))
        {
            concatinate(&index, "d");
        } else {
            concatinate(&index, "-");
        }

        if(fmode & S_IRUSR)
        {
            concatinate(&index, "r");
        } else {
            concatinate(&index, "-");
        }

        if(fmode & S_IWUSR)
        {
            concatinate(&index, "w");
        } else {
            concatinate(&index, "-");
        }

        if(fmode & S_IXUSR)
        {
            concatinate(&index, "x");
        } else {
            concatinate(&index, "-");
        }

        if(fmode & S_IRGRP)
        {
            concatinate(&index, "r");
        } else {
            concatinate(&index, "-");
        }

        if(fmode & S_IWGRP)
        {
            concatinate(&index, "w");
        } else {
            concatinate(&index, "-");
        }

        if(fmode & S_IXGRP)
        {
            concatinate(&index, "x");
        } else {
            concatinate(&index, "-");
        }

        if(fmode & S_IROTH)
        {
            concatinate(&index, "r");
        } else {
            concatinate(&index, "-");
        }

        if(fmode & S_IWOTH)
        {
            concatinate(&index, "w");
        } else {
            concatinate(&index, "-");
        }

        if(fmode & S_IXOTH)
        {
            concatinate(&index, "x");
        } else {
            concatinate(&index, "-");
        }

        concatinate(&index, "  ");
    }
    if(opti == 1)
    {
        char chabuff[128];
        sprintf(chabuff, "%d", fstats.st_nlink);
        concatinate(&index, chabuff);
        concatinate(&index, "  ");
    }

    if(optu == 1)
    {
        struct passwd *pwd;
        char uids[128];

        if( (pwd = getpwuid(fstats.st_uid)) == NULL)
        {
            sprintf(uids, "%d", fstats.st_uid);
            concatinate(&index, uids);
            concatinate(&index, "  ");
        } else {
            concatinate(&index, pwd->pw_name);
            concatinate(&index, "  ");
        }
    }

    if(optg == 1)
    {
        struct group *grp;
        char gids[128];

        if( (grp = getgrgid(fstats.st_gid)) == NULL)
        {
            sprintf(gids, "%d", fstats.st_gid);
            concatinate(&index, gids);
            concatinate(&index, "  ");
        } else {
            concatinate(&index, grp->gr_name);
            concatinate(&index, "  ");
        }
    }

    if(opts == 1)
    {
        const int bytesize = fstats.st_size;

        int kilo = 1024;
        int mega = 1048576;
        int giga  = 1073741824;

        char bs[kilo];
        if(bytesize < kilo)
        {
            sprintf(bs, "%d", bytesize);
            concatinate(&index, bs);
            if(bytesize >= 1000)
            {
                concatinate(&index, " ");
            }
            else if(bytesize >= 100 && bytesize < 1000)
            {
                concatinate(&index, "   ");
            }
            else if(bytesize >= 10 && bytesize < 100)
            {
                concatinate(&index, "    ");
            } else {
                concatinate(&index, "     ");
            }
        }

        if(bytesize / kilo >= 1 && bytesize < mega)
        {
            sprintf(bs, "%dK", bytesize / kilo);
            concatinate(&index, bs);
            if(bytesize / kilo >= 100 && bytesize / kilo < 1000)
            {
                concatinate(&index, "  ");
            }
            else if(bytesize / kilo >= 10 && bytesize / kilo < 100)
            {
                concatinate(&index, "   ");
            } else {
                concatinate(&index, "    ");
            }
        }

        if(bytesize / mega >= 1 && bytesize < giga)
        {
            sprintf(bs, "%dM", bytesize / mega);
            concatinate(&index, bs);
            if(bytesize / mega >= 100 && bytesize / mega < 1000)
            {
                concatinate(&index, "  ");
            }
            else if(bytesize / mega >= 10 && bytesize / mega < 100)
            {
                concatinate(&index, "   ");
            } else {
                concatinate(&index, "    ");
            }
        }

        if( (bytesize / giga) >= 1)
        {
            sprintf(bs, "%dG", bytesize / giga);
            concatinate(&index, bs);
            concatinate(&index, "   ");
        }
    }

    if(optd == 1)
    {
        stat(fname, &fstats);
        char date[17];
        strftime(date, 17, "%b %d, %Y", gmtime(&(fstats.st_ctime)));
        concatinate(&index, date);
        concatinate(&index, "  ");
    }

    if(optt == 1)
    {
        switch(fstats.st_mode & S_IFMT)
        {

            case S_IFBLK:
                concatinate(&index, "block special");
                break;

            case S_IFCHR:
                concatinate(&index, "character special");
                break;

            case S_IFIFO:
                concatinate(&index, "FIFO special");
                break;

            case S_IFREG:
                concatinate(&index, "regular");
                break;

            case S_IFDIR:
                concatinate(&index, "directory");
                break;

            case S_IFLNK:
                concatinate(&index, "symbolic link");
                break;
        }
        concatinate(&index, "     ");
    }


    concatinate(&index, path);
}


void printentry()
{
    int index;

    printf("%s\n", actfsys);
    for(index = 0; index < msize; ++index)
    {
        actfsys[index] = 0;
    }
}


/* Traverses the Directory  */

void filesystem(char* path, int indent, char* root)
{

    struct dirent *de = NULL;
    DIR *dr = NULL;

    /* returns DIR pointer */
    if(!(dr = opendir(path)))
    {
        return;
    }

    while( (de = readdir(dr) ) != NULL)
    {
        if(isdir(path))
        {
            char pathn[1024];

            /* if both of the parameters are equal, strcmp() returns int 0   */
            if(strcmp (de->d_name, ".") == 0 || strcmp (de->d_name, "..") == 0)
            {
                continue;
            }
            snprintf(pathn, sizeof(pathn), "%s/%s", path, de->d_name);
            printinfo(pathn, de->d_name, indent, root);
            printentry();
            enqueue(pathn);

        } else {
            printinfo(de->d_name, de->d_name, indent, root);
            printentry();
        }
    }
closedir(dr);
}



