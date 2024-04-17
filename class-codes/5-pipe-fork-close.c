#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int pipefd[2];
    pipe(pipefd);

    if (!fork()) {
        char write_buffer[] = "Hello!";
        close(pipefd[0]);
        write(pipefd[1], write_buffer, sizeof(write_buffer));
        printf("Child wrote: \"%s\"\n", write_buffer);
    } else {
        char buffer[1024] = {0};
        close(pipefd[1]);
        read(pipefd[0], buffer, sizeof(buffer));
        printf("Parent read: \"%s\"\n", buffer);
        wait(NULL);
    }

    return 0;
}
