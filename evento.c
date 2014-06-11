#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <time.h>
#include <unistd.h>

#include "evento.h"

const char * to_s_te(enum tipo_evento tipo_evento) {
  switch (tipo_evento) {
    case TM_COMMUNICATION_OFFLINE            : return "Communication offline"            ;
    case TM_COMMUNICATION_ERROR              : return "Communication error"              ;
    case TM_LOW_CASH_ALERT                   : return "Low cash alert"                   ;
    case TM_RUNNING_OUT_OF_NOTES_IN_CASSETTE : return "Running out of notes in cassette" ;
    case TM_EMPTY                            : return "Empty"                            ;
    case TM_SERVICE_MODE_ENTERED             : return "Service mode entered"             ;
    case TM_SERVICE_MODE_LEFT                : return "Service mode left"                ;
    case TM_DEVICE_DID_NOT_ANSWER_AS_EXPECTED: return "Device did not answer as expected";
    case TM_THE_PROTOCOL_WAS_CANCELLED       : return "The protocol was cancelled"       ;
    case TM_LOW_PAPER_WARNING                : return "Low paper warning"                ;
    case TM_PRINTER_ERROR                    : return "Printer error"                    ;
    case TM_PAPER_OUT_CONDITION              : return "Paper-out condition"              ;
    case TM_SUCCESS                          : return "Success"                          ;
    default: assert(0);
  }
}

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

void enviar(int socket, struct evento evento) {
  escribir(socket, &evento.origen , sizeof(evento.origen ));
  escribir(socket, &evento.destino, sizeof(evento.destino));
  escribir(socket, &evento.fecha  , sizeof(evento.fecha  ));
  escribir(socket, &evento.tipo   , sizeof(evento.tipo   ));
}

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

struct evento recibir(int socket) {
  struct evento evento;
  leer(socket, &evento.origen , sizeof(evento.origen ));
  leer(socket, &evento.destino, sizeof(evento.destino));
  leer(socket, &evento.fecha  , sizeof(evento.fecha  ));
  leer(socket, &evento.tipo   , sizeof(evento.tipo   ));
  return evento;
}

struct evento evento
  ( uint32_t origen
  , uint32_t destino
  , enum tipo_evento tipo
  )
{
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  struct evento e =
    { .origen  = origen
    , .destino = destino
    , .fecha   = t.tv_sec
    , .tipo    = tipo
    }
  ;
  return e;
}
