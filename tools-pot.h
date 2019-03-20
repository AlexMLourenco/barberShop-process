/**
 * \brief tools pot representation.
 *  
 * \author Miguel Oliveira e Silva - 2018
 */

#ifndef TOOLS_POT_H
#define TOOLS_POT_H

typedef struct _ToolsPot_
{
   int availScissors;
   int availCombs;
   int availRazors;
   int logId;
   char* internal;
} ToolsPot;

int num_lines_tools_pot();
int num_columns_tools_pot();

void init_tools_pot(ToolsPot* pot, int num_scissors, int num_combs, int num_razors, int line, int column);
void term_tools_pot(ToolsPot* pot);
void log_tools_pot(ToolsPot* pot);

void pick_scissor(ToolsPot* pot);
void pick_comb(ToolsPot* pot);
void pick_razor(ToolsPot* pot);

void return_scissor(ToolsPot* pot);
void return_comb(ToolsPot* pot);
void return_razor(ToolsPot* pot);

#endif
