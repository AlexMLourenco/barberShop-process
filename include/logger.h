/*!
 * \file
 * \brief An extended log module
 *
 * \details This module allows a more versatile visualization of log information.
 * A log information is a text string, but its visualization depends on the module's mode:
 *  - It contains the usual line mode, in which each log corresponds to the output of a console line;
 *  - A more flexible window mode is also available, in which each registered log type is attached to
 *     a rectangular window in the console (that should be in a visible portion of the console).
 *
 * Any UTF8 character, and terminal color character sequence are accepted.
 * In the window mode, the visualization of the message is restricted to its registered window
 * (a new-line <code>'\\n'</code> character represents a new line within that window).
 * This module allows also the possibility to filter out (window mode) characters to a more textual and
 * simple line mode log line_mode_logger().
 *
 * The usage of this model requires the following steps:
 *
 * 1. setting line/window mode (default is window mode)
 *    - set_line_mode_logger(), set_window_mode_logger()
 * 2. initialization (thread or process version);
 *    - init_thread_logger(), init_process_logger()
 * 3. registration of log type messages;
 *    - register_logger(), add2filterout_logger(char**)
 * 4. launch logger (thread/process)
 *    - launch_logger()
 * 5. use (as a normal logger)
 *    - send_log()
 * 6. termination
 *    - term_logger()
 *  
 * \author Miguel Oliveira e Silva, 2017-2018
 */

#ifndef LOGGER_H
#define LOGGER_H


/**
 *  \brief Is logger initialized?
 *
 *  \return true (`!=0`) if initialized, false (`0`) otherwise
 */
int initialized_logger();

/**
 *  \brief Is logger in execution?
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>initialized_logger()</code></DD>
 *  </DL>
 *
 *  \return true (`!=0`) if is executing, false (`0`) otherwise
 */
int logger_launched();

/**
 *  \brief Is logger in thread version?
 *
 *  \return true (`!=0`) on thread version, false (`0`) otherwise
 */
int thread_logger();

/**
 *  \brief Is logger in process version?
 *
 *  \return true (`!=0`) on process version, false (`0`) otherwise
 */
int process_logger();

/**
 * \brief Initialize thread version of logger
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>!initialized_logger()</code></DD>
 *  </DL>
 */
void init_thread_logger();

/**
 * \brief Initialize process version of logger
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>!initialized_logger()</code></DD>
 *  </DL>
 */
void init_process_logger();

/**
 * \brief Terminate logger execution (either thread and process version)
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>initialized_logger()</code></DD>
 *  </DL>
 */
void term_logger();


/**
 *  \brief Is logger in line mode?
 *
 *  \return true (`!=0`) if on line mode, false (`0`) otherwise
 */
int line_mode_logger();

/**
 *  \brief Is logger in window mode?
 *
 *  \return true (`!=0`) if on window mode, false (`0`) otherwise
 */
int window_mode_logger();

/**
 * \brief Set the logger for line mode.
 */
void set_line_mode_logger();

/**
 * \brief Set the logger for window mode.
 */
void set_window_mode_logger();


/**
 *  \brief Adds a new list of strings which will be filtered out when line mode is active.
 *
 *  \param [in] remove list of strings to be filtered out (`NULL` terminated)
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>remove != NULL</code></DD>
 *  </DL>
 *
 *  \par <b>Example</b>
 *
 *  @verbatim
    // filter out newlines and tabs:
    const char* filterOutList[] = { "\n", "\t", NULL };
    add2filterout_logger((char**)filterOutList);
    @endverbatim
 *
 */
void add2filterout_logger(char** remove);

/**
 * \brief Create and execute logger thread/process.
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>initialized_logger()</code></DD>
 *  </DL>
 */
void launch_logger();

/**
 *  \brief Is the argument a valid line mode translation list?
 *
 *  \details A valid translation list requires either a `NULL` value (meaning no translation will occur);
 *  or an array of pairs of strings (string in window mode, and its translation to line mode),
 *  and a termination `NULL` entry.
 *  
 *  \param [in] lineModeTranslations `NULL` terminated array of translation string pairs
 *  
 *  \return true (`!=0`) if on window mode, false (`0`) otherwise
 *
 *  \par <b>Example</b>
 *
 *  @verbatim
    // replaces all occurrences of "<pt>" and "<en>" with a more clear word:
    const char* translations[] = {
       "<pt>", "Portuguese",
       "<en>", "English",
       NULL
    };
    @endverbatim
 *
 */
int valid_line_mode_translation_logger(char** lineModeTranslations);

/**
 * \brief Register a new (type) client in logger.
 *
 * \details Each client will be identified by the integer value returned by this function.
 * All future interactions with the logger will require this identification.
 * All logs in window mode will occur only in the defined rectangular area (line, column, numLines, numColumns).
 *  
 *  \param [in] name New type's description name (an empty name is accepted). This description will appear as a prefix in line mode
 *  \param [in] line window mode line location of the new type's rectangular window
 *  \param [in] column window mode column location of the new type's rectangular window
 *  \param [in] numLines window mode number of lines location of the new type's rectangular window
 *  \param [in] numColumns window mode number of columns location of the new type's rectangular window
 *  \param [in] lineModeTranslations `NULL` terminated array of translation string pairs
 *
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>initialized_logger()</code></DD><BR>
 *     <DD><code>name != NULL</code></DD><BR>
 *     <DD><code>line >= 0</code></DD><BR>
 *     <DD><code>column >= 0</code></DD><BR>
 *     <DD><code>numLines >= 0</code></DD><BR>
 *     <DD><code>numColumns >= 0</code></DD><BR>
 *     <DD><code>valid_line_mode_translation_logger(lineModeTranslations)</code></DD>
 *  </DL>
 *
 *  \return the new client registered id
 *
 *  \par <b>Example</b>
 *
 *  @verbatim
    // register a new client occupying, in window mode, box (4,2,3,10)
    // (line, column, num.lines, num.columns), without translations:
    int id = = register_logger("Client: ", 4, 2, 3, 10, NULL);
    @endverbatim
 *
 */
int register_logger(char* name, int line, int column, int numLines, int numColumns, char** lineModeTranslations);

/**
 *  \brief Send a new log to the logger.
 *  
 *  \param [in] logId logger identification
 *  \param [in] text message to log
 *
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>initialized_logger()</code></DD><BR>
 *     <DD><code>logger_launched()</code></DD><BR>
 *     <DD><code>valid_logId(logId)</code></DD><BR>
 *     <DD><code>text != NULL && strlen(text) <= MAX_TEXT_SIZE</code></DD>
 *  </DL>
 */
void send_log(int logId, char* text);

/**
 *  \brief Is the argument a valid (registered) logger identification?
 *  
 *  \param [in] logId logger identification
 *
 *  \return true (`!=0`) if it is a valid identification, false (`0`) otherwise
 */
int valid_logId(int logId);

/**
 *  \brief Get the window mode box position line corresponding to the registered type identification.
 *  
 *  \param [in] logId logger identification
 *
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>valid_logId(logId)</code></DD>
 *  </DL>
 *
 *  \return the line of the registered logger type.
 */
int get_line_logger(int logId);

/**
 *  \brief Get the window mode box position column corresponding to the registered type identification.
 *  
 *  \param [in] logId logger identification
 *
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>valid_logId(logId)</code></DD>
 *  </DL>
 *
 *  \return the column of the registered logger type.
 */
int get_column_logger(int logId);

/**
 *  \brief Get the window mode box position number of lines corresponding to the registered type identification.
 *  
 *  \param [in] logId logger identification
 *
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>valid_logId(logId)</code></DD>
 *  </DL>
 *
 *  \return the number of lines of the registered logger type.
 */
int get_num_lines_logger(int logId);

/**
 *  \brief Get the window mode box position number of columns corresponding to the registered type identification.
 *  
 *  \param [in] logId logger identification
 *
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>valid_logId(logId)</code></DD>
 *  </DL>
 *
 *  \return the number of columns of the registered logger type.
 */
int get_num_columns_logger(int logId);

#endif

