/**
 *
 */
#include "mbed.h"
#include "malisp.h"

// prototype deceament

size_t _getFreeMemorySize();
LIST *memfreesize();

PinName _mbed_pinNameExchange(char *pinNameString);
PinMode _digitalPinMode(char *modeString);

LIST *mbed_wait(LIST *sec);

LIST *mbed_digitalout(LIST *pin, LIST *data);
LIST *mbed_digitalin(LIST *pin, LIST *mode);
LIST *mbed_pwmout(LIST *pin, LIST *period, LIST *dcperc);

LIST *mbed_analogout(LIST *pin, LIST *data);
LIST *mbed_analogin(LIST *pin);

