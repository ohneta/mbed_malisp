/**
 *
 */

#include "mbed.h"
#include "mbed_functions.h"
#include "malisp.h"

extern LIST *TRU;

//----------------------------------------------------------------------
// 
size_t _getFreeMemorySize()
{
    size_t i = 0;
    while(1) {
        void *p = malloc(i);
        if (p == NULL) {
            break;
        }
        free(p);
        i++;
    }

    return i;
}

LIST *memfreesize()
{
    //work_garbageCollect(NULL);

    size_t memsize = _getFreeMemorySize();

    LIST *res = cons(NULL, NULL);

    res->u.num = memsize;
    rplact(res, IATOM);

    return res;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

PinName _mbed_pinNameExchange(char *pinNameString)
{
    if (strcmp(pinNameString, "p5") == 0) return p5;
    if (strcmp(pinNameString, "p6") == 0) return p6;
    if (strcmp(pinNameString, "p7") == 0) return p7;
    if (strcmp(pinNameString, "p8") == 0) return p8;
    if (strcmp(pinNameString, "p9") == 0) return p9;
    if (strcmp(pinNameString, "p10") == 0) return p10;
    if (strcmp(pinNameString, "p11") == 0) return p11;
    if (strcmp(pinNameString, "p12") == 0) return p12;
    if (strcmp(pinNameString, "p13") == 0) return p13;
    if (strcmp(pinNameString, "p14") == 0) return p14;
    if (strcmp(pinNameString, "p15") == 0) return p15;
    if (strcmp(pinNameString, "p16") == 0) return p16;
    if (strcmp(pinNameString, "p17") == 0) return p17;
    if (strcmp(pinNameString, "p18") == 0) return p18;
    if (strcmp(pinNameString, "p19") == 0) return p19;
    if (strcmp(pinNameString, "p20") == 0) return p20;

    if (strcmp(pinNameString, "p21") == 0) return p21;
    if (strcmp(pinNameString, "p22") == 0) return p22;
    if (strcmp(pinNameString, "p23") == 0) return p23;
    if (strcmp(pinNameString, "p24") == 0) return p24;
    if (strcmp(pinNameString, "p25") == 0) return p25;
    if (strcmp(pinNameString, "p26") == 0) return p26;
    if (strcmp(pinNameString, "p27") == 0) return p27;
    if (strcmp(pinNameString, "p28") == 0) return p28;
    if (strcmp(pinNameString, "p29") == 0) return p29;
    if (strcmp(pinNameString, "p30") == 0) return p30;

    if (strcmp(pinNameString, "LED1") == 0) return LED1;
    if (strcmp(pinNameString, "LED2") == 0) return LED2;
    if (strcmp(pinNameString, "LED3") == 0) return LED3;
    if (strcmp(pinNameString, "LED4") == 0) return LED4;

    if (strcmp(pinNameString, "USBTX") == 0) return USBTX;
    if (strcmp(pinNameString, "USBRX") == 0) return USBRX;

    return NC;
}

PinMode _digitalPinMode(char *modeString)
{
    PinMode mode = PullDown;
    if (        (strcmp(modeString, "PullUp") == 0) ||
                (strcmp(modeString, "PU") == 0) ) {
        mode = PullUp;
    } else if ( (strcmp(modeString, "PullNone") == 0) ||
                (strcmp(modeString, "PN") == 0) ) {
        mode = PullNone;
    } else if ( (strcmp(modeString, "PullDown") == 0) ||
                (strcmp(modeString, "PD") == 0) ) {
        mode = PullDown;
    } else if ( (strcmp(modeString, "OpenDrain") == 0) ||
                (strcmp(modeString, "OD") == 0) ) {
        mode = OpenDrain;
    }

    return mode;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
/**
 * wait
 */
LIST *mbed_wait(LIST *sec)
{
    int t = type(sec);
    if (t != IATOM)
        return NULL;
        
    wait(sec->u.num);

    return TRU;
}

//----------------------------------------------------------------------
/**
 * DigitalOut
 * lisp: (dout pinName data mode)
 */
LIST *mbed_digitalout(LIST *pin, LIST *data)
{
    PinName pinName = _mbed_pinNameExchange(getname(car(pin)));
     if (pinName == NC) {
        return NULL;
    }
   DigitalOut dout = DigitalOut(pinName);
    dout.write((int)data->u.num);

    return TRU;
}

//----------------------------------------------------------------------
/**
 * DigitalIn
 * lisp: (din pinName mode)
 */
LIST *mbed_digitalin(LIST *pin, LIST *mode)
{
    PinName pinName = _mbed_pinNameExchange(getname(car(pin)));
    if (pinName == NC) {
        return NULL;
    }
    char *pinModeStr = NULL;

    if (mode != NULL) {
        pinModeStr = getname(car(mode));
    }
    PinMode pinMode = _digitalPinMode(pinModeStr);

    DigitalIn din = DigitalIn(pinName, pinMode);
    int v = din.read();

    LIST *p = cons(NULL, NULL);
    
    p->u.num = v;
    rplact(p, IATOM);

    return p;
}

//----------------------------------------------------------------------
/**
 * AnalogOut
 * lisp: (aout pinName data)
 */
LIST *mbed_analogout(LIST *pin, LIST *data)
{
    PinName pinName = _mbed_pinNameExchange(getname(car(pin)));
    if (pinName == NC) {
        return NULL;
    }
    AnalogOut aout = AnalogOut(pinName);
    aout.write(data->u.num);

    return TRU;
}

//----------------------------------------------------------------------
/**
 * AnalogIn
 * lisp: (ain pinName)
 */
LIST *mbed_analogin(LIST *pin)
{
    PinName pinName = _mbed_pinNameExchange(getname(car(pin)));
    if (pinName == NC) {
        return NULL;
    }
    AnalogIn ain = AnalogIn(pinName);
    float v = ain.read();

    LIST *p = cons(NULL, NULL);
    p->u.num = v;
    rplact(p, RATOM);

    return p;
}

//----------------------------------------------------------------------
/**
 * PwmOut
 * lisp: (pwmout pinName period  duty-cycle)
 */
LIST *mbed_pwmout(LIST *pin, LIST *period, LIST *dcperc)
{
    PinName pinName = _mbed_pinNameExchange(getname(car(pin)));
    if (pinName == NC) {
        return NULL;
    }
    PwmOut pwmout = PwmOut(pinName);

    pwmout.period(period->u.num);
    pwmout.write(dcperc->u.num);
    
    return TRU;
}

//----------------------------------------------------------------------
