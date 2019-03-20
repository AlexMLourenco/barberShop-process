/**
 *  \brief Global parameters and common functions.
 *  
 * \author Miguel Oliveira e Silva - 2017
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include "utf8.h"
#include "utils.h"
#include "timer.h"
#include "thread.h"
#include "process.h"

typedef struct _Parameters_
{
   // barbershop:
   int NUM_BARBER_CHAIRS;
   int NUM_SCISSORS;
   int NUM_COMBS;
   int NUM_RAZORS;
   int NUM_WASHBASINS;
   int NUM_CLIENT_BENCHES_SEATS;
   int NUM_CLIENT_BENCHES;

   // person (barbers and clients):
   //   - vitality  (speed of living)
   int MIN_VITALITY_TIME_UNITS;
   int MAX_VITALITY_TIME_UNITS;

   // barbers:
   //   - random time spending in each activity [MIN_WORK_TIME_UNITS; MAX_WORK_TIME_UNITS]
   int NUM_BARBERS; // also NUM_BARBER_BENCH_SEATS
   int MIN_WORK_TIME_UNITS;
   int MAX_WORK_TIME_UNITS;

   // clients:
   //   - each client goes [MIN_BARBER_SHOP_TRIPS;MAX_BARBER_SHOP_TRIPS] random times to barber shop
   //   - random time spending outside barber shop [MIN_OUTSIDE_TIME_UNITS;MAX_OUTSIDE_TIME_UNITS
   //   - PROB_REQUEST_* determines the probability to choose the specific service
   int NUM_CLIENTS;
   int MIN_BARBER_SHOP_TRIPS;
   int MAX_BARBER_SHOP_TRIPS;
   int MIN_OUTSIDE_TIME_UNITS;
   int MAX_OUTSIDE_TIME_UNITS;
   int PROB_REQUEST_HAIRCUT;
   int PROB_REQUEST_WASHHAIR;
   int PROB_REQUEST_SHAVE;
} Parameters;

 
#define SHM_SHOP_KEY 0x1111
#define SHM_BARBERS_KEY 0x4444
#define SHM_CLIENTS_KEY 0x7777

// requests mask
#define HAIRCUT_REQ    1 // H 
#define WASH_HAIR_REQ  2 // W
#define SHAVE_REQ      4 // S

extern Parameters* global; // global variable with simulation parameters

// simulation limits:
#define MAX_BARBERS 20 // is also max. barber bench seats
#define MAX_BARBER_CHAIRS 10 // position with only one digit!
#define MAX_NUM_TOOLS 99 // max. scissors, combs and razors
#define MAX_WASHBASINS 10 // position with only one digit!
#define MAX_CLIENT_BENCHES_SEATS 20  // also limits number of client benches
#define MAX_CLIENTS 99

#ifdef ASCII_MODE

#define SCISSOR              (char*)"Sc"
#define COMB                 (char*)"Cb"
#define RAZOR                (char*)"Rz"
#define COMPLETE             (char*)"o"
#define INCOMPLETE           (char*)" "
#define SPLASH               (char*)"wash!"
#define BOX_HORIZONTAL       (char*)"-"
#define BOX_HORIZONTAL_DOWN  (char*)"+"
#define BOX_VERTICAL         (char*)"|"

#else

#define SCISSOR              string_concat(NULL, 0, code2utf8(NULL, 0x2702), (char*)" ", NULL)
#define COMB                 string_concat(NULL, 0, code2utf8(NULL, get_code_utf8((char*)"LEFT-SQUARE-BRACKET-WITH-QUILL")), (char*)" ", NULL)
#define RAZOR                string_concat(NULL, 0, code2utf8(NULL, get_code_utf8((char*)"DAGGER-KNIFE")), (char*)" ", NULL)
#define COMPLETE             code2utf8(NULL, get_code_utf8((char*)"MEDIUM-BLACK-CIRCLE"))
#define INCOMPLETE           code2utf8(NULL, get_code_utf8((char*)"MEDIUM-WHITE-CIRCLE"))
#define SPLASH               string_concat(NULL, 0, code2utf8(NULL, get_code_utf8((char*)"SPLASHING")), (char*)" ", code2utf8(NULL, get_code_utf8((char*)"SPLASHING")),NULL)
#define BOX_HORIZONTAL       code2utf8(NULL, get_code_utf8((char*)"BOX-HORIZONTAL"))
#define BOX_HORIZONTAL_DOWN  code2utf8(NULL, get_code_utf8((char*)"BOX-HORIZONTAL-DOWN"))
#define BOX_VERTICAL         code2utf8(NULL, get_code_utf8((char*)"BOX-VERTICAL"))

#endif

#endif
