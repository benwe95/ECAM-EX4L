#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>


struct file_info{
    char*   name;
    long    date;
};

/* The available options for the command */
struct option longopts[] = {
    {"num", required_argument, NULL, 'n'},
    {"dir", required_argument, NULL, 'd'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0}
};


/* Convert a date from a string format to a long format
 * @pre - *date must match the following pattern: "YY-MM-DD hh:mm:ss"
 * @post - return a long value corresponding to the concatenation of the digits in the date
 */
long* dateToLong(char *date)
{
    long *converted = malloc(sizeof (long));
    char *token = strtok(date, " -:");
    char final[13];
    while(token != NULL)
    {
        strcat(final, token);
        token=strtok(NULL, " -:");
    }
    converted = (long) atol(final);
    return (converted);
}

/* Compare the date of a file with other files in an array. Put the file in the array
 * if it belongs to the 'numberFiles' most recent updated files.
 * @pre - files[] is the array containing the X revelant files sorted according to the (long) date
 *        fileToTest is the file to be compared with the array
 *        numberFiles precises how many recent files we are searching
 * @post - update the array files[] by putting fileToTest at the right place it has to
 */
void compare(struct file_info files[], struct file_info fileToTest, int numberFiles)
{
    int i;
    int position = numberFiles ;
    for(i=numberFiles-1; i>=0; i--)
    {
        /* If the tested date is higher than the date of the current item
           it means that the tested file is more recent */
        if(fileToTest.date > files[i].date)
        {
            position --;
        }
    }

    /* Place the fileToTest at the right position and
       move the other items of the array to maintain the order*/
    if(position!=numberFiles)
    {
        int j;
        for(j=numberFiles-1; j>position; j--)
        {
            files[j] = files[j-1];
        }
        files[position] = fileToTest;
    }
}


/* Display the content of a directory and its direct subdirectories (1 level max)
 *  @pre - *directoryPath represent the absolute path to the directory
 */
void displayDirectory(char *directoryPath){
    DIR* repo = NULL;
    struct dirent* file = NULL;

    repo = opendir(directoryPath);

    if(repo == NULL){
        printf("Error: can't open directory");
    }

    while((file = readdir(repo)) != NULL){
        //Check if "file" is a directory
        struct stat entrystat;
        stat( file->d_name, &entrystat );

        if(S_ISDIR( entrystat.st_mode ) & strcmp(file->d_name, ".") & strcmp(file->d_name, "..")){
            printf("DIR ->");
            char dir[30] = "";
            strcat(dir, directoryPath);
            strcat(dir, file->d_name);
            printf("%s\n", dir);
            displayDirectory(dir);
        }
        else{
            printf("--%s/%s\n", directoryPath, file->d_name);
        }
    }
}

/* Display the n last updated files in the selected directory
 * @pre - dir  must be a valid path
 *        numberFiles precises how many recent files we are searching
 * @post - Display the files with their name and the date of the last modification
 */
void handleRecent(char* dir, int numberFiles){
        struct file_info recentFiles[numberFiles+1];

        /* Fill the recentFiles array with default values to avoid NULL values */
        int f;
        for(f=0; f<numberFiles;f++)
        {
            struct file_info info;
            info.name = "";
            info.date = 0;
            recentFiles[f] = info;
        }

        DIR* repo = NULL;
        struct dirent* file = NULL;

        repo = opendir(dir);

        if(repo == NULL){
            printf("Error: can't open directory");
        }

        while((file = readdir(repo)) != NULL){
            /* Only files are taken into account no directories */
            if(file->d_type!=4){
                struct stat attrib;
                stat(file->d_name, &attrib);
                char strDate[18];
                strftime(strDate, 30, "%y-%m-%d %H:%M:%S", localtime(&(attrib.st_mtime)));

                struct file_info fileTested;
                fileTested.name = file->d_name;
                fileTested.date = dateToLong(strDate);

                compare(recentFiles, fileTested, numberFiles);
            }
        }

        printf(" %d MOST RECENT FILES UPDATED\n", numberFiles);
        int i;
        for(i=0; i<numberFiles; i++){
            printf("\t--name: %s\t date: %ld\n", recentFiles[i].name, recentFiles[i].date);
        }

        if(closedir(repo) == -1){
            printf("File can't be closed");
        }
}


/* When the shell executes the program it passes the options given with the command */
int main (int argc, char *argv[]) {
    int c;
    char* dir = ".";
    int nflag, dflag;
    int digit_optind = 0;
    int numberFiles = 3;

    while((c = getopt_long(argc, argv, ":nd:h", longopts, NULL)) != -1){
        switch (c){
            case 'n':
                nflag = 1;
                numberFiles = atoi(argv[2]);
                break;
            case 'd':
                dir = argv[4];
                break;
            case 'h':
            printf("usage: recent[OPTION] ... Display the n last updated files in a directory\n" \
                      "  ----- Options -----\n" \
                      "  -n --num\tPrecises the number of more recent files (The value must be an integer)\n"\
                      "  -d --dir\tPrecises the subdirectory.\n"\
                      "\n\tExample: recent -n 3 -d subdir/subsubdir\n"\
                      "\n\tWill search for the 3 files that have been modified the most recently in subdir/subsubdir\n"\
                      "\n\tNote that the order of the options is important -n -d \n"\
                      );
                return EXIT_SUCCESS;
            case ':':
                fprintf(stderr, "%s: option '-%c' requires an argument\n",
                        argv[0], optopt);
                break;
            case '?':
            default:
                fprintf(stderr, "%s: option '-%c' is invalid: ignored\n",
                        argv[0], optopt);
        }
    }
    if(nflag==1){
        handleRecent(dir, numberFiles);
    }
    else{

    }
    //displayDirectory(".");
}
