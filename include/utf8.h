/*!
 * \file
 *
 * \brief Module to handle UTF8 symbols.
 *
 * \details This module contains an internal table of (some) UTF8 symbols names that can be
 * used to identify symbols.
 * 
 * \author Miguel Oliveira e Silva, 2018
 **/

#ifndef UTF8_H
#define UTF8_H


/**
 *  \brief Maximum number of bytes of an UTF8 character supported by this module (not counting terminator `'\0'`).
 */
#define MAX_UTF8_STRING 4

/**
 *  \brief Counts the number of UTF8 charactes in a string.
 *
 *  \details This function solves the problem of character counting, since with UTF8 charactes the length of a string is not necessarly its number of characters.
 *
 *  \param [in] utf8_string text to be processed
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>utf8_string != NULL</code></DD>
 *  </DL>
 *
 *  \return number of UTF8 character strings
 */
int num_chars_utf8(char* utf8_string);

/**
 *  \brief Converts an UTF8 integer code to the sequence of UTF8 bytes.
 *
 *  \details A terminated string is generated with the sequence of bytes of the given UTF8 code.
 *  If argument `res` is not `NULL`, if must point to a memory address with at least `(MAX_UTF8_STRING+1)` bytes.
 *
 *  \param [in,out] res address of result string (if not `NULL`)
 *  \param [in] code UTF8 integer code
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>code <= 0x001FFFFF</code></DD>
 *  </DL>
 *
 *  \return the converted string (if not `NULL`, it returns `res`)
 */
char* code2utf8(char*res, int code);

/**
 *  \brief Number of UTF8 symbol entries of the module's internal table.
 *
 *  \return the internal table length
 */
int num_entries_utf8();

/**
 *  \brief Total length of all UTF8 symbol names (with an extra `'\\n'` for each name) of the module's internal table.
 *
 *  \return the total length (not counting terminator `'\0'`)
 */
int all_names_size_utf8();

/**
 *  \brief Checks if exists a symbol in the module's internal table for a given non-empty name.
 *
 *  \param [in] name the non-empty name to be searched
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>name != NULL && name[0] != '\0'</code></DD>
 *  </DL>
 *
 *  \return true (`!=0`) if name exists, false (`0`) otherwise
 */
int exists_name_utf8(char* name);

/**
 *  \brief Get the UTF8 integer code of a UTF8 symbol name.
 *
 *  \param [in] name the non-empty name attached to the code
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>exists_name_utf8(name)</code></DD>
 *  </DL>
 *
 *  \return UTF8 integer code
 */
int get_code_utf8(char* name);

/**
 *  \brief Returns the list of all names of internal table.
 *
 *  \details Generates a string with all the names of the internal table, all terminated with the `'\\n'` character.
 *
 *  If argument `res` is not `NULL`, it must point to a memory address with at least `(all_names_size_utf8()+1)` bytes.
 *  If `res` argument is `NULL`, memory is allocated in the heap (using mem_alloc()).
 *
 *  \param [in,out] res address of result string (if not `NULL`)
 *  \param [in] max_length `res` input argument maximum length (not counting terminator `'\0'`), only applies if `(res != NULL)`
 *  
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>result == NULL || max_length >= all_names_size_utf8()</code></DD>
 *  </DL>
 *
 *  \return if `res` is `NULL`, returns the address of the allocated memory, otherwise it return `res`
 */
char* all_names_utf8(char* res, int max_length);

#endif

