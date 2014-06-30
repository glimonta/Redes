/**
 * @file client.c
 * @author Gabriela Limonta 10-10385
 * @author John Delgado 10-10196
 *
 * Contiene la implementación del cliente ATM.
 *
 */
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sysexits.h>
#include <time.h>
#include <unistd.h>

#include "evento.h"

char * program_name;    // Nombre del programa.
const int no_sock = -1; // Indica no socket.
uint32_t origen;        // Numero del ATM.

// Argumentos de línea de comandos.
char * servidor     = NULL;
char * puerto       = NULL;
char * puerto_local = NULL;

pthread_mutex_t mutex_socket = PTHREAD_MUTEX_INITIALIZER; // Mutex para el socket.

/**
 * Se encarga de imprimir un mensaje de error cuando el usuario
 * se equivoca en la invocación del ATM y abortar la ejecución del
 * programa con el mensaje de error indicado.
 * @param exit_code codigo de error con el que se saldrá del programa.
 */
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

/**
 * Se encarga de establecer la conexion con el servidor para luego
 * ejecutar una función con unos datos dados.
 * @param servidor nombre o direccion IP del servidor al que queremos conectarnos.
 * @param puerto puerto en el que queremos conectarnos.
 * @param puerto_local numero de puerto local que se utiliza para la comunicación.
 * @param f función a aplicar.
 * @param datos datos que se le pasan a la función f.
 */
void * with_server(char * servidor, char * puerto, char * puerto_local, void * (*f)(int, void *), void * datos) {
  struct addrinfo hints;
  struct addrinfo * results;
  struct sockaddr_in direccion_local;

  // Llenamos los campos de la direccion local y el puerto, si se especifica.
  memset(&direccion_local, 0, sizeof(struct sockaddr_in));
  direccion_local.sin_family = AF_INET; // Direcciones IPv4
  // Si se especifica el puerto local se busca cual es y se
  // convierte a network byte order.
  if (NULL != puerto_local) {
    int puerto_numerico;
    if (1 != sscanf(puerto_local, "%d", &puerto_numerico)) {
      fprintf(stderr, "El puerto local debe ser un número decimal.\n");
      exit_usage(EX_USAGE);
    }
    direccion_local.sin_port = htons(puerto_numerico);
  }
  direccion_local.sin_addr.s_addr = INADDR_ANY;

  // Llenamos los hints necesarios para buscar la(s) direccion(es) que
  // usaremos para intentar conectarnos.
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_addr      = NULL;
  hints.ai_canonname = NULL;
  hints.ai_family    = AF_INET;                       // Queremos direcciones de la familia IPv4.
  hints.ai_flags     = AI_NUMERICSERV;                // Indicamos que vamos a usar direcciones numericas.
  hints.ai_next      = NULL;
  hints.ai_protocol  = getprotobyname("TCP")->p_proto;// Protocolo TCP.
  hints.ai_socktype  = SOCK_STREAM;                   // Socket basado en conexiones.

  {
    // Buscamos la(s) direccion(es) a las que posiblemente podamos conectarnos.
    int i;
    if ((i = getaddrinfo(servidor, puerto, &hints, &results)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(i));
      exit(EX_OSERR);
    }
  }

  // Recorremos la lista de posibles direcciones que retorna getaddrinfo.
  int socket_servidor = no_sock;
  for (struct addrinfo * result = results; result != NULL; result = result->ai_next) {
    // Si no podemos crear un socket entonces le asignamos a esa dirección el valor de no_sock y continuamos revisando.
    if ((socket_servidor = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1) {
      perror("socket");
      socket_servidor = no_sock;
      continue;
    }

    {
      int on = 1;

      // Intentamos asignar opciones al socket, si falla, cerramos la conexion, asignamos no_sock y continuamos revisando
      // en caso contrario le asignamos la opcion SO_REUSEADDR a nivel de la API de sockets que
      // permite reutilizar las direccioens locales al hacer bind.
      if (-1 == setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
        perror("setsockopt");
        close(socket_servidor);
        socket_servidor = no_sock;
        continue;
      }
    }

    // Intentamos hacer bind con el servidor. Si falla, cerramos el servidor, asignamos no_sock y continuamos revisando,
    // sino, se logra "bindear" exitosamente
    if (bind(socket_servidor, (struct sockaddr *)&direccion_local, sizeof(direccion_local)) == -1) {
      perror("bind");
      close(socket_servidor);
      socket_servidor = no_sock;
      continue;
    }

    // Intentamos conectarnos al servidor. Si falla, cerramos la conexion, asignamos no_sock y continuamos revisando.
    if (connect(socket_servidor, result->ai_addr, result->ai_addrlen) == -1) {
      perror("connect");
      close(socket_servidor);
      socket_servidor = no_sock;
      continue;
    }

    break;
  }

  // Si el socket del servidor es no_sock no se pudo establecer la conexion.
  if (no_sock == socket_servidor) {
    fprintf(stderr, "No se pudo establecer la conexión con el servidor.\n");
    exit(EX_NOHOST);
  }

  // En caso contrario ejecutamos la funcion f con sus datos.
  void * ret = f(socket_servidor, datos);
  // Cerramos el socket del servidor y retornamos el retorno de la función f.
  close(socket_servidor);
  return ret;
}

/**
 * Se encarga de enviar un evento al servidor.
 * @param socket_servidor file descriptor del socket del servidor.
 * @param datos datos que se le envian al servidor (esto será un struct evento)
 * @param retorna NULL si funciona adecuadamente.
 */
void * enviar_evento(int socket_servidor, void * datos) {
  enviar(socket_servidor, *(struct evento *)datos);
  return NULL;
}

/**
 * Se encarga de enviar un hearbeat cada minuto al servidor para indicar que el
 * cliente está vivo.
 * @param datos parametro no utilizado, solo existe para adaptarse a la firma necesaria
 * para funciones de hilos.
 * @return retorno no utilizado, solo existe para adaptarse a la firma necesaria para
 * funciones de hilos.
 */
void * enviar_heartbeat(void * datos) {
  (void)datos;
  while (1) {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);

    // Creamos un evento heartbeat
    struct evento evento =
      { .origen = origen
      , .fecha  = t.tv_sec
      , .tipo   = TE_HEARTBEAT
      , .serial = rand()
      }
    ;

    int s = pthread_mutex_lock(&mutex_socket);
    if (s != 0) {
      // Si el código del programa está bien, esto nunca debería suceder.  Sin embargo, esta verificación puede ayudar a detectar errores de programación.
      errno = s;
      perror("Error intentando entrar en la sección crítica del recibidor de eventos; pthread_mutex_lock");
      exit(EX_SOFTWARE);
    }
    { // Sección Crítica
      // Enviamos el evento creado al servidor.
      with_server(servidor, puerto, puerto_local, enviar_evento, &evento);
    }

    s = pthread_mutex_unlock(&mutex_socket);
    if (s != 0) {
      // Si el código del programa está bien, esto nunca debería suceder.  Sin embargo, esta verificación puede ayudar a detectar errores de programación.
      errno = s;
      perror("Error intentando salir de la sección crítica del recibidor de eventos; pthread_mutex_unlock");
      exit(EX_SOFTWARE);
    }

    sleep(1); //FIXME Debe enviar cada minuto, es mas facil para probar con un segundo.
  }
}

/**
 * Se encarga de recibir constantemente eventos por la entrada estándar.
 * @param datos parametro no utilizado, solo existe para adaptarse a la firma necesaria
 * para funciones de hilos.
 * @return retorno no utilizado, solo existe para adaptarse a la firma necesaria para
 * funciones de hilos.
 */
void * recibir_eventos(void * datos) {
  (void)datos;
  // Parseamos la entrada estándar. Los eventos deben tener el siguiente formato:
  // <mensaje del evento>
  while (1) {
    char * mensaje;
    int i;
    errno = 0;
    switch (i = scanf(" %m[^\n]", &mensaje)) {
      case EOF:
        if (0 == errno) exit(EX_OK);
        perror("scanf");
        exit(EX_IOERR);

      default:
        fprintf(stderr, "El formato de la entrada es inadecuado :(\n");
        exit(EX_DATAERR);

      case 1:
        break;
    }

    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);

    // Creamos un evento a partir de la información tomada anteriormente.
    struct evento evento =
      { .origen = origen
      , .fecha  = t.tv_sec
      , .tipo   = from_s_te(mensaje)
      , .serial = rand()
      }
    ;

    int s = pthread_mutex_lock(&mutex_socket);
    if (s != 0) {
      // Si el código del programa está bien, esto nunca debería suceder.  Sin embargo, esta verificación puede ayudar a detectar errores de programación.
      errno = s;
      perror("Error intentando entrar en la sección crítica del recibidor de eventos; pthread_mutex_lock");
      exit(EX_SOFTWARE);
    }
    { // Sección Crítica
      // Enviamos el evento creado al servidor.
      with_server(servidor, puerto, puerto_local, enviar_evento, &evento);
    }

    s = pthread_mutex_unlock(&mutex_socket);
    if (s != 0) {
      // Si el código del programa está bien, esto nunca debería suceder.  Sin embargo, esta verificación puede ayudar a detectar errores de programación.
      errno = s;
      perror("Error intentando salir de la sección crítica del recibidor de eventos; pthread_mutex_unlock");
      exit(EX_SOFTWARE);
    }

    // Liberamos el espacio de memoria del string mensaje.
    free(mensaje);
  }
}

/**
 * Es el main del programa se encarga de parsear los argumentos de linea de comandos y
 * de leer de entrada estándar los eventos que ocurren en el ATM para enviarlos al servidor.
 */
int main(int argc, char ** argv) {
  char opt;

  { // Usamos esto como semilla para el random.
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    srand(t.tv_nsec);
  }

  origen = rand();
  program_name = argv[0];

  // Parseamos los argumentos -d, -p y -l de la linea de comandos
  // Si se pone alguna opción que no sea una de las anteriores emitimos un error
  // para indicarle la correcta invocación al usuario.
  while ((opt = getopt(argc, argv, "d:p:l:")) != -1) {
    switch (opt) {
      case 'd': servidor     = optarg; break;
      case 'p': puerto       = optarg; break;
      case 'l': puerto_local = optarg; break;
      default:
        exit_usage(EX_USAGE);
    }
  }

  // Si no se indicó el servidor se emite un mensaje de error.
  if (NULL == servidor) {
    fprintf(stderr, "El nombre o dirección IP del servidor es obligatorio.\n");
    exit_usage(EX_USAGE);
  }
  // Si no se indicó el puerto se emite un mensaje de error.
  if (NULL == puerto) {
    fprintf(stderr, "El número del puerto remoto es obligatorio.\n");
    exit_usage(EX_USAGE);
  }

  pthread_t marcapasos, manejador;
  int s;

  // Creamos los hilos marcapasos y manejador.
  s = pthread_create(&marcapasos, NULL, &enviar_heartbeat, NULL);
  if (s != 0) {
    errno = s;
    perror("No fue posible crear hilo marcapasos; pthread_create: ");
    exit(EX_OSERR);
  }

  s = pthread_create(&manejador, NULL, &recibir_eventos, NULL);
  if (s != 0) {
    errno = s;
    perror("No fue posible crear hilo marcapasos; pthread_create: ");
    exit(EX_OSERR);
  }

  // Esperamos a que el manejador termine.
  // Si hay un error se imprime un mensaje.
  if (0 != pthread_join(manejador, NULL)) {
    perror("pthread_join");
    exit(EX_OSERR);
  }

  exit(EX_OK);
}
