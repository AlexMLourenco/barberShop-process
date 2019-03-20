/**
 * \brief client benches representation
 *  
 * \author Miguel Oliveira e Silva - 2018
 */

#ifndef CLIENT_BENCHES_H
#define CLIENT_BENCHES_H

#include "global.h"
#include "client-queue.h"

typedef struct _ClientBenches_
{
   int numSeats;
   int numBenches;
   int id[MAX_CLIENT_BENCHES_SEATS];
   int order[MAX_CLIENT_BENCHES_SEATS];
   int request[MAX_CLIENT_BENCHES_SEATS];
   ClientQueue queue;
   int logId;
   char* internal;
} ClientBenches;

void init_client_benches(ClientBenches* benches, int num_seats, int num_benches, int line, int column);
void term_client_benches(ClientBenches* benches);
void log_client_benches(ClientBenches* benches);

int num_available_benches_seats(ClientBenches* benches);
int occupied_by_id_client_benches(ClientBenches* benches, int pos, int id);

// to use directly by clients:
int random_sit_in_client_benches(ClientBenches* benches, int id, int request);
void rise_client_benches(ClientBenches* benches, int pos, int id);
int seated_in_client_benches(ClientBenches* benches, int id);

// to use directly by barbers:
int no_more_clients(ClientBenches* benches);
RQItem next_client_in_benches(ClientBenches* benches);


#endif
