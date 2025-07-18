#include <stdio.h>
#include <stdlib.h>
#define MAX_LEN 100

int main() {
  // Definir FILESTREAM
  FILE *inputstream = fopen("cmd.txt", "r");
  // Buffer que almacenara el comando que abriremos
  char line[MAX_LEN];

  // fprintf(stdout, "Introduce un comando a ejecutar: ");
  fgets(line, MAX_LEN, inputstream);

  for (;;) {
    system(line);
  }

  fclose(inputstream);
  return 0;
}
