#include <stdlib.h>
#include "dbc.h"
#include "global.h"
#include "utils.h"
#include "box.h"
#include "logger.h"
#include "washbasin.h"

static const char* skel = 
   "@-------@\n"
   "| ####  |\n"
//   "@-------@\n"
//   ".........\n"
   "@-#####-@\n"
   "|C##|B##|\n"
   "@---+---@";

static int skel_length = num_lines_washbasin()*(num_columns_washbasin()+1)*4; // extra space for (pessimistic) utf8 encoding!

static char* basin_not_used = string_concat(NULL, 0, BOX_HORIZONTAL, BOX_HORIZONTAL, BOX_HORIZONTAL_DOWN, BOX_HORIZONTAL, BOX_HORIZONTAL, NULL); 

static char* to_string_washbasin(Washbasin* basin);

int num_lines_washbasin()
{
   return string_num_lines((char*)skel);
}

int num_columns_washbasin()
{
   return string_num_columns((char*)skel);
}

void init_washbasin(Washbasin* basin, int id, int line, int column)
{
   require (basin != NULL, "basin argument required");
   require (id > 0, concat_3str("invalid id (", int2str(id), ")"));
   require (line >= 0, concat_3str("Invalid line (", int2str(line), ")"));
   require (column >= 0, concat_3str("Invalid column (", int2str(column), ")"));

   basin->id = id;
   basin->clientID = 0;
   basin->barberID = 0;
   basin->completionPercentage = -1;
   basin->internal = NULL;
   char buf[31];
   gen_boxes(buf, 30, "Basin #.##: progress:", "#", int2nstr(basin->id, 2));
   static char* translations[] = {
      SPLASH, (char*)"",
      NULL
   };
   basin->logId = register_logger(buf, line ,column , num_lines_washbasin(), num_columns_washbasin(), translations);
}

void term_washbasin(Washbasin* basin)
{
   require (basin != NULL, "basin argument required");

   if (basin->internal != NULL)
   {
      mem_free(basin->internal);
      basin->internal = NULL;
   }
}

void log_washbasin(Washbasin* basin)
{
   require (basin != NULL, "basin argument required");

   spend(random_int(global->MIN_VITALITY_TIME_UNITS, global->MAX_VITALITY_TIME_UNITS));
   send_log(basin->logId, to_string_washbasin(basin));
}

static char* to_string_washbasin(Washbasin* basin)
{
   if (basin->internal == NULL)
      basin->internal = (char*)mem_alloc(skel_length + 1);

   return gen_boxes(basin->internal, skel_length, skel,
         empty_washbasin(basin) ? " ---" : perc2str(basin->completionPercentage),
         empty_washbasin(basin) ? basin_not_used : SPLASH,
         empty_washbasin(basin) ? "--" : int2nstr(basin->clientID, 2),
         empty_washbasin(basin) ? "--" : int2nstr(basin->barberID, 2));
}

int empty_washbasin(Washbasin* basin)
{
   require (basin != NULL, "basin argument required");

   int res = basin->barberID == 0 && basin->clientID == 0;
   return res;
}

int complete_washbasin(Washbasin* basin)
{
   require (basin != NULL, "basin argument required");

   int res = basin->barberID > 0 && basin->clientID > 0;
   return res;
}

int washbasin_with_a_client(Washbasin* basin)
{
   require (basin != NULL, "basin argument required");

   int res = basin->clientID > 0;
   return res;
}

int washbasin_with_a_barber(Washbasin* basin)
{
   require (basin != NULL, "basin argument required");

   int res = basin->barberID > 0;
   return res;
}

int client_seated_in_washbasin(Washbasin* basin, int clientID)
{
   require (basin != NULL, "basin argument required");
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));

   int res = (clientID == basin->clientID);
   return res;
}

int barber_working_in_washbasin(Washbasin* basin, int barberID)
{
   require (basin != NULL, "basin argument required");
   require (barberID > 0, concat_3str("invalid barber id (", int2str(barberID), ")"));

   int res = (barberID == basin->barberID);
   return res;
}

void reserve_washbasin(Washbasin* basin, int barberID)
{
   require (basin != NULL, "basin argument required");
   require (barberID > 0, concat_3str("invalid barber id (", int2str(barberID), ")"));
   require (empty_washbasin(basin), "washbasin is already occupied");

   basin->barberID = barberID;
   basin->completionPercentage = 0;
   log_washbasin(basin);
}

void release_washbasin(Washbasin* basin, int barberID)
{
   require (basin != NULL, "basin argument required");
   require (barberID > 0, concat_3str("invalid barber id (", int2str(barberID), ")"));
   require (barber_working_in_washbasin(basin, barberID), concat_3str("barber ", int2str(barberID), " not working in basin"));
   require (!washbasin_with_a_client(basin), "a client is still in basin");

   basin->barberID = 0;
   basin->completionPercentage = -1;
   log_washbasin(basin);
}

void sit_in_washbasin(Washbasin* basin, int clientID)
{
   require (basin != NULL, "basin argument required");
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));
   require (!washbasin_with_a_client(basin), "basin occupied with a client");

   basin->clientID = clientID;
   log_washbasin(basin);
}

void rise_from_washbasin(Washbasin* basin, int clientID)
{
   require (basin != NULL, "basin argument required");
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));
   require (client_seated_in_washbasin(basin, clientID), concat_3str("client ", int2str(clientID), " not seated in barber basin"));
   require (washbasin_service_finished(basin), "basin service not complete");

   basin->clientID = 0;
   log_washbasin(basin);
}

int washbasin_service_finished(Washbasin* basin)
{
   require (basin != NULL, "basin argument required");

   int res;
   res = basin->completionPercentage == 100;
   return res;
}

void set_completion_washbasin(Washbasin* basin, int completionPercentage)
{
   require (basin != NULL, "basin argument required");
   require (completionPercentage >= 0 && completionPercentage <= 100, concat_3str("invalid percentage (", int2str(completionPercentage), ")"));
   require (complete_washbasin(basin), "washbasin is not complete");

   basin->completionPercentage = completionPercentage;
   log_washbasin(basin);
}

