#include <stdio.h>
#include <iostream>
#include <unistd.h>    //used to fork and pipe threads
#include <stdlib.h>    //when exiting
#include <sys/wait.h>  //wait on threads or other
#include <string.h>
using namespace std;

int main(int argc, char ** argv)
{
// AS DEMONSTRATED IN THE POWERPOINT

    int pid, status;
    int fd1[2], fd2[2]; // file descriptors for pipes
    const int READ = 0, WRITE = 1;
    if (argc < 2) {
        perror("ERROR: Missing Arguments");
        return 0;
    }

    if(pipe(fd1) < 0) perror("Pipe1: Error") ; // create pipe1
    if(pipe(fd2) < 0) perror("Pipe2: Error") ; // create pipe2

    if ((pid = fork()) < 0){ // Create New Child Process
        perror("ERROR: Bad Fork");
        return EXIT_FAILURE;
    }
    else if (pid == 0) {
        if ((pid = fork()) < 0){ // Create New Child Process
            perror("ERROR: Bad Fork");
            return EXIT_FAILURE;
        }
        else if (pid == 0) {

            if ((pid = fork()) < 0){ // Create New Child Process
                perror("ERROR: Bad Fork");
                return EXIT_FAILURE;
            }

                // this else statement is very similar to the class PowerPoint
                // not sure if it needs to be just another "if" statement or "else if"
            else if (pid == 0) {
                //child
                close(fd2[WRITE]); // stop reading at firt pipe
                close(fd1[WRITE]); // stop  reading, second pipe
                close(fd1[READ]);  // stop writing, second pipe
                dup2(fd2[READ], READ); // write to first pipe
                execlp("wc", "wc", "-l" ,NULL);
            }
            else
            {
                //grand child
                close(fd2[READ]); // stop write
                close(fd1[WRITE]);// stope read
                dup2(fd2[WRITE], WRITE); //write to second pipe
                dup2(fd1[READ], READ);    // read whats writed to first pipe
                execlp("grep", "grep", argv[1],NULL);
            }
        }
        else
        {
            //great grand child
            close(fd2[READ]); // done with first pipe
            close(fd2[WRITE]);
            close(fd1[READ]); // close write second pipe
            dup2(fd1[WRITE], WRITE);
            execlp("ps", "ps", "-A" ,NULL);
        }
    }
    else
    {
        //parent and all other processes finished
        wait(&status);
    }
    return 0;
    //EXIT_SUCCESS;

}