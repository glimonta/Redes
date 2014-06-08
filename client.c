#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>

char * program_name;

void exit_usage(int exit_code) {
  fprintf(
    stderr,
    "Uso: %s -d <nombre_módulo_central> -p <puerto_svr_s> [-l <puerto_local>]\n"
    "Opciones:\n"
    "-d <nombre_módulo_central>: Nombre del dominio o dirección IP (versión 4) del equipo donde se deberá ejecutar el módulo central del SVR.\n"
    "-p <puerto_svr_s>: Número de puerto remoto en el que el módulo central atenderá la llamada.\n"
    "-l <puerto_local>: Número de puerto local que el software utilizará para comunicarse con el módulo central.\n",
    program_name
  );
  exit(exit_code);
}

int main(int argc, char ** argv) {
  char opt;
  char * servidor     = NULL;
  char * puerto       = NULL;
  char * puerto_local = NULL;

  program_name = argv[0];

  while ((opt = getopt(argc, argv, "d:p:l:")) != -1) {
    switch (opt) {
      case 'd': servidor     = optarg; break;
      case 'p': puerto       = optarg; break;
      case 'l': puerto_local = optarg; break;
      default:
        exit_usage(EX_USAGE);
    }
  }

  if (NULL == servidor) {
    fprintf(stderr, "El nombre o dirección IP del servidor es obligatorio.\n");
    exit_usage(EX_USAGE);
  }
  if (NULL == puerto) {
    fprintf(stderr, "El número del puerto remoto es obligatorio.\n");
    exit_usage(EX_USAGE);
  }

  exit(EX_OK);
}
