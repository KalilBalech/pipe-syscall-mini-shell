#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    if (!fork()) {
        int pipefd[2];
        pipe(pipefd);
        
        if (!fork()) {
            close(pipefd[0]);

            dup2(pipefd[1], 1);

            execl("/bin/ls", "ls", "-l", NULL);
        } else {
            close(pipefd[1]);

            dup2(pipefd[0], 0);

            execl("/usr/bin/wc", "wc", "-l", NULL);
        }
    } else {
        wait(NULL);
    }

    printf("This line should execute!\n");

    return 0;
}
