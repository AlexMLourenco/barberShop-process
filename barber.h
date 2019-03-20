/**
 *  \brief barber module.
 *  
 * \author Miguel Oliveira e Silva - 2018
 */

#ifndef BARBER_H
#define BARBER_H

#include "global.h"
#include "barber-shop.h"

typedef struct _Barber_
{
   int id; // 1, 2, ...
   int state;

   BarberShop* shop;
   int clientID;

   int reqToDo;       // services requested

   int benchPosition; // -1 if not in bench
   int chairPosition; // -1 if not in barber chair
   int basinPosition; // -1 if not in washbasin

   int tools;         // tools with barber

   int logId;
   char* internal;
} Barber;

// export to simulation:
size_t sizeof_barber();
int num_lines_barber();
int num_columns_barber();
void init_barber(Barber* barber, int id, BarberShop* shop, int line, int column);
void term_barber(Barber* barber);
void log_barber(Barber* barber);
void* main_barber(void* args);

#endif
