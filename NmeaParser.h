
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "PvtManager.h"

typedef enum {
  NMEA_SENTENCE_TYPE_UNKNOWN,
  NMEA_SENTENCE_TYPE_GGA,
  NMEA_SENTENCE_TYPE_RMC,
  NMEA_SENTENCE_TYPE_GNS,
  NMEA_SENTENCE_TYPE_GLL,
  NMEA_SENTENCE_TYPE_GSA,
  NMEA_SENTENCE_TYPE_ZDA,
  NMEA_SENTENCE_TYPE_GST,
  NMEA_SENTENCE_TYPE_GSV
} nmea_sentence_type_t;

typedef struct {
  uint8_t bytes[120];
  nmea_sentence_type_t type;
  int length;
} nmea_sentence_t;


typedef void (*nmeaSentenceReceivedCallback)(nmea_sentence_t*);
typedef void (*nmeaDebugPrintf)(const char* fmt, ...);

extern void nmeaParserInit(nmeaSentenceReceivedCallback callback, nmea_sentence_t *outgoingSentence, nmeaDebugPrintf instrumFcn);
extern void receiveNmeaByte(uint8_t byte);
gnss_location_t processRMC(uint8_t *sentence);



#ifdef __cplusplus
}
#endif
