/*!
 * \file
 *
 * \brief Timer module
 * 
 * \details This module allows expression of time delays in function of time units.
 * 
 * \author Miguel Oliveira e Silva, 2017-2018
 **/


#ifndef TIMER_H
#define TIMER_H

/**
 *  \brief Define timer period (one time unit duration)
 *
 *  \param [in] milliseconds the value of time unit
 *  
 *  <DL><DT><B>Precondition:</B></DT>
       <DD><code>milliseconds > 0</code></DD>
 *  </DL>
 */
void set_time_unit(int milliseconds);

/**
 *  \brief Get the timer period (one time unit duration)
 *
 *  \return the defined one time unit duration
 */
int time_unit();

/**
 *  \brief Sleep `numTimeUnits` time units
 *
 *  \param [in] numTimeUnits the number of time units
 *  
 *  <DL><DT><B>Precondition:</B></DT>
       <DD><code>numTimeUnits >= 0</code></DD>
 *  </DL>
 */
void spend(int numTimeUnits);

#endif
