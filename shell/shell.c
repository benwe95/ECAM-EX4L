#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 10


// Function to recover command entry.
char* get_cmd() {
	// Base allocation of memory.
	char *cmd = calloc(BUFFER_SIZE, sizeof(char));
	size_t len = 0;
	int character = 0;

	// Reallocation when the memory his too little.
	while (read(1, &character, 1) == 1) {
			if (character == '\n') { break; }
			if ((++len % BUFFER_SIZE) == 0) {
					cmd= realloc(cmd, len + BUFFER_SIZE);
			}
			cmd[len-1] = character;
	} if (len == 0) {
			// EOF reached.
			free(cmd);
			printf("EOF Erroe");
			exit(EXIT_FAILURE);
	}
	// Reallocation for null character.
	cmd = realloc(cmd, len+1);
	cmd[len] = '\0';
	return cmd;
}

// Split command entry in array.
char** split_cmd(char *cmd){
	char **array = NULL;
	char *word = strtok(cmd, " ");
	int len = 0;

	// Split command, options or args in array.
	while(word) {
			array = realloc(array, sizeof(char*) * ++len);
			array[len-1] = (char*) calloc(strlen(word)+1, sizeof(char));
			array[len-1] = strncpy(array[len-1], word, strlen(word)+1);
			word = strtok(NULL, " ");
	}
	array = realloc(array, sizeof(char*) * (len + 1));
	array[len] = 0;
	free(cmd);
	return array;
}

// Function to free memory for array.
void free_cmd(char **array) {
	for (size_t i = 0; array[i] != NULL; i++) {
			free(array[i]);
	}
	free(array);
}


// Function to execute command.
int launch_cmd(char **array) {
	if (strcmp(array[0], "exit") == 0) {
		exit(EXIT_SUCCESS);
	}

	// Fork proccessus.
	pid_t pid = fork();
	if (pid == 0) {
			// Error in the processus.
			if (execvp(array[0], array) == -1) {
					free_cmd(array);
					return EXIT_FAILURE;
			}
	} else if (pid == -1) {
			// Error in the processus.
			free_cmd(array);
			return EXIT_FAILURE;
	} else {
			// In the parent.
			pid_t wpid;
			int wpid_status;
			// Wait until child terminaison.
			wpid = waitpid(pid, &wpid_status, 0);
			if (wpid == -1) {
					// Error in the processus.
					free_cmd(array);
					return EXIT_FAILURE;
			}
	}
	free_cmd(array);
	return EXIT_SUCCESS;
}


int main(int argc, char *argv[]) {
	int status;
	char *cmd;
	char **array;

	while(1) {
		write(1, "$ ", 2);
		// Get command entry
		cmd = get_cmd();
		// Split command entry
		array = split_cmd(cmd);
		// Launch command
		status = launch_cmd(array);
		if (status == EXIT_FAILURE) {
				return EXIT_FAILURE;
		}
	}
  return EXIT_SUCCESS;
}
