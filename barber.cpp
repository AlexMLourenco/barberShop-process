#include <stdlib.h>
#include "dbc.h"
#include "global.h"
#include "utils.h"
#include "box.h"
#include "timer.h"
#include "logger.h"
#include "barber-shop.h"
#include "barber.h"

enum State
{
   NONE = 0,
   CUTTING,
   SHAVING,
   WASHING,
   WAITING_CLIENTS,
   WAITING_BARBER_SEAT,
   WAITING_WASHBASIN,
   REQ_SCISSOR,
   REQ_COMB,
   REQ_RAZOR,
   DONE
};

#define State_SIZE (DONE - NONE + 1)

static const char* stateText[State_SIZE] =
{
   "---------",
   "CUTTING  ",
   "SHAVING  ",
   "WASHING  ",
   "W CLIENT ", // Waiting for client
   "W SEAT   ", // Waiting for barber seat
   "W BASIN  ", // Waiting for washbasin
   "R SCISSOR", // Request a scissor
   "R COMB   ", // Request a comb
   "R RAZOR  ", // Request a razor
   "DONE     ",
};

static const char* skel = 
   "@---+---+---@\n"
   "|B##|C##|###|\n"
   "+---+---+-+-+\n"
   "|#########|#|\n"
   "@---------+-@";
static int skel_length = num_lines_barber()*(num_columns_barber()+1)*4; // extra space for (pessimistic) utf8 encoding!

static void life(Barber* barber);

static void sit_in_barber_bench(Barber* barber);
static void wait_for_client(Barber* barber);
static int work_available(Barber* barber);
static void rise_from_barber_bench(Barber* barber);
static void process_resquests_from_client(Barber* barber);
static void release_client(Barber* barber);
static void done(Barber* barber);
static void process_haircut_request(Barber* barber);
static void process_shave_request(Barber* barber);
static void process_washhair_request(Barber* barber);

static char* to_string_barber(Barber* barber);

size_t sizeof_barber()
{
   return sizeof(Barber);
}

int num_lines_barber()
{
   return string_num_lines((char*)skel);
}

int num_columns_barber()
{
   return string_num_columns((char*)skel);
}

void init_barber(Barber* barber, int id, BarberShop* shop, int line, int column)
{
   require (barber != NULL, "barber argument required");
   require (id > 0, concat_3str("invalid id (", int2str(id), ")"));
   require (shop != NULL, "barber shop argument required");
   require (line >= 0, concat_3str("Invalid line (", int2str(line), ")"));
   require (column >= 0, concat_3str("Invalid column (", int2str(column), ")"));

   barber->id = id;
   barber->state = NONE;
   barber->shop = shop;
   barber->clientID = 0;
   barber->reqToDo = 0;
   barber->benchPosition = -1;
   barber->chairPosition = -1;
   barber->basinPosition = -1;
   barber->tools = 0;
   barber->internal = NULL;
   barber->logId = register_logger((char*)("Barber:"), line ,column,
                                   num_lines_barber(), num_columns_barber(), NULL);
}

void term_barber(Barber* barber)
{
   require (barber != NULL, "barber argument required");

   if (barber->internal != NULL)
   {
      mem_free(barber->internal);
      barber->internal = NULL;
   }
}

void log_barber(Barber* barber)
{
   require (barber != NULL, "barber argument required");

   spend(random_int(global->MIN_VITALITY_TIME_UNITS, global->MAX_VITALITY_TIME_UNITS));
   send_log(barber->logId, to_string_barber(barber));
}

void* main_barber(void* args)
{   
   srand(time(NULL) ^ (getpid()<<16));
   Barber* barber = (Barber*)args;
   require (barber != NULL, "barber argument required");
   //debug_log(barber->shop,"main_barber\tStarted the BARBER life %d", barber->id);

   life(barber);
   return NULL;
}

static void life(Barber* barber)
{
   require (barber != NULL, "barber argument required");

   sit_in_barber_bench(barber);
   wait_for_client(barber);
   while(work_available(barber)) // no more possible clients and closes barbershop
   {
      rise_from_barber_bench(barber);
      process_resquests_from_client(barber);
      release_client(barber);
      sit_in_barber_bench(barber);
      wait_for_client(barber);
   }
   done(barber);
}

static void sit_in_barber_bench(Barber* barber)
{
   /** TODO:
    * 1: sit in a random empty seat in barber bench (always available)
    * 
    * DONE:
    * The number of benches created for the barber are exactly the same as the number of barbers
    * but to make sure that they don't seat on top of each others we have to use a mutex to protect this
    * zone and realese it when the barber is seated.
    * 
    **/
   psem_wait(&barber->shop->mutex_barber_bench);

   require (barber != NULL, "barber argument required");
   require (num_seats_available_barber_bench(barber_bench(barber->shop)) > 0, "seat not available in barber shop");
   require (!seated_in_barber_bench(barber_bench(barber->shop), barber->id), "barber already seated in barber shop");

   int num = random_sit_in_barber_bench(barber_bench(barber->shop),barber->id);

   barber->benchPosition = num;
   barber->clientID = 0;
   
   psem_post(&barber->shop->mutex_barber_bench);

   log_barber(barber);
}

static void wait_for_client(Barber* barber)
{
   /** TODO:
    * 1: set the barber state to WAITING_CLIENTS
    * 2: get next client from client benches (if empty, wait) (also, it may be required to check for simulation termination)
    * 3: receive and greet client (receive its requested services, and give back the barber's id)
    * 
    * DONE:
    * 
    **/
   require (barber != NULL, "barber argument required");

   //cleanup old barber positions
   for (int i=1; i <=MAX_CLIENTS; i ++) {
      if (barber->shop->barbers_assigned[i] == barber->id)  
         barber->shop->barbers_assigned[i]=-1;
   }
   barber->state = WAITING_CLIENTS; 
   RQItem res = empty_item();
   do {
      //debug_log(barber->shop,"wait_for_client\tThe barber %d is waitting for clients", barber->id);
      psem_wait(&barber->shop->mutex_client_bench);
      res = next_client_in_benches(client_benches(barber->shop));
      psem_post(&barber->shop->mutex_client_bench);

      if (res.benchPos != -1) {
            barber->clientID = res.clientID; 
            barber->reqToDo = res.request;
          


            barber->shop->barbers_assigned[res.clientID] = barber->id;
            //debug_log(barber->shop,"wait_for_client\tThe barber %d is picking up client %d seated in %d (AB %d)", barber->id, res.clientID,res.benchPos,barber->shop->barbers_assigned[res.clientID] );
            receive_and_greet_client(barber->shop, barber->id, res.clientID);                      
      } else {
            //debug_log(barber->shop,"wait_for_client\tThe barber %d has no clients to attend", barber->id); 
      }
      log_barber(barber);

      //Sleep for a little while 
      sleep(random_int(1,3));      
   } while(res.benchPos == -1 && barber->shop->opened ==1);
}

static int work_available(Barber* barber)
{
   /** TODO:
    * 1: find a safe way to solve the problem of barber termination
    * 
    * DONE: 
    * I think that here we should validate:
    * 
    * - that there are clients to attend (Seatted in the benches)
    * - the store is open
    * - the clients that are wandering outside
    **/
   

   require (barber != NULL, "barber argument required");

   return barber->clientID > 0 && barber->shop->opened==1;
}

static void rise_from_barber_bench(Barber* barber)
{
   /** DONE:
    * 1: rise from the seat of barber bench
    * 
    * COMMENT:
    * From what i see here the barber is available to answer requests and we should remove it from 
    * the bench making it available to attend a customer.
    **/


   require (barber != NULL, "barber argument required");
   require (seated_in_barber_bench(barber_bench(barber->shop), barber->id), "barber not seated in barber shop");

   psem_wait(&barber->shop->mutex_barber_bench);
   rise_barber_bench (barber_bench(barber->shop), barber->benchPosition);
   psem_post(&barber->shop->mutex_barber_bench);

   barber->benchPosition = -1; //clean up

   log_barber(barber);
}

static void process_resquests_from_client(Barber* barber)
{
   /** TODO:
    * Process one client request at a time, until all requests are fulfilled.
    * For each request:
    * 1: select the request to process (any order is acceptable)
    * 2: reserve the chair/basin for the service (setting the barber's state accordingly) 
    *    2.1: set the client state to a proper value
    *    2.2: reserve a random empty chair/basin 
    *    2.2: inform client on the service to be performed
    * 3: depending on the service, grab the necessary tools from the pot (if any)
    * 4: process the service (see [incomplete] process_haircut_request as an example)
    * 3: return the used tools to the pot (if any)
    *
    *
    * At the end the client must leave the barber shop
    **/

   require (barber != NULL, "barber argument required");

   Service s;  

   while (barber->reqToDo != 0) {

      barber->basinPosition = -1;
      barber->chairPosition = -1;
      barber->benchPosition = -1;


      //Select one of the services that the client wants
      int req;
      if (barber->reqToDo & SHAVE_REQ ) {
         req = SHAVE_REQ;
      }else if (barber->reqToDo & WASH_HAIR_REQ ) {
         req = WASH_HAIR_REQ;
      } else {
         req = HAIRCUT_REQ;
      }

      //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / Service %d", barber->id, barber->clientID, req);
      if (req == SHAVE_REQ || req == HAIRCUT_REQ) { //needs a baerber chair
         
         psem_wait(&barber->shop->sem_barber_chairs);  

         //protect the memory zone of the barber chairs
         psem_wait(&barber->shop->mutex_barber_chairs); 
         int idx = reserve_random_empty_barber_chair(barber->shop, barber->id);       
         psem_post(&barber->shop->mutex_barber_chairs);    
                  
         //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / Reserved Chair %d", barber->id, barber->clientID, idx);

         set_barber_chair_service(&s,barber->id,barber->clientID,idx,req);

         barber->chairPosition = idx;

      }  else { // needs a wasbasin
         psem_wait(&barber->shop->sem_washbasins); 

         //protect the memory zone of the washbasins
         psem_wait(&barber->shop->mutex_washbasins);                    
         int idx = reserve_random_empty_washbasin(barber->shop, barber->id);       
         psem_post(&barber->shop->mutex_washbasins); 

         set_washbasin_service(&s,barber->id,barber->clientID,idx);

         barber->basinPosition = idx;
      } 
      
   
      inform_client_on_service(barber->shop,s);

      //Wait for the client to tell that we can continue
      psem_wait(&barber->shop->sem_services_client[s.barberID]); 

      if (req == HAIRCUT_REQ) {
         //Pick up scissor
         psem_wait(&barber->shop->sem_scissors);     
         barber->tools = barber->tools + SCISSOR_TOOL;
         pick_scissor(tools_pot(barber->shop));
         //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / Got Scissor", barber->id, barber->clientID);

         //Pick up Comb
         psem_wait(&barber->shop->sem_combs);      
         barber->tools = barber->tools + COMB_TOOL;
         pick_comb(tools_pot(barber->shop));
         //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / Got Comb", barber->id, barber->clientID);
      }

      if (req == SHAVE_REQ) {
         //Pick up Razor
         psem_wait(&barber->shop->sem_razors);      
         barber->tools = barber->tools + RAZOR_TOOL;
         pick_razor(tools_pot(barber->shop));
         //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / Got Razor", barber->id, barber->clientID);
      }      

      //debug_log(barber->shop,"process_resquests_from_client\tService CL %d / BAR %d / CHAI %d / WB %d / POS %d / REQ %d",s.clientID, s.barberID, s.barberChair, s.washbasin, s.pos, s.request);
      if (req== SHAVE_REQ || req == HAIRCUT_REQ) {
         set_tools_barber_chair(barber_chair(barber->shop,s.pos), barber->tools);
      }
      
      
      //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / Starting Proccess", barber->id, barber->clientID);
      switch (req) {
         case HAIRCUT_REQ:  process_haircut_request(barber); break;
         case WASH_HAIR_REQ: process_washhair_request(barber); break;
         case SHAVE_REQ: process_shave_request(barber); break;
      }
      //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / Finished Proccess", barber->id, barber->clientID);

      if (req == SHAVE_REQ || req == HAIRCUT_REQ) { // Release barber chair
         //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / Try to release chair %d", barber->id, barber->clientID, s.pos);
         psem_wait(&barber->shop->mutex_barber_chairs); //protect the memory zone         
         rise_from_barber_chair(barber_chair(barber->shop,s.pos), s.clientID);
         release_barber_chair(barber_chair(barber->shop,s.pos), s.barberID);         
         psem_post(&barber->shop->mutex_barber_chairs); 
         psem_post(&barber->shop->sem_barber_chairs); 
         //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / RELASED chair %d", barber->id, barber->clientID, s.pos);
      } else {
         //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / Try to washbasin %d", barber->id, barber->clientID, s.pos);
         psem_wait(&barber->shop->mutex_washbasins); //protect the memory zone
         rise_from_washbasin(washbasin(barber->shop,s.pos), s.clientID);
         release_washbasin(washbasin(barber->shop,s.pos), s.barberID);         
         psem_post(&barber->shop->mutex_washbasins); 
         psem_post(&barber->shop->sem_washbasins); 
         //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / RELASED washbasin %d", barber->id, barber->clientID, s.pos);
      }


      if (req == HAIRCUT_REQ) {
         //Return scissor
         //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / Try to return Scissor", barber->id, barber->clientID);         
         barber->tools = barber->tools - SCISSOR_TOOL;
         return_scissor(tools_pot(barber->shop));
         psem_post(&barber->shop->sem_scissors);     
         //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / Return Scissor", barber->id, barber->clientID);

         //Return Comb
         //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / Try to return comb", barber->id, barber->clientID);
         return_comb(tools_pot(barber->shop));
         psem_post(&barber->shop->sem_combs);      
         barber->tools = barber->tools - COMB_TOOL;         
         //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / Return Comb", barber->id, barber->clientID);
      }

      if (req == SHAVE_REQ) {
         //Return Razor
         //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / Try to return Razor", barber->id, barber->clientID);
         return_razor(tools_pot(barber->shop));
         psem_post(&barber->shop->sem_razors);      
         barber->tools = barber->tools - RAZOR_TOOL;         
         //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d /Return Razor", barber->id, barber->clientID);
      }
   
      //debug_log(barber->shop,"process_resquests_from_client\tBarber %d / Client %d / Inform Client Finish", barber->id, barber->clientID);
      psem_post(&barber->shop->sem_services_barber[s.barberID]); 

      barber->reqToDo = barber->reqToDo - req;
      log_barber(barber);
   }   
   
   psem_wait(&barber->shop->sem_services_finish[barber->id]);
   
   
   log_barber(barber); 
}

static void release_client(Barber* barber)
{
   /** TODO:
    * 1: notify client the all the services are done
    **/

   require (barber != NULL, "barber argument required");


   client_done(barber->shop,barber->clientID);

   log_barber(barber);
}

static void done(Barber* barber)
{
   /** TODO:
    * 1: set the client state to DONE
    **/
   require (barber != NULL, "barber argument required");

   log_barber(barber);
}

static void process_haircut_request(Barber* barber)
{
   /** TODO:
    * ([incomplete] example code for task completion algorithm)
    **/
   require (barber != NULL, "barber argument required");
   require (barber->tools & SCISSOR_TOOL, "barber not holding a scissor");
   require (barber->tools & COMB_TOOL, "barber not holding a comb");

   int steps = random_int(5,20);
   int slice = (global->MAX_WORK_TIME_UNITS-global->MIN_WORK_TIME_UNITS+steps)/steps;
   int complete = 0;
   while(complete < 100)
   {
      sleep(slice);
      complete += 100/steps;
      if (complete > 100)
         complete = 100;
      set_completion_barber_chair(barber_chair(barber->shop, barber->chairPosition), complete);
      log_barber(barber); 

   }
}

static void process_shave_request(Barber* barber)
{
   /** TODO:
    * ([incomplete] example code for task completion algorithm)
    **/
   require (barber != NULL, "barber argument required");
   require (barber->tools & RAZOR_TOOL, "barber not holding a razor");

   int steps = random_int(5,20);
   int slice = (global->MAX_WORK_TIME_UNITS-global->MIN_WORK_TIME_UNITS+steps)/steps;
   int complete = 0;
   while(complete < 100)
   {
      sleep(slice);
      complete += 100/steps;
      if (complete > 100)
         complete = 100;
      set_completion_barber_chair(barber_chair(barber->shop, barber->chairPosition), complete);
      log_barber(barber); 

   }
}

static void process_washhair_request(Barber* barber)
{
   /** TODO:
    * ([incomplete] example code for task completion algorithm)
    **/
   require (barber != NULL, "barber argument required");

   int steps = random_int(5,20);
   int slice = (global->MAX_WORK_TIME_UNITS-global->MIN_WORK_TIME_UNITS+steps)/steps;
   int complete = 0;
   while(complete < 100)
   {
      sleep(slice);
      complete += 100/steps;
      if (complete > 100)
         complete = 100;
      
      Washbasin* basin = washbasin(barber->shop, barber->basinPosition);
      //debug_log(barber->shop,"process_washhair_request\tBarber %d / B %d / C %d / CP %d / ID %d", barber->id, basin->barberID, basin->clientID, basin->completionPercentage, basin->id);
      set_completion_washbasin(washbasin(barber->shop, barber->basinPosition), complete);
      log_barber(barber); 

   }
}



static char* to_string_barber(Barber* barber)
{
   require (barber != NULL, "barber argument required");

   if (barber->internal == NULL)
      barber->internal = (char*)mem_alloc(skel_length + 1);

   char tools[4];
   tools[0] = (barber->tools & SCISSOR_TOOL) ? 'S' : '-',
      tools[1] = (barber->tools & COMB_TOOL) ?    'C' : '-',
      tools[2] = (barber->tools & RAZOR_TOOL) ?   'R' : '-',
      tools[3] = '\0';

   char* pos = (char*)"-";
   if (barber->chairPosition >= 0)
      pos = int2nstr(barber->chairPosition+1, 1);
   else if (barber->basinPosition >= 0)
      pos = int2nstr(barber->basinPosition+1, 1);

   return gen_boxes(barber->internal, skel_length, skel,
         int2nstr(barber->id, 2),
         barber->clientID > 0 ? int2nstr(barber->clientID, 2) : "--",
         tools, stateText[barber->state], pos);
}

