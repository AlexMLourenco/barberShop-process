/**
 * \brief service information
 *  
 * \author Miguel Oliveira e Silva - 2018
 */

#ifndef SERVICE_H
#define SERVICE_H

typedef struct _Service_
{
   int barberChair;
   int washbasin;
   int barberID;
   int clientID;
   int pos;
   int request;
} Service;

void set_barber_chair_service(Service* service, int barber_id, int client_id, int pos, int request);
void set_washbasin_service(Service* service, int barber_id, int client_id, int pos);
int is_barber_chair_service(Service* service);
int is_washbasin_service(Service* service);
int service_barber_id(Service* service);
int service_client_id(Service* service);
int service_position(Service* service);
int service_request(Service* service);


#endif
