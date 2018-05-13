/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <stdio.h>
# include <ctype.h>
# include <string.h>
# include <stdlib.h>
# include <stdarg.h>

# include <gsim/gs_string.h>

//# define GS_USE_TRACE1 // constructors / destructors
//# define GS_USE_TRACE2 // member functions
//# define GS_USE_TRACE3 // set() member function
# include <gsim/gs_trace.h>

const char *GsString::_empty = "";

//============================= GsString ==========================================

GsString::GsString ()
 {
   GS_TRACE1 ( "Default Constructor" );
 }

GsString::GsString ( GsOutput& o )
 {
   GS_TRACE1 ( "Output Constructor" );
   o.init(*this);
 }

GsString::GsString ( char c, int len )
 {
   GS_TRACE1 ( "Char Constructor: "<<c );
   if ( len<=0 ) return;
   _data.size ( len+1 );
   _data.pop() = 0;
   memset ( &_data[0], c, len );
 }

GsString::GsString ( const char *st )
 {
   GS_TRACE1 ( "C string Constructor" );
   set ( st );
 }

GsString::GsString ( const GsString& st )
 {
   GS_TRACE1 ( "Copy Constructor" );
   set ( st );
 }

void GsString::set ( char c, int len )
 {
   if ( len<=0 ) len=0;
   _data.size ( len+1 );
   _data.pop() = 0;
   memset ( &_data[0], c, len );
 }

void GsString::set ( const GsString& st )
 {
   _data = st._data;
   if ( _data.size() )
    { _data.push()=0;
      _data.pop();
    }
 }

void GsString::set ( const char *st )
 {
   if ( st==(const char*)_data ) return;

   if ( !st )
    { _data.capacity(0);
    }
   else if ( !st[0] && _data.size()>0 ) 
    { _data.size(1);
      _data.pop()=0;
    }
   else 
    { _data.size ( strlen(st)+1 );
      strcpy ( &_data[0], st );
      _data.pop();
    }

   GS_TRACE3 ( "set:" << (&_data[0]) );
 }

void GsString::setf ( const char *fmt, ... )
 {
   if ( !fmt ) return;
   if ( fmt[0]==0 ) return;

   char buf[256];
   va_list args;

   buf[0] = 0;

   va_start ( args, fmt );
   vsprintf ( buf, fmt, args );
   va_end   ( args );

   set ( buf );
 }

void GsString::len ( int l )
 {
   _data.size(l+1);
   _data.pop()=0; 
 }

void GsString::calclen ()
 {
   int i;
   for ( i=0; i<len(); i++ )
    if ( _data[i]==0 )
     { len(i); return; }
 }

void GsString::compress ()
 {
   if ( _data.capacity()==0 ) return;
   if ( _data.size()==0 ) { _data.compress(); return; }
   _data.push()=0;   // ensures there is an ending 0 char
   _data.compress(); // now compress it
   _data.pop();      // puts back correct len/size
 }

void GsString::input ( int max )
 {
   if ( max<7 ) max=7;
   reserve ( max );

   fflush(stdout);
   fflush(stdin);

   if ( !fgets ( &_data[0], max+1, stdin ) )
    {
      _data[0]=0;
    }
   else
    {
      int sl = strlen(&_data[0]);
      if ( _data[sl-1]=='\n' ) sl--;
      len(sl);
    }

   fflush(stdin);
 }

void GsString::append ( const char* st )
 {
   if ( !st ) return;
   if ( !st[0] ) return;
   int stlen = strlen(st);
   int len = _data.size();
   _data.insert ( len, stlen+1 );
   strcpy ( &_data[len], st );
   _data.pop();
 }

void GsString::insert ( int i, const char *st )
 { 
   if ( !st ) return;
   if ( !st[0] ) return;

   int stlen = strlen(st);

   _data.insert ( i, stlen );
   memmove ( &_data[i], st, stlen );
   _data.push();
   _data.pop()=0; // ensures ending 0 in case i==size()
 }

void GsString::insert ( int i, int n, char c )
 {
   _data.insert ( i, n );
   _data.push();
   _data.pop()=0; // ensures ending 0 in case i==size()
   if ( c ) memset ( &_data[i], c, n );
 }

void GsString::remove ( int i, int n )
 {
   if ( _data.empty() || n<=0 ) return;
   if ( i>=_data.size() || i<0 ) return;
   _data.remove ( i, n );
   _data.push()=0;
   _data.pop();
 }

void GsString::lower ()
 {
   char *pt = &_data[0];
   while ( *pt ) { *pt=(char)GS_LOWER(*pt); pt++; }
 }

void GsString::upper ()
 {
   char *pt = &_data[0];
   while ( *pt ) { *pt=(char)GS_UPPER(*pt); pt++; }
 }

int GsString::atoi () const
 {
   return ::atoi(_data);
 }

long GsString::atol() const
 {
   return ::atol(_data);
 }

float GsString::atof () const
 {
   return (float)::atof(_data);
 }

void GsString::trim ()
 {
   int inf, sup;
   bounds    ( inf, sup );
   substring ( inf, sup );
   GS_TRACE2 ( "trim:[" << _data << "]" );
 }

void GsString::ltrim ()
 {
   int inf, sup;
   bounds    ( inf, sup );
   substring ( inf, -1 );
   GS_TRACE2 ( "ltrim:[" << _data << "]" );
 }

void GsString::rtrim ()
 {
   int inf, sup;
   bounds    ( inf, sup );
   substring ( 0,   sup );
   GS_TRACE2 ( "rtrim:[" << _data << "]" );
 }

void GsString::bounds ( int &xi, int &xf ) const
 {
   xi = 0;
   xf = _data.size()-1;
   if ( xf<0 ) { xi=-1; return; }

   while ( xi<=xf && isspace(_data[xi]) ) xi++;
   while ( xf>=xi && isspace(_data[xf]) ) xf--;
 }

void GsString::substring ( int inf, int sup )
 {
   int max=_data.size()-1;
   
   if ( max<0 ) return;
   if ( inf<0 ) inf=0;
   if ( sup<0 || sup>max ) sup=max;

   int n = sup-inf+1;
   
   // void *memmove( void *dest, const void *src, size_t count );
   // The memmove function copies count bytes of characters from src to dest.
   // If some regions of the source area and the destination overlap,
   // memmove ensures that the original source bytes in the overlapping
   // region are copied before being overwritten.
   memmove ( &_data[0], &_data[inf], n );//   _data.move ( 0, inf, n );
   _data.size ( n+1 );
   _data.pop()=0;
 }

int GsString::search ( char c, int i ) const
 {
   if ( i<0 ) i=0;
   c = GS_LOWER(c);
   for ( ; i<len(); i++ ) if (GS_LOWER(_data[i])==c) return i;
   return -1;
 }

int GsString::searchcs ( char c, int i ) const
 {
   if ( i<0 ) i=0;
   for ( ; i<len(); i++ ) if (_data[i]==c) return i;
   return -1;
 }


int GsString::search ( const char *st, int i ) const
 {
   int stlen = strlen(st);
   if ( stlen==0 ) return -1;
   if ( i<0 ) i=0;

   int lastindex = len()-stlen;
   for ( ; i<=lastindex; i++ )
    { if ( ::gs_compare(&_data(i),st,stlen)==0 ) return i;
    }

   return -1;
 }

int GsString::searchcs ( const char *st, int i ) const
 {
   int stlen = strlen(st);
   if ( stlen==0 ) return -1;
   if ( i<0 ) i=0;

   int lastindex = len()-stlen;
   for ( ; i<=lastindex; i++ )
    { if ( ::gs_comparecs(&_data(i),st,stlen)==0 ) return i;
    }

   return -1;
 }

int GsString::replace ( int i, int n, const char* newst )
 {
   int oldlen = n;
   int newlen = newst? strlen(newst):0;
   
   if ( oldlen<newlen ) // open space
    { insert ( i, newlen-oldlen );
    }
   else if ( oldlen>newlen ) // remove space
    { remove ( i, oldlen-newlen );
    }

   // void *memmove( void *dest, const void *src, size_t count );
   // The memmove function copies count bytes of characters from src to dest.
   // If some regions of the source area and the destination overlap,
   // memmove ensures that the original source bytes in the overlapping
   // region are copied before being overwritten.
   if ( newlen>1 ) memmove ( &_data[i], newst, newlen );
    else if ( newlen==1 ) _data[i]=newst[0];

   return i+newlen;
 }

int GsString::replace ( const char* oldst, const char* newst, int i )
 {
   i = search ( oldst, i );
   if ( i<0 ) return i; // not found
   replace ( i, strlen(oldst), newst );
   return i;
 }

int GsString::replacecs ( const char* oldst, const char* newst, int i )
 {
   i = searchcs ( oldst, i );
   if ( i<0 ) return i; // not found
   replace ( i, strlen(oldst), newst );
   return i;
 }

int GsString::replall ( const char* oldst, const char* newst )
 {
   int count=0, i=0, oldlen=strlen(oldst);
   while ( true )
    { i = search ( oldst, i );
      if ( i<0 ) return count;
      count++;
      i = replace ( i, oldlen, newst );
    }
 }

int GsString::replallcs ( const char* oldst, const char* newst )
 {
   int count=0, i=0, oldlen=strlen(oldst);
   while ( true )
    { i = searchcs ( oldst, i );
      if ( i<0 ) return count;
      count++;
      i = replace ( i, oldlen, newst );
    }
 }

char* GsString::abandon ( char*& s )
 {
   GS_TRACE2 ( "abandon!" ); 
   s = _data.abandon();
   if ( s==0 ) { s=new char[1]; s[0]=0; }
   return s;
 }

# define BUFSIZE 48
GsString& GsString::operator << ( int i )
 {
   int len = GsString::len();
   if ( capacity()<len+BUFSIZE ) reserve ( len+BUFSIZE );
   len += sprintf ( &(_data[len]), "%d", i );
   GsString::len(len);
   return *this; 
 }

GsString& GsString::operator << ( float f )
 {
   int len = GsString::len();
   if ( capacity()<len+BUFSIZE ) reserve ( len+BUFSIZE );
   len += sprintf ( &(_data[len]), "%f", f );
   GsString::len(len);
   return *this; 
 }

GsString& GsString::operator << ( double d )
 {
   int len = GsString::len();
   if ( capacity()<len+BUFSIZE ) reserve ( len+BUFSIZE );
   len += sprintf ( &(_data[len]), "%f", d );
   GsString::len(len);
   return *this; 
 }

GsString& GsString::operator << ( char c )
 {
   char buf[2];
   buf[0]=c; buf[1]=0;
   append(buf);
   return *this; 
 }

//============================ Filename/Path functions ======================================

int remove_filename ( GsString& path )
 {
   int i;
   for ( i=path.len()-1; i>=0; i-- )
    { if ( path[i]=='/' || path[i]=='\\' )
       { path.len(i+1);
         break;
       }
    }
   return i;
 }

int remove_path ( GsString& fname )
 {
   int i;
   for ( i=fname.len()-1; i>=0; i-- )
    { if ( fname[i]=='/' || fname[i]=='\\' )
       { fname.substring(i+1,-1);
         break;
       }
    }
   return i;
 }

int remove_extension ( GsString& fname )
 {
   int i;
   for ( i=fname.len()-1; i>=0; i-- )
    { if ( fname[i]=='.' ) { fname.len(i); return i; }
      if ( fname[i]=='/' || fname[i]=='\\' ) return -1; // no extension
    }
   return i;
 }

int get_filename ( const GsString& path, GsString& fname )
 {
   fname = path;
   return remove_path ( fname );
 }

int get_path ( const GsString& fname, GsString& path )
 {
   path = fname;
   return remove_filename ( path );
 }

int get_extension ( const GsString& fname, GsString& ext )
 {
   int i, j;
   for ( i=fname.len()-1; i>=0; i-- )
    { if ( fname[i]=='.' )
       { ext.len ( fname.len()-i-1 );
         for ( j=0; j<ext.len(); j++ )
          ext[j] = fname[i+j+1];
         break;
       }
      if ( fname[i]=='/' || fname[i]=='\\' ) return -1; // no extension
    }
   return i;
 }

const char* get_extension ( const GsString& fname )
 {
   int i;
   for ( i=fname.len()-1; i>=0; i-- )
    { if ( fname[i]=='.' && i+1<fname.len() )
       { return &fname[i+1];
       }
      if ( fname[i]=='/' || fname[i]=='\\' ) break; // no extension
    }
   return 0;
 }

int extract_filename ( GsString& path, GsString& fname )
 {
   int i = get_filename ( path, fname );
   if ( i>=0 ) path.len(i+1);
   return i;
 }

int extract_extension ( GsString& fname, GsString& ext )
 {
   int i = get_extension ( fname, ext );
   if ( i>=0 ) fname.len(i);
   return i;
 }

bool has_extension ( const GsString& fname, const char* ext )
 {
   const char* fext = get_extension(fname);
   if ( !fext ) return false;
   return gs_compare ( fext, ext )==0? true:false;
 }

bool has_extensioncs ( const GsString& fname, const char* ext )
 {
   const char* fext = get_extension(fname);
   if ( !fext ) return false;
   return gs_comparecs ( fext, ext )==0? true:false;
 }

bool validate_path ( GsString& path )
 {
   path.replall ( "\\", "/" );
   if ( path[0]==0 ) return false;
   if ( path.lchar()!='/' ) path.append ( "/" );
   return true;
 }

//================================= End of File ======================================
