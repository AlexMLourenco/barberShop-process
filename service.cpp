#include "dbc.h"
#include "global.h"
#include "service.h"

void set_barber_chair_service(Service* service, int barber_id, int client_id, int pos, int request)
{
   require (service != NULL, "service argument required");
   require (barber_id > 0, concat_3str("invalid barber id (", int2str(barber_id), ")"));
   require (client_id > 0, concat_3str("invalid client id (", int2str(client_id), ")"));
   require (pos >= 0, concat_3str("invalid position (", int2str(pos), ")"));
   require (request == HAIRCUT_REQ || request == SHAVE_REQ, concat_3str("invalid request (", int2str(request), ")"));

   service->barberChair = 1;
   service->washbasin = 0;
   service->barberID = barber_id;
   service->clientID = client_id;
   service->pos = pos;
   service->request = request;
}

void set_washbasin_service(Service* service, int barber_id, int client_id, int pos)
{
   require (service != NULL, "service argument required");
   require (barber_id > 0, concat_3str("invalid barber id (", int2str(barber_id), ")"));
   require (client_id > 0, concat_3str("invalid client id (", int2str(client_id), ")"));
   require (pos >= 0, concat_3str("invalid position (", int2str(pos), ")"));

   service->barberChair = 0;
   service->washbasin = 1;
   service->barberID = barber_id;
   service->clientID = client_id;
   service->pos = pos;
   service->request = WASH_HAIR_REQ;
}

int is_barber_chair_service(Service* service)
{
   require (service != NULL, "service argument required");

   return service->barberChair;
}

int is_washbasin_service(Service* service)
{
   require (service != NULL, "service argument required");

   return service->washbasin;
}

int service_barber_id(Service* service)
{
   require (service != NULL, "service argument required");

   return service->barberID;
}

int service_client_id(Service* service)
{
   require (service != NULL, "service argument required");

   return service->clientID;
}

int service_position(Service* service)
{
   require (service != NULL, "service argument required");

   return service->pos;
}

int service_request(Service* service)
{
   require (service != NULL, "service argument required");

   return service->request;
}

