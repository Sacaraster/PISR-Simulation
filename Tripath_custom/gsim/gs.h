/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef GS_H
# define GS_H

/** \file gs.h 
 * Main header file of the Scene Graph toolkit
 *
 \code
 *************************************************************************
 *
 *                  GS - Simulation and Graphics toolkit
 *                       Marcelo Kallmann 2005-2010
 *
 *   This toolkit was develloped to support research in modelling,
 *   computational geometry, animation and robotics.
 *
 *   Design goals are to be simple, efficient, and cross-platform:
 *   - Consistent nomenclature
 *   - Angle parameters are in radians, but degrees are used in data files
 *   - float types are mainly used for storage, double for some computations
 *   - Most geometric classes are 3D; otherwise a letter is used to indicate
 *      the dimension, e.g.: GsVec2
 *
 *************************************************************************
 *
 * Nomenclature conventions/examples :
 *
 * global functions          : gs_string_new()
 * global defines            : GS_BIT0, GS_USE_TRACE1
 * global consts             : gse, gspi
 * global typedefs           : gsbyte, gsuint
 * global variables          : gsout
 * global enums              : gsString
 * global classes            : GsArray, GsString, GsVec, GsVec2          
 * class public members      : a.size(), a.size(3), v.normalize()
 * class private members     : _capacity, _size
 * class enums               : class GsClass { enum Mgs { MgsOk, MgsError } };
 * class friends (no prefix) : dist ( const GsVec& x, const GsVec& y )
 *
 *************************************************************************
 \endcode */

// ==================================== Types =================================

/*! \brief gsim main namespace.
    By now, namespace gsim is only used in very few places
    where there is a need to solve namespace conflicts. */
namespace gsim { }

# if defined(_WIN32) || defined(WINDOWS) || defined(WIN32)
# define GS_WINDOWS  //!< Defined if compiled for windows
# ifndef __CYGWIN32__
# pragma warning(disable : 4996) // for vc8 secure functions warning
# endif
# else
# define GS_LINUX    //!< Defined if not compiled in windows
# endif

# ifndef GS_NO_OPENGL
# define GS_OPENGL
# endif

# ifdef GS_DEF_BOOL
enum bool { false, true }; //!< use this for old compilers without bool/true/false keywords
# endif

// The following types should be adjusted according to the used system
typedef void*          gsvoidpt; //!< a pointer to a void type
typedef char*          gscharpt; //!< a pointer to a char
typedef signed char    gschar;   //!< 1 byte signed int, from -127 to 128
typedef unsigned char  gsbyte;   //!< 1 byte unsigned int, from 0 to 255
typedef unsigned char  gscbool;  //!< 1 byte char type intended to store boolean values
typedef unsigned short gsword;   //!< 2 bytes unsigned int, from 0 to 65,535
typedef short int      gssint;   //!< 2 bytes integer, from -32,768 to 32,767
typedef unsigned int   gsuint;   //!< 4 bytes unsigned int, from 0 to 4294967295
typedef signed int     gsint;    //!< 4 bytes signed integer, from -2147483648 to 2147483647
typedef signed long    gslong;   //!< 8 bytes in 64-bit systems, or same as int in 32-bit systems
typedef unsigned long  gsulong;  //!< 8 bytes in 64-bit systems, or same as uint in 32-bit systems

/*! Defines a typedef for a generic comparison function in the form: 
    int gscompare ( const void*, const void* ), that is used by data structure classes. */
typedef int (*gscompare) ( const void*, const void* );

/*! Defines a generic comparison function for the type X, to be used as argument
    for template based classes. Same as: int (*gs_compare_func) (const X*,const X*) */
# define GS_COMPARE_FUNC int (*cmpfunc) (const X*,const X*)

// ================================ Some Constants ==========================

const char gsnl  = '\n'; //!< Contains the newline '\\n' char
const char gspc  = ' ';  //!< Contains the space char
const char gstab = '\t'; //!< Contains the tab '\\t' char

const float gstiny = 1.0E-6f;     //!< 1E-6
const float gsmall = 1.0E-4f;     //!< 1E-4

const gsuint gsuintmax = ((gsuint)0)-1; //!< the unsigned int maximum value

/* floats and doubles have precision of 7 and 15 decimal digits */
# define GS_E      2.7182818284590452 //!< 2.71828...
# define GS_PI     3.1415926535897932 //!< 3.141592...
# define GS_PIDIV2 1.5707963267948966 //!< 1.570796...
# define GS_2PI    6.2831853071795864 //!< 2*pi
# define GS_SQRT2  1.4142135623730950 //!< sqrt(2) = 1.4142...
# define GS_SQRT3  1.7320508075688772 //!< sqrt(3) = 1.7320...
# define GS_SQRT6  2.4494897427831780 //!< sqrt(6) = 2.4494...

const float gse      = float(GS_E);     //!< 2.7182818
const float gspi     = float(GS_PI);    //!< 3.1415926
const float gspidiv2 = float(GS_PIDIV2); //!< 1.57079632
const float gs2pi    = float(GS_2PI);   //!< 2*pi
const float gsqrt2   = float(GS_SQRT2); //!< sqrt(2) = 1.4142135
const float gsqrt3   = float(GS_SQRT3); //!< sqrt(3) = 1.7320508
const float gsqrt6   = float(GS_SQRT6); //!< sqrt(6) = 2.4494897

// ============================== Math Utilities ===========================

/*! linear interpolation between a and b */
float gs_lerp ( float a, float b, float t );

/*! interpolation between a and b, with t mapped to a cubic ease in/out */
float gs_cuberp ( float a, float b, float t );

/*! Returns the convertion from radians to degrees (float version). */
float gs_todeg ( float radians );

/*! Returns the convertion from radians to degrees (double version). */
double gs_todeg ( double radians );

/*! Returns the convertion from degrees to radians (float version). */
float gs_torad ( float degrees );

/*! Returns the convertion from degrees to radians (double version). */
double gs_torad ( double degrees );

/*! Returns the equivalent angle in radians but in the [-pi,pi] range (float version)*/
float gs_angnorm ( float radians );

/*! Returns the (shortest) interpolation between the two given normalized angles */
float gs_anglerp ( float radians1, float radians2, float t );

/*! Returns the distance in radians (by the shortest side) between given normalized angles */
float gs_angdist ( float radians1, float radians2 );

/*! Returns the integer part of x by using a sequence of type casts (float version) */
float gs_trunc ( float x );

/*! Returns the integer part of x by using a sequence of type casts ((float version) */
double gs_trunc ( double x );

/*! Returns the closest integer of x (float version). */
float gs_round ( float x );

/*! Returns float x rounded at the decimal digit given by prec, for
    example gs_round(1.246f,0.01f) will return 1.25f */
float gs_round ( float x, float prec );

/*! Returns the closest integer of x (double version). */
double gs_round ( double x );

/*! Returns double x rounded at the decimal digit given by prec, for
    example gs_round(1.246,0.01) will return 1.25 */
double gs_round ( double x, double prec );

/*! Returns the lowest rounded value of x (float version). */
float gs_floor ( float x );

/*! Returns the lowest rounded value of x (double version). */
double gs_floor ( double x );

/*! Returns the highest rounded value of x (float version). */
float gs_ceil ( float x );

/*! Returns the highest rounded value of x (double version). */
double gs_ceil ( double x );

/*! Returns the square root for integer values, with no use of floating point. */
int gs_sqrt ( int x );

/*! gs_fact returns the factorial of x. */
int gs_fact ( int x );

/*! returns integer "b raised to e", e must be >=0 */
int gs_pow ( int b, int e );

/*! returns float "b raised to e", e must be >=0 */
float gs_pow ( float b, int e );

/*! returns double "b raised to e", e must be >=0 */
double gs_pow ( double b, int e );

/*! returns |a-b|, that is the distance of two points in the line. */
float gs_dist ( float a, float b );

/*! returns |a|, ie, the absolute value of a */
float gs_abs ( float a );

// ============================= Compare Functions ============================

/*! Case insensitive comparison of strings in the C style.
    This function follows the C style of compare functions where 0 is returned if
    s1==s2, <0 if s1<s2, and >0 otherwise. Comparisons are case-insensitive.
    s1 and s2 must be non-null pointers, otherwise unpredictable results will arise. 
    If two strings have the first n characters equal, where one has lenght n, and
    the other has length >n, the smaller one is considered to come first. */
int gs_compare ( const char *s1, const char *s2 );

/*! Case sensitive comparison of strings in the C style */
int gs_comparecs ( const char *s1, const char *s2 );

/*! Case insensitive compare strings, but compares a maximum of n characters. */
int gs_compare ( const char *s1, const char *s2, int n );

/*! Case sensitive compare strings, but compares a maximum of n characters. */
int gs_comparecs ( const char *s1, const char *s2, int n );

/*! Compares two integers, returning 0 if they're equal, <0 if i1<i2, and >0 otherwise. */
int gs_compare ( const int *i1, const int *i2 );

/*! Compares two floats, returning 0 if they're equal, <0 if f1<f2, and >0 otherwise. */
int gs_compare ( const float *f1, const float *f2 );

/*! Compares two doubles, returning 0 if they're equal, <0 if d1<d2, and >0 otherwise. */
int gs_compare ( const double *d1, const double *d2 );

// ============================== C String Utilities ============================

/*! Allocates a string with sufficient size to copy 'tocopy' in it.
    The allocation is simply done with operator new, and the allocated
    memory pointer is returned. If tocopy==0, the value 0 is returned. */
char* gs_string_new ( const char* tocopy );

/*! Calls delete[] for s and set s to be 0 */
void gs_string_delete ( char*& s );

/*! Deletes s, and reallocates s with sufficient size to copy 'tocopy'
    in it. If tocopy==0, s will become a null pointer, otherwise the
    allocation is simply done with operator new, and s is changed to
    point to the new memory allocated. */
void gs_string_set ( char*& s, const char *tocopy );

/*! Deletes s, and reallocates it with the given size also copying its
    contents to the new allocated position. If size<=0, s is simply
    deleted. If the new size is smaller then the original s length,
    the contents will be truncated. In all cases, s will be a valid
    string, having the ending 0 char. This function is similar to the
    C standard realloc function, but using C++ operators new/delete. */
void gs_string_renew ( char*& s, int size );

/*! Reallocates s to the needed size and appends toadd.
    If s is null this is the same as gs_string_new.
    If toadd is null or empty, nothing is done. */
void gs_string_append ( char*& s, const char* toadd );

// ============================== Standard IO ============================

/*! Redirects the C streams stdout and stderr to the files with given
    file names. In case the parameters are null (the default), the
    output is directed to the text files stdout.txt and stderr.txt
    in the current folder */
void gs_output_to_disk ( const char* outfile=0, const char* errfile=0 );

/*! Opens a console and attach it to the std streams. Only applicable
    in Windows, no effect in other platforms. */
void gs_show_console ();

/*! Returns true if gs_show_console() was previously called at some
    point and false otherwise */
bool gs_console_shown ();

/*! Returns true if can open file fname for reading and false otherwise. */
bool gs_canopen ( const char* fname );

/*! Returns true if the file path has a full (not relative) path
    description. Otherwise returns false. */
bool gs_absolute ( const char* path );

/*! Returns a pointer to the first character of a filename in fname,
    without considering eventual directory paths. Null is returned if
    no filename is found */
const char* gs_filename ( const char* fname );

/*! Returns a pointer to the first character of a file extension in fname.
    Null is returned if no extension is found */
const char* gs_extension (  const char* fname );

/*! Returns true if given file name exists and false otherwise */
bool gs_exist ( const char* fname );

/*! Returns true if given file name is of a directory and false otherwise */
bool gs_isdir ( const char* fname );

/*! Returns the size in bytes of given file name */
gsuint gs_size ( const char* fname );

/*! Returns the size in bytes of given file name, long long version */
unsigned long long gs_sizel ( const char* fname );

/*! Returns the modification time of the file as a Unix timestamp
    (number of seconds since the start of 1970 in GMT). Returns 0
    if the file does not exist.*/
gsuint gs_mtime ( const char* fname );

/*! Exit the aplication. */
void gs_exit ( int code=0 );

/*! Sleep the processor for a given number of miliseconds */
void gs_sleep ( int milisecs );

// ================================= Timer ==================================

/*! Returns the time in seconds since midnight (00:00:00) of
    January 1, 1970, coordinated universal time (UTC). 
    This function uses the high performance counter in windows */
double gs_time ();

// ============================== Random Numbers ==================================

/*! Set seed of the random generator */
void gs_rseed ( gsuint s );

/*! Returns a float random number in the closed interval [0,1] */
float gs_random ();

/*! Returns a float number in the closed interval [min,max] */
float gs_random ( float min, float max );

/*! Returns a 53-bit precision double number in the open interval [min,max) */
double gs_random ( double min, double max );

/*! Returns a random integer in the set {min, min+1, ..., max-1, max} */
int gs_random ( int min, int max );

// ================================= Macros ==================================

/*! Macro that returns the lower case character of c. */
# define GS_LOWER(c) ( (c)>='A'&&(c)<='Z'? (c)-'A'+'a':(c) )

/*! Macro that puts c in upper case if c is a valid lower case letter. */
# define GS_UPPER(c) ( (c)>='a'&&(c)<='z'? (c)-'a'+'A':(c) )

/*! Macro that returns if c is upper case. */
# define GS_ISUPPER(c) ( (c)>='A'&&(c)<='Z'? true:false )

/*! Macro that returns if c is lower case. */
# define GS_ISLOWER(c) ( (c)>='a'&&(c)<='z'? true:false )

/*! Macro that returns 0 if a is equal to b, 1 if a>b, and -1 otherwise. */
# define GS_COMPARE(a,b) (a==b)? 0: (a>b)? 1: -1

/*! Macro that swaps the value of a boolean type. */
# define GS_SWAPB(b) b = !b // B from bool

/*! Macro that swaps the values of a and b using three xor logical operations. */
# define GS_SWAPX(a,b) { a^=b; b^=a; a^=b; } // x from xor

/*! Macro that swaps the values of a and b, using tmp as temporary variable. */
# define GS_SWAPT(a,b,tmp) { tmp=a; a=b; b=tmp; }

/*! Macro that swaps the values of a and b, given that a temporary 
    variable named tmp, of the same type as a and b exists. */
# define GS_SWAP(a,b) { tmp=a; a=b; b=tmp; }

/*! Macro that makes m to be x, if x is greater than m. */
# define GS_UPDMAX(m,x) if((x)>(m)) m=x

/*! Macro that makes m to be x, if x is smaller than m. */
# define GS_UPDMIN(m,x) if((x)<(m)) m=x

/*! Macro that tests if x is inside the interval [i,s]. */
# define GS_BOUNDED(x,i,s) ((i)<=(x) && (x)<=(s))

/*! Macro that tests if x is inside the interval [i,s] (same as GS_BOUNDED). */
# define GS_INRANGE(x,i,s) ((i)<=(x) && (x)<=(s))

/*! Macro that returns x clipped by the interval [i,s]. */
# define GS_BOUND(x,i,s) (x)<(i)? (i): (x)>(s)? (s): (x)

/*! Macro that clips x to be inside the interval [i,s]. */
# define GS_CLIP(x,i,s) if(x<i)x=i; else if(x>s)x=s

/*! Macro that forces a to be positive by negating it if it is negative. */
# define GS_POS(a) if((a)<0) a=-(a)

/*! Macro that forces a to be negative by negating it if it is positive. */
# define GS_NEG(a) if((a)>0) a=-(a)

/*! Macro that returns -2t^3 +3t^2 */
# define GS_CUBIC(t) (-(2.0f*((t)*(t)*(t))) + (3.0f*((t)*(t))))

/*! Macro that returns x, so that x = a(1-t) + bt. */
# define GS_LERP(a,b,t) ((a)*(1-(t))+(b)*(t)) // return x = a(1-t) + bt

/*! Macro that returns t, so that x = a(1-t) + bt. */
# define GS_PARAM(a,b,x) ((x)-(a))/((b)-(a)) // return t : x = a(1-t) + bt

/*! Macro that truncates x, with an int typecast. */
# define GS_TRUNC(x) ( (int) (x) )

/*! Macro that returns x rounded to the nearest integer. */
# define GS_ROUND(x) ( (int) ((x>0)? (x+0.5f):(x-0.5f)) )

/*! Macro that rounds x to the nearest integer, but
    considering that x is positive. This macro adds 0.5 
    and does an int typecast. */
# define GS_ROUNDPOS(x) ( (int) (x+0.5) )

/*! Macro that returns the lowest integer of x. */
# define GS_FLOOR(x) ( int( ((x)>0)? (x):((x)-1) ) )

/*! Macro that returns the highest integer of x. */
# define GS_CEIL(x) ( int( ((x)>0)? ((x)+1):(x) ) )

/*! Macro that returns the maximum value of the two arguments. */
# define GS_MAX(a,b) ((a)>(b)? (a):(b))

/*! Macro that returns the maximum value of the three arguments. */
# define GS_MAX3(a,b,c) ((a)>(b)? (a>c?(a):(c)):((b)>(c)?(b):(c)))

/*! Macro that returns the minimum value of the two arguments. */
# define GS_MIN(a,b) ((a)<(b)? (a):(b))

/*! Macro that returns the minimum value of the three arguments. */
# define GS_MIN3(a,b,c) ((a)<(b)? ((a)<(c)?(a):(c)):((b)<(c)?(b):(c)))

/*! Macro that returns the absolute value of x. */
# define GS_ABS(x) ((x)>0? (x):-(x))

/*! Macro that returns |a-b|, that is the distance of two points in the line. */
# define GS_DIST(a,b) ( (a)>(b)? ((a)-(b)):((b)-(a)) )

/*! Macro that tests if the distance between a and b is closer or equal to ds. */
# define GS_NEXT(a,b,ds) ( ( (a)>(b)? ((a)-(b)):((b)-(a)) )<=(ds) )

/*! Macro that tests if the distance between a and 0 is closer or equal to ds. */ 
# define GS_NEXTZ(a,eps) ( (a)>-(eps) && (a)<(eps) ) // z from zero

/*! Macro that returns -1 if x is negative, 1 if x is positive and 0 if x is zero. */
# define GS_SIGN(x) ((x)<0)? -1: ((x)>0)? 1: 0

/*! Returns the converted angle, from radians to degrees (float version). */
# define GS_TODEG(r) (180.0f*float(r)/gspi)

/*! Returns the converted angle, from degrees to radians (float version). */
# define GS_TORAD(d) (gspi*float(d)/180.0f)

/*! Returns the converted angle, from radians to degrees (double version). */
# define GS_TODEGd(r) (180.0*double(r)/GS_PI)

/*! Returns the converted angle, from degrees to radians (double version). */
# define GS_TORADd(d) (GS_PI*double(d)/180.0)

// ============================== Bit Operation ============================

/*! Tests if flg has the given bit set. */
# define GS_FLAG_TEST(flg,bit) ((flg)&(bit))

/*! Sets on the given bit of the flag. */
# define GS_FLAG_ON(flg,bit) flg|=bit

/*! Swaps the given bit of the flag. */
# define GS_FLAG_SWAP(flg,bit) flg^=bit

/*! Sets off the given bit of the flag. This requires two instructions, so 
    that GS_FLAG_SWAP is faster. */ 
# define GS_FLAG_OFF(flg,bit) if((flg)&(bit)) flg^=bit

/*! Sets the given bit of the flag to be on or off, according to the boolean value of val. */
# define GS_FLAG_SET(flg,bit,val) flg = (val)? (flg)|(bit) : ((flg)&(bit)? (flg)^(bit):flg)

# define GS_BIT0        1 //!< Same as 1
# define GS_BIT1        2 //!< Same as 2
# define GS_BIT2        4 //!< Same as 4
# define GS_BIT3        8 //!< Same as 8
# define GS_BIT4       16 //!< Same as 16
# define GS_BIT5       32 //!< Same as 32
# define GS_BIT6       64 //!< Same as 64
# define GS_BIT7      128 //!< Same as 128
# define GS_ALLBITS   255 //!< Same as 255
# define GS_NOBITS      0 //!< Same as 0

//============================== end of file ===============================

# endif // GS_H

/*! \mainpage GraphSim Simulation Toolkit
 *  
 *  \author Marcelo Kallmann 
 *  \date 2010 
 * 
 *  \section term_dist_section Terms of Distribution
 *
 *  Copyright 2010 Marcelo Kallmann. All Rights Reserved.
 *  This software is distributed for noncommercial use only, without
 *  any warranties, and provided that all copies contain the full copyright
 *  notice licence.txt located at the base folder of the distribution.
 *
 *  This software includes portions of a library distributed by the
 *  University of North Carolina at Chapel Hill, which is also distributed
 *  only for noncommercial use. The full copyright notice can be found in
 *  the files using the library, for ex: src/gsim/cd_tree.cpp
 *  
 * \section overview_section Overview
 *
 *  GSim is a toolkit with several programming tools useful for research in
 *  animation and robotics. Read the provided readme.txt in the base folder
 *  for latest information
 * 
 */
