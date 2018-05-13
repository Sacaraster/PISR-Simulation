/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/
 
# include <stdlib.h>
# include <string.h>

# include <gsim/gs_array.h>

# define DATA(i)     ((char*)_data)+(sizeofx*(i))
# define DESTDATA(i) ((char*)desta._data)+(sizeofx*(i))
# define NEWDATA(i)  ((char*)newdata)+(sizeofx*(i))

//================================= methods =======================================================

GsArrayBase::GsArrayBase ( unsigned sizeofx, int s, int c ) 
            : _size(s), _capacity(c)
 { 
   if ( _capacity<_size ) _capacity=_size;
   _data = _capacity>0? malloc(sizeofx*_capacity) : 0;
 }

GsArrayBase::GsArrayBase ( unsigned sizeofx, const GsArrayBase& a ) 
            : _size(a._size), _capacity(a._size)
 { 
   if ( _capacity>0 ) 
    { _data = malloc ( sizeofx*_capacity );
      if ( _size>0 ) memcpy ( _data, a._data, sizeofx*_size );
    }
   else _data = 0;
 }

GsArrayBase::GsArrayBase ( void* pt, int s, int c ) 
            : _data(pt), _size(s), _capacity(c)
 {
 }

void GsArrayBase::free_data ()
 { 
   if (_data) free(_data); 
 }

void GsArrayBase::size ( unsigned sizeofx, int ns )
 {
   _size = ns;

   if ( _size>_capacity )
    { _capacity = _size;
      _data = realloc ( _data, sizeofx*_capacity ); // if _data==0, realloc reacts as malloc.
    }
 }

void GsArrayBase::capacity ( unsigned sizeofx, int nc )
 {
   if ( nc==_capacity ) return;
   if ( nc<=0 ) { if(_data)free(_data); _data=0; _capacity=_size=0; return; }

   _capacity = nc;
   if ( _size>_capacity ) _size=_capacity;

   _data = realloc ( _data, sizeofx*_capacity ); // if _data==0, realloc reacts as malloc.
 }

int GsArrayBase::vid ( int index ) const
 {
   if ( index<0 ) index += _size * ( -index/_size + 1 );
   return index%_size;
 }

void GsArrayBase::compress ( unsigned sizeofx )
 {
   if ( _size==_capacity ) return;

   if ( !_size ) { free ( _data ); _data=0; }
    else _data = realloc ( _data, sizeofx*_size );

   _capacity = _size;
 }

void GsArrayBase::remove ( unsigned sizeofx, int i, int n )
 { 
   if ( _size==0 ) return;
   if ( i<_size-n ) memmove ( DATA(i), DATA(i+n), sizeofx*(_size-(i+n)) );
   _size-=n;
 }

void GsArrayBase::insert ( unsigned sizeofx, int i, int n )
 { 
   _size += n;

   if ( _size>_capacity ) 
    { _capacity = _size*2; 
      _data = realloc ( _data, sizeofx*_capacity );
    }

   if ( i<_size-n )
    { memmove ( DATA(i+n), DATA(i), sizeofx*(_size-n-i) ); // ok with overlap
    }
 }

void GsArrayBase::push ( unsigned sizeofx )
 { 
   _size++;

   if ( _size>_capacity ) 
    { _capacity = _size*2; 
      _data = realloc ( _data, sizeofx*_capacity );
    }
 }

void GsArrayBase::copy ( unsigned sizeofx, int dest, int src, int n )
 { 
   //If some regions of the source area and the destination overlap, memmove ensures that
   //the original source bytes in the overlapping region are copied before being overwritten.
   memmove ( DATA(dest), DATA(src), sizeofx*(n) );
 }

void GsArrayBase::copyto ( unsigned sizeofx, GsArrayBase& desta, int dest, int src, int n ) const
 { 
   //If some regions of the source area and the destination overlap, memmove ensures that
   //the original source bytes in the overlapping region are copied before being overwritten.
   memmove ( DESTDATA(dest), DATA(src), sizeofx*(n) );
 }

void GsArrayBase::sort ( unsigned sizeofx, gscompare cmp ) 
 { 
   ::qsort ( _data, (size_t)_size, (size_t)sizeofx, cmp ); 
 }

int GsArrayBase::lsearch ( unsigned sizeofx, const void *x, gscompare cmp ) const
 { 
   char *pt = (char*)_data;

   for ( int i=0; i<_size; i++ ) { if ( cmp(pt,x)==0 ) return i; pt+=sizeofx; }

   return -1;
 }

int GsArrayBase::bsearch ( unsigned sizeofx, const void *x, gscompare cmp, int *pos ) const
 {
   int comp;
   int i, e, p;

   comp=1; i=0; e=_size; p=e/2;
   
   while ( comp!=0 && i!=e )
    { comp = cmp ( x, DATA(p) );
      if ( comp<0 ) e = p;
       else if ( comp>0 ) i = p+1;
      p = i + (e-i)/2;
    }

   if (pos) *pos=p;

   return comp==0? p : -1;
 }

int GsArrayBase::insort ( unsigned sizeofx, const void *x, gscompare cmp )
 { 
   int pos;
   bsearch ( sizeofx, x, cmp, &pos );
   insert ( sizeofx, pos, 1 );
   return pos;
 }

int GsArrayBase::uniqinsort ( unsigned sizeofx, const void *x, gscompare cmp )
 { 
   int pos;
   if ( bsearch(sizeofx,x,cmp,&pos)>=0 ) return -1; // already there
   insert ( sizeofx, pos, 1 );
   return pos;
 }

void GsArrayBase::copyfrom ( unsigned sizeofx, const GsArrayBase& a )
 { 
   if ( _data==a._data ) return;

   if ( _capacity<a._size  ) 
    { _capacity = a._size;
      _data = realloc ( _data, sizeofx*_capacity );
    }

   _size = a._size;
   if ( _size>0 ) memcpy ( _data, a._data, sizeofx*_size );
 }

void* GsArrayBase::abandon ()
 {
   void *pt = _data;     
   _data=0;
   _size=0;
   _capacity=0;
   return pt;
 }

void GsArrayBase::adopt ( GsArrayBase& a )
 {
   if ( _data ) free ( _data );
   _data     = a._data;     a._data=0;
   _size     = a._size;     a._size=0;
   _capacity = a._capacity; a._capacity=0;
 }

void GsArrayBase::adopt ( void* pt, int s, int c )
 {
   if ( _data ) free ( _data );
   _data     = pt;
   _size     = s;
   _capacity = c;
 }

//============================== end of file ===============================
