#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
/* Pour la recherche dans les dossiers */
#include <dirent.h>
#ifndef WIN32
    #include <sys/types.h>
#endif

/*Devrait se trouver dans un fichier à part qui définit toutes les structures*/
/*le type 'option' est déjà définit dans la librairie getopt.h, il ne doit pas
être redéfinit
struct option {
    const char *name;
    int         has_arg;
    int         *flag;
    int         val;
};*/

struct option longopts[] = {
    {"bonjour", required_argument, NULL, 'b'},
    {"dir", no_argument, NULL, 'd'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0}
};

int main (int argc, char *argv[]) {
    int c;
    int dir = 0;
    int digit_optind = 0;

    while((c = getopt_long(argc, argv, ":dhb:", longopts, NULL)) != -1){
        switch (c){
            case 'b':
                printf("bonjour %s\n", optarg);
                break;
            case 'd':
                dir = 1;
                break;
            case 'h':
                printf("help");
                break;
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

    printf("%d\n", dir);

    if(dir==1){
        DIR* repo = NULL;
        struct dirent* file = NULL;

        repo = opendir("../Codes");

        if(repo == NULL){
            printf("Error: can't open directory");
        }
        else{
            printf("Pointer repo: %p\n", (void*) repo);
        }

        while((file = readdir(repo)) != NULL){
            printf("File name: %s\t File type: %d\n",
                    file->d_name, file->d_type);
        }

        if(closedir(repo) == -1){
            printf("File can't be closed");
        }
    }
}

