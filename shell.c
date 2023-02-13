#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include "interpreter.h"
#include "shellmemory.h"
//#include "interpreter.c"
//#include "interpreter.o"
//#include "shellmemory.o"

//#include "Makefile"
#define MAX_TOKENS 100
#define MAX_TOKEN_LENGTH 100

int MAX_USER_INPUT = 1000;
int parseInput(char ui[]);

// Start of everything
int main(int argc, char *argv[]) {
	printf("%s\n", "Shell version 1.2 Created January 2023");
	help();

	char prompt = '$';  				// Shell prompt
	char userInput[MAX_USER_INPUT];		// user's input stored here
	int errorCode = 0;					// zero means no error, default

	//init user input
	for (int i=0; i<MAX_USER_INPUT; i++)
		userInput[i] = '\0';
	
	//init shell memory
	mem_init();
	while(1) {
        //if statement checking if we are in interactive mode
        if(isatty(STDIN_FILENO) && isatty(STDOUT_FILENO) && isatty(STDERR_FILENO)){
            //if so print the dollar sign
            printf("%c ",prompt);
        }
        //here you should check the unistd library 
        //so that you can find a way to not display $ in the batch mode
        //if we reached end of file without reading "quit"
        if(fgets(userInput, MAX_USER_INPUT - 1, stdin) == NULL){
            //consider stdin taken from interactive
            freopen("/dev/tty", "r", stdin);
        }else{
            errorCode = parseInput(userInput);
            if (errorCode == -1){
                exit(99); // ignore all other errors
            }
            memset(userInput, 0, sizeof(userInput));
        }
	}
}

//helper method that returns the number of commands in the input using ";"
//as the delimiter
int comm_num(char di[]){
    //we will always have at least one command
    int num_of_commands=1;
    for(int d=0;d<1000; d++){
        //if we reached the end of the input
        if (di[d]=='\n'){
            break;
        }
        //if there's a ";", then there must be at least one more command
        if(di[d]==';'){
            num_of_commands++;
        }
    }
    return num_of_commands;
}

int parseInput(char ui[]) {
    char tmp[200];
    char *words[100];                            
    int a = 0;
    int b;                            
    int w=0; // wordID
    int errorCode;
    //get number of commands in the input
    int num_of_commands =comm_num(ui);
    //if number of commands is not 1
    //seperate the commands in the input using ";" as the delimiter
    if (num_of_commands!=1){
        //2D array to hold the commands after seperating them
        char commands[num_of_commands][200];
        int i = 0;
        int j = 0;
        int k = 0;
        for (i = 0; i < strlen(ui); i++) {
            if (ui[i] != ';') {
                commands[j][k] = ui[i];
                k++;
            } else {
                commands[j][k] = '\0';
                j++;
                k = 0;
            }
        }
        commands[j][k] = '\0';
        for (i = 0; i < num_of_commands; i++) {
            //call parseInput recursively on each command after seperating them
            //parseInput calls interpreter on each command if the number of commands in the input is 1 
            parseInput(commands[i]);
        }

    }
    //if number of commands is 1, we call interpreter on the command
    else{
        //GIVEN AS STARTER CODE
        for(a=0; ui[a]==' ' && a<1000; a++);        // skip white spaces
        while(ui[a] != '\n' && ui[a] != '\0' && a<1000) {
            for(b=0; ui[a]!=';' && ui[a]!='\0' && ui[a]!='\n' && ui[a]!=' ' && a<1000; a++, b++){
                tmp[b] = ui[a];                        
                // extract a word
            }
            tmp[b] = '\0';
            words[w] = strdup(tmp);
            w++;
            if(ui[a] == '\0') break;
            a++; 
        }
        errorCode = interpreter(words, w);
        return errorCode;
    }
}
