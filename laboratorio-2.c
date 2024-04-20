#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int MAX_NUMBER_OF_WORDS = 100;
int MAX_WORD_LENGTH = 20;

void defineStandardOutput(char string[]){
    int fdout = open(string, O_WRONLY | O_CREAT, 0644);
    dup2(fdout, 1);
}

void defineStandardInput(char string[]){
    int fdin = open(string, O_RDONLY);
    int fd2 = dup2(fdin,0);
}

void processComand(char input[], int pipeCount){
    int i, isOutputFile = 0, isInputFile = 0;

    char words[MAX_NUMBER_OF_WORDS][MAX_WORD_LENGTH]; // auxiliary array of strings that pre-process each word
    char fileName[MAX_WORD_LENGTH]; // string that stores the input or output file name after read < signal or > signal
    char *args[MAX_NUMBER_OF_WORDS + 1]; // array of strings that will be passed to execve() syscall
    int numberOfWordsRead = 0;

    numberOfWordsRead = 0;
    isOutputFile = 0;
    isInputFile = 0;
    for(i = 0; i < MAX_NUMBER_OF_WORDS;i++){ // initialize every args element with NULL
        args[i] = NULL;
    }

    pid_t retorno = fork(); // creates a fork syscall

    if(retorno == 0){ // child fork process
        if(input[strlen(input) - 1] == '\n'){ // transform the last "input variable" element into \0, that is the standard end of string
            input[strlen(input) - 1] = '\0';
        }
        if (numberOfWordsRead < MAX_NUMBER_OF_WORDS) {
            char *token = strtok(input, " \n"); // split the whole input based on spaces
            while (token != NULL) { // reads every input word
                strncpy(words[numberOfWordsRead], token, MAX_WORD_LENGTH - 1); // copy word by word to "words variable"
                if(strcmp(words[numberOfWordsRead], ">") == 0) isOutputFile = 1; // check if it is an output file
                else{
                    if(strcmp(words[numberOfWordsRead], "<") == 0) isInputFile = 1; // check if it is an input file
                    else{
                        if(isOutputFile == 1){ // if it is an output file, define it to be output
                            strncpy(fileName, token, MAX_WORD_LENGTH - 1);  // copy file name to "fileName variable"
                            fileName[MAX_WORD_LENGTH - 1] = '\0'; // fill the last element with /0 - standard string final char
                            defineStandardOutput(fileName); // define as output
                            isOutputFile = 0;
                        }
                        else{
                            if(isInputFile == 1){ // if it is an input file, define it to be input
                                strncpy(fileName, token, MAX_WORD_LENGTH - 1); // copy file name to "fileName variable"
                                fileName[MAX_WORD_LENGTH - 1] = '\0'; // fill the last element with /0 - standard string final char
                                defineStandardInput(fileName); // define as input
                                isInputFile = 0;
                            }
                            else{
                                words[numberOfWordsRead][MAX_WORD_LENGTH - 1] = '\0';
                                args[numberOfWordsRead] = words[numberOfWordsRead]; // copy the word to the real args array of strings
                                numberOfWordsRead++;
                            }
                        }
                    }
                }
                token = strtok(NULL, " \n"); // lê a próxima palavra
            }
        }
        if(pipeCount==0){
            execve(args[0], args, NULL); // call the execve syscall based on what is on args
        }
        else if(numberOfWordsRead==1){
            char aux[MAX_WORD_LENGTH];
            strcpy(aux, "/usr/bin/");
            strcat(aux, args[0]);
            execl(aux, args[0], NULL);
        }
        else if(numberOfWordsRead==2){
            char aux[MAX_WORD_LENGTH];
            strcpy(aux, "/usr/bin/");
            strcat(aux, args[0]);
            execl(aux, args[0], args[1], NULL);
        }
        else if(numberOfWordsRead==3){
            char aux[MAX_WORD_LENGTH];
            strcpy(aux, "/usr/bin/");
            strcat(aux, args[0]);
            execl(aux, args[0], args[1], args[2], NULL);
        }
        printf("Erro na execucao do comando. Por favor, estude mais. Voce esta precisando!\n");
    }
    else{ // parent syscall proeess that waits for its child
        wait(NULL);
    }
}

void onePipeCall(char command1[], char command2[]) {
    int pipefd[2];
    pipe(pipefd);

    if (!fork()) {
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        processComand(command1, 1);
        exit(0);
    }

    if (!fork()) {
        close(pipefd[1]);
        dup2(pipefd[0], 0);
        processComand(command2, 1);
        exit(0);
    }
    close(pipefd[0]);
    close(pipefd[1]);
}

void twoPipeCalls(char command1[], char command2[], char command3[]) {
    int pipefd1[2];
    pipe(pipefd1);

    if (!fork()) {
        close(pipefd1[0]);
        dup2(pipefd1[1], 1);
        processComand(command1, 2);
        exit(0);    
    }

    int pipefd2[2];
    pipe(pipefd2);

    if (!fork()) {
        close(pipefd1[1]);
        dup2(pipefd1[0], 0);
        dup2(pipefd2[1], 1);
        processComand(command2, 2);
        exit(0);
    }

    close(pipefd1[0]);
    close(pipefd1[1]);
    
    if (!fork()) {
        close(pipefd2[1]);
        dup2(pipefd2[0], 0);
        processComand(command3, 2);
        exit(0);
    }
    close(pipefd2[0]);
    close(pipefd2[1]);        
}

int main(){

    char input[1024];
    char c;
    int pipeCount = 0, i;
    char command1[MAX_WORD_LENGTH], command2[MAX_WORD_LENGTH], command3[MAX_WORD_LENGTH];

    printf("Welcome to the miniature-shell.\n");

    printf("cmd> "); // creates a friendly UI message
    while(1){

        fgets(input, sizeof(input), stdin); // reads the whole input, i.e. every word until \n

        if(input[strlen(input) - 1] == '\n'){ // transform the last "input variable" element into \0, that is the standard end of string
            input[strlen(input) - 1] = '\0';
        }

        // check how many pipe calls
        for(i=0; i<strlen(input); i++){
            if(input[i] == '|') pipeCount++;
        }

        // if there are 1 pipe calls
        if(pipeCount == 0){
            processComand(input, pipeCount);
        }
        else if(pipeCount == 1){
            char *token = strtok(input, "|");
            strcpy(command1, token);
            if(command1[strlen(command1) - 1] == ' '){ // transform the last "input variable" element into \0, that is the standard end of string
                command1[strlen(command1) - 1] = '\0';
            }
            token = strtok(NULL, "|"); // reads next command
            strcpy(command2, token);
            if(command2[0] == ' '){ // transform the last "input variable" element into \0, that is the standard end of string
                for(i=0; i< strlen(command2)-1; i++){
                    command2[i] = command2[i+1];
                }
                command2[i] = '\0';
            }
            onePipeCall(command1, command2);
        }
        else if(pipeCount == 2){
            char *token = strtok(input, "|");
            strcpy(command1, token);
            if(command1[strlen(command1) - 1] == ' '){ // transform the last "input variable" element into \0, that is the standard end of string
                command1[strlen(command1) - 1] = '\0';
            }
            token = strtok(NULL, "|"); // reads next command
            strcpy(command2, token);
            if(command2[0] == ' '){ // transform the last "input variable" element into \0, that is the standard end of string
                for(i=0; i< strlen(command2)-1; i++){
                    command2[i] = command2[i+1];
                }
                command2[i] = '\0';
            }
            if(command2[strlen(command2) - 1] == ' '){ // transform the last "input variable" element into \0, that is the standard end of string
                command2[strlen(command2) - 1] = '\0';
            }
            token = strtok(NULL, "|"); // reads next command
            strcpy(command3, token);
            if(command3[0] == ' '){ // transform the last "input variable" element into \0, that is the standard end of string
                for(i=0; i< strlen(command3)-1; i++){
                    command3[i] = command3[i+1];
                }
                command3[i] = '\0';
            }
            twoPipeCalls(command1, command2, command3);
        }

        pipeCount = 0;
    }

    return 0;
}
