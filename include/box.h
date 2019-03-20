/*!
 * \file
 * \brief A set of functions to ease the construction of textual boxes.
 *
 * \author Miguel Oliveira e Silva, 2018
 */

#ifndef BOX_H
#define BOX_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

/** @name Directions definitions
 *  @anchor directions @{
 */
#define BOX_NORTH_DIRECTION 0x8
#define BOX_EAST_DIRECTION  0x4
#define BOX_WEST_DIRECTION  0x2
#define BOX_SOUTH_DIRECTION 0x1
#define BOX_NO_DIRECTION    0x0

#define BOX_UP_DIRECTION BOX_NORTH_DIRECTION
#define BOX_RIGHT_DIRECTION BOX_EAST_DIRECTION
#define BOX_LEFT_DIRECTION BOX_WEST_DIRECTION
#define BOX_DOWN_DIRECTION BOX_SOUTH_DIRECTION
/** @} */

/** @name Boxes mode definition
 *  @anchor boxes_modes @{
 */

/**
 *  \brief Activate ASCII mode for boxes.
 *
 *  \details In ASCII mode, boxes are drawn only with ASCII characters.
 */
void set_ascii_mode_boxes();

/**
 *  \brief Activate UTF8 mode for boxes.
 *
 *  \details In UTF8 mode, boxes are drawn with UTF8 line drawing characters.
 */
void set_utf8_mode_boxes();

/**
 *  \brief Is ASCII mode for boxes active?
 *
 *  \return true (`!=0`) if active, false (`0`) otherwise
 */
int ascii_mode_boxes();

/**
 *  \brief Is UTF8 mode for boxes active?
 *
 *  \return true (`!=0`) if active, false (`0`) otherwise
 */
int utf8_mode_boxes();

/** @} */

/**
 *  \brief Remove line drawing characters for logger line mode.
 *
 *  \details Invokes logger's function add2filterout_logger() to remove line drawing characters for logger line moe.
 */
void logger_filter_out_boxes();

/**
 *  \brief Determines the exact result string length of a gen_boxes() application.
 *
 *  \details See gen_boxes() for a description of a box specification.
 *
 *  \param [in] box the skeleton box definition
 *  \param [in] ... optional arguments (one per each sequence of `#`'s)
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>box != NULL</code></DD>
 *  </DL>
 *
 *  \return the length of the box definition (not counting terminator `'\0'`)
 */
int gen_boxes_length(const char* box, ...);

/**
 *  \brief Generates boxes and lines with argument replacement.
 *
 *  \details Generates a string with lines, corners, and boxes, from a
 *  skeleton box specification, with optional argument replacement.
 *
 *  Optional arguments are defined in the skeleton box specification through
 *  contiguous sequences of `#` characters.  The number of `#`
 *  determines the maximum number of columns usable by its applicable argument.
 *  Arguments appear after the box argument, in the order prescribed by the
 *  skeleton, are are all interpreted as strings.
 *
 *  \param [in,out] res address of result string (if not `NULL`)
 *  \param [in] max_length `res` input argument maximum length (not counting terminator `'\0'`), only applies if `(res != NULL)`
 *  \param [in] box the skeleton box definition
 *  \param [in] ... optional arguments (one per each sequence of `#`'s)
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>box != NULL</code></DD><BR>
 *     <DD><code>res == NULL || max_length >= gen_boxes_length(box, ...)</code></DD>
 *  </DL>
 *
 *  \note Function gen_boxes_length() can be used to determine the exact length of result string.
 *
 *  \par <b>Skeleton box specification</b>
 *  @verbatim
       Special characters:
         '.': ignore character (occupies no space to allow the specification of boxes joined together)
         '-': horizontal segment
         '|': vertical segment
         '+': corner segment (its translation depends on the neighbours)
         '@': round corner segment (its translation depends on the neighbours)
         '#': arguments in box specification
  
       sequence of # represent an optional argument
    @endverbatim
 *
 *  \par <b>Example</b>
 *
 *  @verbatim
    gen_boxes(NULL, 100,                    // destination string
              "  +--------+\n"              // BOX definition
              "  |Philo ##|\n"              // first argument with 2 positions
              "+-+--------+-+\n"
              "|#|########|#|\n"            // arguments 2, 3 and 4
              "+-+--------+-+"
              "#1", " ", "THINKING", " ");  // the four arguments related to box definition
    @endverbatim
 *
 *  \return the converted string (if not `NULL`, it returns `res`)
 */
char* gen_boxes(char* res, int max_length, const char* box, ...);

/**
 *  \brief Determines the exact result string length of a gen_rect() application.
 *
 *  \details See gen_rect() for a description of a rectangle specification.
 *
 *  \param [in] num_lines rectangle number of lines
 *  \param [in] num_columns rectangle number of columns
 *  
 *  \return the length of the rectangle definition (not counting terminator `'\0'`)
 */
int gen_rect_length(int num_lines, int num_columns);

/**
 *  \brief Generates an ASCII text rectangle with optional wall drawing.
 *
 *  \details Generates a string with a rectangle, with optional North/East/West/South (N.E.W.S) wall drawing.
 *
 *  The rectangle drawing mask (argument wall_mask), is specified by the four lower bits of the integer
 *  (corresponding to the order N.E.W.S).
 *
 *  \param [in,out] res address of result string (if not `NULL`)
 *  \param [in] max_length `res` input argument maximum length (not counting terminator `'\0'`), only applies if `(res != NULL)`
 *  \param [in] num_lines rectangle number of lines
 *  \param [in] num_columns rectangle number of columns
 *  \param [in] wall_mask mask specifying which wall are to be drawn (lowest 4 bits with N.E.W.S. order) 
 *  \param [in] round_corners boolean argument indicating a rounded rectangle
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>num_lines > 1</code></DD><BR>
 *     <DD><code>num_columns > 1</code></DD><BR>
 *     <DD><code>res == NULL || (max_length >= gen_rect_length(num_lines, num_columns))</code></DD><BR>
 *     <DD><code>wall_mask >= 0x0 && wall_mask <= 0xF</code></DD>
 *  </DL>
 *
 *  \note Function gen_rect_length() can be used to determine the exact length of result string.
 *
 *  \par <b>Example</b>
 *  @verbatim
    gen_rect(NULL, 100,      // destination string
             4, 5,           // num_lines, num_columns
             0xF,            // mask bits: N.E.W.S
                             //   (wall at north/up, wall at east/right,
                             //    wall at west/left, wall at south/down)
             1);             // round corners
    @endverbatim
 *
 *  \return the converted string (if not `NULL`, it returns `res`)
 */
char* gen_rect(char* res, int max_length, int num_lines, int num_columns, int wall_mask, int round_corners);

/**
 *  \brief Generates an text rectangle filled with spaces.
 *
 *  \param [in,out] res address of result string (if not `NULL`)
 *  \param [in] max_length `res` input argument maximum length (not counting terminator `'\0'`), only applies if `(res != NULL)`
 *  \param [in] num_lines rectangle number of lines
 *  \param [in] num_columns rectangle number of columns
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>num_lines > 1</code></DD><BR>
 *     <DD><code>num_columns > 1</code></DD><BR>
 *     <DD><code>res == NULL || (max_length >= gen_rect_length(num_lines, num_columns))</code></DD>
 *  </DL>
 *
 *  \note Function gen_rect_length() can be used to determine the exact length of result string.
 *
 *  \return the converted string (if not `NULL`, it returns `res`)
 */
char* gen_empty_rect(char* res, int max_length, int num_lines, int num_columns);

/**
 *  \brief Determines the exact result string length of a gen_matrix() application.
 *
 *  \details See gen_matrix() for a description of a matrix specification.
 *
 *  \param [in] num_lines matrix number of lines
 *  \param [in] num_columns matrix number of columns
 *  \param [in] rect_num_lines number of lines of each matrix rectangle
 *  \param [in] rect_num_columns number of columns of each matrix rectangle
 *  
 *  \return the length of the matrix definition (not counting terminator `'\0'`)
 */
int gen_matrix_length(int num_lines, int num_columns, int rect_num_lines, int rect_num_columns);

/**
 *  \brief Generates an ASCII text matrix.
 *
 *  \details Generates a string with a matrix.
 *
 *  \param [in,out] res address of result string (if not `NULL`)
 *  \param [in] max_length `res` input argument maximum length (not counting terminator `'\0'`), only applies if `(res != NULL)`
 *  \param [in] num_lines rectangle number of lines
 *  \param [in] num_columns rectangle number of columns
 *  \param [in] rect_num_lines number of lines of each matrix rectangle
 *  \param [in] rect_num_columns number of columns of each matrix rectangle
 *  \param [in] round_corners boolean argument indicating a rounded rectangle
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>num_lines > 1</code></DD><BR>
 *     <DD><code>num_columns > 1</code></DD><BR>
 *     <DD><code>rect_num_lines > 1</code></DD><BR>
 *     <DD><code>rect_num_columns > 1</code></DD><BR>
 *     <DD><code>res == NULL || (max_length >= gen_matrix_length(num_lines, num_columns, rect_num_lines, rect_num_columns))</code></DD>
 *  </DL>
 *
 *  \note Function gen_matrix_length() can be used to determine the exact length of result string.
 *
 *  \par <b>Example</b>
 *  @verbatim
    gen_matrix(NULL, 100,      // destination string
               1, 5,           // num_lines, num_columns of matrix
               3, 5,           // num_lines, num_columns each matrix rect
               1);             // round corners
    @endverbatim
 *
 *  \return the converted string (if not `NULL`, it returns `res`)
 */
char* gen_matrix(char* res, int max_length, int num_lines, int num_columns, int rect_num_lines, int rect_num_columns, int round_corners);

/**
 *  \brief Is direction valid?
 *
 *  \param [in] direction the direction
 *  
 *  \return true (`!=0`) if direction valid, false (`0`) otherwise
 */
int valid_direction(int direction);

/**
 *  \brief Determines the exact result string length of a gen_lines() application.
 *
 *  \details See gen_lines() for a description of a line drawing specification.
 *
 *  \param [in] direction the direction
 *  \param [in] ... optional arguments (groups of two arguments, each group with a direction and length definition)
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>valid_direction(direction)</code></DD><BR>
 *     <DD><code>all other directions are valid</code></DD><BR>
 *     <DD><code>all lengths are valid</code></DD>
 *  </DL>
 *
 *  \return the length of the draw lines definition (not counting terminator `'\0'`)
 */
int gen_lines_length(int direction, ...);

/**
 *  \brief Generates a string representing generic continuous line drawing.
 *
 *  \details Generates a string with horizontal and/or vertical continuous line drawing.
 *  Each line is specified by a pair of values: direction and length. The drawing terminates
 *  with direction BOX_NO_DIRECTION value.
 *
 *  \param [in,out] res address of result string (if not `NULL`)
 *  \param [in] max_length `res` input argument maximum length (not counting terminator `'\0'`), only applies if `(res != NULL)`
 *  \param [in] direction the direction
 *  \param [in] ... optional arguments (groups of two arguments, each group with a direction and length definition)
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>valid_direction(direction)</code></DD><BR>
 *     <DD><code>all other directions are valid</code></DD><BR>
 *     <DD><code>all lengths are valid</code></DD><BR>
 *     <DD><code>res == NULL || max_length >= gen_lines_length(direction, ...)</code></DD>
 *  </DL>
 *
 *  \note Function gen_lines_length() can be used to determine the exact length of result string.
 *
 *  \par <b>Example</b>
 *  @verbatim
    gen_lines(NULL, 100,     // destination string
              BOX_LEFT_DIRECTION, 8, BOX_UP_DIRECTION, 10,
              BOX_RIGHT_DIRECTION, 14, BOX_DOWN_DIRECTION, 10,
              BOX_LEFT_DIRECTION, 2, BOX_NO_DIRECTION)
    @endverbatim
 *
 *  \return the converted string (if not `NULL`, it returns `res`)
 */
char* gen_lines(char* res, int max_length, int direction, ...);

/**
 *  \brief Determines a generic box dimensions (number of lines and number of columns).
 *
 *  \note When determining the number of columns, character `'\\n'` is not counted.
 *
 *  \param [in] box the box definition
 *  \param [out] pnum_lins address of variable where the box number of lines is to be stored
 *  \param [out] pnum_cols address of variable where the box number of columns is to be stored
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>box != NULL</code></DD><BR>
 *     <DD><code>pnum_lins != NULL</code></DD><BR>
 *     <DD><code>pnum_cols != NULL</code></DD>
 *  </DL>
 */
void box_dimensions(char* box, int* pnum_lins, int* pnum_cols);

/**
 *  \brief Determines the exact result string length of a gen_overlap_boxes() application.
 *
 *  \details See gen_overlap_boxes() for a description of an overlapping box specification.
 *
 *  \param [in] first_box the base string box
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>first_box != NULL</code></DD>
 *  </DL>
 *
 *  \return the length of the box definition (not counting terminator `'\0'`)
 */
int gen_overlap_boxes_length(char* first_box);

/**
 *  \brief Checks if overlapping boxes arguments are valid in a gen_overlap_boxes() application.
 *
 *  \details Valid arguments are those whose box position is inside the base string box.
 *
 *  \param [in] first_box the base string box
 *  \param [in] ... optional arguments (three per each overlapping box)
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>first_box != NULL</code></DD>
 *  </DL>
 *
 *  \return true (`!=0`) if all boxes are valid, false (`0`) otherwise
 */
int gen_overlap_valid_boxes(char* first_box, ...);

/**
 *  \brief Generates a text box resulting from overlapping any number of ASCII character boxes (with any printable ASCII character).
 *
 *  \details This function has the following characteristics:
 *    - each overlapping box definition is specified by a triple set of values:
 *      string box, destination line, and destination column.
 *      The box definition terminates with a `NULL` argument.
 *    - is only applicable to (printable) ASCII characters;
 *    - all boxes need to be fully contained in first_box;
 *    - to change an existing string, destination string can be first_box argument (simply by passing `NULL` in res).
 *    - in all box definition, the following conditions apply:
 *       -# `line(box)` in `[0;num_lines(first_box)[`
 *       -# `column(box)` in `[0;num_columns(first_box)[`
 *       -# `line(box)+num_lines(box)` in `[1;num_lines(first_box)]`
 *       -# `column(box)+num_columns(box)` in `[1;num_columns(first_box)]`
 *
 *  \param [in,out] res address of result string (if not `NULL`)
 *  \param [in] max_length `res` input argument maximum length (not counting terminator `'\0'`), only applies if `(res != NULL)`
 *  \param [in] first_box the base string box
 *  \param [in] ... optional arguments (three per each overlapping box)
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>first_box != NULL</code></DD><BR>
 *     <DD><code>res == NULL || res == first_box || max_length >= gen_overlap_boxes_length(first_box)</code></DD><BR>
 *     <DD><code>gen_overlap_valid_boxes(first_box, ...)</code></DD>
 *  </DL>
 *
 *  \note Function gen_overlap_boxes_length() can be used to determine the exact length of result string.
 *
 *  \par <b>Example</b>
 *  @verbatim
    gen_overlap_boxes(NULL, 100,     // destination string (accepts first_box as target)
                      first_box,     // first 'background' box
                      box2, 3, 2,    // overlap with box2 at line 3, column 2 of first_box
                      box3, 4, 1,    // overlap with box3 at line 4, column 1 of first_box
                      NULL);         // no more boxes
    @endverbatim
 *
 *  \return the converted string (if not `NULL`, it returns `res`)
 */
char* gen_overlap_boxes(char* res, int max_length, char* first_box, ...);

/**
 *  \brief Determines the exact result string length of a progress_bar() application.
 *
 *  \details See progress_bar() for a description of a progress bar specification.
 *
 *  \param [in] num_cols number of columns of the progress bar line
 *  \param [in] complete_char UTF8 character for complete indication
 *  \param [in] incomplete_char UTF8 character for incomplete indication
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>num_cols > 0</code></DD><BR>
 *     <DD><code>complete_char != NULL && num_chars_utf8(complete_char) == 1</code></DD><BR>
 *     <DD><code>incomplete_char != NULL && num_chars_utf8(incomplete_char) == 1</code></DD>
 *  </DL>
 *
 *  \return the length of the progress bar line definition (not counting terminator `'\0'`)
 */
int progress_bar_length(int num_cols, char* complete_char, char* incomplete_char);

/**
 *  \brief Generates an UFT8 progress bar text line for a given percentage.
 *
 *  \details The percentage value determines the balance of complete_char and incomplete_char characters drawn.
 *
 *  \param [in,out] res address of result string (if not `NULL`)
 *  \param [in] max_length `res` input argument maximum length (not counting terminator `'\0'`), only applies if `(res != NULL)`
 *  \param [in] percentage an integer number with a percentage value ([0;100])
 *  \param [in] num_cols number of columns of the progress bar line
 *  \param [in] complete_char UTF8 character for complete indication
 *  \param [in] incomplete_char UTF8 character for incomplete indication
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>percentage >= 0 && percentage <= 100</code></DD><BR>
 *     <DD><code>num_cols > 0</code></DD><BR>
 *     <DD><code>complete_char != NULL && num_chars_utf8(complete_char) == 1</code></DD><BR>
 *     <DD><code>incomplete_char != NULL && num_chars_utf8(incomplete_char) == 1</code></DD><BR>
 *     <DD><code>res == NULL || max_length >= progress_bar_length(num_cols, complete_char, incomplete_char)</code></DD>
 *  </DL>
 *
 *  \note Function progress_bar_length() can be used to determine the exact length of result string.
 *
 *  \return the converted string (if not `NULL`, it returns `res`)
 */
char* progress_bar(char* res, int max_length, int percentage, int num_cols, char* complete_char, char* incomplete_char);

#endif

