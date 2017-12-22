#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>

//Open a directory and get the list of files/directories inside
// @dir[]: give the name of the directory
// @extension[]: give the extension you're looking for (txt, docx, py, c, ...)
void openAndGet(char dir[],char extension[]){
  struct dirent *pDirent;
  DIR *pDir;
  char *p;
  pDir = opendir (dir);
  // test if there is a dir with that name
  if (pDir == NULL) {
   printf ("Cannot open directory '%s'\n", dir);
  }
  else{

    //Read what's inside of the pDir
    while ((pDirent = readdir(pDir)) != NULL) {
      struct stat st;
      lstat(pDirent->d_name, &st);
      int isDir = 0;
      // get name of the file/directory
      // just links to previous directories? (. or ..)
      if (strncmp(pDirent->d_name,".",100)==0 || strncmp(pDirent->d_name,"..",100)==0){
        //
      }
      else
      {
        //get position of the last "." and then read from after the point to the end
        //char *initialdot = strtok(pDirent->d_name,".");
        char *dot = strtok(pDirent->d_name,".");
        while (dot != NULL){
          p = dot;
          dot = strtok(NULL,".");
        }
        if (p == pDirent->d_name){
          isDir = 1;
          char firstpath[100], middlepath[5], lastpath[100];
          strcpy(firstpath,dir);
          strcpy(middlepath,"/");
          strcpy(lastpath,pDirent->d_name);
          strcat(firstpath,middlepath);
          strcat(firstpath,lastpath);
          openAndGet(firstpath,extension);
        }
        if (strncmp(extension,"all",50)==0){
          printf("%s/%s",dir,pDirent->d_name);
          if (isDir == 0 && strncmp(".DS_Store",pDirent->d_name,50)!=0)
            printf(".%s\n",p);
          else
            printf("\n");
        }
        else if (strncmp(p,extension,50)==0){
          printf("%s/%s.%s\n",dir,pDirent->d_name,extension);
        }
      }
    }
    // close dir pDir
    if (pDir != NULL){
      closedir(pDir);
      pDir = NULL;
    }
  }

}

int main (int c, char *v[]) {
   int len;
   struct dirent *pDirent;
   struct dirent *newDirent;
   DIR *pDir;
   DIR *newDir;
   char *p;
   char *q;
   struct stat fileStat;
   char buf[PATH_MAX + 1];

   if (c < 2) {
       printf ("Usage: ./listf <dirname> <ext>\n");
			 printf("usage: listf <dirname> <ext> --Display the n files in a dirname with ext--\n" \
								 "  ----- Argument -----\n" \
								 "  dirname \tPrecises the directory.\n"\
								 "  ext \tPrecises the extension.\n"\
								 "\n\tExample: listf /jonh js   --Display all files with .js extension--\n"\
								 "\n\tExample: listf /jonh all --Display all files--\n"\
								 );
       return 1;
   }
   printf("dirname: %s\n", v[1]);
   printf("extension: %s\n", v[2]);

   openAndGet(v[1],v[2]);

   return 0;
}
