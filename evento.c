/**
 * @file evento.c
 * @author Gabriela Limonta 10-10385
 * @author John Delgado 10-10196
 *
 * Contiene la implementación de las funciones que
 * permiten el manejo de mensajes de eventos entre
 * el SVR y el ATM.
 *
 */
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "evento.h"

/**
 * Se encarga de transformar un código de evento en su mensaje asociado.
 * @param tipo_evento código del evento.
 * @return retorna una representación en string del mensaje asociado al
 * código de mensaje.
 */
const char * to_s_te(enum tipo_evento tipo_evento) {
  switch (tipo_evento) {
    case TE_COMMUNICATION_OFFLINE            : return "Communication Offline"            ;
    case TE_COMMUNICATION_ERROR              : return "Communication error"              ;
    case TE_LOW_CASH_ALERT                   : return "Low Cash alert"                   ;
    case TE_RUNNING_OUT_OF_NOTES_IN_CASSETTE : return "Running Out of notes in cassette" ;
    case TE_EMPTY                            : return "empty"                            ;
    case TE_SERVICE_MODE_ENTERED             : return "Service mode entered"             ;
    case TE_SERVICE_MODE_LEFT                : return "Service mode left"                ;
    case TE_DEVICE_DID_NOT_ANSWER_AS_EXPECTED: return "device did not answer as expected";
    case TE_THE_PROTOCOL_WAS_CANCELLED       : return "The protocol was cancelled"       ;
    case TE_LOW_PAPER_WARNING                : return "Low Paper warning"                ;
    case TE_PRINTER_ERROR                    : return "Printer error"                    ;
    case TE_PAPER_OUT_CONDITION              : return "Paper-out condition"              ;
    default: assert(0);
  }
}

/**
 * Se encarga de transformar un string en su código de evento asociado.
 * @param mensaje string de mensaje del evento.
 * @return retorna el código de evento asociado al string de mensaje indicado.
 */
enum tipo_evento from_s_te(const char * mensaje) {
    if (0 == strcmp("Communication Offline"            , mensaje)) return TE_COMMUNICATION_OFFLINE            ;
    if (0 == strcmp("Communication error"              , mensaje)) return TE_COMMUNICATION_ERROR              ;
    if (0 == strcmp("Low Cash alert"                   , mensaje)) return TE_LOW_CASH_ALERT                   ;
    if (0 == strcmp("Running Out of notes in cassette" , mensaje)) return TE_RUNNING_OUT_OF_NOTES_IN_CASSETTE ;
    if (0 == strcmp("empty"                            , mensaje)) return TE_EMPTY                            ;
    if (0 == strcmp("Service mode entered"             , mensaje)) return TE_SERVICE_MODE_ENTERED             ;
    if (0 == strcmp("Service mode left"                , mensaje)) return TE_SERVICE_MODE_LEFT                ;
    if (0 == strcmp("device did not answer as expected", mensaje)) return TE_DEVICE_DID_NOT_ANSWER_AS_EXPECTED;
    if (0 == strcmp("The protocol was cancelled"       , mensaje)) return TE_THE_PROTOCOL_WAS_CANCELLED       ;
    if (0 == strcmp("Low Paper warning"                , mensaje)) return TE_LOW_PAPER_WARNING                ;
    if (0 == strcmp("Printer error"                    , mensaje)) return TE_PRINTER_ERROR                    ;
    if (0 == strcmp("Paper-out condition"              , mensaje)) return TE_PAPER_OUT_CONDITION              ;
    return TE_ERROR;
    assert(0);
}

/**
 * Se encarga de escribir en el file descriptor del socket el contenido
 * de buf.
 * @param fd file descriptor del socket donde se va a escribir.
 * @param buf contenido que se va a escribir en fd.
 * @param count tamaño del contenido que se va a escribir.
 */
void escribir(int fd, void * buf, size_t count) {
  while (count > 0) {
    int escrito = write(fd, buf, count);
    if (-1 == escrito) {
      if (EINTR == errno) continue;
      perror("write");
      exit(EX_IOERR);
    }

    count -= escrito;
    buf += escrito;
  }
}

/**
 * Se encarga de enviar un evento al socket.
 * @param socket file descriptor del socket al que se va a enviar.
 * @param evento evento que se va a enviar a través del socket.
 */
void enviar(int socket, struct evento evento) {
  escribir(socket, &evento.origen, sizeof(evento.origen));
  escribir(socket, &evento.fecha , sizeof(evento.fecha ));
  escribir(socket, &evento.tipo  , sizeof(evento.tipo  ));
  escribir(socket, &evento.serial, sizeof(evento.serial));
}

/**
 * Se encarga de leer del file descriptor del socket el contenido
 * de buf.
 * @param socket file descriptor del socket del que se va a leer.
 * @param buf direccion donde se almacenará lo leido en fd.
 * @param count tamaño del contenido que se va a leer.
 */
void leer(int socket, void * buf, size_t count) {
  while (count > 0) {
    int leido = read(socket, buf, count);

    if (-1 == leido) {
      if (EINTR == errno) continue;
      perror("read");
      exit(EX_IOERR);
    }

    count -= leido;
    buf += leido;
  }
}

/**
 * Se encarga de recibir un evento del socket.
 * @param socket file descriptor del socket del que se va a recibir.
 * @return retorna un struct evento con la información recibida.
 */
struct evento recibir(int socket) {
  struct evento evento;
  leer(socket, &evento.origen, sizeof(evento.origen));
  leer(socket, &evento.fecha , sizeof(evento.fecha ));
  leer(socket, &evento.tipo  , sizeof(evento.tipo  ));
  leer(socket, &evento.serial, sizeof(evento.serial));
  return evento;
}

/**
 * Se encarga de indicar si un evento es válido.
 * @param evento evento que se verificará.
 * @return retorna 0 si es un evento válido, retorna -1 si es inválido.
 */
int evento_valido(struct evento evento) {
  switch (evento.tipo) {
    case TE_COMMUNICATION_OFFLINE            :
    case TE_COMMUNICATION_ERROR              :
    case TE_LOW_CASH_ALERT                   :
    case TE_RUNNING_OUT_OF_NOTES_IN_CASSETTE :
    case TE_EMPTY                            :
    case TE_SERVICE_MODE_ENTERED             :
    case TE_SERVICE_MODE_LEFT                :
    case TE_DEVICE_DID_NOT_ANSWER_AS_EXPECTED:
    case TE_THE_PROTOCOL_WAS_CANCELLED       :
    case TE_LOW_PAPER_WARNING                :
    case TE_PRINTER_ERROR                    :
    case TE_PAPER_OUT_CONDITION              :
      return 1;

    default:
      return 0;
  }
}
