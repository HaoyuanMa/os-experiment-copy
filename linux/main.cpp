#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <utime.h>
#include <sys/time.h>


void cp(char *s, char *t)
{
	//char *curdir = getcwd(NULL, 0);
	//printf("%s\n", curdir);
	struct stat date;
	lstat(s, &date);

	struct utimbuf mytime;
	mytime.actime = date.st_atim.tv_sec;
	mytime.modtime = date.st_mtim.tv_sec;

	char d[200], ns[200];
	int j = 0, i = 0;
	for (i = strlen(s); s[i] != '/'; i--)
		d[j++] = s[i];
	i = 0;
	while (j >= 0)
		ns[i++] = d[--j];

	if (S_ISDIR(date.st_mode))
	{
		char *mcurdir = getcwd(NULL, 0);
		if (chdir(t))
		{
			mkdir(t, 0777);
			chdir(t);
		}

		mkdir(ns, date.st_mode);

		char target[200];
		strcpy(target, t);
		strcat(target, "/");
		strcat(target, ns);

		chdir(mcurdir);
		chdir(s);

		DIR *dir = opendir(".");
		struct dirent *ddate;
		while (ddate = readdir(dir))
		{
			if (ddate->d_name[0] == '.')
				continue;
			chdir(s);
			char ss[200];
			
			char source[200];
			char *cur = getcwd(NULL, 0);
			strcpy(source, cur);
			strcat(source, "/");
			strcat(source, ddate->d_name);

			cp(source, target);

		}

		chdir(t);
		chmod(ns, date.st_mode);
		utime(ns, &mytime);

		closedir(dir);
	}
	else if (S_ISLNK(date.st_mode))
	{
		//char* mcurdir = getcwd(NULL, 0);

		char lk[200];
		/*
		int size;
		size = readlink(s, lk, 200*sizeof(lk[0]));
		lk[size] = '\0';
		*/
		char target[200];
		strcpy(target, t);
		strcat(target, "/");
		strcat(target, ns);
		
		//char nlk[50];

		realpath(s, lk);
		symlink(lk, target);
		
		timeval ltime[2];
		ltime[0].tv_sec = date.st_atim.tv_sec;
		ltime[0].tv_usec = date.st_atim.tv_nsec / 1000;
		ltime[1].tv_sec = date.st_mtim.tv_sec;
		ltime[1].tv_usec = date.st_mtim.tv_nsec / 1000;
		
		chdir(t);
		lchmod(ns, date.st_mode);
		lutimes(ns, ltime);

	}
	else
	{
		int file = open(s, O_RDONLY);

		if (chdir(t))
		{
			mkdir(t, 0777);
			chdir(t);
		}

		int nfile = creat(ns, date.st_mode);
		char *buffer;
		buffer = (char*)malloc(date.st_size + 1);
		read(file, buffer, date.st_size);
		write(nfile, buffer, date.st_size);

		chmod(ns, date.st_mode);
		utime(ns, &mytime);

		close(file);
		close(nfile);
	}
}


int main(int argc, char **argv)
{
	char source[200], target[200];

	if (argv[1][0] != '/')
	{
		char *curdir = getcwd(NULL, 0);
		strcpy(source, curdir);
		strcat(source, "/");
		strcat(source, argv[1]);
	}
	
	realpath(argv[2], target);

	cp(source, target);
	
}