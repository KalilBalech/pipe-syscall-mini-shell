#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    char write_msg[] = "Hello, World!";
    char read_msg[sizeof(write_msg)];

    int pipefd[2];

    pipe(pipefd);

    write(pipefd[1], write_msg, sizeof(write_msg));

    read(pipefd[0], read_msg, sizeof(read_msg));

    printf("read_msg: \"%s\"\n", read_msg);

    return 0;
}
