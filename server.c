#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>

char * program_name;

void exit_usage(int exit_code) {
  fprintf(
    stderr,
    "Uso: %s -l <puerto_svr_s> -b <archivo_bitácora>\n"
    "Opciones:\n"
    "-l <puerto_svr_s>: Número de puerto local en el que el módulo central atenderá la llamada.\n"
    "-b <archivo_bitácora>: Nombre y dirección relativa o absoluta de un archivo de texto que realiza operaciones de bitácora.\n",
    program_name
  );
  exit(exit_code);
}

int main(int argc, char ** argv) {
  char opt;
  char * puerto = NULL;
  char * bitacora = NULL;
  program_name = argv[0];

  while ((opt = getopt(argc, argv, "l:b:")) != -1) {
    switch (opt) {
      case 'l':
        puerto = optarg;
        break;
      case 'b':
        bitacora = optarg;
        break;
      default:
        exit_usage(EX_USAGE);
    }
  }

  if (NULL == puerto) {
    fprintf(stderr, "El número de puerto local es obligatorio.\n");
    exit_usage(EX_USAGE);
  }
  if (NULL == bitacora) {
    fprintf(stderr, "El nombre del archivo bitácora es obligatorio.\n");
    exit_usage(EX_USAGE);
  }
}
