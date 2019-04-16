#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define READ_END 0
#define WRITE_END 1

size_t BUFFER_SIZE;

FILE *copy_file;

int main(int argc, char *argv[]) {
    pid_t pid;
    // Actual pipe
    int kittens[2];

    if(pipe(kittens) == -1){
        printf("Failed to create pipe\n");
        return 1;
    }

    /* Step2: set up the pipe */
    /* make sure your program closes file handles*/
    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Failed");
        return 1;
    }

    /*
     * Step3: create the processes
     * read from the input file and write to the pipe
     * read from the pipe and write to the output file
     */
    if (pid > 0) {
        // Parent process
        close(kittens[READ_END]);
        char current_character;
        int input_file_descriptor = open(argv[1], O_RDONLY);
        if(input_file_descriptor == -1){
            fprintf(stderr, "Unable to open file %s \n", argv[1]);
            close(kittens[WRITE_END]);
            return 1;
        }
        while(read(input_file_descriptor, &current_character, 1) > 0) {
            printf("Sent Character: %c \n", current_character);
            write(kittens[WRITE_END], &current_character, 1);
        }
        close(kittens[WRITE_END]);
        close(input_file_descriptor);
        wait(NULL);
    } else {
        // child process
        close(kittens[WRITE_END]);
        char current_character;
        int output_file_descriptor = open(argv[2], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if(output_file_descriptor == -1) {
            fprintf(stderr, "Unable to open file %s \n", argv[2]);
            close(kittens[READ_END]);
            return 1;
        }
            while (read(kittens[READ_END], &current_character, 1) > 0) {
                printf("Received Character: %c \n", current_character);
                write(output_file_descriptor, &current_character, 1);
            }
        close(kittens[READ_END]);
        close(output_file_descriptor);
        //timestamp here 
        printf("Timestamp: %d\n",(int)time(NULL));
        return 0;
    }
    return 0;
}

