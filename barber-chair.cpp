#include <stdlib.h>
#include "dbc.h"
#include "global.h"
#include "utils.h"
#include "box.h"
#include "logger.h"
#include "barber-chair.h"

static const char* skel = 
   "@-------@\n"
   "| ####  |\n"
   "@-+---+-@\n"
   "##|C##|##\n"
   "  +---+\n"
   "  |B##|\n"
   "  @---@";

static int skel_length = num_lines_barber_chair()*(num_columns_barber_chair()+1)*4; // extra space for (pessimistic) utf8 encoding!

static char* to_string_barber_chair(BarberChair* chair);

int num_lines_barber_chair()
{
   return string_num_lines((char*)skel);
}

int num_columns_barber_chair()
{
   return string_num_columns((char*)skel);
}

void init_barber_chair(BarberChair* chair, int id, int line, int column)
{
   require (chair != NULL, "chair argument required");
   require (id > 0, concat_3str("invalid id (", int2str(id), ")"));
   require (line >= 0, concat_3str("Invalid line (", int2str(line), ")"));
   require (column >= 0, concat_3str("Invalid column (", int2str(column), ")"));

   chair->id = id;
   chair->clientID = 0;
   chair->barberID = 0;
   chair->toolsHolded = 0;
   chair->completionPercentage = -1;
   chair->internal = NULL;
   char buf[31];
   gen_boxes(buf, 30, "Chair #.##: progress:", "#", int2nstr(chair->id, 2));
   static char* translations[] = {
      SCISSOR, (char*)"Scissor",
      COMB, (char*)"Comb",
      RAZOR, (char*)"Razor",
      NULL
   };
   chair->logId = register_logger(buf, line ,column , num_lines_barber_chair(), num_columns_barber_chair(), translations);
}

void term_barber_chair(BarberChair* chair)
{
   require (chair != NULL, "chair argument required");

   if (chair->internal != NULL)
   {
      mem_free(chair->internal);
      chair->internal = NULL;
   }
}

void log_barber_chair(BarberChair* chair)
{
   require (chair != NULL, "chair argument required");

   spend(random_int(global->MIN_VITALITY_TIME_UNITS, global->MAX_VITALITY_TIME_UNITS));
   send_log(chair->logId, to_string_barber_chair(chair));
}

static char* to_string_barber_chair(BarberChair* chair)
{
   if (chair->internal == NULL)
      chair->internal = (char*)mem_alloc(skel_length + 1);

   char t1[7];
   memset(t1, 0, 7);
   char t2[7];
   memset(t2, 0, 7);
   if (chair->toolsHolded & SCISSOR_TOOL)
   {
      strcpy(t1, SCISSOR);
      if (chair->toolsHolded & COMB_TOOL)
         strcpy(t2, COMB);
      else if (chair->toolsHolded & RAZOR_TOOL)
         strcpy(t2, RAZOR);

   }
   else if (chair->toolsHolded & COMB_TOOL)
   {
      strcpy(t1, COMB);
      if (chair->toolsHolded & RAZOR_TOOL)
         strcpy(t2, RAZOR);
   }
   else if (chair->toolsHolded & RAZOR_TOOL)
      strcpy(t1, RAZOR);
   return gen_boxes(chair->internal, skel_length, skel,
                    chair->completionPercentage < 0 ? " ---" : perc2str(chair->completionPercentage),
                    t1, barber_chair_with_a_client(chair) ? int2nstr(chair->clientID, 2) : "--", t2,
                    barber_chair_with_a_barber(chair) ? int2nstr(chair->barberID, 2) : "--");
}

int empty_barber_chair(BarberChair* chair)
{
   require (chair != NULL, "chair argument required");

   int res = chair->barberID == 0 && chair->clientID == 0;
   return res;
}

int complete_barber_chair(BarberChair* chair)
{
   require (chair != NULL, "chair argument required");

   int res = chair->barberID > 0 && chair->clientID > 0;
   return res;
}

int barber_chair_with_a_client(BarberChair* chair)
{
   require (chair != NULL, "chair argument required");

   int res = chair->clientID > 0;
   return res;
}

int barber_chair_with_a_barber(BarberChair* chair)
{
   require (chair != NULL, "chair argument required");

   int res = chair->barberID > 0;
   return res;
}

int client_seated_in_barber_chair(BarberChair* chair, int clientID)
{
   require (chair != NULL, "chair argument required");
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));

   int res = (clientID == chair->clientID);
   return res;
}

int barber_working_in_barber_chair(BarberChair* chair, int barberID)
{
   require (chair != NULL, "chair argument required");
   require (barberID > 0, concat_3str("invalid barber id (", int2str(barberID), ")"));

   int res = (barberID == chair->barberID);
   return res;
}

void reserve_barber_chair(BarberChair* chair, int barberID)
{
   require (chair != NULL, "chair argument required");
   require (barberID > 0, concat_3str("invalid barber id (", int2str(barberID), ")"));
   require (empty_barber_chair(chair), "barber chair is already occupied");

   chair->barberID = barberID;
   chair->completionPercentage = 0;
   log_barber_chair(chair);
}

void release_barber_chair(BarberChair* chair, int barberID)
{
   require (chair != NULL, "chair argument required");
   require (barberID > 0, concat_3str("invalid barber id (", int2str(barberID), ")"));
   require (barber_working_in_barber_chair(chair, barberID), concat_3str("barber ", int2str(barberID), " not working in chair"));
   require (!barber_chair_with_a_client(chair), "a client is still in chair");

   chair->barberID = 0;
   chair->toolsHolded = 0;
   chair->completionPercentage = -1;
   log_barber_chair(chair);
}

void sit_in_barber_chair(BarberChair* chair, int clientID)
{
   require (chair != NULL, "chair argument required");
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));
   require (!barber_chair_with_a_client(chair), "chair occupied with a client");

   chair->clientID = clientID;
   log_barber_chair(chair);
}

void rise_from_barber_chair(BarberChair* chair, int clientID)
{
   require (chair != NULL, "chair argument required");
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));
   require (client_seated_in_barber_chair(chair, clientID), concat_3str("client ", int2str(clientID), " not seated in barber chair"));
   require (barber_chair_service_finished(chair), "barber chair service not complete");

   chair->clientID = 0;
   log_barber_chair(chair);
}

void set_tools_barber_chair(BarberChair* chair, int tools)
{
   require (chair != NULL, "chair argument required");
   require ((tools & 0x7) != 0, concat_3str("invalid tools mask (", int2str(tools), ")"));
   require (complete_barber_chair(chair), "barber chair is not complete");

   chair->toolsHolded = tools;
   log_barber_chair(chair);
}

int barber_chair_service_finished(BarberChair* chair)
{
   require (chair != NULL, "chair argument required");

   int res;
   res = chair->completionPercentage == 100;
   return res;
}

void set_completion_barber_chair(BarberChair* chair, int completionPercentage)
{
   require (chair != NULL, "chair argument required");
   require (completionPercentage >= 0 && completionPercentage <= 100, concat_3str("invalid percentage (", int2str(completionPercentage), ")"));
   require ((chair->toolsHolded & 0x7) != 0, "barber not holding tools");
   require (complete_barber_chair(chair), "barber chair is not complete");

   chair->completionPercentage = completionPercentage;
   log_barber_chair(chair);
}

