#include <stdlib.h>
#include "dbc.h"
#include "global.h"
#include "utils.h"
#include "box.h"
#include "logger.h"
#include "barber-bench.h"

static const int skel_length = (MAX_BARBERS*4*3+3)*4;

static char* to_string_barber_bench(BarberBench* bench);

void init_barber_bench(BarberBench* bench, int num_seats, int vertical_orientation, int line, int column)
{
   require (bench != NULL, "bench argument required");
   require (num_seats > 0 && num_seats <= MAX_BARBERS, concat_5str("invalid number of seats (", int2str(num_seats), " not in [1,", int2str(MAX_BARBERS), "])"));
   require (line >= 0, concat_3str("Invalid line (", int2str(line), ")"));
   require (column >= 0, concat_3str("Invalid column (", int2str(column), ")"));

   bench->numSeats = num_seats;
   for(int i = 0; i < num_seats; i++)
      bench->id[i] = 0; // empty
   bench->verticalOrientation = vertical_orientation;
   bench->internal = (char*)mem_alloc(skel_length + 1);
   bench->logId = register_logger((char*)("Barber bench:"), line ,column
                                  ,vertical_orientation ? num_seats*2+1 : 3
                                  ,vertical_orientation ? 5 : num_seats*4+1
                                  ,NULL);
}

void term_barber_bench(BarberBench* bench)
{
   require (bench != NULL, "bench argument required");

   mem_free(bench->internal);
}

void log_barber_bench(BarberBench* bench)
{
   require (bench != NULL, "bench argument required");

   spend(random_int(global->MIN_VITALITY_TIME_UNITS, global->MAX_VITALITY_TIME_UNITS));
   send_log(bench->logId, to_string_barber_bench(bench));
}

static char* to_string_barber_bench(BarberBench* bench)
{
   char s[skel_length+1];
   gen_matrix(s, skel_length, bench->verticalOrientation ? bench->numSeats : 1,
              bench->verticalOrientation ? 1 : bench->numSeats, 3, 5, 1);
   int i;
   for(i = 0; s[i] != '|'; i++)
      ;
   i++;
   for(int pos = 0; pos < bench->numSeats ; pos++)
   {
      if (bench->id[pos] > 0)
      {
         char* id = int2nstr(bench->id[pos], 2);
         s[i] = 'B';
         s[i+1] = id[0];
         s[i+2] = id[1];
      }
      i += 4;
      if (bench->verticalOrientation)
         i+=2+6;
   }

   return gen_boxes(bench->internal, skel_length, s);
}

int empty_barber_bench(BarberBench* bench)
{
   require (bench != NULL, "bench argument required");

   int res = 1;
   for(int pos = 0; res && pos < bench->numSeats ; pos++)
      res = (bench->id[pos] == 0);

   return res;
}

int num_seats_available_barber_bench(BarberBench* bench)
{
   require (bench != NULL, "bench argument required");

   int res = 0;
   for(int pos = 0; pos < bench->numSeats ; pos++)
      if (bench->id[pos] == 0)
         res++;

   return res;
}

int occupied_by_barber(BarberBench* bench, int pos, int id)
{
   require (bench != NULL, "bench argument required");
   require (pos >= 0 && pos < bench->numSeats, concat_5str("invalid bench position (", int2str(pos), " not in [0,", int2str(bench->numSeats), "[)"));
   require (id > 0, concat_3str("invalid id (", int2str(id), ")"));

   return bench->id[pos] == id;
}

int random_empty_seat_position_barber_bench(BarberBench* bench)
{
   require (bench != NULL, "bench argument required");
   require (num_seats_available_barber_bench(bench) > 0, "empty seat not available in barber bench");

   int r = random_int(1, num_seats_available_barber_bench(bench));
   int res;
   for(res = 0; r > 0 && res < bench->numSeats ; res++)
      if (bench->id[res] == 0)
         r--;
   res--;

   ensure (res >= 0 && res < bench->numSeats, "");

   return res;

}

int bench_seat_occupied(BarberBench* bench, int pos)
{
   require (bench != NULL, "bench argument required");
   require (pos >= 0 && pos < bench->numSeats, concat_5str("invalid bench position (", int2str(pos), " not in [0,", int2str(bench->numSeats), "[)"));

   return bench->id[pos] > 0;
}

int random_sit_in_barber_bench(BarberBench* bench, int id)
{
   require (bench != NULL, "bench argument required");
   require (id > 0, concat_3str("invalid id (", int2str(id), ")"));
   require (num_seats_available_barber_bench(bench) > 0, "empty seat not available in barber bench");
   require (!seated_in_barber_bench(bench, id), concat_3str("barber ",int2str(id)," is already seated"));

   int r = random_int(1, num_seats_available_barber_bench(bench));
   int res;
   for(res = 0; r > 0 && res < bench->numSeats ; res++)
      if (bench->id[res] == 0)
         r--;
   res--;

   check (bench->id[res] == 0, "");

   bench->id[res] = id;
   log_barber_bench(bench);

   ensure (res >= 0 && res < bench->numSeats, "");

   return res;
}

void rise_barber_bench(BarberBench* bench, int pos)
{
   require (bench != NULL, "bench argument required");
   require (pos >= 0 && pos < bench->numSeats, concat_5str("invalid bench position (", int2str(pos), " not in [0,", int2str(bench->numSeats), "[)"));
   require (bench_seat_occupied(bench, pos), concat_3str("barber bench sit ",int2str(pos)," is empty"));

   bench->id[pos] = 0;
   log_barber_bench(bench);
}

int seated_in_barber_bench(BarberBench* bench, int id)
{
   require (bench != NULL, "bench argument required");
   require (id > 0, concat_3str("invalid id (", int2str(id), ")"));

   int res = 0;
   for(int i = 0; !res && i < bench->numSeats ; i++)
      res = (bench->id[i] == id);

   return res;
}

