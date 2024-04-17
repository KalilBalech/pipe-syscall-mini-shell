#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int pipefd[2];
    pipe(pipefd);

    if (!fork()) {
        printf("oi do filho\n");
        printf("oi 2 do filho\n");
        char write_buffer[] = "Hello!";
        printf("oi 3 do filho\n");
        write(pipefd[1], write_buffer, sizeof(write_buffer));
        printf("Child wrote: \"%s\"\n", write_buffer);
    } else {
        printf("oi do pai\n");
        printf("oi 2 do pai\n");
        char buffer[1024] = {0};
        printf("Agora o pai vai esperar o filho, pois precisa ler o que vai pro pipe, e, por enquanto nao tem nada no pipe.\n");
        printf("Fala filhão\n");
        read(pipefd[0], buffer, sizeof(buffer));
        printf("Parent read: \"%s\"\n", buffer);
        wait(NULL);
    }

    return 0;
}
