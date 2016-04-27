#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

void printFileInfo(struct stat entryInfo);
void scanDirectory(char *dirName);

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: %s path\n", argv[0]);
		return 0;
	}
	char *path = argv[1];
	struct stat entryInfo;
	if(stat(path, &entryInfo) == 0)
	{
		if(S_ISDIR(entryInfo.st_mode))
			scanDirectory(path);
		else if(S_ISREG(entryInfo.st_mode))
			printFileInfo(entryInfo);
		else
			printf("\tOther\n");
	}
	else
		printf("Access denied\n");
	return 0;
}

void scanDirectory(char *dirName)
{
	DIR *dir;
	struct dirent entry;
	struct dirent *entryPtr;
	int retval;
	dir = opendir(dirName);
	if (dir == NULL)
	{
		printf("Can't open directory");
		return;
	}
	retval = readdir_r(dir, &entry, &entryPtr);
	while( entryPtr != NULL ) {
		struct stat entryInfo;
		char path[PATH_MAX + 1];
		strncpy(path, dirName, PATH_MAX);
		strncat(path, "/", PATH_MAX);
		strncat(path, entry.d_name, PATH_MAX);
		printf("%s\n", path);
		if(stat(path, &entryInfo) == 0)
		{
			if(S_ISDIR(entryInfo.st_mode))
				printf("\tDirectory\n");
			else if(S_ISREG(entryInfo.st_mode))
				printFileInfo(entryInfo);
			else
				printf("\tOther\n");
		}
		else
			printf("\tAccess denied\n");
		retval = readdir_r( dir, &entry, &entryPtr );
	}
}

void printFileInfo(struct stat entryInfo)
{
	char buf[80];
	struct tm ts;
	printf("\tSize: %lld bytes\n", (long long)entryInfo.st_size);
	ts = *localtime(&entryInfo.st_atime);
	strftime(buf, sizeof(buf), "%d.%m.%Y,%H:%m:%S", &ts);
	printf("\tTime of last access: %s\n", buf);
	ts = *localtime(&entryInfo.st_mtime);
	strftime(buf, sizeof(buf), "%d.%m.%Y,%H:%m:%S", &ts);
	printf("\tTime of last modification: %s\n", buf);
	ts = *localtime(&entryInfo.st_ctime);
	strftime(buf, sizeof(buf), "%d.%m.%Y,%H:%m:%S", &ts);
	printf("\tTime of last change: %s\n", buf);
}
