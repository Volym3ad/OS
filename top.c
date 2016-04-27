#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>

int main()
{
	DIR *dirPtr;
	FILE *filePtr;
	struct dirent *dirInfo;
	char path[20],comm[30],state,buffer[1000];
	int pid,totalSize,freeSize,buffers,size,rsize;
	int i=0;
	if(!(dirPtr=opendir("/proc")))
	{
		printf("Cannot do opendir() !\n");
		return 0;
	}
	strcpy(path,"/proc/meminfo");
	if(!(filePtr=fopen(path,"r")))
		return 0;
	fscanf(filePtr,"%s%d%s",buffer,&totalSize,buffer);
	fscanf(filePtr,"%s%d%s",buffer,&freeSize,buffer);
	fscanf(filePtr,"%s%d",buffer,&buffers);
	printf("Memory: %d Kb total,%d Kb free,%d Kb buffers\n",totalSize,freeSize,buffers);
	printf("%3s%25s%6s%4s%9s%9s\n","NUM","COMMAND","PID","S","VIRT","RES");
	while(dirInfo=readdir(dirPtr))
	{
		if(!isdigit(dirInfo->d_name[0]))
			continue;
		strcpy(path,"/proc/");
		strcat(path,dirInfo->d_name);
		strcat(path,"/stat");
		if(!(filePtr=fopen(path,"r")))
			continue;
		fscanf(filePtr,"%d",&pid);
		fscanf(filePtr,"%s",comm);
		fscanf(filePtr,"%c",&state);
		strcpy(path,"/proc/");
		strcat(path,dirInfo->d_name);
		strcat(path,"/statm");
		if(!(filePtr=fopen(path,"r")))
			continue;
		fscanf(filePtr,"%d",&size);
		fscanf(filePtr,"%d",&rsize);
		printf("%3d%25s%6d%4c%6d Kb%6d Kb\n",++i,comm,pid,state,4*size,4*rsize);
		fclose(filePtr);
	}
	closedir(dirPtr);
	return 0;
}
