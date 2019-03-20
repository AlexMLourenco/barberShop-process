/**
 * \brief washbasin representation.
 *  
 * \author Miguel Oliveira e Silva - 2018
 */

#ifndef WASHBASIN_H
#define WASHBASIN_H

typedef struct _Washbasin_
{
   int id; // 1, 2, ... (position in barber shop)
   int clientID;
   int barberID;
   int completionPercentage; // [0;100]
   int logId;
   char* internal;
} Washbasin;

int num_lines_washbasin();
int num_columns_washbasin();

void init_washbasin(Washbasin* basin, int id, int line, int column);
void term_washbasin(Washbasin* basin);
void log_washbasin(Washbasin* basin);

int empty_washbasin(Washbasin* basin);
int complete_washbasin(Washbasin* basin); // with both a barber and a client
int washbasin_with_a_client(Washbasin* basin);
int washbasin_with_a_barber(Washbasin* basin);
int client_seated_in_washbasin(Washbasin* basin, int clientID);
int barber_working_in_washbasin(Washbasin* basin, int barberID);

void reserve_washbasin(Washbasin* basin, int barberID);
void release_washbasin(Washbasin* basin, int barberID);
void sit_in_washbasin(Washbasin* basin, int clientID);
void rise_from_washbasin(Washbasin* basin, int clientID);

int washbasin_service_finished(Washbasin* basin);

void set_completion_washbasin(Washbasin* basin, int completionPercentage);


#endif
