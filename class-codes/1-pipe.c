#include <unistd.h>

int main(int argc, char *argv[]) {
    int pipefd[2];

    pipe(pipefd);

    return 0;
}

