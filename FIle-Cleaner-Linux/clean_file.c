#include <stdio.h>	
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <ctype.h>


void remove_comments(FILE *filePoint1, FILE *filePoint2) {
	char c;
	int end_check = 0;
	c = fgetc(filePoint1); 
	//the if while loop is quite awful but gets the job done
	while (c != EOF) { 
		if (c == '/') {
			c = fgetc(filePoint1);
			//checks for single line comment
			if (c == '/') {
				c = fgetc(filePoint1);
				while ( c != '\n') {
					c = fgetc(filePoint1);
				}
			}
			//checks for longer comment
			else if (c == '*') {
				c = fgetc(filePoint1);
				//finds the end of the comment
				while ( end_check != 1 && c != EOF) {
					if ( c == '*') {
						c = fgetc(filePoint1);
						if (c == '/') {
							end_check = 1;
						}
					}
					c = fgetc(filePoint1);
				}
				end_check = 0;
				if (c == EOF) {
					printf("Your file is missing an end comment, so the clean file might be corrupted \n");
				}
			}
		}
		//if there is no comments, writes the thing to file
		else {
		fputc(c, filePoint2); 
		c = fgetc(filePoint1); 
		}
	} 
}

void remove_emptylines(FILE *filePoint1, FILE *filePoint2) {
	char buffer[256];
	int emptyline = 0, i = 0;
	//goes one line at a time checking if there is an empty line
	while (fgets(buffer, sizeof(buffer), filePoint1)) {
		i = 0;
		emptyline = 0;
		while (buffer[i] != '\n') {
			//if there is no empty line, allows the next loop to write to file
			if ( isspace(buffer[i]) == 0) {
				emptyline = 1;
			}
			i++; 
		}
		//writes the buffer to file
		if (emptyline == 1) {
			i = 0;
			while (buffer[i] != '\n') {
				fputc(buffer[i], filePoint2);
				i++;
			}
			fputc('\n', filePoint2);
		}	
	}  
}

int clean_file(char *file_name) {
	//setting variables
	FILE *source, *temp, *final;
	char temp_name[60] = "temp_";
	//making a temp file to have a certain name
	strcat(temp_name,file_name);
	//opening source file
	source = fopen(file_name, "r");
	if (source == NULL) {
		perror("Opening file failed");
		return 0;
	}
	//locking the source file
	flock(fileno(source), 1);
	//opening temp file and removing comments
	temp = fopen(temp_name, "w");
	remove_comments(source, temp);
	//closing source file + temp and opening lock
	flock(fileno(source), 8);
	fclose(source);
	fclose(temp);
	//making the cleaned file have a .clean at the end
	strcat(file_name, ".clean");
	//opening the temp file to delete empty lines
	temp = fopen(temp_name, "r");
	final = fopen(file_name, "w");
	if (final == NULL) {
		perror("Creating a clean version failed");
		return 0;
	}
	remove_emptylines(temp, final);
	//closing the files
	fclose(temp);
	fclose(final);
	
	if (remove(temp_name) != 0) {
		perror("removing the temp file was not successful");
		return 0;
	}
	//returning 1 if all is good
	return 1;
}
