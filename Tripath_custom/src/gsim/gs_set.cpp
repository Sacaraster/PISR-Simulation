/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

//# include <stdlib.h>
# include <gsim/gs_set.h>

//============================= GsSetBase ================================

int GsSetBase::insert ( void* pt )
 {
   int i;

   if ( !pt )
    { i=_data.size(); _freepos.push()=i; _data.push()=pt; }
   else if ( _freepos.size()>0 )
    { i=_freepos.pop(); _data[i]=pt; }
   else
    { i=_data.size(); _data.push()=pt; }

   return i;
 }

void* GsSetBase::extract ( int i )
 {
   if ( i<0 || i>=_data.size() ) return 0;
   if ( !_data[i] ) return 0;

   void* pt = _data[i];
   _data[i]=0;

   if ( i==_data.size()-1 )
    { _data.pop(); }
   else
    { _freepos.push()=i; }

   return pt;
 }

void GsSetBase::compress ()
 {
   if ( _data.size()>0 )
    { int i, n;
      while ( _data.top()==0 )
       { _data.pop();
         n = _data.size();
         for ( i=0; i<_freepos.size(); i++ )
           if ( _freepos[i]==n ) { _freepos[i]=_freepos.pop(); break; }
       }
    }
   _data.compress();
   _freepos.compress();
 }

bool GsSetBase::remove_gaps ( GsArray<int>& newindices )
 {
   int i, j;

   newindices.size ( _data.size() );

   // advance untill first null
   for ( i=0; i<_data.size(); i++ )
    { if ( _data[i] ) newindices[i]=i;
       else break;
    }
   if ( i==_data.size() ) return false; // nothing done

   j = i;
   while ( i<_data.size() )
    { if ( !_data[i] )
       { newindices[i] = -1; }
      else
       { _data[j]=_data[i];
         _data[i]=0;
         newindices[i] = j;
         j++;
       }
      i++;
    }

   _data.size ( j );
   _freepos.size ( 0 );
   return true;
 }

//============================== EOF ================================
