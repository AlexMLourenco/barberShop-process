/**
 *  \brief client module.
 *  
 * \author Miguel Oliveira e Silva - 2018
 */

#ifndef CLIENT_H
#define CLIENT_H

#include "global.h"
#include "barber-shop.h"

typedef struct _Client_
{
   int id; // 1, 2, ...
   int state;

   BarberShop* shop;
   int barberID;

   int num_trips_to_barber;
   int requests;      // services to be requested

   int benchesPosition; // -1 if not in benches
   int chairPosition; // -1 if not in client chair
   int basinPosition; // -1 if not in washbasin

   int logId;
   char* internal;
} Client;


int _generate_random_request(int requests[], int probabilities[], int size);
int _find_ceil(int arr[], int r, int l, int h);



// export to simulation:
size_t sizeof_client();
int num_lines_client();
int num_columns_client();
void init_client(Client* client, int id, BarberShop* shop, int num_trips_to_barber, int line, int column);
void term_client(Client* client);
void log_client(Client* client);
void* main_client(void* args);

#endif
