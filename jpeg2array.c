#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define ANSI_COLOR_RED     "\x1b[31;1m"
#define ANSI_COLOR_GREEN   "\x1b[32;1m"
#define ANSI_COLOR_BLUE    "\x1b[34;1m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void logError(char* error){
  fprintf(stderr, ANSI_COLOR_RED "\nerror: " ANSI_COLOR_RESET "%s",
    error);
}

void logInfo(char* info){
  fprintf(stdout, ANSI_COLOR_BLUE "\nlog: " ANSI_COLOR_RESET "%s",
    info);
}

int main(int argc, char *argv[]){
  FILE *jpegFile;
  FILE *arrayFile;  
  unsigned char data;
  long int offset=0;
  char line_len = 0;

  

  if(argc < 2) {
    logError("Usage: ./jpeg2array jpeg_filename\n");
    return 1;
  }

  if(access(argv[1],R_OK) != -1){
    jpegFile = fopen(argv[1], "r");
    logInfo("Opened ");
    fprintf(stdout, "%s\n",argv[1]);
    
    //Extract name of jpeg file
    char *ptr,*filename,c;
    int i;

    ptr = strrchr(argv[1], '/'); // Find last slash in filename
    if(ptr) ptr++;         // First character of filename (path stripped)
    else    ptr = argv[1]; // No path; font in local dir.
    
    filename = malloc(strlen(ptr));
    strcpy(filename, ptr);
    ptr = strrchr(filename, '.'); // Find last period (file ext)
    
    filename[(strlen(filename)-strlen(ptr))] = '\0';
    //Space and punctuation chars in name replaced w/ underscores.  
    for(i=0; (c=filename[i]); i++) {
      if(isspace(c) || ispunct(c)) filename[i] = '_';
    }

    strcat(filename,".h");
    logInfo("Creating header file\n");
    arrayFile = fopen(filename,"w");
    fputs("//File made by jpeg2array\n\r",arrayFile);
    fputs("//Author: Rohit Gujarathi\n\r",arrayFile);
    fputs("#include <pgmspace.h>\n\r",arrayFile);
    fputs("const uint8_t ",arrayFile); fputs(filename,arrayFile); fputs("[] PROGMEM = {\n\r",arrayFile);
    
    fseek(jpegFile,0,SEEK_SET);
    while(1){
      if( feof(jpegFile) ) {
        break ;
      }
      data = getc(jpegFile);
      if(line_len >= 32){
        line_len = 0;
        fprintf(arrayFile, "\n");
        fprintf(stdout,".");
      } 
      fprintf(arrayFile,"0x%02X,",data);
      line_len++;  
    }
    fputs("}",arrayFile);
    fprintf(stdout,"\n");
    fclose(arrayFile);
    fclose(jpegFile);    
  }else{
    logError("File does not exist or read permission not granted.\n");
  }

  return 0;
}