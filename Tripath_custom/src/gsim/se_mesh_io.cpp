/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <gsim/se_mesh.h>
# include <gsim/gs_string.h>

//# define GS_USE_TRACE1  // IO trace
# include <gsim/gs_trace.h>

//================================ IO =========================================

# define ID(se) long(se->_edge)

void SeMeshBase::_elemsave ( GsOutput& out, SeMeshBase::ElemType type, SeElement* first )
 {
   GsManagerBase* man;
   SeElement *e;
   int esize;
   const char* st;

   if ( type==TypeVertex )
    { esize=_vertices; man=_vtxman; st="Vertices"; }
   else if ( type==TypeEdge )
    { esize=_edges; man=_edgman; st="Edges"; }
   else
    { esize=_faces; man=_facman; st="Faces"; }

   out << gsnl << st << gspc << esize << gsnl;

   e = first;
   do { e->_index = 0;
        out << ID(e->_symedge) <<gspc;
        man->output(out,e);
        out << gsnl;
        e = e->nxt(); 
      } while (e!=first);

   GS_TRACE1 ( esize << " elements written !" );
 }

bool SeMeshBase::save ( GsOutput& out )
 {
   SeBase *se;
   SeElement *el, *eli;
   int symedges, i, j;

   out << "SYMEDGE MESH DESCRIPTION\n\n";
   GS_TRACE1("Header written.");

   symedges = _edges*2;
   out << "SymEdges " << symedges << gsnl;

   if ( empty() ) return true;

   GsArray<SeBase*> ses(symedges);
   GsArray<SeEdge*> edg(symedges);

   // get adjacency information of symedges to save:  
   GS_TRACE1("Mounting S array...");
   i=0; 
   el = eli = _first->edg();
   do { se = el->se();
        el->_index = i/2;
        for ( j=0; j<2; j++ )
         { if ( j==1 ) se = se->sym();
           ses[i] = se;
           edg[i] = se->_edge;     // save edge pointer
           se->_edge = (SeEdge*)i; // and use it to keep indices
           i++;
	     }
        el = el->nxt();
      } while ( el!=eli );

   // adjust vtx and fac indices:
   GS_TRACE1("Adjusting indexes...");

   i = 0;
   el = eli = _first->vtx();
   do { el->_index=i++; el=el->nxt(); } while (el!=eli);

   i = 0;
   el = eli = _first->fac();
   do { el->_index=i++; el=el->nxt(); } while (el!=eli);

   for ( i=0; i<ses.size(); i++ )
    { out << ID(ses[i]->_next) << gspc
          << ID(ses[i]->_rotate) << gspc
          << ses[i]->_vertex->_index << gspc
          << edg[i]->_index << gspc
          << ses[i]->_face->_index << gspc
          << gsnl;
    }

   GS_TRACE1("Symedges written.");

   _elemsave ( out, TypeVertex, _first->vtx() );
   _elemsave ( out, TypeEdge,   edg[0] );
   _elemsave ( out, TypeFace,   _first->fac() );

   _curmark = 1;
   _marking = _indexing = false;
   for ( i=0; i<ses.size(); i++ ) ses[i]->_edge = edg[i];

   GS_TRACE1 ( "save OK !" );
   return true;
 }

# undef ID

//---------------------------------- load --------------------------------

void SeMeshBase::_elemload ( GsInput& inp, GsArray<SeElement*>& E,
                             const GsArray<SeBase*>& S, GsManagerBase* man )
 {
   int i, x;

   inp.get(); // skip elem type label

   E.size ( inp.geti() );
   
   for ( i=0; i<E.size(); i++ )
    { x = inp.geti();
      E[i] = (SeElement*)man->alloc();
      E[i]->_symedge = S.get(x);
      man->input ( inp, E[i] );
      //man->read ( E[i], f );
      if ( i>0 ) E[0]->_insert(E[i]);
    }

   GS_TRACE1 ( e.size() << " elements loaded !" );
 }

bool SeMeshBase::load ( GsInput& inp )
 {
   long i;

   inp.get(); if ( inp.ltoken()!="SYMEDGE" ) return false;
   inp.get(); if ( inp.ltoken()!="MESH" ) return false;
   inp.get(); if ( inp.ltoken()!="DESCRIPTION" ) return false;
   GS_TRACE1 ( "Signature ok." );

   // destroy actual structure to load the new one:
   destroy ();

   // load indices and store them in an array:
   inp.get(); // skip SymEdges label
   i = inp.getl();
   GsArray<SeBase*> S(i);
   for ( i=0; i<S.size(); i++ )
    { S[i] = new SeBase;
      S[i]->_next   = (SeBase*)inp.getl();
      S[i]->_rotate = (SeBase*)inp.getl();
      S[i]->_vertex = (SeVertex*)inp.getl();
      S[i]->_edge   = (SeEdge*)inp.getl();
      S[i]->_face   = (SeFace*)inp.getl();
    }
   GS_TRACE1 ( "Symedges loaded." );

   // load infos:
   GsArray<SeElement*> V;
   GsArray<SeElement*> E;
   GsArray<SeElement*> F;
   _elemload ( inp, V, S, _vtxman );
   _elemload ( inp, E, S, _edgman );
   _elemload ( inp, F, S, _facman );

   // convert indices to pointers:
   for ( i=0; i<S.size(); i++ )
    { S[i]->_next   = S[(long)(S[i]->_next)];
      S[i]->_rotate = S[(long)(S[i]->_rotate)];
      S[i]->_vertex = V[(long)(S[i]->_vertex)];
      S[i]->_edge   = E[(long)(S[i]->_edge)];
      S[i]->_face   = F[(long)(S[i]->_face)];
    }

   // adjust internal variables:
   _first     = S[0];
   _vertices  = V.size();
   _edges     = E.size();
   _faces     = F.size();
   _curmark   = 1;
   _marking = _indexing = false;

   GS_TRACE1 ( "load OK !" );
   return true;
 }
 
//=== End of File ===================================================================
