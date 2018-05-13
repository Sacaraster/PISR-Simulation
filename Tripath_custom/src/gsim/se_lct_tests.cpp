/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <math.h>
# include <stdlib.h>

# include <gsim/gs_geo2.h>
# include <gsim/gs_heap.h>
# include <gsim/gs_deque.h>

# include <gsim/se_lct.h>
# include "se_triangulator_internal.h"

# define GS_TRACE_ONLY_LINES
//# define GS_USE_TRACE1 // entrance analysis
//# define GS_USE_TRACE2 // entrance analysis details
//# define GS_USE_TRACE3 // analyze local path
# include <gsim/gs_trace.h>

# define CONSTR(s) s->edg()->is_constrained()

//================================================================================
//============================== sector tests ====================================
//================================================================================

// This is called only when there is a "transitional traversal" right after/before a departure/arrival.
// Arrival: e/s are the ent/exit edges in the triangle before the arrival triangle which is s->sym()->fac()
// Departure: e/s are the exit/ent edges in the triangle after the departure triangle which is s->sym()->fac()
bool SeLct::_local_transition_free ( SeDcdtSymEdge* e, SeDcdtSymEdge* s, double d2, double px, double py )
 {
   SeDcdtSymEdge * sn = s->nxt();
   SeDcdtSymEdge * sp = sn->nxt();
   const GsPnt2& a = s->vtx()->p;
   const GsPnt2& b = sn->vtx()->p;
   const GsPnt2& c = sp->vtx()->p;
   const GsPnt2& d = s->sym()->nxn()->vtx()->p;

   if ( angle(a-c,b-c)<=gspidiv2 ) return true; // if ang <= pi/2 c cannot create disturbances

   SeDcdtSymEdge * ssn = s->sym()->nxt();
   SeDcdtSymEdge * ssp = ssn->nxt();

   if ( sn==e )
    { if ( !CONSTR(sn) && ccw(c,d,b)>0 )
       { if ( !_sector_clear ( ssp, d2, c, d, b ) )
          { return gs_ccw(c.x,c.y,_secblockpt.x,_secblockpt.y,px,py)>0? true:false;
          }
       }
    }
   else
    { if ( !CONSTR(sp) && ccw(c,a,d)>0 )
       { if ( !_sector_clear ( ssn, d2, c, a, d ) )
          { return gs_ccw(c.x,c.y,_secblockpt.x,_secblockpt.y,px,py)>0? false:true;
          }
       }
    }

   return true;
 }

// This is the function called for computing the maximum clearance during precomputation.
// It is called by SeLct::compute_clearance ()
double SeLct::_sector_clearance ( SeDcdtSymEdge* s, double maxr2, const GsVec2& c, const GsVec2& p1, const GsVec2& p2 )
 {
   double t, d2, e2;

   // check if s is distant enough from c:
   d2 = gs_point_segment_dist2(c.x,c.y,p1.x,p1.y,p2.x,p2.y,t);
   if ( d2+_epsilon>maxr2 ) return maxr2; // ok enought space, return maximum possible clearance
   // Note: the epsilon addition has not been tested if it is indeed needed, but it is
   // used here to make sure cases of exact maxr2 are always detected (note that maxr2=min{edg1len,edg2len})

   // if not distant enough, test if we're trying to cross an obstacle:
   if ( s->edg()->is_constrained() )
    { return d2; } // pt-seg blockage found, so maximum clearance value found

   //recurse:
   d2 = -1;
   s=s->sym()->nxt();
   const GsVec2& p3 = s->nvtx()->p;
   if ( gs_ccw(c.x,c.y,p3.x,p3.y,p2.x,p2.y)>0 ) // c-p3-p2
    { 
      d2 = _sector_clearance(s->nxt(),maxr2,c,p3,p2);
    }
   if ( gs_ccw(c.x,c.y,p1.x,p1.y,p3.x,p3.y)>0 ) // c-p1-p3
    { 
      e2 = _sector_clearance(s,maxr2,c,p1,p3);
      d2 = d2<0? e2 : GS_MIN(d2,e2);
    }

   return d2;
 }

// The main difference between _sector_clear() and SeTriangulator::sector_free() is that here if a "disturbance"
// is too close to the endpoints, by epsilon, it is not considered a real collision.
// Optional parameter c2 is non-null only when called from SeLct::_needs_refinement(), so that disturbances
// will be searched also when c2 is too close to a constraint.
// When precomputation is not used, this is the method called by _canpass() during channel search
bool SeLct::_sector_clear ( SeDcdtSymEdge* s, double r2, const GsVec2& c, const GsVec2& p1, const GsVec2& p2, const GsVec2* c2 )
 {
   double t, d2;

   // check if s is distant enought from c:
   d2 = gs_point_segment_dist2(c.x,c.y,p1.x,p1.y,p2.x,p2.y,t);
   if ( d2>=r2 ) // ok enought space
    { if ( c2==0 )
       { return true;
       }
      else // check distance to the "2nd center"
       { double c2t, c2d2;
         c2d2 = gs_point_segment_dist2(c2->x,c2->y,p1.x,p1.y,p2.x,p2.y,c2t);
         if ( c2d2>=r2 ) return true; // ok enought space
         if ( s->edg()->is_constrained() )
          { if ( c2t<=0 || c2t>=1 ) return true; // closest point is an end point
            _secblockd2 = d2; // clearance is not affected
            _secblockse = s; // but s may need to be subdivided
            return false; // s too close to c2: requires disturbance checking
          }
       }
    }

   // if not, test if we're trying to cross an obstacle:
   if ( s->edg()->is_constrained() )
    { 
      if ( t<=0 || t>=1 ) return true; // closest point is an end point
      GsVec2 p = p1*float(1.0-t)+p2*float(t);
      if ( gs_dist2(p.x,p.y,p1.x,p1.y)<=_epsilon2 ) return true; // closest point too close to an end point
      if ( gs_dist2(p.x,p.y,p2.x,p2.y)<=_epsilon2 ) return true; // closest point too close to an end point
      _secblockd2 = d2;
      _secblockse = s;
      _secblockpt = p;
      return false; // pt-seg blockage found: not passable
    }

   //recurse:
   if ( s->edg()->border() ) return true;
   s=s->sym()->nxt();
   const GsVec2& p3 = s->nvtx()->p;
   if ( gs_ccw(c.x,c.y,p3.x,p3.y,p2.x,p2.y)>0 ) // c-p3-p2
    { 
      if ( !_sector_clear(s->nxt(),r2,c,p3,p2) ) return false;
    }
   if ( gs_ccw(c.x,c.y,p1.x,p1.y,p3.x,p3.y)>0 ) // c-p1-p3
    { 
      if ( !_sector_clear(s,r2,c,p1,p3) ) return false;
    }

   return true;
 }

// This method is the one called for clearance tests during the entrance tests (arrival or departure).
// The method has worked well in all tests performed so far, but it can probably be better optimized
// and understood.
// Note: there is no particular reason for the parameters in double - they could be converted to GsVecs
bool SeLct::_entsector_clear ( const Sector& sc, SeBase* s, 
                               double x1, double y1, double x2, double y2, SeVertex* v )
 {
   double x3, y3, t;

   // entrance vertices are not to be confused with disturbances:
   if ( v!=s->vtx() && v!=s->nxt()->vtx() )
    {
      // check if the current edge is "far enought":
      if ( gs_point_segment_dist2(sc.cx,sc.cy,x1,y1,x2,y2,t)>=sc.r2 ) return true; // passable

      // test if the sector is trying to cross an obstacle:
      if ( _man->is_constrained(s->edg()) ) 
       { 
         GS_CLIP(t,0.0,1.0);
         x3 = x1*(1.0-t)+x2*t;
         y3 = y1*(1.0-t)+y2*t;
         bool insideb = gs_ccw(sc.cx,sc.cy,x3,y3,sc.bx,sc.by)>0? true:false; //c-p3-b
         bool insidea = gs_ccw(sc.cx,sc.cy,sc.ax,sc.ay,x3,y3)>0? true:false; //c-a-p3
         if ( insideb && insidea ) return false; // closest point is inside sector, thus not passable
         // no need so far for testing endpoint cases here, not even entrance vertex endpoints.
   
         if ( !insideb )
          { if ( !gs_segment_line_intersect ( x1,y1,x2,y2,sc.cx,sc.cy,sc.bx,sc.by,x3,y3 ) ) return true; // protection case
            if ( gs_dist2(sc.bx,sc.by,x3,y3)<=_epsilon2 ) return true; // incident point not inside
          }
         else // if ( !insidea )
          { if ( !gs_segment_line_intersect ( x1,y1,x2,y2,sc.cx,sc.cy,sc.ax,sc.ay,x3,y3 ) ) return true; // protection case
            if ( gs_dist2(sc.ax,sc.ay,x3,y3)<=_epsilon2 ) return true; // incident point not inside
          }
      
         if ( gs_dist2(sc.cx,sc.cy,x3,y3)<sc.r2 ) return false; // not passable

         return true; // passable in respect to sector
       }
    }

   //recurse:
   s=s->ret();
   _man->get_vertex_coordinates ( s->nvtx(), x3, y3 );

   if ( gs_ccw(sc.cx,sc.cy,x3,y3,sc.bx,sc.by)>0 ) // c-p3-b
    { 
      if ( !_entsector_clear(sc,s->nxt(),x3,y3,x2,y2,v) ) return false;
    }
   if ( gs_ccw(sc.cx,sc.cy,sc.ax,sc.ay,x3,y3)>0 ) // c-a-p3
    { 
      if ( !_entsector_clear(sc,s,x1,y1,x3,y3,v) ) return false;
    }

   return true;
 }

bool SeLct::_entsector_clear ( SeBase* s, double r2, double cx, double cy,
                               double ax, double ay, double bx, double by, SeVertex* v )
 {
   GS_TRACE5 ( "Sector Free: "<<GsPnt2(cx,cy)<<": "<<GsPnt2(ax,ay)<<", "<<GsPnt2(bx,by) );
   Sector sc;
   sc.r2 = r2;
   sc.cx = cx;
   sc.cy = cy;
   sc.ax = ax;
   sc.ay = ay;
   sc.bx = bx;
   sc.by = by;
   _secblockse = 0;
   double x1, y1, x2, y2;
   _man->get_vertex_coordinates ( s->vtx(), x1, y1 );
   _man->get_vertex_coordinates ( s->nvtx(), x2, y2 );
   bool b = _entsector_clear ( sc, s, x1, y1, x2, y2, v );
   GS_TRACE5 ( "Ok." );
   return b;
 }

//================================================================================
//============================ entrance analysis =================================
//================================================================================

static void _fp_add_point ( GsArray<GsPnt2>& fp, GsArray<SeVertex*>& fv, int fst, SeVertex* v, double x, double y )
 {
   // check if x,y is to be inserted:
   int i, s=fp.size()-1; // (note that when we are adding to the entrance main side, the other side is not yet processed)
   for ( i=fst; i<s; i++ )
    { if ( gs_ccw( fp[i].x,fp[i].y, fp[i+1].x,fp[i+1].y, x, y )>0 ) break;
    }
   if ( i==s ) return; // x,y does not change the funnel side being constructed

   // insert x,y and check if it invalidates remaining points:
   fv.insert(i) = v;
   fp.insert(++i).set(x,y);
   while ( i<fp.size()-2 && gs_ccw(x,y,fp[i+1].x,fp[i+1].y,fp[i+2].x,fp[i+2].y)>0 )
    { fv.remove(i);
      fp.remove(i+1);
    }
 }

// s is the edge being tested, x1,y1-x2,y2 its coordinates
// v is the vertex being tested, which will be x1,y1 or x2,y2
void SeLct::_collect_entrance_disturbances (
            int i, int fst, bool top, SeBase* s, SeVertex* v, 
            double x1, double y1, double x2, double y2, const Capsule& c )
 {
   // get point to test:
   double x3, y3;
   if ( s->vtx()==v ) { x3=x1; y3=y1; } else { x3=x2; y3=y2; }

   // test if point is a valid disturbance:
   GsArray<GsPnt2>& fp = _ent[i].fp;
   if ( !_mesh->marked(v) && 
        gs_ccw(c.bx,c.by,fp[fst].x,fp[fst].y,x3,y3)>0 &&
        gs_ccw(x3,y3,fp.top().x,fp.top().y,c.ax,c.ay)>0 &&
        gs_point_segment_dist2(x3,y3,c.ax,c.ay,c.bx,c.by)<c.r2 )
    {
      if ( !top )
       { if ( gs_ccw(c.bx,c.by,x3,y3,fp.top().x,fp.top().y)>0 ) // bottom entrance test
          _fp_add_point ( fp, _ent[i].fv, fst, v, x3, y3 ); // insert point
       }
      else
       { if ( gs_ccw(x3,y3,c.ax,c.ay,fp[fst].x,fp[fst].y)>0 ) // top entrance test
          _fp_add_point ( fp, _ent[i].fv, fst, v, x3, y3 ); // insert point
       }
    }

   // mark point as already tested:
   _mesh->mark(v);
   if ( i==3 ) _elembuffer.push().set(v,0);

   // if current edge is constrained, do not further recurse:
   if ( _man->is_constrained(s->edg()) ) return;

   // recurse if needed:
   SeBase* sn = s->ret();
   SeBase* sp = sn->nxt();
   _man->get_vertex_coordinates ( sp->vtx(), x3, y3 );

   if ( !_mesh->marked(sn->edg()) )
    { _mesh->mark(sn->edg());
      if ( i==3 ) _elembuffer.push().set(0,sn);
      if ( gs_segment_segment_dist2(x1,y1,x3,y3,c.ax,c.ay,c.bx,c.by)<c.r2 )
        _collect_entrance_disturbances ( i, fst, top, sn, sp->vtx(), x1, y1, x3, y3, c );
    }

   if ( !_mesh->marked(sp->edg()) )
    { _mesh->mark(sp->edg());
      if ( i==3 ) _elembuffer.push().set(0,sp);
      if ( gs_segment_segment_dist2(x3,y3,x2,y2,c.ax,c.ay,c.bx,c.by)<c.r2 )
        _collect_entrance_disturbances ( i, fst, top, sp, sp->vtx(), x3, y3, x2, y2, c );
    }
 }

bool SeLct::_analyze_entrance_side ( SeBase* s, int i, bool main, bool top, double cx, double cy, double r )
 {
   // get vertices:
   double x1, y1, x2, y2, x3, y3;
   SeBase* sn = s->nxt();
   SeBase* sp = sn->nxt();
   _man->get_vertex_coordinates ( s->vtx(), x1, y1 );
   _man->get_vertex_coordinates ( sn->vtx(), x2, y2 );
   _man->get_vertex_coordinates ( sp->vtx(), x3, y3 );

   GS_TRACE2 ( "AnaEnt " << i << ": " << (main?"main call":"secondary call") );

   // initialize funnel side:
   GsArray<GsPnt2>& fp = _ent[i].fp;
   GsArray<SeVertex*>& fv = _ent[i].fv;
   int fst = _ent[i].fps1;
   if ( main )
    { fv.size(0); fp.size(2); }
   else
    { fp.size ( fst+2 ); }

   // select top or bottom entrance corner:
   #define SetCapsule(c,w,s,t,u,v) Capsule c; c.r2=w; c.ax=s; c.ay=t; c.bx=u; c.by=v
   SeBase* side1;
   SeBase* side2;
   SeVertex* entv;
   double x, y, tx, ty;

   if ( top==false ) // bottom entrance
    { side1 = sp->sym();
      side2 = sn->sym();
      entv = s->vtx();
      if ( main ) _ent[i].top = top;
      gs_tangent ( x1, y1, cx, cy, r, tx, ty, x, y ); // ((x1,y1),c,t) has CCW orientation
      x = x1+(cx-tx); y = y1+(cy-ty); // capsule end-point at entrance edge
      fp[fst].set ( tx, ty );
      fp[fst+1].set ( x1, y1 );
      if ( i<3 ) _mesh->begin_marking(); else _elembuffer.push().set(0,sp);
      _mesh->mark(sp->edg());
      GS_TRACE2 ( "AnaEnt: getting bottom" );
      SetCapsule ( c, r*r, x, y, cx, cy );
      _collect_entrance_disturbances ( i, fst, top, sp, sp->vtx(), x3, y3, x1, y1, c ); // main side
      _collect_entrance_disturbances ( i, fst, top, sn, sp->vtx(), x2, y2, x3, y3, c ); // extra
      if ( i<3 ) _mesh->end_marking();
    }
   else
    { top = true; // top entrance
      side1 = sn->sym();
      side2 = sp->sym();
      entv = sn->vtx();
      if ( main ) _ent[i].top = top;
      gs_tangent ( x2, y2, cx, cy, r, x, y, tx, ty ); // ((x2,y2),c,t2) has CW orientation
      x = x2+(cx-tx); y = y2+(cy-ty); // capsule end-point at entrance edge
      fp[fst].set ( x2, y2 );
      fp[fst+1].set ( tx, ty );
      if ( i<3 ) _mesh->begin_marking(); else _elembuffer.push().set(0,sn);
      _mesh->mark(sn->edg());
      GS_TRACE2 ( "AnaEnt: getting top" );
      SetCapsule ( c, r*r, cx, cy, x, y );
      _collect_entrance_disturbances ( i, fst, top, sn, sp->vtx(), x2, y2, x3, y3, c ); // main side
      _collect_entrance_disturbances ( i, fst, top, sp, sp->vtx(), x3, y3, x1, y1, c ); // extra
      if ( i<3 ) _mesh->end_marking();
      fv.revert(fst,fv.size()-1);
      fp.revert(fst,fp.size()-1);
    }
   #undef SetCapsule

   if ( main ) 
    { _ent[i].fps1=fp.size(); fv.push(); fv.push(); }
   else
    { _ent[i].fps2=fp.size()-fst; }

   // even if there are no disturbances we still need to check if it is passable:
   SeVertex* v=0;
   SeBase* side;
   double d = 2.0*r;
   double d2 = d*d;
   GsVec2 v1, v2, c;
   int k, max=fp.size()-2;
   if ( top )
    { for ( k=fst; k<=max; k++ )
       { GS_TRACE2 ( "AnaEnt: testing top k="<<k );
         c = fp[k+1];
         v1 = (fp[k]-c).ortho();
         if ( k==max )
          { if ( gs_ccw(x1,y1,c.x+v1.x,c.y+v1.y,x2,y2)>0 ) break; // last test not needed
            v2.set ( x1-x2, y1-y2 );
            v = entv;
          } else v2 = (c-fp[k+2]).ortho();
         side = _man->ccw(c.x,c.y,side2->vtx(),side2->nxt()->vtx())? side2:side1;
         if ( !_entsector_clear(side, d2, c.x, c.y, c.x+v1.x, c.y+v1.y, c.x+v2.x, c.y+v2.y, v?v:fv[k]) )
          { _ent[i].type = EntBlocked;
            GS_TRACE2 ( "Top entrance "<<i<<" blocked at k="<<k );
            return false;
          }
       }
    }
   else
    { for ( k=fst; k<=max; k++ )
       { GS_TRACE2 ( "AnaEnt: testing bottom k="<<k );
         c = fp[k+1];
         v1 = (c-fp[k]).ortho();
         if ( k==max )
          { if ( gs_ccw(x1,y1,c.x+v1.x,c.y+v1.y,x2,y2)>0 ) break; // last test not needed
            v2.set ( x2-x1, y2-y1 );
            v = entv;
          } else { v2 = (fp[k+2]-c).ortho(); }
         side = _man->ccw(c.x,c.y,side2->vtx(),side2->nxt()->vtx())? side2:side1;
         if ( !_entsector_clear(side, d2, c.x, c.y, c.x+v2.x, c.y+v2.y, c.x+v1.x, c.y+v1.y, v?v:fv[k]) )
          { _ent[i].type = EntBlocked;
            GS_TRACE2 ( "Bottom entrance "<<i<<" blocked at k="<<k );
            return false;
          }
       }
    }
   GS_TRACE2 ( "AnaEnt: returning true." );
   return true;
 }

void SeLct::_analyze_entrance ( SeBase* s, int i, double cx, double cy, double r )
 {
   _ent[i].s = (SeDcdtSymEdge*)s;
   _ent[i].fps1 = 0;
   _ent[i].fps2 = 0;
   if ( _man->is_constrained(s->edg()) ) { _ent[i].type=EntBlocked; return; }

   // test edge lenght:
   const GsPnt2& p1 = ((SeDcdtSymEdge*)s)->vtx()->p;
   const GsPnt2& p2 = ((SeDcdtSymEdge*)s)->nvtx()->p;
   if ( dist2(p1,p2)<4*r*r ) { _ent[i].type=EntBlocked; return; }

   // select closest entrance corner to be the entrance type:
   bool top = gs_dist2(cx,cy,p1.x,p1.y)<gs_dist2(cx,cy,p2.x,p2.y)? false:true;

   // check if passable by the closest corner:
   if ( !_analyze_entrance_side ( s, i, true/*main side*/, top, cx, cy, r ) ) return;

   // now check if also passable in respect to possible disturbances on the other side:
   if ( !_analyze_entrance_side ( s, i, false/*main side*/, !top, cx, cy, r ) ) return;

   // ok, entrance is not blocked:
   _ent[i].type = _ent[i].fp.size()<=4? EntTrivial:EntNotTrivial;
 }

void SeLct::_analyze_entrances ( SeBase* s, double cx, double cy, double r )
 {
   GS_TRACE1 ( "Analyzing Entrances:" );

   _analyze_entrance(s,0,cx,cy,r);
   GS_TRACE1 ( "Entrance 0 "<<(_ent[0].type==EntBlocked?"blocked":_ent[0].type==EntTrivial?"trivial":"not trivial") );
   GS_TRACE1 ( "Entrance 0 disturbances: "<<_ent[0].fv.size() );

   s=s->nxt(); _analyze_entrance(s,1,cx,cy,r);
   GS_TRACE1 ( "Entrance 1 "<<(_ent[1].type==EntBlocked?"blocked":_ent[1].type==EntTrivial?"trivial":"not trivial") );
   GS_TRACE1 ( "Entrance 1 disturbances: "<<_ent[1].fv.size() );

   s=s->nxt(); _analyze_entrance(s,2,cx,cy,r);
   GS_TRACE1 ( "Entrance 2 "<<(_ent[2].type==EntBlocked?"blocked":_ent[2].type==EntTrivial?"trivial":"not trivial") );
   GS_TRACE1 ( "Entrance 2 disturbances: "<<_ent[2].fv.size() );
 }

// SeLct::_expand_lowest_cost_leaf() only calls _analyze_arrival() for i==3 (but not SeLcto::_osearch_init())
// ne and ns is the entrance/exit pair on the triangle before the arrival triangle, such that nex->sym()==s
// s is at the arrival edge and at the arrival triangle.
void SeLct::_analyze_arrival ( SeBase* s, int i, double r, float d2, SeDcdtSymEdge* ne, SeDcdtSymEdge* ns )
 {
   _elembuffer.size(0); // will store all marked vertices and edges during arrival analysis (for i==3)

   _analyze_entrance ( s, i, _xg, _yg, r );

   // when i==3 we will need to unmark vertices and edges marked in the entrance 
   // analysis in order to leave the marking consistent:
   while ( _elembuffer.size()>0 )
    { ConstrElem& c = _elembuffer.pop();
      mesh()->unmark ( c.v? c.v : c.e->edg() );
    }

   if ( _ent[i].type==EntBlocked ) return; // entrance blocked

   // if not blocked, check transition to the final triangle:
   if ( ne ) { if ( !_local_transition_free(ne,ns,d2,_xg,_yg) ) _ent[i].type=EntBlocked; }
 }

//================================================================================
//========================= analyze local path ===================================
//================================================================================

// Parameter s is the symedge with source vertex (x1,y1) and destination vertex (x2,y2)
void SeLct::_collect_capsule_disturbances
            ( SeBase* s, double x1, double y1, double x2, double y2, const Capsule& c, const Sector& sec )
 {
   // Note that: 1) the initial and goal discs are known to be valid, and
   // 2) their centers are on a same triangle so that no edge separates them.
   // Therefore if a constrained segment intersects the capsule, one of its
   // vertices has to be inside the capsule and so point-seg tests are enought
   // and a seg-seg test below is not needed:
   // if ( gs_segment_segment_dist2(c1x,c1y,c2x,c2y,x1,y1,x2,y2)>=r2 ) return; // ok enought space

   _mesh->mark ( s->edg() );

   // check if there are disturbing vertices:
   double t;
   bool vfound=false;
   SeVertex* v1 = s->vtx();
   SeVertex* v2 = s->nxt()->vtx();

   if ( !_mesh->marked(v1) )
    { _mesh->mark(v1);
      if ( gs_point_segment_dist2 ( x1, y1, c.ax, c.ay, c.bx, c.by, t )<c.r2 )
       { _ent[1].fv.push()=v1; _ent[1].fp.push().set(x1,y1); vfound=true; }
    }

   if ( !_mesh->marked(v2) )
    { _mesh->mark(v2); 
      if ( gs_point_segment_dist2 ( x2, y2, c.ax, c.ay, c.bx, c.by, t )<c.r2 )
       { _ent[1].fv.push()=v2; _ent[1].fp.push().set(x2,y2); vfound=true; }
    }

   // check if current edge is "far away enought" to stop recursion:
   if ( !vfound )
    { if ( gs_point_segment_dist2 ( c.ax, c.ay, x1, y1, x2, y2, t )>=c.r2 &&
           gs_point_segment_dist2 ( c.bx, c.by, x1, y1, x2, y2, t )>=c.r2 )
      return; // stop recursion
    }

   // test if we are trying to cross an obstacle:
   if ( _man->is_constrained(s->edg()) ) return;

   // recurse remaining in the test sector:
   SeBase* sn = s->ret();
   SeBase* sp = sn->nxt();
   double x3, y3;
   _man->get_vertex_coordinates ( sp->vtx(), x3, y3 );
   if ( !_mesh->marked(sp->edg()) )
    if ( gs_ccw(sec.cx,sec.cy,x3,y3,sec.bx,sec.by)>0 ) // c-p3-b
     { _collect_capsule_disturbances ( sp, x3, y3, x2, y2, c, sec );
     }

   if ( !_mesh->marked(sn->edg()) )
    if ( gs_ccw(sec.cx,sec.cy,sec.ax,sec.ay,x3,y3)>0 ) // c-a-p3
     { _collect_capsule_disturbances ( sn, x1, y1, x3, y3, c, sec );
     }
 }

// s is a valid entrance edge: not constrained and lenght>=2r
SeLct::PathResult SeLct::_analyze_local_path ( SeBase* s, double r )
 {
   // get vertices:
   double x1, y1, x2, y2, x3, y3;
   SeBase* sn = s->nxt();
   SeBase* sp = sn->nxt();
   _man->get_vertex_coordinates ( s->vtx(), x1, y1 );
   _man->get_vertex_coordinates ( sn->vtx(), x2, y2 );
   _man->get_vertex_coordinates ( sp->vtx(), x3, y3 );

   // collect disturbances:
   GsArray<SeVertex*>& fv1 = _ent[1].fv;
   GsArray<GsPnt2>& fp1 = _ent[1].fp;
   fv1.size(0);
   fp1.size(0);
   double r2 = r*r;
   double d2 = 4.0*r2;
   #define SetSector(c,x,y,s,t,u,v) c.cx=x; c.cy=y; c.ax=s; c.ay=t; c.bx=u; c.by=v
   #define SetCapsule(c,w,s,t,u,v)  c.r2=w; c.ax=s; c.ay=t; c.bx=u; c.by=v
   Capsule cap;
   SetCapsule(cap,r2,_xi,_yi,_xg,_yg);
   _mesh->begin_marking ();
   Sector sec;
   SetSector(sec,x3,y3,x1,y1,x2,y2); _collect_capsule_disturbances ( s,  x1,y1,x2,y2, cap,sec );
   SetSector(sec,x1,y1,x2,y2,x3,y3); _collect_capsule_disturbances ( sn, x2,y2,x3,y3, cap,sec );
   SetSector(sec,x2,y2,x3,y3,x1,y1); _collect_capsule_disturbances ( sp, x3,y3,x1,y1, cap,sec );
   _mesh->end_marking ();
   #undef SetCapsule

   // check if there are disturbances:
   GS_TRACE3 ( "Disturbances found: "<<fv1.size() );
   if ( fv1.empty() ) return TrivialPath;

   // now we need to correct disturbances and check if correction is passable.
   // the key point here is that disturbances to the capsule
   // can only occur on one side of the capsule, the other side being
   // protected by the Delaunay circle in respect to constrained vertices.
   // so disturbances can only occur on one side, and passable tests are
   // only needed on the other side. check side of one vertex:
   bool top = gs_ccw(fp1[0].x,fp1[0].y,_xi,_yi,_xg,_yg)>0? true:false;
   _ent[0].top = top;
   GS_TRACE3 ( "Disturbances side: "<< (top?"top":"bottom") );
   // all disturbances have to be on the same side, to debug this:
   /*for ( int i=0; i<fv1.size(); i++ )
     { bool itop = gs_ccw(fp1[i].x,fp1[i].y,_xi,_yi,_xg,_yg)>0? true:false;
       if ( itop!=top ) gsout.fatal("ERROR IN CAPSULE CORRECTION SIDE!!!");
     }*/

   // sort only needed disturbances in _ent[0]:
   GsArray<GsPnt2>& fp = _ent[0].fp;
   GsArray<SeVertex*>& fv = _ent[0].fv;
   fv.size(0);
   fp.size(2);
   int i;
   double x, y;
   x=-(_yg-_yi); y=_xg-_xi; gs_len(x,y,r);
   if ( top )
    { fp[0].set ( _xg+x, _yg+y );
      fp[1].set ( _xi+x, _yi+y );
      for ( i=0; i<fp1.size(); i++ )
       { _fp_add_point ( fp, fv, 0, fv1[i], fp1[i].x, fp1[i].y ); }
    }
   else
    { fp[0].set ( _xi-x, _yi-y );
      fp[1].set ( _xg-x, _yg-y );
      for ( i=0; i<fp1.size(); i++ )
       { _fp_add_point ( fp, fv, 0, fv1[i], fp1[i].x, fp1[i].y ); }
    }

   GS_TRACE3 ( "Disturbances to be corrected: "<<fv.size() );

   // now test if it is passable:
   # define TTEST(s) if ( !_entsector_clear(s, d2, c.x, c.y, c.x+v1.x, c.y+v1.y, c.x+v2.x, c.y+v2.y, fv[k] ) ) return GlobalPath;
   # define BTEST(s) if ( !_entsector_clear(s, d2, c.x, c.y, c.x+v2.x, c.y+v2.y, c.x+v1.x, c.y+v1.y, fv[k] ) ) return GlobalPath;

   double d = 2.0*r;
   GsVec2 v1, v2, c;
   int k, max=fp.size()-2;
   if ( top )
    { fv.revert();
      fp.revert();
      for ( k=0; k<max; k++ )
       { c = fp[k+1];
         v1 = (fp[k]-c).ortho();
         v2 = (c-fp[k+2]).ortho();
         if ( gs_ccw(c.x,c.y,x1,y1,x2,y2)>0 ) { TTEST(s->sym()); } else { TTEST(s); }
         if ( gs_ccw(c.x,c.y,x2,y2,x3,y3)>0 ) { TTEST(sn->sym()); } else { TTEST(sn); }
         if ( gs_ccw(c.x,c.y,x3,y3,x1,y1)>0 ) { TTEST(sp->sym()); } else { TTEST(sp); }
       }
    }
   else
    { for ( k=0; k<max; k++ )
       { c = fp[k+1];
         v1 = (c-fp[k]).ortho();
         v2 = (fp[k+2]-c).ortho();
         if ( gs_ccw(c.x,c.y,x1,y1,x2,y2)>0 ) { BTEST(s); } else { BTEST(s->sym()); }
         if ( gs_ccw(c.x,c.y,x2,y2,x3,y3)>0 ) { BTEST(sn); } else { BTEST(sn->sym()); }
         if ( gs_ccw(c.x,c.y,x3,y3,x1,y1)>0 ) { BTEST(sp); } else { BTEST(sp->sym()); }
       }
    }

   // ok, not trivial but passable:
   return LocalPath;
 }

//============================ End of File =================================
