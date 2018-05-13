/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <gsim/se_dcdt_elements.h>

//============================== SeDcdtVertex ==================================

// this could be optimized but we are usually dealing with only 1-2 ids per array
inline void insertIfNotThere ( GsArray<int>& a, int id )
 {
   for ( int i=0; i<a.size(); i++ ) { if ( a[i]==id ) return; }
   a.push() = id;
 }

inline void insertIds ( GsArray<int>& a, const GsArray<int>& ids )
 {
   for ( int i=0; i<ids.size(); i++ ) insertIfNotThere ( a, ids[i] );
 }

void SeDcdtVertex::get_references ( GsArray<int>& ids )
 {
   SeDcdtEdge *e;
   SeDcdtSymEdge *si, *s;

   ids.size ( 0 );
   si = s = se();
   do { e = s->edg();
        if ( e->is_constrained() ) insertIds ( ids, e->ids );
        s = s->rot();
      } while ( s!=si );
 }

GsOutput& operator<< ( GsOutput& out, const SeDcdtVertex& v )
 {
   return out << v.p;
 }

GsInput& operator>> ( GsInput& inp, SeDcdtVertex& v )
 { 
   return inp >> v.p;
 }

//=============================== SeDcdtEdge ==================================

bool SeDcdtEdge::has_id ( int id ) const
 {
   for ( int i=0; i<ids.size(); i++ )
    if ( ids[i]==id ) return true;
   return false;
 }
    
bool SeDcdtEdge::has_id ( const GsArray<int>& ida ) const
 {
   for ( int i=0; i<ida.size(); i++ )
    if ( has_id(ida[i]) ) return true;
   return false;
 }

bool SeDcdtEdge::has_other_id_than ( int id ) const
 {
   for ( int i=0; i<ids.size(); i++ )
    if ( ids[i]!=id ) return true;
   return false;
 }

bool SeDcdtEdge::remove_id ( int id ) // remove all occurences
 {
   bool removed=false;
   int i=0;
   while ( i<ids.size() )
    { if ( ids[i]==id )
       { ids[i]=ids.top(); ids.pop(); removed=true; }
      else
       { i++; }
    }
   return removed;
 }

void SeDcdtEdge::add_constraints ( const GsArray<int>& cids )
 {
   int i, st=ids.size();
   ids.size ( st+cids.size() );
   for ( i=0; i<cids.size(); i++ ) ids[st+i] = cids[i];
 }

GsOutput& operator<< ( GsOutput& out, const SeDcdtEdge& e )
 {
   return out << e.ids;
 }

GsInput& operator>> ( GsInput& inp, SeDcdtEdge& e )
 {
   return inp >> e.ids;
 }

//=============================== SeDcdtFace ==================================

bool SeDcdtFace::junction () const
 {
   SeDcdtSymEdge* s = se();
   if ( s->edg()->is_constrained() ) return false; s=s->nxt();
   if ( s->edg()->is_constrained() ) return false;
   if ( s->nxt()->edg()->is_constrained() ) return false;
   return true;
 }

bool SeDcdtFace::border () const
 { 
   SeDcdtSymEdge* s = se();
   if ( s->vtx()->border ) return true; s=s->nxt();
   if ( s->vtx()->border ) return true;
   if ( s->nvtx()->border ) return true;
   return false;
 }

//=============================== End of File ===============================
