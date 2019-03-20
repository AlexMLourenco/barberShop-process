#include <stdlib.h>
#include "dbc.h"
#include "global.h"
#include "utils.h"
#include "box.h"
#include "timer.h"
#include "logger.h"
#include "service.h"
#include "client.h"

enum ClientState
{
   NONE = 0,                   // initial state
   WANDERING_OUTSIDE,          // client outside barshop doing (important) things
   WAITING_BARBERSHOP_VACANCY, // a place to sit in the barber shop clients benches
   SELECTING_REQUESTS,         // choosing barber shop services
   WAITING_ITS_TURN,           // waiting for a barber assignment
   WAITING_SERVICE,            // waiting service selection (includes seat id)
   WAITING_SERVICE_START,      // client already seated in chair/basin waiting service start
   HAVING_A_HAIRCUT,           // haircut in progress
   HAVING_A_SHAVE,             // shave in progress
   HAVING_A_HAIR_WASH,         // hair wash in progress
   DONE                        // final state
};

#define State_SIZE (DONE - NONE + 1)

static const char* stateText[State_SIZE] =
{
   "---------",
   "WANDERING",
   "W VACANCY",
   "REQUESTS ",
   "Wait TURN",
   "Wt SERVCE",
   "SERV STRT",
   "HAIRCUT  ",
   "SHAVE    ",
   "HAIR WASH",
   "DONE     ",
};

static const char* skel = 
   "@---+---+---@\n"
   "|C##|B##|###|\n"
   "+---+---+-+-+\n"
   "|#########|#|\n"
   "@---------+-@";
static int skel_length = num_lines_client()*(num_columns_client()+1)*4; // extra space for (pessimistic) utf8 encoding!

static void life(Client* client);

static void notify_client_birth(Client* client);
static void notify_client_death(Client* client);
static void wandering_outside(Client* client);
static int vacancy_in_barber_shop(Client* client);
static void select_requests(Client* client);
static void wait_its_turn(Client* client);
static void rise_from_client_benches(Client* client);
static void wait_all_services_done(Client* client);

static char* to_string_client(Client* client);

size_t sizeof_client()
{
   return sizeof(Client);
}

int num_lines_client()
{
   return string_num_lines((char*)skel);
}

int num_columns_client()
{
   return string_num_columns((char*)skel);
}

void init_client(Client* client, int id, BarberShop* shop, int num_trips_to_barber, int line, int column)
{
   require (client != NULL, "client argument required");
   require (id > 0, concat_3str("invalid id (", int2str(id), ")"));
   require (shop != NULL, "barber shop argument required");
   require (num_trips_to_barber > 0, concat_3str("invalid number of trips to barber (", int2str(num_trips_to_barber), ")"));
   require (line >= 0, concat_3str("Invalid line (", int2str(line), ")"));
   require (column >= 0, concat_3str("Invalid column (", int2str(column), ")"));

   client->id = id;
   client->state = NONE;
   client->shop = shop;
   client->barberID = 0;
   client->num_trips_to_barber = num_trips_to_barber;
   client->requests = 0;
   client->benchesPosition = -1;
   client->chairPosition = -1;
   client->basinPosition = -1;
   client->internal = NULL;
   client->logId = register_logger((char*)("Client:"), line ,column,
                                   num_lines_client(), num_columns_client(), NULL);
}

void term_client(Client* client)
{
   require (client != NULL, "client argument required");

   if (client->internal != NULL)
   {
      mem_free(client->internal);
      client->internal = NULL;
   }
}

void log_client(Client* client)
{
   require (client != NULL, "client argument required");
   spend(random_int(global->MIN_VITALITY_TIME_UNITS, global->MAX_VITALITY_TIME_UNITS));
   send_log(client->logId, to_string_client(client));
}

void* main_client(void* args)
{
   srand(time(NULL) ^ (getpid()<<16));
   Client* client = (Client*)args;
   require (client != NULL, "client argument required");
   //debug_log(client->shop,"main_client\tStarted the CLIENT %d", client->id );
   life(client);
   return NULL;
}

static void life(Client* client)
{
   require (client != NULL, "client argument required");
   int i = 0;

   notify_client_birth(client);
   while(i < client->num_trips_to_barber)
   {
      wandering_outside(client);
      if (vacancy_in_barber_shop(client))
      {
         select_requests(client);
         wait_its_turn(client);
         rise_from_client_benches(client);
         wait_all_services_done(client);
         i++;
      }
   }
   notify_client_death(client);
}

static void notify_client_birth(Client* client)
{
   require (client != NULL, "client argument required");

   /** DONE:
    * 1: (if necessary) inform simulation that a new client begins its existence.
    * 
    * COMMENT
    * 
    * It is not necessary to inform that a new client began it's existence
    **/

   log_client(client);
}

static void notify_client_death(Client* client)
{
   /** DONE:
    * 1: (if necessary) inform simulation that a new client ceases its existence.
    * 
    * COMMENT
    * 
    * We don't need to do anything here, because the process will terminate and the parent is waitting for 
    * his termination.
    * 
    **/

   require (client != NULL, "client argument required");

   log_client(client);
}

static void wandering_outside(Client* client)
{
   /** DONE:
    * 1: set the client state to WANDERING_OUTSIDE
    * 2. random a time interval [global->MIN_OUTSIDE_TIME_UNITS, global->MAX_OUTSIDE_TIME_UNITS]         
    **/
  
   client->state = WANDERING_OUTSIDE;
   
   int random_time = random_int(global->MIN_OUTSIDE_TIME_UNITS, global->MAX_OUTSIDE_TIME_UNITS);

   //debug_log(client->shop,"wandering_outside\tCLIENT %d - is wandering outside for %d seconds", client->id , random_time);
   
   sleep(random_time);
   require (client != NULL, "client argument required");

   log_client(client);
}

static int vacancy_in_barber_shop(Client* client)
{
   /** DONE:
    * 1: set the client state to WAITING_BARBERSHOP_VACANCY
    * 2: check if there is an empty seat in the client benches (at this instante, later on it may fail)
    * 
    * COMMENT:
    * When the client is set to this state we should seat it in the bench (or try to seat it)
    * It is not supposed here to seat the client, we only have to know if there are empty seats 
    * that's why later it can fail
    * 
    **/    
    client->state = WAITING_BARBERSHOP_VACANCY;
    client->barberID = 0;
    client->basinPosition = -1;
    client->benchesPosition = -1;
    client->chairPosition = -1;
    client->requests = 0;

    psem_wait(&client->shop->mutex_client_bench);
    int res = (num_available_benches_seats(client_benches(client->shop))>0);
    psem_post(&client->shop->mutex_client_bench);
   
    require (client != NULL, "client argument required");

    log_client(client);
    return (res);
}

static void select_requests(Client* client)
{
   /** DONE:
    * 1: set the client state to SELECTING_REQUESTS
    * 2: choose a random combination of requests
    * 
    * COMMENT
    * To select a random combination of requests we have to first generate a number between 1 and 3 to see how many
    * the client will ask, and then select a request based on the probabilities of each service.
    **/ 
   int num = random_int(1,3);
   //debug_log(client->shop,"select_requests\tThe client %d will perform %d services", client->id, num);
   int services[3] = { HAIRCUT_REQ, SHAVE_REQ, WASH_HAIR_REQ};
   int probabilities[3] = {global->PROB_REQUEST_HAIRCUT,global->PROB_REQUEST_SHAVE, global->PROB_REQUEST_WASHHAIR};
   int req = 0;
   for (int i = 1; i <= num; i++) {      
      int temp_req = _generate_random_request(services,probabilities,3);      

      for (int j = 0; j < 3; j++) {
         if (services[j] == temp_req) {
            services[j] = -1;
            probabilities[j]= -1;
         }
      }
      
      req = temp_req + req;
   } 

   client->state = SELECTING_REQUESTS;
   client->requests = req;

   //debug_log(client->shop,"select_requests\tThe client %d has selected the services %d", client->id, req);

   require (client != NULL, "client argument required");

   log_client(client);
}

static void wait_its_turn(Client* client)
{
   /** DONE:
    * 1: set the client state to WAITING_ITS_TURN
    * 2: enter barbershop (if necessary waiting for an empty seat)
    * 3. "handshake" with assigned barber (greet_barber)
    * 
    * 
    * COMMENT
    * 
    * We will wait for an empty seat if none is available (random between 1 and 3), if we can get a seat then 
    * we will wait for a barber to be assigned to the client.
    * 
    **/
   client->state = WAITING_ITS_TURN;
   //debug_log(client->shop,"wait_its_turn\tThe client %d is waitting for its turn", client->id);
   int idx = -1;

   do {

      psem_wait(&client->shop->mutex_client_bench);

      if (num_available_benches_seats(client_benches(client->shop))>0) {
         idx = enter_barber_shop(client->shop,client->id, client->requests);
         client->benchesPosition = idx;
         //debug_log(client->shop,"wait_its_turn\tThe client %d is sitted in %d position", client->id, client->benchesPosition);
      } else {
         //debug_log(client->shop,"wait_its_turn\tThe client %d has no seats available", client->id);
      }
      
      psem_post(&client->shop->mutex_client_bench);

      if (idx != -1) {         
         //debug_log(client->shop,"wait_its_turn\tThe client %d is greatting the barber", client->id);         
         client->barberID = greet_barber(client->shop,client->id);
         //debug_log(client->shop,"wait_its_turn\tThe client %d has been assigned barber %d", client->id, client->barberID);
      }
      
      log_client(client);

      sleep(random_int(1,3));

   } while (idx == -1);

   require (client != NULL, "client argument required");
}

static void rise_from_client_benches(Client* client)
{
   /** DONE:
    * 1: (exactly what the name says)
    * 
    * COMMENT:
    * 
    * Remove the client from the bench
    **/


   require (client != NULL, "client argument required");
   require (client != NULL, "client argument required");
   require (seated_in_client_benches(client_benches(client->shop), client->id), concat_3str("client ",int2str(client->id)," not seated in benches"));


   psem_wait(&client->shop->mutex_client_bench);
   rise_client_benches(client_benches(client->shop),client->benchesPosition, client->id);
   psem_post(&client->shop->mutex_client_bench);
   //debug_log(client->shop, "rise_from_client_benches\tRemoved the client %d from position %d ", client->id, client->benchesPosition);   
   
   client->benchesPosition = -1;


   log_client(client);
}

static void wait_all_services_done(Client* client)
{
   /** TODO:
    * Expect the realization of one request at a time, until all requests are fulfilled.
    * For each request:
    * 1: set the client state to WAITING_SERVICE
    * 2: wait_service_from_barber from barbershop
    * 3: set the client state to WAITING_SERVICE_START
    * 4: sit in proper position in destination (chair/basin depending on the service selected)
    * 5: set the client state to the active service
    * 6: rise from destination
    *
    * At the end the client must leave the barber shop
    **/

   require (client != NULL, "client argument required");

   client->state = WAITING_SERVICE;
   log_client(client);

   int req = client->requests;
   Service s = wait_service_from_barber(client->shop, client->barberID);
   while (s.request!=-1) {
      client->basinPosition = -1;
      client->chairPosition = -1;
      client->state = WAITING_SERVICE_START;      

      if (s.request == HAIRCUT_REQ || s.request == SHAVE_REQ) {
         //debug_log(client->shop, "wait_service_from_barber\tThe client %d is seatting in barber chair position %d", s.clientID, s.pos);   
         //Sit the client in the barber chair
         psem_wait(&client->shop->mutex_barber_chairs);
         sit_in_barber_chair(barber_chair(client->shop,s.pos), client->id);
         psem_post(&client->shop->mutex_barber_chairs);
         //debug_log(client->shop, "wait_service_from_barber\tThe client %d is seated", s.clientID);
      }else {
         //debug_log(client->shop, "wait_service_from_barber\tThe client %d is seatting in washbasin position %d", s.clientID, s.pos);   
         //Sit the client in the washbasin
         psem_wait(&client->shop->mutex_washbasins);
         sit_in_washbasin(washbasin(client->shop,s.pos), client->id);
         psem_post(&client->shop->mutex_washbasins);
         //debug_log(client->shop, "wait_service_from_barber\tThe client %d is seated", s.clientID);
      }

      
      if (s.request == HAIRCUT_REQ) {
         client->state = HAVING_A_HAIRCUT;
         client->chairPosition = s.pos;
      } else if (s.request = SHAVE_REQ) {
         client->state = HAVING_A_HAIR_WASH;
         client->chairPosition = s.pos;
      } else {
         client->state = HAVING_A_SHAVE;
         client->basinPosition = s.pos;
      }
               
      //debug_log(client->shop, "wait_service_from_barber\tClient %d inform barber can  start", s.clientID);         
      //Inform the barber that he can continue to perform the service
      psem_post(&client->shop->sem_services_client[s.barberID]); 

      //debug_log(client->shop, "wait_service_from_barber\tClient %d waitting for barber %d to finish", s.clientID, s.barberID); 
      psem_wait(&client->shop->sem_services_barber[s.barberID]); 
      //debug_log(client->shop, "wait_service_from_barber\tClient %d waitting for barber %d SERVICE COMPLETED", s.clientID, s.barberID);
   
      log_client(client);   

      req = req - s.request;

      if (req ==0) break;

      s = wait_service_from_barber(client->shop, client->barberID);   
   } 

   psem_post(&client->shop->sem_services_finish[client->barberID]); 

   leave_barber_shop(client->shop,client->id);

   log_client(client);

}


static char* to_string_client(Client* client)
{
   require (client != NULL, "client argument required");

   if (client->internal == NULL)
      client->internal = (char*)mem_alloc(skel_length + 1);

   char requests[4];
   requests[0] = (client->requests & HAIRCUT_REQ) ?   'H' : ':',
   requests[1] = (client->requests & WASH_HAIR_REQ) ? 'W' : ':',
   requests[2] = (client->requests & SHAVE_REQ) ?     'S' : ':',
   requests[3] = '\0';

   char* pos = (char*)"-";
   if (client->chairPosition >= 0)
      pos = int2nstr(client->chairPosition+1, 1);
   else if (client->basinPosition >= 0)
      pos = int2nstr(client->basinPosition+1, 1);

   return gen_boxes(client->internal, skel_length, skel,
                    int2nstr(client->id, 2),
                    client->barberID > 0 ? int2nstr(client->barberID, 2) : "--",
                    requests, stateText[client->state], pos);
}

/*
  NEW FUNCTIONS
*/

int _find_ceil(int arr[], int r, int l, int h) 
{ 
    int mid; 
    while (l < h) 
    { 
         mid = l + ((h - l) >> 1);  // Same as mid = (l+h)/2 
        (r > arr[mid]) ? (l = mid + 1) : (h = mid); 
    } 
    return (arr[l] >= r) ? l : -1; 
}

int _generate_random_request(int requests[], int probabilities[], int size) 
{     
    int real_size = 0 ;
    for (int i= 0; i < size;i++){
       if (requests[i] != -1 ) real_size++;
    }

    int real_requests[real_size];
    int real_probabilities[real_size];
    int real_idx = 0;  
    for (int i= 0; i < size;i++){
       if (requests[i] != -1 ) {
          real_requests[real_idx] = requests[i];
          real_probabilities[real_idx] = probabilities[i];
          real_idx++;
       }
    }

    int prefix[real_size], i; 
    prefix[0] = real_probabilities[0]; 
    for (i = 1; i < real_size; ++i) 
        prefix[i] = prefix[i - 1] + real_probabilities[i]; 
  
    int r = (rand() % prefix[real_size - 1]) + 1; 
 
    int indexc = _find_ceil(prefix, r, 0, real_size - 1); 
    return real_requests[indexc]; 
} 

