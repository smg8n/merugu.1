#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
//#include <sys/dirent.h>
#include <unistd.h>

#include <pwd.h>
#include <grp.h>
//#include <uuid/uuid.h>

#include <time.h>

#define BUFF_SIZE 4096



struct sline {
	char *mode;
	int link;
	char *user;
	char *group;
	int size;
	//char *datetime;
	char *file;
};

struct queue {
	char *dir;
	struct queue *next;
};

int create_stat_info(struct dirent *entry, struct sline *lentry, const char *root);
int get_digit(int num);
int enqueue(const char *dir, const char *root);
struct queue dequeue(struct queue qp);

static int luser=0, llink=0, lgroup=0, lsize=0;
static struct queue *qp;

int main(int argc, char* argv[])
{
	DIR *dp;
	struct dirent *dent;
	struct sline *lines;
	struct queue *tmp;
	int i=0, csize=1, cline=0;
	char *root;

	if (1 < argc) {
		root = argv[1];
	} else {
		root = malloc(sizeof(char) * 2);
		strcpy(root, ".");
	}


	qp = malloc(sizeof(struct queue));
	qp->dir = root;
	qp->next = NULL;

	while(qp != NULL) {
		luser=0; llink=0; lgroup=0; lsize=0;
		i=0; csize=1; cline=0;

		if ((dp=opendir(qp->dir)) == NULL) {
			printf("failed to opendir.");
			exit(-1);
		}

		// initialize malloc.
		lines = malloc(sizeof(struct sline) * csize);

		while ((dent=readdir(dp)) != NULL) {
			if (cline+1 > csize) {
				csize *= 2;
				lines = realloc(lines, sizeof(struct sline) * csize);
			}

			if (create_stat_info(dent, &lines[cline], qp->dir) != -1) {
				cline++;
			}
		}

		// print out
		printf("--- stat info of dir '%s' ---\n", qp->dir);
		for(i=0; i<cline; i++) {
			printf("%s ", lines[i].mode);
			printf("%*d ", llink+1, lines[i].link);
			printf("%*s ", luser+1, lines[i].user);
			printf("%*s ", lgroup+1, lines[i].group);
			printf("%*d ", lsize+1, lines[i].size);
			//printf("%s ", lines[i].datetime);
			printf("%s \n", lines[i].file);
		}
		printf("\n");

		tmp = qp;
		qp = qp->next;
		free(tmp);
		free(lines);
		closedir(dp);
	}

	return 1;
}

int create_stat_info(struct dirent *entry, struct sline *lentry, char const *root)
{
	// init
	struct stat info;
	int len;
	char *smode, *suser, *sgroup, *sfile, *stime;
	struct sline line;
	char *filepath;

	filepath = malloc(sizeof(char) * (strlen(entry->d_name) + strlen(root) + 1));
	strcpy(filepath, root);
	len = strlen(root);
	filepath[len] = '/';
	strcpy(&filepath[len+1], entry->d_name);

	// validation.
	if (stat(filepath, &info) == -1) {
		return -1;
	}

	if (S_ISDIR(info.st_mode)) {
		if (strcmp(".", entry->d_name) != 0 && strcmp("..", entry->d_name) != 0) {
			// enqueue
			// printf("%s\n", entry->d_name);
			if (enqueue(entry->d_name, root) == -1) {
				printf("failed to enqueu.");
				exit(-1);
			}
		}
	}

	// create mode.
	smode = malloc(sizeof(char) * 11);
	strcpy(smode, "----------");

	// inode type.
	if (S_ISDIR(info.st_mode)) { smode[0] = 'd'; }
	if (S_ISCHR(info.st_mode)) { smode[0] = 'c'; }
	if (S_ISBLK(info.st_mode)) { smode[0] = 'b'; }

	// permission.
	if (info.st_mode & S_IRUSR) { smode[1] = 'r'; }
	if (info.st_mode & S_ISUID) { smode[1] = 's'; }
	if (info.st_mode & S_IWUSR) { smode[2] = 'w'; }
	if (info.st_mode & S_IXUSR) { smode[3] = 'x'; }
	if (info.st_mode & S_IRGRP) { smode[4] = 'r'; }
	if (info.st_mode & S_ISGID) { smode[4] = 's'; }
	if (info.st_mode & S_IWGRP) { smode[5] = 'w'; }
	if (info.st_mode & S_IXGRP) { smode[6] = 'x'; }
	if (info.st_mode & S_IROTH) { smode[7] = 'r'; }
	if (info.st_mode & S_IWOTH) { smode[8] = 'w'; }
	if (info.st_mode & S_IXOTH) { smode[9] = 'x'; }
	if (info.st_mode & S_ISVTX) { smode[9] = 't'; }

	// user
	struct passwd* user = getpwuid(info.st_uid);
	len = strlen(user->pw_name);
	suser = malloc(sizeof(char) * len);
	strcpy(suser, user->pw_name);

	if (len > luser) {
		luser = len;
	}

	// group
	struct group* group = getgrgid(info.st_gid);
	len = strlen(group->gr_name);
	sgroup = malloc(sizeof(char) * len);
	strcpy(sgroup, group->gr_name);

	if (len > lgroup) {
		lgroup = len;
	}

	// file
	len = strlen(entry->d_name);
	sfile = malloc(sizeof(char) * len);
	strcpy(sfile, entry->d_name);

	// time
	/*struct tm* stm = localtime(&info.st_atimespec.tv_sec);
	stime = malloc(sizeof(char) * 17);
	strcpy(stime, "                ");
	sprintf(stime, "%d %2d %2d %02d:%02d", stm->tm_year+1900, stm->tm_mon+1, stm->tm_mday, stm->tm_hour, stm->tm_min);
*/
	// link
	len = get_digit(info.st_nlink);
	if (len > llink) {
		llink = len;
	}

	// size
	len = get_digit(info.st_size);
	if (len > lsize) {
		lsize = len;
	}

	line.mode = smode;
	line.user = suser;
	line.group = sgroup;
	line.file = sfile;
	line.link = info.st_nlink;
	line.size = info.st_size;
	//line.datetime = stime;

	*lentry = line;

	return 1;
}

int get_digit(int num)
{
	int count = 0;

	while(1) {
		count++;
		if (num < 10) {
			return count;
		}
		num = (int)(num / 10);
	}

	return count;
}

int enqueue(const char *dir, const char *root)
{
	char *dirname;
	char buff[BUFF_SIZE];
	int idx, len;
	struct queue *q, *p;

	q = malloc(sizeof(struct queue));

	strcpy(buff, root);
	idx = strlen(root);
	buff[idx] = '/';
	strcpy(&buff[idx+1], dir);

	len = strlen(buff);
	dirname = malloc(sizeof(char) * len);
	strcpy(dirname, buff);
	q->dir = dirname;
	q->next = NULL;
	p = qp;
	while (p->next != NULL) {
		p = p->next;
	}
	p->next = q;

	return 1;
}
