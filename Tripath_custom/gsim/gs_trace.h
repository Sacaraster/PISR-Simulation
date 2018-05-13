/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

/** \file gs_trace.h 
 * Macros to easily trace and debug programs
 *
 * To trace/debug your source code, you can define GS_USE_TRACEx (x in {1,..,9}),
 * and then include gs_trace.h. In this way, macros GS_TRACEx will be defined and
 * will allow you to print messages to gsout. To stop using the trace, you just
 * need to comment the defined GS_USE_TRACEx.
 *
 * If GS_USE_TRACE (without any number) is defined, it will have the same
 * effect as defining all GS_USE_TRACEx, x in {1,..,9}.
 *
 * To change the used gsout output stream, it is possible to define 
 * GS_TRACE_OSTREAM to any other output stream. 
 *
 * GS_TRACE_MSG can be defined by the user for customizing the trace messages.
 * If not defined, the default message format is displayed and the option
 * to display only line numbers is activated by defining GS_TRACE_ONLY_LINES.
 **/

# ifndef GS_TRACE_H
# define GS_TRACE_H

# if defined(GS_USE_TRACE) || defined(GS_USE_TRACE1) || defined(GS_USE_TRACE2) || defined(GS_USE_TRACE3) || defined(GS_USE_TRACE4) || defined(GS_USE_TRACE5) || defined(GS_USE_TRACE6) || defined(GS_USE_TRACE7) || defined(GS_USE_TRACE8) || defined(GS_USE_TRACE9)
#  ifndef GS_TRACE_OSTREAM
#   include <gsim/gs_output.h>
#   define GS_TRACE_OSTREAM gsout
#  endif 
# endif

# ifndef GS_TRACE_MSG
#  if defined(GS_TRACE_ONLY_LINES)
#   define GS_TRACE_MSG "line " << __LINE__ << ": "
#  else
#   define GS_TRACE_MSG gs_filename(__FILE__) << ' ' << __LINE__ << " : "
#  endif
# endif

# if defined(GS_USE_TRACE1) || defined(GS_USE_TRACE)
# define GS_TRACE1(msg) GS_TRACE_OSTREAM << GS_TRACE_MSG << msg << '\n'
# else
# define GS_TRACE1(msg) 
# endif

# if defined(GS_USE_TRACE2) || defined(GS_USE_TRACE)
# define GS_TRACE2(msg) GS_TRACE_OSTREAM << GS_TRACE_MSG << msg << '\n'
# else
# define GS_TRACE2(msg) 
# endif

# if defined(GS_USE_TRACE3) || defined(GS_USE_TRACE)
# define GS_TRACE3(msg) GS_TRACE_OSTREAM << GS_TRACE_MSG << msg << '\n'
# else
# define GS_TRACE3(msg) 
# endif

# if defined(GS_USE_TRACE4) || defined(GS_USE_TRACE)
# define GS_TRACE4(msg) GS_TRACE_OSTREAM << GS_TRACE_MSG << msg << '\n'
# else
# define GS_TRACE4(msg) 
# endif

# if defined(GS_USE_TRACE5) || defined(GS_USE_TRACE)
# define GS_TRACE5(msg) GS_TRACE_OSTREAM << GS_TRACE_MSG << msg << '\n'
# else
# define GS_TRACE5(msg) 
# endif

# if defined(GS_USE_TRACE6) || defined(GS_USE_TRACE)
# define GS_TRACE6(msg) GS_TRACE_OSTREAM << GS_TRACE_MSG << msg << '\n'
# else
# define GS_TRACE6(msg) 
# endif

# if defined(GS_USE_TRACE7) || defined(GS_USE_TRACE)
# define GS_TRACE7(msg) GS_TRACE_OSTREAM << GS_TRACE_MSG << msg << '\n'
# else
# define GS_TRACE7(msg) 
# endif

# if defined(GS_USE_TRACE8) || defined(GS_USE_TRACE)
# define GS_TRACE8(msg) GS_TRACE_OSTREAM << GS_TRACE_MSG << msg << '\n'
# else
# define GS_TRACE8(msg) 
# endif

# if defined(GS_USE_TRACE9) || defined(GS_USE_TRACE)
# define GS_TRACE9(msg) GS_TRACE_OSTREAM << GS_TRACE_MSG << msg << '\n'
# else
# define GS_TRACE9(msg) 
# endif

#endif // GS_TRACE_H

