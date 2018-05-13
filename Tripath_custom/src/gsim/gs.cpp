/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution.
  =======================================================================*/

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/stat.h>

# include <gsim/gs.h>
# include <gsim/gs_output.h>

# ifdef GS_WINDOWS
# include <Windows.h>
# include <sys/timeb.h>
# include <io.h>
# else
# include <unistd.h>
# include <sys/time.h>
# endif

//================================ math ============================================

float gs_lerp ( float a, float b, float t )
 {
   return GS_LERP(a,b,t);
 }

float gs_cuberp ( float a, float b, float t )
 {
   t = GS_CUBIC(t); // cubic spline mapping
   return GS_LERP(a,b,t);
 }

float gs_todeg ( float radians )
 {
   return 180.0f * radians / float(GS_PI);
 }

double gs_todeg ( double radians )
 {
   return 180.0 * radians / double(GS_PI);
 }

float gs_torad ( float degrees )
 {
   return float(GS_PI) * degrees / 180.0f;
 }

double gs_torad ( double degrees )
 {
   return double(GS_PI) * degrees / 180.0;
 }

inline float gs_angnorm_inline ( float radians )
 {
   // this is best as usually the angle is just sligtly not in [-pi,pi]:
   while ( radians>gspi ) radians -= gs2pi;
   while ( radians<-gspi ) radians += gs2pi;
   return radians;
 }

float gs_angnorm ( float radians )
 {
   return gs_angnorm_inline ( radians );
 }

float gs_anglerp ( float radians1, float radians2, float t )
 {
   float a, a1, a2;

   //radians1 = gs_angnorm_inline(radians1);
   //radians2 = gs_angnorm_inline(radians2);

   if ( radians1<radians2 )
    { a1=radians1; a2=radians2; }
   else
    { a1=radians2; a2=radians1; }

   if ( a2-a1<=gspi )
    { a = GS_LERP(radians1,radians2,t);
    }
   else // by the other side is shorter
    { a1 += gs2pi;
      if ( radians1<radians2 )
       { a = GS_LERP(a1,a2,t); }
      else
       { a = GS_LERP(a2,a1,t); }
      if ( a>=gs2pi ) a-=gs2pi;
    }
   //gsout<<gs_todeg(radians1)<<gspc<<gs_todeg(radians2)<<gspc<<gs_todeg(a)<<gsnl;
   return a;
 }

float gs_angdist ( float radians1, float radians2 )
 {
   float a1, a2;

   if ( radians1<radians2 )
    { a1=radians1; a2=radians2; }
   else
    { a1=radians2; a2=radians1; }

   float adist = a2-a1;

   if ( a2-a1>gspi ) // by the other side is shorter
    { a1 += gs2pi;
      adist = GS_DIST(a1,a2);
      if ( adist>=gs2pi ) adist-=gs2pi;
    }

   return adist;
 }

float gs_trunc ( float x )
 {
   return (float) (int) (x);
 }

double gs_trunc ( double x )
 {
   return (double) (int) (x);
 }

float gs_round ( float x )
 {
   return (float) (int) ((x>0.0)? (x+0.5f) : (x-0.5f));
 }

float gs_round ( float x, float prec )
 {
   return (float) (double(prec) * gs_round( double(x)/double(prec) ));
 }

double gs_round ( double x )
 {
   return (double) (int) ((x>0.0)? (x+0.5) : (x-0.5));
 }

double gs_round ( double x, double prec )
 {
   x = gs_round( x/prec );
   return x*prec;
 }

float gs_floor ( float x )
 {
   return (float) (int) ((x>0.0)? x : (x-1.0f));
 }

double gs_floor ( double x )
 {
   return (double) (int) ((x>0.0)? x : (x-1.0));
 }

float gs_ceil ( float x )
 {
   return (float) (int) ((x>0.0)? (x+1.0f) : (x));
 }

double gs_ceil ( double x )
 {
   return (double) (int) ((x>0.0)? (x+1.0) : (x));
 }

int gs_sqrt ( int n )
 {
   register int i, s=0, t;

   for ( i=15; i>=0; i-- )
    { t = ( s | (1<<i) );
      if (t*t<=n) s=t;
    }
   return s;
 }

int gs_fact ( int x )
 {
   if ( x<2 ) return 1;
   int m = x;
   while ( --x>1 ) m *= x;
   return m;
 }

int gs_pow ( int b, int e )
 {
   if ( e<=0 ) return 1;
   int pow=b;
   while ( --e>0 ) pow*=b;
   return pow;
 }

float gs_pow ( float b, int e )
 {
   if ( e<=0 ) return 1.0f;
   float pow=b;
   while ( --e>0 ) pow*=b;
   return pow;
 }

double gs_pow ( double b, int e )
 {
   if ( e<=0 ) return 1.0;
   double pow=b;
   while ( --e>0 ) pow*=b;
   return pow;
 }

float gs_dist ( float a, float b )
 {
   return GS_DIST(a,b);
 }

float gs_abs ( float a )
 {
   return GS_ABS(a);
 }

//========================== compare ==========================

int gs_compare ( const char *s1, const char *s2 )
 {
   int c1, c2; // ANSI definition of toupper() uses int types
   while ( *s1 && *s2 )
    { c1 = GS_UPPER(*s1);
      c2 = GS_UPPER(*s2);
      if ( c1!=c2 ) return c1-c2;
      s1++; s2++;
    }
   if ( !*s1 && !*s2 ) return 0;
   return !*s1? -1:1;
 }

int gs_comparecs ( const char *s1, const char *s2 )
 {
   int c1, c2; // ANSI definition of toupper() uses int types

   while ( *s1 && *s2 )
    { c1 = *s1;
      c2 = *s2;
      if ( c1!=c2 ) return c1-c2;
      s1++; s2++;
    }
   if ( !*s1 && !*s2 ) return 0;
   return !*s1? -1:1;
 }

int gs_compare ( const char *s1, const char *s2, int n )
 {
   int c1, c2; // ANSI definition of toupper() uses int types

   //   printf("[%s]<>[%s] (%d)\n",s1,s2,n);

   while ( *s1 && *s2 )
    { c1 = GS_UPPER(*s1);
      c2 = GS_UPPER(*s2);
      if ( c1!=c2 ) return c1-c2;
      s1++; s2++; n--;
      if ( n==0 ) return n; // are equal
    }
   if ( !*s1 && !*s2 ) return 0;
   return !*s1? -1:1;
 }

int gs_comparecs ( const char *s1, const char *s2, int n )
 {
   int c1, c2; // ANSI definition of toupper() uses int types

   while ( *s1 && *s2 )
    { c1 = *s1;
      c2 = *s2;
      if ( c1!=c2 ) return c1-c2;
      s1++; s2++; n--;
      if ( n==0 ) return n; // are equal
    }
   if ( !*s1 && !*s2 ) return 0;
   return !*s1? -1:1;
 }

int gs_compare ( const int *i1, const int *i2 )
 {
   return *i1-*i2;
 }

int gs_compare ( const float *f1, const float *f2 )
 {
   return GS_COMPARE(*f1,*f2);
 }

int gs_compare ( const double *d1, const double *d2 )
 {
   return GS_COMPARE(*d1,*d2);
 }

//========================== string ==========================

char* gs_string_new ( const char *tocopy )
 {
   if ( !tocopy ) return 0;
   char *s = new char [ strlen(tocopy)+1 ];
   strcpy ( s, tocopy );
   return s;
 }

void gs_string_delete ( char*& s )
 {
   delete[] s;
   s=0;
 }

void gs_string_set ( char*& s, const char *tocopy )
 {
   delete[] s;
   if ( !tocopy ) { s=0; return; }
   s = new char [ strlen(tocopy)+1 ];
   strcpy ( s, tocopy );
 }

void gs_string_renew ( char*& s, int size )
 {
   char *news = 0;

   if ( size>0 )
    { news = new char[size];
      news[0] = 0;
      if ( s )
       { int i;
         for ( i=0; i<size; i++ )
          { news[i]=s[i]; if(!s[i]) break; }
       }
      news[size-1] = 0;
    }

   delete []s;
   s = news;
 }

void gs_string_append ( char*&s, const char* toadd )
 {
   if ( !toadd ) return;
   if ( !toadd[0] ) return;

   char *tmp=0;
   if ( toadd==s ) { tmp=gs_string_new(toadd); toadd=tmp; }

   int slen = s? strlen(s):0;
   int newlen = slen+strlen(toadd)+1;
   gs_string_renew ( s, newlen );
   strcat ( s, toadd );

   delete[] tmp;
 }

//========================== IO ==========================

# ifdef GS_WINDOWS
static bool gsConsoleShown=false;
# endif

void gs_output_to_disk ( const char* outfile, const char* errfile )
 {
   # ifdef GS_WINDOWS
   gsConsoleShown=true;
   # endif

   if ( freopen ( outfile? outfile:"stdout.txt", "w", stdout ) )
     setbuf ( stdout, NULL );

   if ( freopen ( errfile? errfile:"stderr.txt", "w", stderr ) )
     setbuf ( stderr, NULL );
 }

void gs_show_console ()
 {
   # ifdef GS_WINDOWS 
   # ifndef __CYGWIN32__
   gsConsoleShown = true;
   AllocConsole();
   freopen("conin$", "r", stdin);
   freopen("conout$", "w", stdout);
   freopen("conout$", "w", stderr);
   # endif
   # endif
 }

bool gs_console_shown ()
 {
   # ifdef GS_WINDOWS
   return gsConsoleShown;
   # else
   return true;
   # endif
 }

bool gs_canopen ( const char* fname )
 {
   FILE* fp = fopen ( fname, "r" );
   if ( fp )
    { fclose(fp); return true; }
   else
    { return false; }
 }

bool gs_absolute ( const char* path )
 {
   if ( !path ) return false;
   if ( !path[0] ) return false;
   if ( path[0]=='\\' || path[0]=='/' ) return true; // /dir, \\machine\dir
   if ( !path[1] ) return false; // eg a 1-letter filename
   if ( path[1]==':' ) return true; // c:dir is considered absolute in windows
   return false;
 }

const char* gs_filename ( const char* fname )
 {
   if ( !fname ) return 0;

   int i, len = strlen(fname);

   for ( i=len-1; i>=0; i-- )
    { if ( fname[i]=='/' || fname[i]=='\\' )
       { return fname+i+1;
       }
    }

   return fname;
 }

const char* gs_extension (  const char* fname )
 {
   if ( !fname ) return 0;

   int i, len = strlen(fname);

   for ( i=len-1; i>=0; i-- )
    { if ( fname[i]=='.' ) { return fname+i+1; }
      if ( fname[i]=='/' || fname[i]=='\\' ) return 0; // no extension
    }

   return 0;
 }

static struct stat _lstat;
static char *_lstatname = 0;
static bool _lsresult = false;

static bool fill_stat ( const char *name )
 {
  if ( _lstatname && strcmp(_lstatname,name)==0 ) return _lsresult;
  gs_string_set ( _lstatname, name );
  _lsresult = ( stat(name,&_lstat)==0 );
  return _lsresult;
}

bool gs_exist ( const char* name )
 {
   return name && *name && fill_stat(name);
 }

bool gs_isdir ( const char* name )
 {
   if ( !fill_stat(name) ) return false;
   return ( _lstat.st_mode&0170000 )==0040000;
 }

gsuint gs_size ( const char* name )
 {
   if ( !fill_stat(name) ) return 0;
   return (gsuint)_lstat.st_size;
 }

unsigned long long gs_sizel ( const char* name )
 {
   if ( !fill_stat(name) ) return 0;
   return (unsigned long long)_lstat.st_size;
 }

gsuint gs_mtime ( const char *name )
 {
   if ( !fill_stat(name) ) return 0;
   if ( _lstat.st_mtime ) return (gsuint) _lstat.st_mtime;
   if ( _lstat.st_atime ) return (gsuint) _lstat.st_atime;
   return (gsuint) _lstat.st_ctime;
 }

void gs_exit ( int code )
 {
   exit ( code );
 }

void gs_sleep ( int milisecs )
 {
   # ifdef GS_WINDOWS
   Sleep ( (DWORD)(milisecs) );
   # else
   usleep ( ((useconds_t)milisecs)*useconds_t(1000) );
   # endif
 }

// ================================= Timer ==================================

double gs_time ()
 {
   static bool first=true;
   #ifdef GS_WINDOWS
   static double _perf_freq = 0.0;
   static double _utc_origin = 0.0;
   #endif

   if ( first )
    { first = false;
      # ifdef GS_WINDOWS
      LARGE_INTEGER lpFrequency;
      LARGE_INTEGER lpPerformanceCount;
      struct _timeb tbuf;
      if ( QueryPerformanceFrequency(&lpFrequency)==false )
       { gsout.put("GsTimer: WIN32 High Resolution Performance Counter not supported.\n"); }
      else
       { _perf_freq = (double)lpFrequency.QuadPart;
         QueryPerformanceCounter ( &lpPerformanceCount );
         _ftime( &tbuf ); // get UTC time in seconds from Jan 1, 1970
         double hrcTime = double(lpPerformanceCount.QuadPart) / double(lpFrequency.QuadPart);
         double utcTime = double(tbuf.time) + double(tbuf.millitm)*1.0E-3;
         _utc_origin = utcTime - hrcTime;
       }
      # endif
    }

   # ifdef GS_WINDOWS
   if ( _perf_freq==0 ) // not available
    { _timeb tp;
      _ftime(&tp);
      return 0.001*(double)tp.millitm + (double)tp.time;
    }
   else
    { LARGE_INTEGER lpPerformanceCount;
      QueryPerformanceCounter (&lpPerformanceCount);
      return _utc_origin + double(lpPerformanceCount.QuadPart) / _perf_freq;
    }
   # endif

   # ifdef GS_LINUX
   timeval tp;
   if ( gettimeofday(&tp,0)==-1 ) return 0;
   return 0.000001*(double)tp.tv_usec + (double)tp.tv_sec;
   # endif
 }

// =============================== Random Methods ==================================

#ifdef GS_SYSTEM_RAND_LIBS

// The docs say: rand function returns a pseudorandom integer in the range 0 to RAND_MAX (32767)

void gs_rseed ( gsuint i )
 {
   srand ( unsigned(i) );
 }

# define RAND01  ((double)rand()/(RAND_MAX))

float gs_random () // in [0,1]
 {
   return (float)RAND01;
 }

float gs_random ( float min, float max ) // in [min,max]
 {
   return min + (max-min)*float(RAND01);
 }

double gs_random ( double min, double max )
 {
   return min + (max-min)*double(RAND01);
 }

int gs_random ( int min, int max )
 {
   return min + int ( (float(rand())/float(RAND_MAX+1)) * float(max-min) );
 }

#endif // GS_SYSTEM_RAND_LIBS

//=== End of File =======================================================================

