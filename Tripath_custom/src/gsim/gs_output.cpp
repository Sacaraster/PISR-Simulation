/*=======================================================================
Copyright 2010 Marcelo Kallmann. All Rights Reserved.
This software is distributed for noncommercial use only, without
any warranties, and provided that all copies contain the full copyright
notice licence.txt located at the base folder of the distribution. 
=======================================================================*/

# include <ctype.h>
# include <stdarg.h> 
# include <string.h> 
# include <stdlib.h> 
# include <stdio.h>

# include <gsim/gs_output.h>
# include <gsim/gs_string.h>

//============================== Global data ====================================

GsOutput gsout;

static const char* DefaultIntFmt = "%d"; 
static const char* DefaultFloatFmt = "%g";
static const char* DefaultDoubleFmt = "%g";

//=============================== GsOutput ======================================

void GsOutput::_construct () // this can be only called from constructors
 { 
   _type = TypeStdout;
   _margin = 0;
   _margin_char = '\t';
   _auto_console = 1; // true
   _intfmt = (char *) DefaultIntFmt;
   _floatfmt = (char *) DefaultFloatFmt;
   _doublefmt = (char *) DefaultDoubleFmt;
   _func_udata = 0;
   _filename = 0;
 }

GsOutput::GsOutput ( FILE *f )
 { 
   _construct ();
   _type = TypeFile;
   _device.file = f; 
 }

GsOutput::GsOutput ( void(*f)(const char*,void*), void* udata )
 {
   _construct ();
   _type = TypeFunction;
   _func_udata = udata;
   _device.func = f;
 }

GsOutput::~GsOutput ()
 {
   close ();
   fmtdefaults (); // will free pointers only if needed
 }

void GsOutput::leave_file ()
 {
   _device.file=0;
   init ();
 }

void GsOutput::init () 
 {
   close(); 
   _type=TypeStdout; 
 }

void GsOutput::init ( FILE *f ) 
 { 
   close(); 
   _type=TypeFile; 
   _device.file=f; 
 }

bool GsOutput::open ( const char* filename )
 { 
   init ( fopen(filename,"w") );
   gs_string_set(_filename,filename);
   return _device.file? true:false;
 }

void GsOutput::init ( GsString &s ) 
 { 
   close(); 
   _type=TypeString; 
   _device.string=&s; 
 }

void GsOutput::init ( void(*f)(const char*,void*), void* udata ) 
 { 
   close(); 
   _type = TypeFunction; 
   _device.func = f;
   _func_udata = udata; 
 }

bool GsOutput::valid ()
 {
   switch ( _type )
    { case TypeStdout   : return true;
      case TypeFile     : return _device.file ?   true:false;
      case TypeString   : return _device.string ? true:false;
      case TypeFunction : return _device.func ?   true:false;
    }
   return false;
 }

void GsOutput::close ()
 { 
   if ( _type==TypeFile )
    { if ( _device.file ) 
       { fclose ( _device.file ); 
         _device.file = 0;
       } 
    }
   gs_string_set ( _filename, 0 );
 }

static void set_fmt ( char*& fmt, const char* def, const char* newfmt )
 {
   if ( fmt==def ) fmt=0; else gs_string_set ( fmt, 0 ); 

   if ( newfmt == def ) fmt = (char *) def; else gs_string_set ( fmt, newfmt );
 }

void GsOutput::fmtint ( const char* s )
 {
   set_fmt ( _intfmt, DefaultIntFmt, s );
 }

void GsOutput::fmtfloat ( const char* s )
 {
   set_fmt ( _floatfmt, DefaultFloatFmt, s );
 }

void GsOutput::fmtdouble ( const char* s )
 {
   set_fmt ( _doublefmt, DefaultDoubleFmt, s );
 }

void GsOutput::outm ()
 {
   int i;
   for ( i=0; i<_margin; i++ ) put ( _margin_char ); 
 }

void GsOutput::flush ()
 {
   switch ( _type )
    { case TypeStdout   : fflush(stdout); break;
      case TypeFile     : fflush(_device.file); break;
      case TypeString   : break;
      case TypeFunction : break;
    }
 }

void GsOutput::fmtdefaults () 
 { 
   fmtint ( DefaultIntFmt );
   fmtfloat ( DefaultFloatFmt );
   fmtdouble ( DefaultDoubleFmt );
 }

#define CHKCONSOLE  if(_auto_console && !gs_console_shown()) gs_show_console()
void GsOutput::put ( char c )
 {
   switch ( _type )
    { case TypeStdout   : CHKCONSOLE; fputc(c,stdout); break;
      case TypeFile     : fputc(c,_device.file); break;
      case TypeString   : { char st[2]; st[0]=c; st[1]=0; _device.string->append(st); } break;
      case TypeFunction : { char st[2]; st[0]=c; st[1]=0; _device.func(st,_func_udata); } break;
    }
 }

void GsOutput::put ( const char *st )
 {
   switch ( _type )
    { case TypeStdout   : CHKCONSOLE; fputs(st,stdout); break;
      case TypeFile     : fputs(st,_device.file); break;
      case TypeString   : _device.string->append(st); break;
      case TypeFunction : _device.func(st,_func_udata); break;
    }
 }

void GsOutput::putf ( const char *fmt, ... )
 {
   char buf[256];
   va_list args;

   buf[0] = 0;

   va_start ( args, fmt );
   vsprintf ( buf, fmt, args );
   va_end   ( args );

   put ( buf );
 }

void GsOutput::exit ( int code )
 {
   if ( gs_console_shown() )
    { put ( "Press Enter to exit..." );
      fflush(stdout);
      fflush(stdin);
      getchar();
    }
   ::exit ( code );
 }

void GsOutput::fatal ( const char *fmt, ... )
 {
   char buf[256];
   va_list args;

   buf[0] = 0;

   va_start ( args, fmt );
   vsprintf ( buf, fmt, args );
   va_end   ( args );

   put ( "\nGS FATAL ERROR:\n" );
   put ( buf );
   put ( gsnl );
   flush ();
   close ();

   exit ( 1 );
 }

void GsOutput::warning ( const char *fmt, ... )
 {
   char buf[256];
   va_list args;

   buf[0] = 0;

   va_start ( args, fmt );
   vsprintf ( buf, fmt, args );
   va_end   ( args );

   put ( "\nGS Warning:\n" );
   put ( buf );
   put ( gsnl );
   flush ();
 }

void GsOutput::safewrite ( const char* s )
 {
   GsOutput& o = *this;

   if ( !s ) { o.put("\"\""); return; } // we are really safe here

   bool ok = true;

   int i, len=strlen(s);
   if ( len==0 ) { o.put("\"\""); return; }

   if ( isdigit(s[0]) )
    { ok = false; }
   else
    { for ( i=0; i<len; i++ )
       { if ( !isalnum(s[i]) && s[i]!='_' ) { ok=false; break; }
       }
    }

   if ( ok ) { o.put(s); return; }

   o<<'"';
   for ( i=0; i<len; i++ )
    { if ( s[i]=='"' ) o<<'\\';
      o<<s[i];
    }
   o<<'"';
 }

//=============================== friends ===============================

GsOutput& operator<< ( GsOutput& o, const GsString& s )
 {
   o.put ( (const char*)s );
   return o;
 }

GsOutput& operator<< ( GsOutput& o, char c )
 {
   o.put(c);
   return o; 
 }

GsOutput& operator<< ( GsOutput& o, gsbyte c )
 {
   o.putf(o._intfmt,(int)c);
   return o;
 }

GsOutput& operator<< ( GsOutput& o, int i )
 {
   o.putf(o._intfmt,i);
   return o; 
 }

GsOutput& operator<< ( GsOutput& o, long i )
 {
   o.putf(o._intfmt,i);
   return o; 
 }

GsOutput& operator<< ( GsOutput& o, bool b )
 {
   o.put ( b? "true":"false" );
   return o; 
 }

GsOutput& operator<< ( GsOutput& o, short s )
 {
   o.putf ( o._intfmt, (int)s );
   return o;
 }

GsOutput& operator<< ( GsOutput& o, gsuint i )
 {
   o.putf ( o._intfmt, (gsuint) i );
   return o; 
 }

GsOutput& operator<< ( GsOutput& o, gsulong i )
 {
   o.putf ( o._intfmt, (gsuint) i );
   return o; 
 }

GsOutput& operator<< ( GsOutput& o, float f )
 {
   const char* fmt = o._floatfmt;

   if ( fmt[0]=='g' )
    { if ( f==int(f) )
       o.putf ( "%d.0", int(f) );
      else
       o.putf("%g",f);
    }
   else if ( fmt[0]=='f' )
    { if ( f==int(f) )
       o.putf("%d",int(f));
      else
       o.putf("%f",f);
    }
   else
    { o.putf(o._floatfmt,f);
    }

   return o;
 }

GsOutput& operator<< ( GsOutput& o, double d )
 {
   const char* fmt = o._doublefmt;

   if ( fmt[0]=='g' )
    { if ( d==int(d) )
       o.putf ( "%d.0", int(d) );
      else
       o.putf("%g",d);
    }
   else if ( fmt[0]=='f' )
    { if ( d==int(d) )
       o.putf("%d",int(d));
      else
       o.putf("%f",d);
    }
   else
    { o.putf(o._doublefmt,d);
    }

   return o;
 }

//============================== end of file ===============================
