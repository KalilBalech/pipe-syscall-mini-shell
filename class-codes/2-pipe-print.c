#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    printf("pipefd[0]: %d\n", pipefd[0]);
    printf("pipefd[1]: %d\n", pipefd[1]);

    return 0;
}
