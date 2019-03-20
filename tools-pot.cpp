#include <stdlib.h>
#include "dbc.h"
#include "global.h"
#include "utils.h"
#include "box.h"
#include "logger.h"
#include "tools-pot.h"

static const int skel_length = 20*5*2+1; // extra space for (pessimistic) utf8 encoding!
static char skel[skel_length];

static char* to_string_tools_pot(ToolsPot* pot);

int num_lines_tools_pot()
{
   return 5;
}

int num_columns_tools_pot()
{
   return 19;
}

void init_tools_pot(ToolsPot* pot, int num_scissors, int num_combs, int num_razors, int line, int column)
{
   require (pot != NULL, "pot argument required");
   require (num_scissors > 0 && num_scissors <= MAX_NUM_TOOLS, concat_5str("invalid number of scissors (", int2str(num_scissors), " not in [1,", int2str(MAX_NUM_TOOLS), "])"));
   require (num_combs > 0 && num_combs <= MAX_NUM_TOOLS, concat_5str("invalid number of combs (", int2str(num_combs), " not in [1,", int2str(MAX_NUM_TOOLS), "])"));
   require (num_razors > 0 && num_razors <= MAX_NUM_TOOLS, concat_5str("invalid number of razors (", int2str(num_razors), " not in [1,", int2str(MAX_NUM_TOOLS), "])"));
   require (line >= 0, concat_3str("Invalid line (", int2str(line), ")"));
   require (column >= 0, concat_3str("Invalid column (", int2str(column), ")"));

   gen_rect(skel, skel_length, 5, 19, 0xF, 1);
   gen_overlap_boxes(skel, 0, skel,
                     "Scissors (##): ##", 1, 1,
                     "   Combs (##): ##", 2, 1,
                     "  Razors (##): ##", 3, 1,
                     NULL);
   check (num_lines_tools_pot() == string_num_lines((char*)skel), "");
   check (num_columns_tools_pot() == string_num_columns((char*)skel), "");
   pot->availScissors = num_scissors;
   pot->availCombs = num_combs;
   pot->availRazors = num_razors;
   pot->internal = NULL;
   static char* translations[] = {
      string_concat(NULL, 0, (char*)" (", SCISSOR,(char*)")",NULL), (char*)"",
      string_concat(NULL, 0, (char*)" (", COMB,   (char*)")",NULL), (char*)"",
      string_concat(NULL, 0, (char*)" (", RAZOR,  (char*)")",NULL), (char*)"",
      NULL
   };
   pot->logId = register_logger((char*)"Tools Pot:", line ,column , num_lines_tools_pot(), num_columns_tools_pot(), translations);
}

void term_tools_pot(ToolsPot* pot)
{
   require (pot != NULL, "pot argument required");

   if (pot->internal != NULL)
   {
      mem_free(pot->internal);
      pot->internal = NULL;
   }
}

void log_tools_pot(ToolsPot* pot)
{
   require (pot != NULL, "pot argument required");

   spend(random_int(global->MIN_VITALITY_TIME_UNITS, global->MAX_VITALITY_TIME_UNITS));
   send_log(pot->logId, to_string_tools_pot(pot));
}

static char* to_string_tools_pot(ToolsPot* pot)
{
   if (pot->internal == NULL)
      pot->internal = (char*)mem_alloc(skel_length + 1);

   return gen_boxes(pot->internal, skel_length, skel,
                    SCISSOR, int2nstr(pot->availScissors, 2),
                    COMB,    int2nstr(pot->availCombs, 2),
                    RAZOR,   int2nstr(pot->availRazors, 2));
}

void pick_scissor(ToolsPot* pot)
{
   require (pot != NULL, "pot argument required");
   require (pot->availScissors > 0, pot->availScissors == 0 ? "scissor not available" : concat_3str("invalid number of scissors (", int2str(pot->availScissors), ")"));

   pot->availScissors--;
   log_tools_pot(pot);
}

void pick_comb(ToolsPot* pot)
{
   require (pot != NULL, "pot argument required");
   require (pot->availCombs > 0, pot->availCombs == 0 ? "comb not available" : concat_3str("invalid number of combs (", int2str(pot->availCombs), ")"));

   pot->availCombs--;
   log_tools_pot(pot);
}

void pick_razor(ToolsPot* pot)
{
   require (pot != NULL, "pot argument required");
   require (pot->availRazors > 0, pot->availRazors == 0 ? "razor not available" : concat_3str("invalid number of razors (", int2str(pot->availRazors), ")"));

   pot->availRazors--;
   log_tools_pot(pot);
}

void return_scissor(ToolsPot* pot)
{
   require (pot != NULL, "pot argument required");
   require (pot->availScissors < MAX_NUM_TOOLS, concat_3str("invalid number of scissors (", int2str(pot->availScissors), ")"));

   pot->availScissors++;
   log_tools_pot(pot);
}

void return_comb(ToolsPot* pot)
{
   require (pot != NULL, "pot argument required");
   require (pot->availCombs < MAX_NUM_TOOLS, concat_3str("invalid number of combs (", int2str(pot->availCombs), ")"));

   pot->availCombs++;
   log_tools_pot(pot);
}

void return_razor(ToolsPot* pot)
{
   require (pot != NULL, "pot argument required");
   require (pot->availRazors < MAX_NUM_TOOLS, concat_3str("invalid number of razors (", int2str(pot->availRazors), ")"));

   pot->availRazors++;
   log_tools_pot(pot);
}

