/**
 * \brief barber chair representation.
 *  
 * \author Miguel Oliveira e Silva - 2018
 */

#ifndef BARBER_CHAIR_H
#define BARBER_CHAIR_H

typedef struct _BarberChair_
{
   int id; // 1, 2, ... (position in barber shop)
   int clientID;
   int barberID;
   int toolsHolded;
   int completionPercentage; // [0;100]
   int logId;
   char* internal;
} BarberChair;

// tools mask:
#define SCISSOR_TOOL 1
#define COMB_TOOL    2
#define RAZOR_TOOL   4

int num_lines_barber_chair();
int num_columns_barber_chair();

void init_barber_chair(BarberChair* chair, int id, int line, int column);
void term_barber_chair(BarberChair* chair);
void log_barber_chair(BarberChair* chair);

int empty_barber_chair(BarberChair* chair);
int complete_barber_chair(BarberChair* chair); // with both a barber and a client
int barber_chair_with_a_client(BarberChair* chair);
int barber_chair_with_a_barber(BarberChair* chair);
int client_seated_in_barber_chair(BarberChair* chair, int clientID);
int barber_working_in_barber_chair(BarberChair* chair, int barberID);

void reserve_barber_chair(BarberChair* chair, int barberID);
void release_barber_chair(BarberChair* chair, int barberID);
void sit_in_barber_chair(BarberChair* chair, int clientID);
void rise_from_barber_chair(BarberChair* chair, int clientID);

int barber_chair_service_finished(BarberChair* chair);

void set_tools_barber_chair(BarberChair* chair, int tools);
void set_completion_barber_chair(BarberChair* chair, int completionPercentage);

#endif
