#include "types.h"
#include "fcntl.h"
#include "user.h"

int main (int argc, char *argv[]) {

    char *givenFile = argv[1];									// get the file from input
			
    unlink(givenFile);										// get rid of old file if its not being used

    int parentFile = open(givenFile, O_CREATE | O_RDWR);					// Open the file within the parent process

    if (parentFile < 0) {									// Checks if file opened properly

        printf(2, "Parent could not open %s\n", givenFile);
        exit();

    }
 
    int processId = fork();									// fork to create a child process

    if (processId == 0) {									// if pid = 0, we are in the child process

        int childFile = open(givenFile, O_CREATE | O_RDWR);					// open the file within the child process

        if (childFile < 0) {									// Checks if file opened properly

            printf(2, "Child could not open %s\n", givenFile);					
            exit();

        }

        char greetMessage[] = "Hello!\n";							// Hardcoded greet message

        write(childFile, greetMessage, strlen(greetMessage));					// write the message to the file

        close(childFile);									// close the file on the child process side

        printf(1, "%d: Greeted my parent!\n", getpid());					// print the greet confirmation message

	exit();											// exit the child process

    }    

    wait();											// parent waits for child to exit before reading file

    char receivedMsg[100];									// buffer to contain the message read from the file

    int readIndex = read(parentFile, receivedMsg, 80);						// read the file for the message (max length of 80)

    receivedMsg[readIndex] = 0;									// Add null terminator to the end of the message

    close(parentFile);										// close the file on the parent process side

    printf(1, "%d: Recevied greetings from %d. %s\n", getpid(), processId, receivedMsg); 	// Final greeting message with child and parent pids and message

    exit();											// exit the parent process

}
