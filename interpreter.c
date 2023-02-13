#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "shellmemory.h"
#include "shell.h"
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>

int MAX_ARGS_SIZE = 3;

int badcommand(){
	printf("%s\n", "Unknown Command");
	return 1;
}

// For run command only
int badcommandFileDoesNotExist(){
	printf("%s\n", "Bad command: File not found");
	return 3;
}
int badcommand_set(){
	printf("%s\n", "Bad command: Too many tokens");
	return 1;
}

int help();
int quit();
int set(char* var, char* value);
int print(char* var);
int run(char* script);
int badcommandFileDoesNotExist();

// Interpret commands and their arguments
int interpreter(char* command_args[], int args_size){
	int i;
	if (args_size<1){
		return badcommand();
	}
	for ( i=0; i<args_size; i++){ //strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	//this condition is not working in badcommand.txt
	} if ((args_size > MAX_ARGS_SIZE) && (strcmp(command_args[0], "set")!=0))
	{
		return badcommand_set();
	
	}if (strcmp(command_args[0], "help")==0){
	    //help
	    if (args_size != 1) return badcommand();
	    return help();
	
	} else if (strcmp(command_args[0], "quit")==0) {
		//quit
		if (args_size != 1) return badcommand();
		return quit();

	} else if (strcmp(command_args[0], "set")==0) {
		//set
		if (args_size >7) return badcommand_set();	
		char res[1000] = "";
		for (int i = 2; i < args_size; i++){
			strcat(res,command_args[i]);
			strcat(res, " ");
			//strncpy(val, mem_get_value(name1), sizeof(val));
		}
		return set(command_args[1],res);
	
	} else if (strcmp(command_args[0], "print")==0) {
		if (args_size != 2) return badcommand();
		return print(command_args[1]);
	
	} else if (strcmp(command_args[0], "run")==0) {
		if (args_size != 2) return badcommand();
		return run(command_args[1]);

	//adding the echo command	
	}else if (strcmp(command_args[0], "echo")==0) 
	{
		char string1[100];
		//checking if first character is a dollar sign
        if(strchr(command_args[1],'$')!=NULL)
        {
			//copying the string
            strcpy(string1, command_args[1]+1);

            //check if it's in the shell memory
            if (strcmp(mem_get_value(string1),"Variable does not exist")== 0)
			{
                printf("\n");
				return 0;	

            }else //if it's not oin the shell memory
			{
                print(string1);
				return 0;
            }
        }else
		{
            printf("%s\n",command_args[1]);
            return 0;
        }
	//adding ls command
	
	}else if (strcmp(command_args[0], "my_ls")==0){
		int compare(const void **a, const void **b) {
			return strcmp((const char **)a, (const char **)b);
		}
		DIR *directory = opendir("."); //opening current directory
		struct dirent *content;
		int num_entries = 0;
		char *files[100];

		while ((content =readdir(directory)) != NULL){ //looping through files/directories
			if (strcmp(content->d_name, ".") != 0 && strcmp(content->d_name, "..") != 0) {
				files[num_entries]= content->d_name; //adding names to the arraay
				num_entries ++;
			}
		}
		qsort(files, num_entries, sizeof(char *), compare); //sorting the files and directories
		for (int j = 0; j < num_entries; j++) { //looping through the array to display each file on a separate line
        	printf("%s\n", files[j]);
    	}
		closedir(directory);
		return 0;
	}else if (strcmp(command_args[0],"my_mkdir")==0)
    {

        //if the first argument does not contain a dollar sign
        if(strchr(command_args[1],'$')==NULL)
        {
			char curr[] = "/";
			strcat(curr,command_args[1]); //creating a pathanme
            int my_mkdir;
            my_mkdir = mkdir(command_args[1],S_IRWXU);
            return 0;
        }else 
		{
			char name1[100];
			strcpy(name1, command_args[1]+1);
			int contains_space = 0;
			char val[100];
			//extracting the value using the helper function
			//strcpy(val,mem_get_value(name1)); olddd
			strncpy(val, mem_get_value(name1), sizeof(val));
  			val[sizeof(mem_get_value(name1)) - 1] = '\0';
			//checks if it's only one string
			for (int i = 0; i < strlen(val) -1; i++) {  
				if (isspace(val[i]))  {
					contains_space = 1;
				}
			}
            //check if it's in the shell memory and is o1nly a single string
            if ((strcmp(mem_get_value(name1),"Variable does not exist")!= 0) && (contains_space == 0))
			{
				int int_mkdir;
				int_mkdir = mkdir(val,S_IRWXU);
				return 0;
			}else{
				printf("Bad command: my_mkdir\n");
				return 0;
			}
		}
	//creating the my_touch command
	} else if (strcmp(command_args[0],"my_touch")==0)
	{
		//creating a file with the filename
		FILE *file;
		char curr[] = "./";
		strcat(curr,command_args[1]); //creating a pathanme
		file = fopen(curr, "w"); //old was to use command_args[1] instead of curr
		return 0;
	}else if (strcmp(command_args[0],"my_cd")==0) {
		//use chdir command to go to existing directory 
		int dir = chdir(command_args[1]);
		//if directory does not exist
		if (dir!=0){
			printf("Bad command: my_cd\n");
			return 0;
		}
	}
	else return badcommand();
	
}

int help(){

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
	printf("%s\n", help_string);
	return 0;
}

int quit(){
	printf("%s\n", "Bye!");
	exit(0);
}

int set(char* var, char* value){
	char *link = "=";
	char buffer[1000];
	strcpy(buffer, var);
	strcat(buffer, link);
	strcat(buffer, value);

	mem_set_value(var, value);

	return 0;

}

int print(char* var){
	printf("%s\n", mem_get_value(var)); 
	return 0;
}

int run(char* script){
	int errCode = 0;
	char line[1000];
	FILE *p = fopen(script,"rt");  // the program is in a file

	if(p == NULL){
		return badcommandFileDoesNotExist();
	}

	fgets(line,999,p);
	while(1){
		errCode = parseInput(line);	// which calls interpreter()
		memset(line, 0, sizeof(line));

		if(feof(p)){
			break;
		}
		fgets(line,999,p);
	}

    fclose(p);

	return errCode;
}
