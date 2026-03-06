// sensorhub.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

struct messages_t {
    const char* "hallo";
    const char* "how are you?";
    const char* "Why don't you respond?";
    const char* "Are you there?";
    const char* "Goodbye!";
};



int main(void) {
    // create a pipe child -> parent
    int child_tx[2]; // child_tx[0] = read end, child_tx[1] = write end
    if (pipe(child_tx) == -1) {
        perror("pipe_tx");
        return 1;
    }

    // create a pipe parent -> child
    // int child_rx[2]; // child_rx[0] = read end, child_rx[1] = write end
    // if (pipe(child_rx) == -1) {
    //     perror("pipe_rx");
    //     return 1;
    // }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // CHILD
        close(child_tx[0]); // close the read end
        // close(child_rx[1]); // close the write end

        // write loop -> print what you write
        messages_t messages;
        for (int i = 0; i < 5; i++) {
            snprintf(messages[i], sizeof(messages[i]), "Message %d from parent", i + 1);
            write(child_tx[1], messages[i], sizeof(messages[i]));
            sleep(1); // simulate some delay
        }

        close(child_tx[1]); // close the write end
        // close(child_rx[0]); // close the read end
        exit(0);
        return 0;
    } else {
        // PARENT
        close(child_tx[1]); // close the write end
        // close(child_rx[0]); // close the read end

        // read 5 messages from the child with some delay in between
        char buffer[1024];
        ssize_t bytes_read;
        while ((bytes_read = read(child_tx[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            printf("PARENT received: %s\n", buffer);
        }


        // close(child_tx[0]); // close the read end
        close(child_rx[1]); // close the write end
        // wait() for the child to finish
        int status = 0;
        waitpid(pid, &status, 0);
        return 0;
    }
}