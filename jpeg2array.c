#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define ANSI_COLOR_RED     "\x1b[31;1m"
#define ANSI_COLOR_GREEN   "\x1b[32;1m"
#define ANSI_COLOR_BLUE    "\x1b[34;1m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void logError(char* error) {
  fprintf(stderr, ANSI_COLOR_RED "error: " ANSI_COLOR_RESET "%s",
    error);
}

void logInfo(char* info) {
  fprintf(stdout, ANSI_COLOR_BLUE "log: " ANSI_COLOR_RESET "%s",
    info);
}

void strUpper(char * temp) {
  while (*temp) {
    *temp = toupper((unsigned char) *temp);
    temp++;
  }
}

void printUsage() {
  printf("Convert jpeg image to C/C++ array for use with arduino (default) or stm32.\n");
  printf("Usage: ./jpeg2array jpeg_filename [options]\n");
  printf("Options:\n--stm32\t\tGenerate array for stm32\n");
}

int main(int argc, char *argv[]){
  FILE *jpegFile;
  FILE *arrayFile;
  unsigned char data;
  char line_len = 0;
  int mode = 0; // 0 - arduino, 1 - stm32

  if ((argc < 2) || (argc > 3)) {
    if (argc > 3)
      logError("Wrong number of arguments!\n");
    printUsage();
    return 1;
  }

  if (argc == 3) {
    if (strcmp(argv[2],"--stm32") == 0)
      mode = 1;
    else {
      logError("Second argument is invalid!\n");
      printUsage();
      return 1;
    }
  }

  if (access(argv[1], R_OK) != -1) {
    jpegFile = fopen(argv[1], "r");
    logInfo("Opened ");
    fprintf(stdout, "%s\n",argv[1]);

    //Extract name of jpeg file
    char *ptr, *filename, *filename_header, *arrayname, c;
    int i;

    ptr = strrchr(argv[1], '/');  // Find last slash in filename
    if(ptr) ptr++;                // First character of filename (path stripped)
    else    ptr = argv[1];        // No path; font in local dir.

    filename = malloc(strlen(ptr)+2);
    strcpy(filename, ptr);
    ptr = strrchr(filename, '.'); // Find last period (file ext)

    // Space and punctuation chars in name replaced w/ underscores.
    for(i=0; (c=filename[i]); i++) {
      if(isspace(c) || ispunct(c)) filename[i] = '_';
    }

    // Make array name
    arrayname = malloc(strlen(filename));
    strcpy(arrayname, filename);

    // Make file name
    strcat(filename,".h");

    // Make header name
    filename_header = malloc(strlen(filename)+2);
    strcpy(filename_header, "__");
    strcat(filename_header, filename);
    filename_header[strlen(filename_header)-2] = '_';
    strUpper(filename_header);

    // Print log info
    logInfo("Creating header file ");
    printf("%s for ", filename);
    const char * target = mode == 0 ? "arduino" : "stm32";
    printf("%s\n", target);
    arrayFile = fopen(filename,"w");

    fputs("/*************************************************\n", arrayFile);
    fputs(" * File made by jpeg2array                       *\n", arrayFile);
    fputs(" * Authors: Rohit Gujarathi and Arthur Golubtsov *\n", arrayFile);
    fputs(" *************************************************/\n\n", arrayFile);

    if (mode == 0) {
      fputs("#include <pgmspace.h>\n\n", arrayFile);
      fputs("const uint8_t ", arrayFile); fputs(arrayname, arrayFile); fputs("[] PROGMEM = {\n", arrayFile);
    } else {
      fputs("#ifndef ", arrayFile); fputs(filename_header, arrayFile); fputs("\n", arrayFile);
      fputs("#define ", arrayFile); fputs(filename_header, arrayFile); fputs("\n\n", arrayFile);
      fputs("const char ", arrayFile); fputs(arrayname, arrayFile); fputs("[] = {\n", arrayFile);
    }

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
    fputs("\n}",arrayFile);
    if (mode == 1) {
      fputs(";\n\n#endif", arrayFile);
    }
    fputs("\n", arrayFile);

    fprintf(stdout,"\n");
    fclose(arrayFile);
    fclose(jpegFile);
  } else {
    logError("File does not exist or read permission not granted.\n");
  }
  return 0;
}