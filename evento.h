enum tipo_evento
  { TE_ERROR                             = -1
  , TE_COMMUNICATION_OFFLINE             = 1
  , TE_COMMUNICATION_ERROR               = 2
  , TE_LOW_CASH_ALERT                    = 3
  , TE_RUNNING_OUT_OF_NOTES_IN_CASSETTE  = 4
  , TE_EMPTY                             = 5
  , TE_SERVICE_MODE_ENTERED              = 6
  , TE_SERVICE_MODE_LEFT                 = 7
  , TE_DEVICE_DID_NOT_ANSWER_AS_EXPECTED = 8
  , TE_THE_PROTOCOL_WAS_CANCELLED        = 9
  , TE_LOW_PAPER_WARNING                 = 10
  , TE_PRINTER_ERROR                     = 11
  , TE_PAPER_OUT_CONDITION               = 12
  }
;

struct evento {
  uint32_t origen;
  uint64_t fecha ;
  uint8_t  tipo  ;
  uint32_t serial;
};

const char * to_s_te(enum tipo_evento tipo_evento);
enum tipo_evento from_s_te(const char * mensaje);

void leer(int socket, void * buf, size_t count);
void escribir(int fd, void * buf, size_t count);

struct evento recibir(int socket);
void enviar(int socked, struct evento evento);
int evento_valido(struct evento evento);
