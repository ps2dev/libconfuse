/* Configuration file parser -*- tab-width: 4; -*-
 *
 * Copyright (c) 2002-2003, Martin Hedenfalk <mhe@home.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

/** A configuration file parser library.
 * @file confuse.h
 *
 */

/**
 * \mainpage libConfuse Documentation
 *
 * \section intro
 *
 * Copyright &copy; 2002-2003 Martin Hedenfalk &lt;mhe@home.se&gt;
 *
 * The latest versions of this manual and the libConfuse software are
 * available at http://www.e.kth.se/~e97_mhe/confuse.shtml
 *
 *
 * <em>If you can't convince, confuse.</em>
 */

#ifndef _cfg_h_
#define _cfg_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>

#ifdef _WIN32

# ifdef HAVE__FILENO
#  define fileno _fileno
# endif
# ifdef HAVE__ISATTY
#  define isatty _isatty
# endif

# ifdef BUILDING_DLL
#  define DLLIMPORT __declspec (dllexport)
# else /* Not BUILDING_DLL */
#  define DLLIMPORT __declspec (dllimport)
# endif /* Not BUILDING_DLL */

#else /* ! _WIN32 */
# define DLLIMPORT

#endif /* _WIN32 */

#ifndef __BORLANDC__
# define __export
#endif

/** Fundamental option types */
enum cfg_type_t {
	CFGT_NONE,
	CFGT_INT,     /**< integer */
	CFGT_FLOAT,   /**< floating point number */
	CFGT_STR,     /**< string */
	CFGT_BOOL,    /**< boolean value */
	CFGT_SEC,     /**< section */
	CFGT_FUNC     /**< function */
};
typedef enum cfg_type_t cfg_type_t;

/** Flags. */
#define CFGF_NONE 0
#define CFGF_MULTI 1       /**< option may be specified multiple times */
#define CFGF_LIST 2        /**< option is a list */
#define CFGF_NOCASE 4      /**< configuration file is case insensitive */
#define CFGF_TITLE 8       /**< option has a title (only applies to section) */
#define CFGF_ALLOCATED 16
#define CFGF_RESET 32

/** Return codes from cfg_parse(). */
#define CFG_SUCCESS 0
#define CFG_FILE_ERROR -1
#define CFG_PARSE_ERROR 1

#define is_set(f, x) ((f & x) == f)

typedef union cfg_value_t cfg_value_t;
typedef struct cfg_opt_t cfg_opt_t;
typedef struct cfg_t cfg_t;
typedef struct cfg_defvalue_t cfg_defvalue_t;
typedef int cfg_flag_t;

/** Function prototype used by CFGT_FUNC options.
 *
 * This is a callback function, registered with the CFG_FUNC
 * initializer. Each time libConfuse finds a function, the registered
 * callback function is called (parameters are passed as strings, any
 * conversion to other types should be made in the callback
 * function). libConfuse does not support any storage of the data
 * found; these are passed as parameters to the callback, and it's the
 * responsibility of the callback function to do whatever it should do
 * with the data.
 *
 * @param cfg The configuration file context.
 * @param opt The option.
 * @param argc Number of arguments passed. The callback function is
 * responsible for checking that the correct number of arguments are
 * passed.
 * @param argv Arguments as an array of character strings.
 *
 * @return On success, 0 should be returned. All other values
 * indicates an error, and the parsing is aborted. The callback
 * function should notify the error itself, for example by calling
 * cfg_error().
 *
 * @see CFG_FUNC
 */
typedef int (*cfg_func_t)(cfg_t *cfg, cfg_opt_t *opt,
						  int argc, const char **argv);

/** Value parsing callback prototype
 *
 * This is a callback function (different from the one registered with
 * the CFG_FUNC initializer) used to parse a value. This can be used
 * to override the internal parsing of a value.
 *
 * Suppose you want an integer option that only can have certain
 * values, for example 1, 2 and 3, and these should be written in the
 * configuration file as "yes", "no" and "maybe". The callback
 * function would be called with the found value ("yes", "no" or
 * "maybe") as a string, and the result should be stored in the result
 * parameter.
 *
 * @param cfg The configuration file context.
 * @param opt The option.
 * @param value The value found in the configuration file.
 * @param result Pointer to storage for the result, cast to a void pointer.
 *
 * @return On success, 0 should be returned. All other values
 * indicates an error, and the parsing is aborted. The callback
 * function should notify the error itself, for example by calling
 * cfg_error().
 */
typedef int (*cfg_callback_t)(cfg_t *cfg, cfg_opt_t *opt,
							  const char *value, void *result);

/** Boolean values. */
typedef enum {cfg_false, cfg_true} cfg_bool_t;

/** Error reporting function. */
typedef void (*cfg_errfunc_t)(cfg_t *cfg, const char *fmt, va_list ap);

/** Data structure holding information about a "section". Sections can
 * be nested. A section has a list of options (strings, numbers,
 * booleans or other sections) grouped together.
 */
struct cfg_t {
	cfg_flag_t flags;       /**< Any flags passed to cfg_init() */
	char *name;             /**< The name of this section, the root
							 * section returned from cfg_init() is
							 * always named "root" */
	cfg_opt_t *opts;        /**< Array of options */
	char *title;            /**< Optional title for this section, only
							 * set if CFGF_TITLE flag is set */
	char *filename;         /**< Name of the file being parsed */
	int line;               /**< Line number in the config file */
	cfg_errfunc_t errfunc;  /**< This function (if set with
							 * cfg_set_error_function) is called for
							 * any error message. */
};

/** Data structure holding the value of a fundamental option value.
 */
union cfg_value_t {
	long int number;        /**< integer value */
	double fpnumber;        /**< floating point value */
	cfg_bool_t boolean;     /**< boolean value */
	char *string;           /**< string value */
	cfg_t *section;         /**< section value */
};

struct cfg_defvalue_t {
	long int number;
	double fpnumber;
	cfg_bool_t boolean;
	char *string;
	char *parsed;
};

/** Data structure holding information about an option. The value(s)
 * are stored as an array of fundamental values (strings, numbers).
 */
struct cfg_opt_t {
	char *name;             /**< The name of the option */
	cfg_type_t type;        /**< Type of option */
	unsigned int nvalues;   /**< Number of values parsed */
	cfg_value_t **values;   /**< Array of found values */
	cfg_flag_t flags;       /**< Flags */
	cfg_opt_t *subopts;     /**< Suboptions (only applies to sections) */
	cfg_defvalue_t def;     /**< Default value */
	cfg_func_t func;        /**< Function callback for CFGT_FUNC options */
	void *simple_value;     /**< Pointer to user-specified variable to
							 * store simple values (created with the
							 * CFG_SIMPLE_* initializers) */
	cfg_callback_t cb;      /**< Value parsing callback function */
};

extern const char __export confuse_copyright[];
extern const char __export confuse_version[];
extern const char __export confuse_author[];

#define __CFG_STR(name, def, flags, svalue, cb) \
  {name,CFGT_STR,0,0,flags,0,{0,0,cfg_false,def,0},0,svalue,cb}
#define __CFG_STR_LIST(name, def, flags, svalue, cb) \
  {name,CFGT_STR,0,0,flags | CFGF_LIST,0,{0,0,cfg_false,0,def},0,svalue,cb}

/** Initialize a string option
 */
#define CFG_STR(name, def, flags) \
  __CFG_STR(name, def, flags, 0, 0)

/** Initialize a string list option
 */
#define CFG_STR_LIST(name, def, flags) \
  __CFG_STR_LIST(name, def, flags, 0, 0)

/** Initialize a string option with a value parsing callback
 */
#define CFG_STR_CB(name, def, flags, cb) \
  __CFG_STR(name, def, flags, 0, cb)

/** Initialize a string list option with a value parsing callback
 */
#define CFG_STR_LIST_CB(name, def, flags, cb) \
  __CFG_STR_LIST(name, def, flags, 0, cb)

/** Initialize a "simple" string option.
 *
 * "Simple" options (in lack of a better expression) does not support
 * lists of values or multiple sections. LibConfuse will store the
 * value of a simple option in the user-defined location specified by
 * the value parameter in the initializer. Simple options are not
 * stored in the cfg_t context (you can thus not use the cfg_get*
 * functions to get the value). Sections can not be initialized as a
 * "simple" option.
 *
 * @param name name of the option
 * @param svalue pointer to a character pointer (a char **). This value
 * must be initalized either to NULL or to a malloc()'ed string. You
 * can't use
 * <pre>
 * char *user = "joe";
 * ...
 * cfg_opt_t opts[] = {
 *     CFG_SIMPLE_STR("user", &user),
 * ...
 * </pre>
 * since libConfuse will try to free the static string "joe" (which is
 * an error) when a "user" option is found. Rather, use the following
 * code snippet:
 *
 * <pre>
 * char *user = strdup("joe");
 * ...
 * cfg_opt_t opts[] = {
 *      CFG_SIMPLE_STR("user", &user),
 * ...
 * </pre>
 *
 */
#define CFG_SIMPLE_STR(name, svalue) \
  __CFG_STR(name, 0, CFGF_NONE, svalue, 0)


#define __CFG_INT(name, def, flags, svalue, cb) \
  {name,CFGT_INT,0,0,flags,0,{def,0,cfg_false,0,0},0,svalue,cb}
#define __CFG_INT_LIST(name, def, flags, svalue, cb) \
  {name,CFGT_INT,0,0,flags | CFGF_LIST,0,{0,0,cfg_false,0,def},0,svalue,cb}

/** Initialize an integer option
 */
#define CFG_INT(name, def, flags) \
  __CFG_INT(name, def, flags, 0, 0)

/** Initialize an integer list option
 */
#define CFG_INT_LIST(name, def, flags) \
  __CFG_INT_LIST(name, def, flags, 0, 0)

/** Initialize an integer option with a value parsing callback
 */
#define CFG_INT_CB(name, def, flags, cb) \
  __CFG_INT(name, def, flags, 0, cb)

/** Initialize an integer list option with a value parsing callback
 */
#define CFG_INT_LIST_CB(name, def, flags, cb) \
  __CFG_INT_LIST(name, def, flags, 0, cb)

/** Initialize a "simple" integer option (see documentation for
 * CFG_SIMPLE_STR for more information).
 */
#define CFG_SIMPLE_INT(name, svalue) \
  __CFG_INT(name, 0, CFGF_NONE, svalue, 0)



#define __CFG_FLOAT(name, def, flags, svalue, cb) \
  {name,CFGT_FLOAT,0,0,flags,0,{0,def,cfg_false,0,0},0,svalue,cb}
#define __CFG_FLOAT_LIST(name, def, flags, svalue, cb) \
  {name,CFGT_FLOAT,0,0,flags | CFGF_LIST,0,{0,0,cfg_false,0,def},0,svalue,cb}

/** Initialize a floating point option
 */
#define CFG_FLOAT(name, def, flags) \
  __CFG_FLOAT(name, def, flags, 0, 0)

/** Initialize a floating point list option
 */
#define CFG_FLOAT_LIST(name, def, flags) \
  __CFG_FLOAT_LIST(name, def, flags, 0, 0)

/** Initialize a floating point option with a value parsing callback
 */
#define CFG_FLOAT_CB(name, def, flags, cb) \
  __CFG_FLOAT(name, def, flags, 0, cb)

/** Initialize a floating point list option with a value parsing callback
 */
#define CFG_FLOAT_LIST_CB(name, def, flags, cb) \
  __CFG_FLOAT_LIST(name, def, flags, 0, cb)

/** Initialize a "simple" floating point option (see documentation for
 * CFG_SIMPLE_STR for more information).
 */
#define CFG_SIMPLE_FLOAT(name, svalue) \
  __CFG_FLOAT(name, 0, CFGF_NONE, svalue, 0)



#define __CFG_BOOL(name, def, flags, svalue, cb) \
  {name,CFGT_BOOL,0,0,flags,0,{0,0,def,0,0},0,svalue,cb}
#define __CFG_BOOL_LIST(name, def, flags, svalue, cb) \
  {name,CFGT_BOOL,0,0,flags | CFGF_LIST,0,{0,0,cfg_false,0,def},0,svalue,cb}

/** Initialize a boolean option
 */
#define CFG_BOOL(name, def, flags) \
  __CFG_BOOL(name, def, flags, 0, 0)

/** Initialize a boolean list option
 */
#define CFG_BOOL_LIST(name, def, flags) \
  __CFG_BOOL_LIST(name, def, flags, 0, 0)

/** Initialize a boolean option with a value parsing callback
 */
#define CFG_BOOL_CB(name, def, flags, cb) \
  __CFG_BOOL(name, def, flags, 0, cb)

/** Initialize a boolean list option with a value parsing callback
 */
#define CFG_BOOL_LIST_CB(name, def, flags, cb) \
  __CFG_BOOL_LIST(name, def, flags, 0, cb)

/** Initialize a "simple" boolean option (see documentation for
 * CFG_SIMPLE_STR for more information).
 */
#define CFG_SIMPLE_BOOL(name, svalue) \
  __CFG_BOOL(name, cfg_false, CFGF_NONE, svalue, 0)



/** Initialize a section
 *
 * @param name The name of the option
 * @param opts Array of options that are valid within this section

 * @param flags Flags, specify CFGF_MULTI if it should be possible to
 * have multiples of the same section, and CFGF_TITLE if the
 * section(s) must have a title (which can be used in the
 * cfg_gettsec() function)
 *
 */
#define CFG_SEC(name, opts, flags) \
  {name,CFGT_SEC,0,0,flags,opts,{0,0,cfg_false,0,0},0,0,0}



/** Initialize a function
 * @param name The name of the option
 * @param func The callback function.
 *
 * @see cfg_func_t
 */
#define CFG_FUNC(name, func) \
  {name,CFGT_FUNC,0,0,CFGF_NONE,0,{0,0,cfg_false,0,0},func,0,0}



/** Terminate list of options. This must be the last initializer in
 * the option list.
 */
#define CFG_END() \
   {0,CFGT_NONE,0,0,CFGF_NONE,0,{0,0,cfg_false,0,0},0,0,0}



/** Create and initialize a cfg_t structure. This should be the first
 * function called when setting up the parsing of a configuration
 * file. The options passed in the first parameter is typically
 * statically initialized, using the CFG_* initializers. The last
 * option in the option array must be CFG_END(), unless you like
 * segmentation faults.
 *
 * The options must also be defined in the same scope as where the
 * cfg_xxx functions are used. This means that you should either
 * define the option array in main(), statically in another function,
 * as global variables or dynamically using malloc(). The option array
 * is used in nearly all calls.
 *
 * @param opts An arrary of options
 * @param flags One or more flags (bitwise or'ed together)
 *
 * @return A configuration context structure. This pointer is passed
 * to all other functions as the first parameter.
 */
DLLIMPORT cfg_t * __export cfg_init(cfg_opt_t *opts, cfg_flag_t flags);

/** Parse a configuration file. Tilde expansion is performed on the
 * filename before it is opened. After a configuration file has been
 * initialized (with cfg_init()) and parsed (with cfg_parse()), the
 * values can be read with the cfg_getXXX functions.
 *
 * @param cfg The configuration file context as returned from cfg_init().
 * @param filename The name of the file to parse.
 *
 * @return On success, CFG_SUCCESS is returned. If the file couldn't
 * be opened for reading, CFG_FILE_ERROR is returned. On all other
 * errors, CFG_PARSE_ERROR is returned and cfg_error() was called with
 * a descriptive error message.
 */
DLLIMPORT int __export cfg_parse(cfg_t *cfg, const char *filename);

/** Same as cfg_parse() above, but takes an already opened file as
 * argument. Reading begins at the current position. After parsing,
 * the position is not reset. The caller is responsible for closing
 * the file.
 *
 * @param cfg The configuration file context as returned from cfg_init().
 * @param fp An open file stream.
 *
 * @see cfg_parse()
 */
DLLIMPORT int __export cfg_parse_fp(cfg_t *cfg, FILE *fp);

/** Same as cfg_parse() above, but takes a character buffer as
 * argument.
 *
 * @param cfg The configuration file context as returned from cfg_init().
 * @param buf A string with configuration directives.
 *
 * @see cfg_parse()
 */
DLLIMPORT int __export cfg_parse_buf(cfg_t *cfg, const char *buf);

/** Free the memory allocated for the values of a given option. Only
 * the values are freed, not the option itself (it is often statically
 * initialized).
 */
DLLIMPORT void __export cfg_free_value(cfg_opt_t *opt);

/** Free a cfg_t context. All memory allocated by the cfg_t context
 * structure are freed, and can't be used in any further cfg_* calls.
 */
DLLIMPORT void __export cfg_free(cfg_t *cfg);

/** Install a user-defined error reporting function.
 * @return The old error reporting function is returned.
 */
DLLIMPORT cfg_errfunc_t __export cfg_set_error_function(cfg_t *cfg,
														cfg_errfunc_t errfunc);

/** Show a parser error. Any user-defined error reporting function is called.
 * @see cfg_set_error_function
 */
DLLIMPORT void __export cfg_error(cfg_t *cfg, const char *fmt, ...);

/** Returns the value of an integer option. This is the same as
 * calling cfg_getnint with index 0.
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @return The requested value is returned. If the option was not set
 * in the configuration file, the default value given in the
 * corresponding cfg_opt_t structure is returned. If no option is found
 * with that name, 0 is returned.
 */
DLLIMPORT long int __export cfg_getint(cfg_t *cfg, const char *name);

/** Returns the value of a floating point option.
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @return The requested value is returned. If the option was not set
 * in the configuration file, the default value given in the
 * corresponding cfg_opt_t structure is returned. If no option is found
 * with that name, cfg_error is called and 0 is returned.
 */
DLLIMPORT double __export cfg_getfloat(cfg_t *cfg, const char *name);

/** Returns the value of a string option.
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @return The requested value is returned. If the option was not set
 * in the configuration file, the default value given in the
 * corresponding cfg_opt_t structure is returned. If no option is found
 * with that name, cfg_error is called and NULL is returned.
 */
DLLIMPORT char * __export cfg_getstr(cfg_t *cfg, const char *name);

/** Returns the value of a boolean option.
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @return The requested value is returned. If the option was not set
 * in the configuration file, the default value given in the
 * corresponding cfg_opt_t structure is returned. If no option is found
 * with that name, cfg_error is called and cfg_false is returned.
 */
DLLIMPORT cfg_bool_t __export cfg_getbool(cfg_t *cfg, const char *name);

/** Returns the value of a section option. The returned value is
 * another cfg_t structure that can be used in following calls to
 * cfg_getint, cfg_getstr or other get-functions.
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @return The requested value is returned.  If no section is found
 * with that name, 0 is returned. Note that there can be no default
 * values for a section.
 */
DLLIMPORT cfg_t * __export cfg_getsec(cfg_t *cfg, const char *name);

/** Return the number of values this option has. If no default value
 * is given for the option and no value was found in the config file,
 * 0 will be returned (ie, the option value is not set at all). It that
 * case, calling cfg_getXXX will abort.
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 */
DLLIMPORT unsigned int __export cfg_size(cfg_t *cfg, const char *name);

/** Indexed version of cfg_getint().
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param index Index of values. Zero based.
 * @see cfg_getint
 */
DLLIMPORT long int __export cfg_getnint(cfg_t *cfg, const char *name,
										unsigned int index);

/** Indexed version of cfg_getfloat().
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param index Index of values. Zero based.
 * @see cfg_getint
 */
DLLIMPORT double __export cfg_getnfloat(cfg_t *cfg, const char *name,
										unsigned int index);

/** Indexed version of cfg_getstr().
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param index Index of values. Zero based.
 * @see cfg_getstr
 */
DLLIMPORT char * __export cfg_getnstr(cfg_t *cfg, const char *name,
									  unsigned int index);

/** Indexed version of cfg_getbool().
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param index Index of values. Zero based.
 * @see cfg_getstr
 */
DLLIMPORT cfg_bool_t __export cfg_getnbool(cfg_t *cfg, const char *name,
										   unsigned int index);

/** Indexed version of cfg_getsec().
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param index Index of values. Zero based.
 * @see cfg_getsec
 */
DLLIMPORT cfg_t * __export cfg_getnsec(cfg_t *cfg, const char *name,
									   unsigned int index);

/** Return a section given the title.
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param title The title of this section. The CFGF_TITLE flag must
 * have been set for this option.
 */
DLLIMPORT cfg_t * __export cfg_gettsec(cfg_t *cfg, const char *name,
									   const char *title);

/** Return the title of a section.
 *
 * @param cfg The configuration file context.
 * @return Returns the title, or 0 if there is no title. This string
 * should not be modified.
 */
DLLIMPORT const char * __export cfg_title(cfg_t *cfg);

/** Predefined include-function. This function can be used in the
 * options passed to cfg_init() to specify a function for including
 * other configuration files in the parsing. For example:
 * CFG_FUNC("include", &cfg_include)
 */
DLLIMPORT int __export cfg_include(cfg_t *cfg, cfg_opt_t *opt, int argc,
								   const char **argv);

/** Does tilde expansion (~ -> $HOME) on the filename.
 * @return The expanded filename is returned. If a ~user was not
 * found, the original filename is returned. In any case, a
 * dynamically allocated string is returned, which should be free()'d
 * by the caller.
 */
DLLIMPORT char * __export cfg_tilde_expand(const char *filename);

/** Parse a boolean option string. Accepted "true" values are "true",
 * "on" and "yes", and accepted "false" values are "false", "off" and
 * "no".
 *
 * @return Returns 1 or 0 (true/false) if the string was parsed
 * correctly, or -1 if an error occurred.
 */
DLLIMPORT int __export cfg_parse_boolean(const char *s);

/** Return an option given it's name.
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 *
 * @return Returns a pointer to the option, or NULL if the option is
 * not found (an error message is also printed).
 */
DLLIMPORT cfg_opt_t * __export cfg_getopt(cfg_t *cfg, const char *name);

/** Set a value of an integer option.
 *
 * @param cfg The configuration file context.
 * @param opt The option structure (eg, as returned from cfg_getopt())
 * @param value The value to set.
 * @param index The index in the option value array that should be
 * modified. It is an error to set values with indices larger than 0
 * for options without the CFGF_LIST flag set.
 */
DLLIMPORT void __export cfg_opt_setnint(cfg_t *cfg, cfg_opt_t *opt,
										long int value, unsigned int index);

/** Set the value of an integer option given its name.
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param value The value to set.
 */
DLLIMPORT void __export cfg_setint(cfg_t *cfg, const char *name,
								   long int value);

/** Set a value of an integer option given its name.
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param value The value to set.
 * @param index The index in the option value array that should be
 * modified. It is an error to set values with indices larger than 0
 * for options without the CFGF_LIST flag set.
 */
DLLIMPORT void __export cfg_setnint(cfg_t *cfg, const char *name,
									long int value, unsigned int index);

/** Set a value of a floating point option.
 *
 * @param cfg The configuration file context.
 * @param opt The option structure (eg, as returned from cfg_getopt())
 * @param value The value to set.
 * @param index The index in the option value array that should be
 * modified. It is an error to set values with indices larger than 0
 * for options without the CFGF_LIST flag set.
 */
DLLIMPORT void __export cfg_opt_setnfloat(cfg_t *cfg, cfg_opt_t *opt,
										  double value, unsigned int index);

/** Set the value of a floating point option given its name.
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param value The value to set.
 */
DLLIMPORT void __export cfg_setfloat(cfg_t *cfg, const char *name,
									 double value);

/** Set a value of a floating point option given its name.
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param value The value to set.
 * @param index The index in the option value array that should be
 * modified. It is an error to set values with indices larger than 0
 * for options without the CFGF_LIST flag set.
 */
DLLIMPORT void __export cfg_setnfloat(cfg_t *cfg, const char *name,
									  double value, unsigned int index);

/** Set a value of a boolean option.
 *
 * @param cfg The configuration file context.
 * @param opt The option structure (eg, as returned from cfg_getopt())
 * @param value The value to set.
 * @param index The index in the option value array that should be
 * modified. It is an error to set values with indices larger than 0
 * for options without the CFGF_LIST flag set.
 */
DLLIMPORT void __export cfg_opt_setnbool(cfg_t *cfg, cfg_opt_t *opt,
										 cfg_bool_t value, unsigned int index);

/** Set the value of a boolean option given its name.
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param value The value to set.
 */
DLLIMPORT void __export cfg_setbool(cfg_t *cfg, const char *name,
									cfg_bool_t value);

/** Set a value of a boolean option given its name.
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param value The value to set.
 * @param index The index in the option value array that should be
 * modified. It is an error to set values with indices larger than 0
 * for options without the CFGF_LIST flag set.
 */
DLLIMPORT void __export cfg_setnbool(cfg_t *cfg, const char *name,
									 cfg_bool_t value, unsigned int index);

/** Set a value of a string option.
 *
 * @param cfg The configuration file context.
 * @param opt The option structure (eg, as returned from cfg_getopt())
 * @param value The value to set. Memory for the string is allocated
 * and the value is copied. Any privious string value is freed.
 * @param index The index in the option value array that should be
 * modified. It is an error to set values with indices larger than 0
 * for options without the CFGF_LIST flag set.
 */
DLLIMPORT void __export cfg_opt_setnstr(cfg_t *cfg, cfg_opt_t *opt,
										const char *value, unsigned int index);

/** Set the value of a string option given its name.
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param value The value to set. Memory for the string is allocated
 * and the value is copied. Any privious string value is freed.
 */
DLLIMPORT void __export cfg_setstr(cfg_t *cfg, const char *name,
								   const char *value);

/** Set a value of a boolean option given its name.
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param value The value to set. Memory for the string is allocated
 * and the value is copied. Any privious string value is freed.
 * @param index The index in the option value array that should be
 * modified. It is an error to set values with indices larger than 0
 * for options without the CFGF_LIST flag set.
 */
DLLIMPORT void __export cfg_setnstr(cfg_t *cfg, const char *name,
									const char *value, unsigned int index);

DLLIMPORT void __export cfg_setlist(cfg_t *cfg, const char *name,
									unsigned int nvalues, ...);


DLLIMPORT void __export cfg_addlist(cfg_t *cfg, const char *name,
									unsigned int nvalues, ...);

#ifdef __cplusplus
}
#endif

#endif

/** @example cfgtest.c
 */

/** @example simple.c
 */

/** @example reread.c
 */