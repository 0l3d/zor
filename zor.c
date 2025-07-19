#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const char help[] = "zor - Simple XOR encryption utility.\n\n"
                    "usage: zor [arguments]\n\n"
                    "arguments: \n"
                    "-h: this message.\n"
                    "-p: set encryption password.\n"
                    "-f: encrypt a file (zorfile)\n"
                    "-d: print hexdump table for a file\n";

void xor(char *string, int len, char key) {
  for (int i = 0; i < len; i++) {
    string[i] = string[i] ^ key;
  }
}

void zor(char *string, int len, char *password) {
  int passwordlen = strlen(password);
  for (int i = 0; i < passwordlen; i++) {
    xor(string, len, password[i]);
  }
}

void zor_file(char *file_path, char *password) {
  FILE *file = fopen(file_path, "r");
  char line[4096];
  if (file != NULL) {
    char output_filename[50];
    snprintf(output_filename, sizeof(output_filename), "%s.zor", file_path);
    FILE *ptr = fopen(output_filename, "wb");
    if (ptr == NULL) {
      perror("write file is failed");
      fclose(ptr);
      return;
    }
    while (fgets(line, sizeof(line), file)) {
      int len = strlen(line);
      zor(line, len, password);
      fwrite(line, len, 1, ptr);
    }
    fclose(file);
    fclose(ptr);
  } else {
    perror("file read failed");
    return;
  }
}

#define OFFSET 16

void hexdump(const char *filepath) {
  char buff[OFFSET] = "";
  int i;
  int read;
  int address = 0;
  FILE *ptr = fopen(filepath, "rb");

  if (ptr == 0) {
    perror("hexdump failed");
    return;
  }

  while ((read = fread(buff, 1, sizeof(buff), ptr)) > 0) {
    printf("%2x | ", address);
    address += OFFSET;

    for (i = 0; i < OFFSET; i++) {
      if (i >= read || buff[i] == 0) {
        printf("   ");
      } else {
        printf("%02hhx ", (unsigned char)buff[i]);
      }
    }

    printf("| ");
    for (i = 0; i < OFFSET; i++) {
      printf("%c", (buff[i] >= ' ' && buff[i] <= '~' ? buff[i] : ' '));
    }
    printf("\n");
  }
  fclose(ptr);
}

struct Options {
  char *password;
  char *data;
};

struct Options opts;

int main(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "hp:d:f:")) != -1) {
    switch (opt) {
    case 'h':
      printf("%s", help);
      break;
    case 'p':
      opts.password = optarg;
      break;
    case 'd':
      hexdump(optarg);
      break;
    case 'f':
      if (opts.password != NULL)
        zor_file(optarg, opts.password);
      else {
        printf("you must be select a password with : -p");
        return -1;
      }
      break;
    }
  }
  return 0;
}
