/**
 * @file evento.h
 * @author Gabriela Limonta 10-10385
 * @author John Delgado 10-10196
 *
 * Contiene la definición de las funciones que
 * permiten el manejo de mensajes de eventos entre
 * el SVR y el ATM.
 *
 */

/**
 * Tipo enumerado que representa a los distintos
 * eventos que pueden haber en el ATM y sus códigos
 * de evento asociado.
 */
enum tipo_evento
  { TE_ERROR                             = -1
  , TE_HEARTBEAT                         = 0
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
  , TE_FALLA_DE_CONEXION                 = 13
  }
;

/**
 * Estructura que representa un evento de un ATM.
 * Esta estructura tiene 4 campos;
 * el código del ATM de origen, la fecha en la que
 * ocurre el evento, el tipo de evento que es y el
 * serial que identifica a cada evento diferente.
 */
struct evento {
  uint32_t origen;
  uint64_t fecha ;
  uint8_t  tipo  ;
  uint32_t serial;
};

/**
 * Se encarga de transformar un código de evento en su mensaje asociado.
 * @param tipo_evento código del evento.
 * @return retorna una representación en string del mensaje asociado al
 * código de mensaje.
 */
const char * to_s_te(enum tipo_evento tipo_evento);

/**
 * Se encarga de transformar un string en su código de evento asociado.
 * @param mensaje string de mensaje del evento.
 * @return retorna el código de evento asociado al string de mensaje indicado.
 */
enum tipo_evento from_s_te(const char * mensaje);

/**
 * Se encarga de leer del file descriptor del socket el contenido
 * de buf.
 * @param socket file descriptor del socket del que se va a leer.
 * @param buf direccion donde se almacenará lo leido en fd.
 * @param count tamaño del contenido que se va a leer.
 */
void leer(int socket, void * buf, size_t count);

/**
 * Se encarga de escribir en el file descriptor del socket el contenido
 * de buf.
 * @param fd file descriptor del socket donde se va a escribir.
 * @param buf contenido que se va a escribir en fd.
 * @param count tamaño del contenido que se va a escribir.
 */
void escribir(int fd, void * buf, size_t count);

/**
 * Se encarga de recibir un evento del socket
 * @param socket file descriptor del socket del que se va a recibir.
 * @return retorna un struct evento con la información recibida.
 */
struct evento recibir(int socket);

/**
 * Se encarga de enviar un evento al socket
 * @param socket file descriptor del socket al que se va a enviar.
 * @param evento evento que se va a enviar a través del socket.
 */
void enviar(int socket, struct evento evento);

/**
 * Se encarga de indicar si un evento es válido.
 * @param evento evento que se verificará.
 * @return retorna 0 si es un evento válido, retorna -1 si es inválido.
 */
int evento_valido(struct evento evento);
