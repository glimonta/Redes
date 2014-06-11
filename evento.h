enum tipo_evento
  { TM_COMMUNICATION_OFFLINE             = 1
  , TM_COMMUNICATION_ERROR               = 2
  , TM_LOW_CASH_ALERT                    = 3
  , TM_RUNNING_OUT_OF_NOTES_IN_CASSETTE  = 4
  , TM_EMPTY                             = 5
  , TM_SERVICE_MODE_ENTERED              = 6
  , TM_SERVICE_MODE_LEFT                 = 7
  , TM_DEVICE_DID_NOT_ANSWER_AS_EXPECTED = 8
  , TM_THE_PROTOCOL_WAS_CANCELLED        = 9
  , TM_LOW_PAPER_WARNING                 = 10
  , TM_PRINTER_ERROR                     = 11
  , TM_PAPER_OUT_CONDITION               = 12
  , TM_SUCCESS                           = 13
  }
;

struct evento {
  uint32_t origen ;
  uint32_t destino;
  uint64_t fecha  ;
  uint8_t  tipo   ;
};

const char * to_s_te(enum tipo_evento tipo_evento);

void leer(int socket, void * buf, size_t count);
void escribir(int fd, void * buf, size_t count);

struct evento recibir(int socket);
void enviar(int socked, struct evento evento);

struct evento evento(uint32_t origen, uint32_t destino, enum tipo_evento tipo);
