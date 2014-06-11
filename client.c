#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sysexits.h>
#include <unistd.h>

#include "evento.h"

char * program_name;
const int no_sock = -1;

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

  struct addrinfo hints;
  struct addrinfo * results;
  struct sockaddr_in direccion_local;

  memset(&direccion_local, 0, sizeof(struct sockaddr_in));
  direccion_local.sin_family = AF_INET;
  if (NULL != puerto_local) {
    int puerto_numerico;
    if ( 1 != sscanf(puerto_local, "%d", &puerto_numerico)) {
      fprintf(stderr, "El puerto local debe ser un número decimal.\n");
      exit_usage(EX_USAGE);
    }
    direccion_local.sin_port = htons(puerto_numerico);
  }
  direccion_local.sin_addr.s_addr = INADDR_ANY;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_addr      = NULL;
  hints.ai_canonname = NULL;
  hints.ai_family    = AF_INET;
  hints.ai_flags     = AI_NUMERICSERV;
  hints.ai_next      = NULL;
  hints.ai_protocol  = getprotobyname("TCP")->p_proto;
  hints.ai_socktype  = SOCK_STREAM;

  {
    int i;
    if ((i = getaddrinfo(servidor, puerto, &hints, &results)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(i));
      exit(EX_OSERR);
    }
  }

  int socket_servidor;
  for (struct addrinfo * result = results; result != NULL; result = result->ai_next) {
    if ((socket_servidor = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1) {
      socket_servidor = no_sock;
      continue;
    }

    if (bind(socket_servidor, (struct sockaddr *)&direccion_local, sizeof(direccion_local)) == -1) {
      close(socket_servidor);
      socket_servidor = no_sock;
      continue;
    }

    if (connect(socket_servidor, result->ai_addr, result->ai_addrlen) == -1) {
      close(socket_servidor);
      socket_servidor = no_sock;
      continue;
    }

    break;
  }

  if (no_sock == socket_servidor) {
    fprintf(stderr, "No se pudo establecer la conexión con el servidor");
    exit(EX_NOHOST);
  }

  enviar(socket_servidor, evento(12345, 67890, 8));

  exit(EX_OK);
}
