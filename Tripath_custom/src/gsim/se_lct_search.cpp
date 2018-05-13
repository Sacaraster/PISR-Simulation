/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <math.h>
# include <stdlib.h>

# include <gsim/gs_geo2.h>

# include <gsim/se_lct.h>
# include "se_triangulator_internal.h"

# define GS_TRACE_ONLY_LINES
//# define GS_USE_TRACE1 // main search method
//# define GS_USE_TRACE2 // search expansion
//# define GS_USE_TRACE3
# include <gsim/gs_trace.h>

//================================================================================
//========================== search path tree ====================================
//================================================================================

// nen/nex are the entrance/exit edges of the node being expanded, which is already in the search tree
// en/ex are the entrance/exit edge of the current traversal being evaluated for expansion
// (p1,p2) are en coordinates
bool SeLct::_canpass ( SeDcdtSymEdge* nen, SeDcdtSymEdge* nex, SeDcdtSymEdge* en, SeDcdtSymEdge* ex,
                       const GsPnt2& p1, const GsPnt2& p2, const GsPnt2& p3, float r, float d2 )
 {
   // check if can traverse en/ex traversal:
   if ( _man->is_constrained(ex->edg()) ) return false;

   // test if next triangle being tested has been already visited:
   if ( _mesh->marked(en->fac()) ) return false;

   if ( en->nxt()==ex ) // bot
    { 
      if ( _pre_clearance )
       { float cl = ex->edg()->cl(ex);
         if ( cl<d2 ) return false;
       }
      else
       { if ( dist2(p2,p3)<d2 ) return false;
         if ( !_sector_clear(ex->nxt(),d2,p2,p3,p1) ) return false;
       }
      if ( nex->fac()==_fi && nex->nxt()==nen ) // top->bot departure transition
       { if ( !_local_transition_free(ex,en,d2,_xi,_yi) ) return false;
       }
    }
   else // top
    {
      if ( _pre_clearance )
       { float cl = en->edg()->cl(en);
         if ( cl<d2 ) return false;
       }
      else
       { if ( dist2(p3,p1)<d2 ) return false;
         if ( !_sector_clear(en->nxt(),d2,p1,p2,p3) ) return false;
       }
      if ( nex->fac()==_fi && nen->nxt()==nex ) // bot->top departure transition
       { if ( !_local_transition_free(ex,en,d2,_xi,_yi) ) return false;
       }
    }

   return true;
 }

// en is the entrance edge, ex the exit edge, (p1,p2) are en coordinates
void SeLct::_trytoadd ( SeDcdtSymEdge* en, SeDcdtSymEdge* ex, int mi, const GsPnt2& p1, const GsPnt2& p2, const GsPnt2& p3 )
 {
   // verify if it is passable:
   PathNode& n = _ptree->nodes[mi];
   if ( !_canpass ( (SeDcdtSymEdge*)n.en, (SeDcdtSymEdge*)n.ex, en, ex, p1, p2, p3, _ptree->radius, _ptree->diam2 ) ) return;

   // ok it is passable, compute cost:
   double x, y;

   if ( en->nxt()==ex )
    _getcostpoint ( &n, n.x, n.y, p2.x, p2.y, p3.x, p3.y, x, y, _ptree->radius ); // bot
   else
    _getcostpoint ( &n, n.x, n.y, p3.x, p3.y, p1.x, p1.y, x, y, _ptree->radius ); // top
   // insert:
   # define PTDIST(a,b,c,d) float(sqrt(gs_dist2(a,b,c,d)))
   _ptree->add_child ( mi, en, ex, n.ncost+PTDIST(n.x,n.y,x,y), PTDIST(x,y,_xg,_yg), x,y ); // A* heuristic
   # undef PTDIST
 }

# define ExpansionNotFinished  -1
# define ExpansionBlocked      -2

int SeLct::_expand_lowest_cost_leaf ()
 {
   int min_i;

   if ( _ptree->leafs.size()>_maxfronts ) _maxfronts=_ptree->leafs.size();

   min_i = _ptree->lowest_cost_leaf ();
   GS_TRACE2 ( "Expanding leaf: "<<min_i );

   if ( min_i<0 ) return ExpansionBlocked; // no more leafs: path could not be found!

   // attention: array references may be invalidated due array reallocation during insertion
   SeDcdtSymEdge* s = (SeDcdtSymEdge*) _ptree->nodes[min_i].ex->sym();
   SeDcdtSymEdge* sn = s->nxt();
   SeDcdtSymEdge* sp = sn->nxt();
   const GsPnt2& p1 = s->vtx()->p; // note: s is in the triangle to expand
   const GsPnt2& p2 = sn->vtx()->p;
   const GsPnt2& p3 = sp->vtx()->p;
   float d2 = _ptree->diam2;

   // test if next triangle contains goal point:
   if ( gs_in_triangle(p1.x,p1.y,p2.x,p2.y,p3.x,p3.y,_xg,_yg) ) // reached goal triangle !
    { GS_TRACE1 ( "Goal triangle reached..." );
      double r = _ptree->radius;
      if ( !pt2circfree(this,s,_xg,_yg,r) ) // we do not know if the goal location is valid, so test it now
       { GS_TRACE1 ( "Goal location is invalid." );
         return ExpansionBlocked;
       }
      GS_TRACE1 ( "Goal location valid." );
      GS_TRACE1 ( "Analyzing arrival..." );
      SeDcdtSymEdge* nen = (SeDcdtSymEdge*)_ptree->nodes[min_i].en;
      SeDcdtSymEdge* nex = (SeDcdtSymEdge*)_ptree->nodes[min_i].ex;
      _analyze_arrival ( s, 3, r, d2, nen, nex );
      if ( _ent[3].type==EntBlocked )
       { if ( (sn->edg()->is_constrained()||dist2(p2,p3)<d2) &&
              (sp->edg()->is_constrained()||dist2(p3,p1)<d2) ) 
          { GS_TRACE1 ( "Arrival blocked from all possible entries." );
            return ExpansionBlocked;
          }
         else
          { GS_TRACE1 ( "Arrival is blocked, but search can continue..." );
            // at this point the arrival is not valid but the search will continue, and
            // note that the arrival triangle may still be used as passage so we let
            // the expansion tests in _trytoadd() proceed.
          }
       }
      else
       { GS_TRACE1 ( "Arrival tests passed." );
         GS_TRACE1 ( "Arrival is valid " << (_ent[3].type==EntTrivial?"and trivial.":"but non trivial.") );
         return min_i; // FOUND!
       }
    }
 
   int nsize = _ptree->nodes.size();
   _trytoadd ( s, sn, min_i, p1, p2, p3 ); // bot
   _trytoadd ( s, sp, min_i, p1, p2, p3 ); // top

   if ( _ptree->nodes.size()>nsize ) _mesh->mark ( s->fac() ); // only mark traversed faces

   if (_searchcb) _searchcb(_sudata);

   return ExpansionNotFinished; // continue the expansion
 }

void SeLct::_ptreeaddent ( SeDcdtSymEdge* s, bool top, bool edge )
 {
   const GsPnt2& p1 = s->vtx()->p;
   const GsPnt2& p2 = s->nvtx()->p;
   double x, y;
   if ( edge )
    { x=_xi; y=_yi; }
   else
    { _getcostpoint ( 0, _xi, _yi, p1.x, p1.y, p2.x, p2.y, x, y, _ptree->radius ); }

   // insert:
   # define PTDIST(a,b,c,d) (float)sqrt(gs_dist2(a,b,c,d))
   _ptree->add_child ( -1, top? s->nxt():s->nxn(), s, PTDIST(_xi,_yi,x,y), PTDIST(x,y,_xg,_yg), x, y );
   # undef PTDIST
 }

//================================================================================
//============================== search path =====================================
//================================================================================

/* - This is the A* algorithm that takes O(nf), f is the faces in the "expansion frontier". */
bool SeLct::_search_channel ( double x1, double y1, double x2, double y2, float radius, const SeFace* iniface )
 {
   GS_TRACE1 ( "Starting Search Path..." );

   if ( !_ptree ) _ptree = new PathTree;
   _clear_path(); // clear data from previous query and set _path_result to NoPath

   _channel.size(0);
   _xi=x1; _yi=y1; _xg=x2; _yg=y2;

   if ( !iniface ) return false;

   // Even if p1 is on an edge, locate_point will return in s a face that 
   // can be considered to contain p1 (p1 would be invalid if in a vertex)
   SeBase *s;
   LocateResult res=locate_point ( iniface, x1, y1, s );
   if ( res==NotFound )
    { GS_TRACE1 ( "Could not locate first point!" );
      _path_result=NoPath;
      return false;
    }
   _fi = s->fac(); // save initial face
   if ( !pt1circfree(this,s,x1,y1,radius) ) { _path_result=NoPath; return false; }

   // Check if we are to solve trivial or local paths, testing if both points are in the same triangle:
   if ( _man->in_triangle(s->vtx(),s->nxt()->vtx(),s->nxn()->vtx(),x2,y2) )
    { GS_TRACE1 ( "Both points are in the same triangle..." );
      if ( radius==0 )
       { GS_TRACE1 ( "Trivial path returned." );
         _path_result=TrivialPath; return true; // this is it
       }

      if ( !pt2circfree(this,s,x2,y2,radius) )
       { GS_TRACE1 ( "Goal point in same triangle invalid. No path returned." );
         _path_result=NoPath; return false;
       }

      _path_result = _analyze_local_path ( s, radius );
      if ( _path_result==TrivialPath )
       { GS_TRACE1 ( "Capsule free. Trivial path returned." );
         return true; // path exists
       }
      else if ( _path_result==LocalPath )
       { GS_TRACE1 ( "Deformable capsule is passable. Local path returned." );
         return true; // path exists
       }
      // at this point the result may be a GlobalPath or a NoPath
      GS_TRACE1 ( "Deformable capsule is not passable." );
      // we then let the normal entrance analysis and search to proceed.
      // the entrance that blocked the capsule will also be blocked but
      // there may be a global path to get there so we just do not
      // mark the initial face as visited, allowing it to be found by the global search.   
    }

   GS_TRACE1 ( "Searching for a global path..." );
   GS_TRACE1 ( "Analyzing entrances..." );

   _analyze_entrances ( s, _xi, _yi, radius );

   GS_TRACE1 ( "Entrance 0: "<<(_ent[0].type==EntBlocked?"blocked":_ent[0].type==EntTrivial?"trivial":"not trivial") );
   GS_TRACE1 ( "Entrance 1: "<<(_ent[1].type==EntBlocked?"blocked":_ent[1].type==EntTrivial?"trivial":"not trivial") );
   GS_TRACE1 ( "Entrance 2: "<<(_ent[2].type==EntBlocked?"blocked":_ent[2].type==EntTrivial?"trivial":"not trivial") );

   GS_TRACE1 ( "Initializing A* search..." );
   _mesh->begin_marking ();
   _ptree->init ( radius );
   if ( _ent[0].type!=EntBlocked ) _ptreeaddent ( _ent[0].s, _ent[0].top, res==EdgeFound? true:false );
   if ( _ent[1].type!=EntBlocked ) _ptreeaddent ( _ent[1].s, _ent[1].top, false );
   if ( _ent[2].type!=EntBlocked ) _ptreeaddent ( _ent[2].s, _ent[2].top, false );

   if (_searchcb) _searchcb(_sudata);

   GS_TRACE1 ( "Expanding leafs..." );
   int found = ExpansionNotFinished;
   while ( found==ExpansionNotFinished )
    found = _expand_lowest_cost_leaf();

   _mesh->end_marking ();

   if ( found==ExpansionBlocked )
    { GS_TRACE1 ( "Points are not connectable!" );
      _path_result = NoPath;
      return false;
    }

   _finalsearchnode = found;
   int n = found; // the starting leaf
   s = _ptree->nodes[n].ex->sym();

   do { _channel.push() = _ptree->nodes[n].ex;
        n = _ptree->nodes[n].parent; 
      } while ( n!=-1 );
   _channel.revert();

   GS_TRACE1 ( "Path crosses "<<_channel.size()<<" edges." );

   _path_result = GlobalPath;
   return true;
 }

void SeLct::get_search_nodes ( GsArray<SeBase*>& e )
 {
   e.size ( 0 );
   for ( int i=0; i<_ptree->nodes.size(); i++ )
    { e.push() = _ptree->nodes[i].en;
      e.push() = _ptree->nodes[i].ex;
    }
 }

int SeLct::get_search_nodes () const
 {
   return _ptree? _ptree->nodes.size() : 0;
 }

void SeLct::get_search_metric ( GsArray<GsPnt2>& pnts )
 {
   pnts.size ( 0 );

   if ( !_ptree ) return;
   for ( int i=0; i<_ptree->nodes.size(); i++ )
    { 
      PathNode& n = _ptree->nodes[i];
      if ( n.parent<0 )
       { pnts.push().set ( _xi, _yi ); }
      else
       { PathNode& np = _ptree->nodes[n.parent];
         pnts.push().set ( np.x, np.y );
       }
      pnts.push().set ( n.x, n.y );
    }
 }

void SeLct::get_search_front ( GsArray<SeBase*>& e )
 {
   e.size ( 0 );
   for ( int i=0; i<_ptree->leafs.size(); i++ )
    { e.push() = _ptree->nodes[ _ptree->leafs.elem(i) ].ex;
    }
 }

//============================ End of File =================================

