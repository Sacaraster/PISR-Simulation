/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef GS_OUTPUT_H
# define GS_OUTPUT_H

/** \file gs_output.h 
 * Output to file, function, string or stdout */

# include <gsim/gs.h>
# include <stdio.h>

class GsString;

/*! \class GsOutput gs_output.h
    \brief Generic output class

    Outputs to a file, function, string or stdout. GsOutput purpose
    is similar to C++ ostream, but ensures cross-platform compatibility
    by being based only on C streams. Additionally it has a simple
    mechanism to be easily redirected to a string, file or function. */
class GsOutput
 { public :
    /*! The type of the output, that can be connected to a file, function, string or stdout. */
    enum Type { TypeStdout,  //!< Output is directed to the console
                TypeFile,    //!< Output is directed to a file
                TypeString,  //!< Output is directed to a GsString
                TypeFunction //!< Output is directed to a function
              };

   private :
    Type _type;
    int  _margin;
    char _margin_char;
    char _auto_console;
    union { GsString *string;
            void (*func) (const char *,void*);
            FILE *file;
          } _device;

    void* _func_udata;  // used only when the output is a function
    char* _intfmt;      // format used to print integers
    char* _floatfmt;    // format used to print floats
    char* _doublefmt;   // format used to print doubles
    char* _filename;    // optional file name of the open file
    void _construct();

   public : 
    /*! Default constructor: set the output to stdout. */
    GsOutput () { _construct(); }

    /*! Constructor that sets the output to an open file. */
    GsOutput ( FILE *f );

    /*! Constructor that inits the output to a function. The function receives
        the string to output and a void pointer to use as user data. */
    GsOutput ( void(*f)(const char*,void*), void* udata );

    /*! The destructor will close the associated output device with close(). */
   ~GsOutput ();

    /*! If the output type is File, return its FILE pointer, otherwise return 0 */
    FILE* filept () { return _type==TypeFile? _device.file:0; }

    /*! If output is done to a file, do not close the file and set GsOutput
        to no longer be associated with the file and become of type stdout. */
    void leave_file ();

    /*! Closes current ouput and makes it be of type stdout. */
    void init ();

    /*! Closes current ouput and set it to an open file. */
    void init ( FILE *f );

    /*! Closes current ouput and opens a file with fopen(filename,"wt").
        Returns true if file could be open or false otherwise.
        The filename is stored and can be retrieved with filename(). */
    bool open ( const char* filename );

    /*! Closes current output and set it to a GsString. 
        String s must exist during all the time GsOutput uses it. */
    void init ( GsString &s );

    /*! Closes current ouput and redirects it to a function. The function
        receives the string to output and a user data void pointer. */
    void init ( void(*f)(const char*,void*), void* udata );

    /*! Returns the type of the output. */
    Type type () const { return _type; }

    /*! Set on or off the behaviour of automatically opening a console window
        when something is sent to stdout for the 1st time. Default is on.
        Only applicable when in TypeStdout and in Windows platform */
    void auto_console ( bool b ) { _auto_console=char(b); }
    
    /*! Returns the on/off state of the automatic console behaviour */
    bool auto_console () const { return _auto_console? true:false; }

    /*! Returns the file name used for opening a file output, or null if not available */
    const char* filename () const { return _filename; }
    
    /*! Associates with the output a file name. The string is stored but not used by GsOutput. */
    void filename ( const char* s ) { gs_string_set(_filename,s); }

    /*! Returns the current margin being used. The number returned represents 
        the number of tabs or spaces to be used as current margin */
    int margin () const { return _margin; }

    /*! Changes the current margin being used. */
    void margin ( int m ) { _margin=m; }

    /*! Set margin character, default is '\\t' */
    void margin_char ( char c ) { _margin_char=c; }

    /*! returns the current margin character. */
    char margin_char () const { return _margin_char; }

    /*! Changes the current margin being used by adding parameter inc
        to the current value. Normally, inc is given as 1 or -1. */
    void incmargin ( int inc ) { _margin+=inc; }

    /*! Checks whether the associated output device pointer is valid.
        A console type output will always be valid, returning true.
        Other types are valid if their access pointers are not zero. */
    bool valid ();

    /*! Will close the associated output device if this is the case.
        If GsOutput is associated with a file, and if the file pointer is not zero, then
        fclose() will be called to close this file and GsOutput will become not valid().
        For other devices, nothing is done. The filename is set to null. */
    void close ();
 
    /*! Set the desired "printf format" for integers, "%d" is the default */
    void fmtint ( const char* s );

    /*! Set the desired "printf format" for floats, "%g" is the default.
        There are two special formats GsOutput understands: 
        "f" prints with %f but zeros are printed without decimals;
        and "g" prints with %g but zeros are printed as "0.0" */
    void fmtfloat ( const char* s );

    /*! Set the desired "printf format" for doubles, "%g" is the default.
        The same special formats described in method fmtfloat are vlso valid here. */
    void fmtdouble ( const char* s );

    const char* fmtint () { return _intfmt; }       //!< Returns the the current integer "printf format"
    const char* fmtfloat () { return _floatfmt; }   //!< Returns the the current float "printf format"
    const char* fmtdouble () { return _doublefmt; } //!< Returns the the current double "printf format"

    /*! Outputs the margin as defined by methods margin() and margin_char(). */
    void outm ();

    /*! Will fflush() when output is a file or stdout. */
    void flush ();

    /*! Sets the GsString int, float, double formats to their default values,
        which are: "%d", "g", and "g". Format "g" is interpreted as a "%g", but with
        an added ".0" in case the real number is equal to its integer part.
        Format "i" will output real numbers as integer when equal to their integer part */
    void fmtdefaults ();

    /*! Outputs a character to the connected device. No validity checkings are done. */
    void put ( char c );

    /*! Outputs a string to the connected device. No validity checkings are done. */
    void put ( const char *st );

    /*! Outputs a formated string to the connected device. The format string is the same
        as for the printf() function. An internal 256 bytes buffer is used to translate
        the format string. And then put() method is called. */
    void putf ( const char *fmt, ... );

    /*! Calls stdlib exit(code). If there is a shown attached console, it will first wait
        for a key press (in Windows only). */
    void exit ( int code=0 );

    /*! Outputs a formated error message and calls exit(1) (buffer of 256 bytes used). */
    void fatal ( const char *fmt, ... );

    /*! Outputs a formatted warning message (buffer of 256 bytes used, no call to exit()). */
    void warning ( const char *fmt, ... );

    /*! Outputs the string making sure it can be read again as a string,
        by enclosing the string with double quotes if needed. */
    void safewrite ( const char* s );

    /*! Outputs a string using the put() method. */
    friend GsOutput& operator<< ( GsOutput& o, const char *s ) { o.put(s); return o; }
 
    /*! Outputs a GsString using the put() method. */
    friend GsOutput& operator<< ( GsOutput& o, const GsString& s );

    /*! Outputs a character using the put() method. */
    friend GsOutput& operator<< ( GsOutput& o, char c );

    /*! Outputs a gsbyte using intfmt with the putf() method. */
    friend GsOutput& operator<< ( GsOutput& o, gsbyte c );

    /*! Outputs an int using intfmt with the putf() method. */
    friend GsOutput& operator<< ( GsOutput& o, int i );

    /*! Outputs an long using intfmt with the putf() method. */
    friend GsOutput& operator<< ( GsOutput& o, long i );

    /*! Outputs a bool as "true" or "false" with the put() method. */
    friend GsOutput& operator<< ( GsOutput& o, bool b );

    /*! Outputs a short int using intfmt with the putf() method. */
    friend GsOutput& operator<< ( GsOutput& o, short s );

    /*! Outputs a gsuint using intfmt with the putf() method. */
    friend GsOutput& operator<< ( GsOutput& o, gsuint i );

    /*! Outputs a gsuint using intfmt with the putf() method. */
    friend GsOutput& operator<< ( GsOutput& o, gsulong i );

    /*! Outputs a float using floatfmt with the putf() method. */
    friend GsOutput& operator<< ( GsOutput& o, float f );

    /*! Outputs a double using doublefmt with the putf() method. */
    friend GsOutput& operator<< ( GsOutput& o, double d );

    /*! If an output o is sent to another output nothing happens. */
    GsOutput& operator<< ( GsOutput& o ) { return o; }
 };

/*! The global output gsout is used in the gsim library to output error messages and
    by the trace functions. Its type is stdout but can be redirected if needed. */
extern GsOutput gsout;

/*! This class is available as a trick to call safewrite() with operators,
    for example as in here: out << "name " << GsSafeWrite(_name) << gsnl; */
struct GsSafeWrite
 { const char* s;
   GsSafeWrite(const char* st) { s=st; }
   friend GsOutput& operator<< ( GsOutput& o, const GsSafeWrite& sw ) { o.safewrite(sw.s); return o; }
 };

//============================== end of file ===============================

# endif  // GS_OUTPUT_H

