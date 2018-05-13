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

# include <gsim/se_triangulator.h>
# include "se_triangulator_internal.h"

//# define GS_USE_TRACE1 // search path
//# define GS_USE_TRACE2 // funnel
//# define GS_USE_TRACE3 // channel 
//# define GS_USE_TRACE4 // expand leaf
//# define GS_USE_TRACE5 // sector free
# include <gsim/gs_trace.h>

//=========================== debugging tools ====================================

# define PNL printf("\n")

# define PRINTP(x,y) printf("(%+9.7f,%+9.7f) ", x, y)

# define PRINTV(v)  { double vx, vy; \
                      _man->get_vertex_coordinates ( v, vx, vy ); \
                      PRINTP(vx,vy); }

//================================================================================
//=========================== ray intersection ===================================
//================================================================================

bool SeTriangulator::ray_intersection ( SeBase* s, double x1, double y1, double x2, double y2,
                                        int intersections, GsArray<SeBase*>* constraints,
                                        GsArray<GsPnt2>* pts, GsArray<SeBase*>* edges )
 {
   if ( constraints ) constraints->size(0);
   if ( edges ) edges->size(0);
   if ( pts ) pts->size(0);

   # define INTER(a,b,c,d) gs_segments_intersect(x1,y1,x2,y2,a,b,c,d)

   int i=0;
   while ( !_man->segments_intersect(x1,y1,x2,y2,s->vtx(),s->nxt()->vtx()) )
    { s=s->nxt();
      if ( ++i==3 ) // no intersections, ie, seg in triangle
       { GS_TRACE4 ( "No Intersections!" );
         return true;
       }
    }

   // Continue until end:
   bool ret = false;
   SeBase *s1, *s2;
   double ax, ay, bx, by, cx, cy;

   while ( true )
    {
      if ( edges ) edges->push()=s;

      s1=s->ret(); s2=s1->nxt();
      _man->get_vertex_coordinates ( s->vtx(), ax, ay );
      _man->get_vertex_coordinates ( s2->vtx(), bx, by );
      _man->get_vertex_coordinates ( s2->nxt()->vtx(), cx, cy );

      if ( _man->is_constrained(s->edg()) )
       { GS_TRACE4 ( "ray crossed a constraint" );
         ret = true;
         if ( constraints ) constraints->push() = s;
         if ( pts )
          { double x, y;
            gs_segments_intersect ( x1, y1, x2, y2, ax, ay, cx, cy, x, y );
            pts->push().set ( float(x), float(y) );
          }
         if ( --intersections<=0 ) { GS_TRACE4 ( "depth reached!" ); return ret; }
       }

      if ( INTER(ax,ay,bx,by) )
       { GS_TRACE4 ( "ret() intersection..." );
         s=s1;
       }
      else if ( INTER(bx,by,cx,cy) )
       { GS_TRACE4 ( "ret()->nxt() intersection..." );
         s=s2;
       }
      else
       { GS_TRACE4 ( "Ray test ended!" );
         return ret;
       }
    }

   # undef INTERX
   # undef INTER
 }

//================================================================================
//============================ sector free =======================================
//================================================================================

//this is the original and simplest sector free algorithm, without any tests for special cases
bool SeTriangulator::sector_free ( SeBase* s, double r2, double cx, double cy,
                                   double x1, double y1, double x2, double y2 )
 {
   double t, d2, x3, y3;

   // check if s is distant enought from c:
   d2 = gs_point_segment_dist2(cx,cy,x1,y1,x2,y2,t);
   if ( d2>=r2 ) return true; // ok enought space

   // if not, test if we're trying to cross an obstacle:
   if ( _man->is_constrained(s->edg()) ) return false; // pt-seg blockage found: not passable

   // recurse:
   s=s->sym()->nxt();
   _man->get_vertex_coordinates ( s->nvtx(), x3, y3 );
   if ( gs_ccw(cx,cy,x3,y3,x2,y2)>0 ) // c-p3-p2
    { 
      if ( !sector_free(s->nxt(),r2,cx,cy,x3,y3,x2,y2) ) return false;
    }
   if ( gs_ccw(cx,cy,x1,y1,x3,y3)>0 ) // c-p1-p3
    { 
      if ( !sector_free(s,r2,cx,cy,x1,y1,x3,y3) ) return false;
    }

   return true;
 }

bool SeTriangulator::disc_free ( SeBase* s, double cx, double cy, double r )
 {
   double x1, y1, x2, y2, x3, y3;
   SeBase* sn = s->nxt();
   SeBase* sp = sn->nxt();
   _man->get_vertex_coordinates ( s->vtx(), x1, y1 );
   _man->get_vertex_coordinates ( sn->vtx(), x2, y2 );
   _man->get_vertex_coordinates ( sp->vtx(), x3, y3 );
   r = r*r; // remember that sector_free() expects the squared radius
   if ( !sector_free(s,r,cx,cy,x1,y1,x2,y2) ) return false;
   if ( !sector_free(sn,r,cx,cy,x2,y2,x3,y3) ) return false;
   if ( !sector_free(sp,r,cx,cy,x3,y3,x1,y1) ) return false;
   return true;
 }

//================================================================================
//========================== search path tree ====================================
//================================================================================

//#define MID_EDG_COST    // use this for the mid edge cost
//#define MID_TRI_COST    // use this for the mid triangle cost
//#define GD_EDG_COST   // best point at edge in the direction of goal
#define GDCLEAR_EDG_COST  // "gdir" cost with adjustment to respect r distance from vertices
void SeTriangulator::_getcostpoint ( PathNode* n, double xn, double yn, double x1, double y1, double x2, double y2, double& x, double& y, double r )
 {
   #ifdef MID_EDG_COST
   x = (x1+x2)/2; y = (y1+y2)/2; return; // only take mid point of edge
   #endif

   #ifdef MID_TRI_COST
   if (!n) { x=xn; y=yn; return; } // take initial point
   SeBase* s = n->ex->sym();
   _man->get_vertex_coordinates(s->vtx(),x1,y1);
   _man->get_vertex_coordinates(s->nxt()->vtx(),x2,y2);
   _man->get_vertex_coordinates(s->nxn()->vtx(),xn,yn);
   x=(x1+x2+xn)/3.0; y=(y1+y2+yn)/3.0;
   return;
   #endif

   #ifdef GD_EDG_COST
   if ( gs_segments_intersect ( xn,yn,_xg,_yg, x1,y1,x2,y2, x,y ) )
    { // x,y will contain the intersection point }
   else // take edge center
    { x = (x1+x2)/2; y = (y1+y2)/2; }
   #endif

   #ifdef GDCLEAR_EDG_COST
   if ( gs_segments_intersect ( xn,yn,_xg,_yg, x1,y1,x2,y2, x,y ) )
    { // x,y will contain the intersection point
      if ( r<=0 ) return;
      double d=sqrt(gs_dist2(x1,y1,x,y));
      if ( d<r ) { x=((x-x1)/d)*r; y=((y-y1)/d)*r; x+=x1; y+=y1; return; }
      d=sqrt(gs_dist2(x2,y2,x,y));
      if ( d<r ) { x=((x-x2)/d)*r; y=((y-y2)/d)*r; x+=x2; y+=y2; return; }
    }
   else // take edge center
    { x = (x1+x2)/2;
      y = (y1+y2)/2;
      if ( r<=0 ) return;
      if ( gs_dist2(x1,y1,_xg,_yg)<gs_dist2(x2,y2,_xg,_yg))
       { double d=sqrt(gs_dist2(x1,y1,x,y)); x=((x-x1)/d)*r; y=((y-y1)/d)*r; x+=x1; y+=y1; return; }
      else
       { double d=sqrt(gs_dist2(x2,y2,x,y)); x=((x-x2)/d)*r; y=((y-y2)/d)*r; x+=x2; y+=y2; return; }
    }
   #endif
 }

void SeTriangulator::_trytoadd ( SeBase* s, int mi, PathNode* n, double x1, double y1, double x2, double y2 )
 {
    // first test if we're trying to cross an obstacle:
    if ( _man->is_constrained(s->edg()) ) return;

    SeBase* sym = s->sym();

    // for meshes without a constrained border, add this security test to
    // avoid entering a backface in case the goal is outside the domain:
    // if ( sym->nxt()->nxt()->nxt()!=sym ) return;

	// test if the next triangle is already visited:
    if ( _mesh->marked(sym->fac()) ) return;

   double xn, yn, x, y; float ncost;

   if ( !n )
    { _man->get_vertex_coordinates ( s->vtx(), x1, y1 );
      _man->get_vertex_coordinates ( s->nxt()->vtx(), x2, y2 );
      xn=_xi; yn=_yi; ncost=0;
      if ( mi==-2 ) { mi=-1; x=_xi; y=_yi; }
      else _getcostpoint ( n, xn, yn, x1, y1, x2, y2, x, y );
    }
   else
    { xn=n->x; yn=n->y; ncost=n->ncost;
      _getcostpoint ( n, xn, yn, x1, y1, x2, y2, x, y );
    }

   # define PTDIST(a,b,c,d) float(sqrt(gs_dist2(a,b,c,d)))
   _ptree->add_child ( mi, 0, s, ncost+PTDIST(xn,yn,x,y), PTDIST(x,y,_xg,_yg), x,y );
   # undef PTDIST
 }

void SeTriangulator::_ptree_init ( SeTriangulator::LocateResult res, SeBase* s, float r )
 {
   _ptree->init ( r );
   int mi = res==EdgeFound? -2:-1;
   GS_TRACE1 ( "Initializing from " <<((const char*)mi==-1?"face":"edge")<<"..." );
   _mesh->mark ( s->fac() );
   _trytoadd ( s, mi ); mi=-1;
   s = s->nxt();
   _trytoadd ( s, mi );
   s = s->nxt();
   _trytoadd ( s, mi );
 }

# define ExpansionNotFinished  -1
# define ExpansionBlocked      -2

int SeTriangulator::_expand_lowest_cost_leaf ()
 {
   double x1, y1, x2, y2, x3, y3;
   int min_i;

   min_i = _ptree->lowest_cost_leaf ();
   GS_TRACE4 ( "Expanding leaf: "<<min_i );

   if ( min_i<0 ) return ExpansionBlocked; // no more leafs: indicates that path could not be found!

   // Attention: remember that array references may be invalidated due array reallocation during insertion
   SeBase* s = _ptree->nodes[min_i].ex;
   _man->get_vertex_coordinates ( s->vtx(), x1, y1 ); s = s->sym();
   _man->get_vertex_coordinates ( s->vtx(), x2, y2 );
   _man->get_vertex_coordinates ( s->pri()->vtx(), x3, y3 );

   // note that (p1,p2,p3) is not ccw, so we test (p2,p1,p3):
   if ( gs_in_triangle(x2,y2,x1,y1,x3,y3,_xg,_yg) ) // Reached goal triangle !
    { GS_TRACE4 ( "Goal triangle reached..." );
      return min_i; // FOUND!
    }

   _mesh->mark ( s->fac() );

   s = s->nxt();
   _trytoadd ( s, min_i, &_ptree->nodes[min_i], x1, y1, x3, y3 );

   s = s->nxt();
   _trytoadd ( s, min_i, &_ptree->nodes[min_i], x3, y3, x2, y2 );

   return ExpansionNotFinished; // continue the expansion
 }

//================================================================================
//============================== search path =====================================
//================================================================================

/* - This is the A* algorithm that takes O(nf), f is the faces in the "expansion frontier". */
bool SeTriangulator::search_channel ( double x1, double y1, double x2, double y2, const SeFace* iniface )
 {
   LocateResult res;
   SeBase *s;

   GS_TRACE1 ( "Starting Search Path..." );

   if ( !_ptree ) _ptree = new PathTree;
   _channel.size(0);
   _xi=x1; _yi=y1; _xg=x2; _yg=y2;

   if ( !iniface ) return _path_found=false;

   // Even if p1 is on an edge, locate_point will return in s a face that 
   // can be considered to contain p1 (p1 would be invalid if in a vertex)
   res=locate_point ( iniface, x1, y1, s );
   if ( res==NotFound )
    { GS_TRACE1 ( "Could not locate first point!" );
      return _path_found=false;
    }

   // Check if we are to solve trivial or local paths, testing if both points are in the same triangle:
   if ( _man->in_triangle(s->vtx(),s->nxt()->vtx(),s->pri()->vtx(),x2,y2) )
    { GS_TRACE1 ( "Both points are in the same triangle..." );
      GS_TRACE1 ( "Trivial path returned." );
      return _path_found=true; // this is it
    }

   GS_TRACE1 ( "Initializing A* search..." );

   _mesh->begin_marking ();
   _ptree_init ( res, s );

   GS_TRACE1 ( "Expanding leafs..." );
   int found = ExpansionNotFinished;
   while ( found==ExpansionNotFinished )
    found = _expand_lowest_cost_leaf();

   _mesh->end_marking ();

   if ( found==ExpansionBlocked )
    { GS_TRACE1 ( "Points are not connectable!" );
      return _path_found=false;
    }

   int n = found; // the starting leaf
   s = _ptree->nodes[n].ex->sym();
   do { _channel.push() = _ptree->nodes[n].ex;
        n = _ptree->nodes[n].parent; 
      } while ( n!=-1 );
   _channel.revert();

   GS_TRACE1 ( "Path crosses "<<_channel.size()<<" edges." );

   return _path_found=true;
 }

void SeTriangulator::get_channel_boundary ( GsPolygon& channel )
 {
   GS_TRACE3 ( "Get channel bry: size:"<<_channel.size() );

   channel.size(0);
   if ( _channel.empty() ) return; // path in a same triangle

   int i;
   SeVertex* v;
   SeVertex* lastv=0;

   channel.push().set ( (float)_xi, (float)_yi );

   double x, y;
   for ( i=0; i<_channel.size(); i++ )
    { v = _channel[i]->vtx();
      if ( v==lastv ) continue;
      lastv = v;
      _man->get_vertex_coordinates ( v, x, y );
      channel.push().set ( (float)x, (float)y );
    }

   channel.push().set ( (float)_xg, (float)_yg );

   for ( i=_channel.size()-1; i>=0; i-- )
    { v = _channel[i]->nxt()->vtx();
      if ( v==lastv ) continue;
      lastv = v;
      _man->get_vertex_coordinates ( v, x, y );
      channel.push().set ( (float)x, (float)y );
    }

   GS_TRACE3 ( "Done." );
   return;
 }

void SeTriangulator::get_channel_skeleton ( GsPolygon& path )
 {
   path.open ( true );
   if ( !_path_found ) { path.size(0); return; }

   int i;
   double x1, y1, x2, y2;
   path.size(0);
   path.push().set ( (float)_xi, (float)_yi );
   for ( i=0; i<_channel.size(); i++ )
    { _man->get_vertex_coordinates ( _channel[i]->vtx(), x1, y1 );
      _man->get_vertex_coordinates ( _channel[i]->nxt()->vtx(), x2, y2 );
      path.push().set ( (float)(x1+x2)/2, (float)(y1+y2)/2 );
    }
   path.push().set ( (float)_xg, (float)_yg );
 }

//================================================================================
//=========================== funnel shortest path ===============================
//================================================================================

static bool ordccw ( bool normal_order, GsPnt2 p1, GsPnt2 p2, float x, float y )
 {
   if ( normal_order )
    return GS_CCW(p1.x,p1.y,p2.x,p2.y,x,y)>=0? true:false;
   else
    return GS_CCW(p2.x,p2.y,p1.x,p1.y,x,y)>=0? true:false;
 }

void SeTriangulator::_funneladd ( bool intop, GsPolygon& path, const GsPnt2& p )
 {
   FunnelDeque& dq = *_fdeque;

   dq.topmode ( intop );

   bool opening;
   bool order = intop;
   bool newapex = false;
   if ( dq.size()<=1 ) { dq.push().set(p.x,p.y); return; }

   while(1)
    { SeFunnelPt& b = dq.get();
      SeFunnelPt& a = dq.get(1);

      // if the apex is passed, the orientation test changes
      if ( b.apex ) { order=!order; newapex=true; }
      opening = ordccw ( order, a, b, p.x, p.y );
      if ( opening ) break;

      dq.pop();
      if ( newapex ) path.push()=a;

      if ( dq.size()==1 ) break;
    }

   if ( newapex ) dq.get().apex=true;
   dq.push().set(p.x,p.y);

   GS_TRACE2 ( " psize:"<<path.size() <<" dqsize:"<<dq.size() << ((char*)intop? ", top ":", bot ") << (int)p.x << "," << (int) p.y );
 }

void SeTriangulator::make_funnel_path ( GsPolygon& path )
 {
   GS_TRACE2 ( "Entering funnel path..." );
   path.open ( true );
   path.size ( 0 );

   if ( !_path_found ) return;

   path.push().set((float)_xi,(float)_yi);

   if ( _channel.empty() )
    { path.push().set((float)_xg,(float)_yg);
      return;
    }

   if ( !_fdeque ) _fdeque = new FunnelDeque(32); else _fdeque->init();
   FunnelDeque& dq = *_fdeque;
   
   // add the first apex:
   GS_TRACE2 ( "Adding apex..." );
   dq.pusht().set((float)_xi,(float)_yi); dq.top().apex=1;

   // init the funnel:
   int i;
   double x, y;
   SeBase* s = _channel[0];
   GsPnt2 p1, p2;
   _man->get_vertex_coordinates ( s->vtx(), x, y ); p1.set((float)x,(float)y);
   _man->get_vertex_coordinates ( s->nxt()->vtx(), x, y ); p2.set((float)x,(float)y);
   dq.topmode ( 0 );
   dq.push().set(p1.x,p1.y,0); // !intop
   dq.topmode ( 1 );
   dq.push().set(p2.x,p2.y,1); // intop

   // pass the funnel:
   int max = _channel.size()-1;
   SeBase *s1, *s2;
   for ( i=0; i<max; i++ )
    { GS_TRACE2 ( "Processing channel edge "<<i );
      s1 = _channel[i];
      s2 = _channel[i+1];
      
      if ( s1->vtx()==s2->vtx() ) // upper vertex rotates
       { _man->get_vertex_coordinates ( s2->nxt()->vtx(), x, y );
         _funneladd ( 1, path, GsPnt2(x,y) ); // add top edge vertex
       }
      else
       { _man->get_vertex_coordinates ( s2->vtx(), x, y );
         _funneladd ( 0, path, GsPnt2(x,y) ); // add bottom edge vertex
       }
   }

   // To finalize:
   // 1. check where is the zone in the funnel that the goal is:
   bool order=true;
   dq.topmode ( order );
   while ( dq.size()>1 )
    { if ( dq.get().apex ) { order=!order; dq.topmode(order); }
      if ( ordccw(order,dq.get(),dq.get(1),(float)_xg,(float)_yg) ) dq.pop();
       else break;
    }

   // 2. add the needed portion of the funnel to the path:
   for ( i=0; dq.get(i).apex==0; i++ ) ; // stop at the apex
   while ( --i>=0 ) path.push() = dq.get(i); // add funnel zone to the path

   // 3. end path:
   path.push().set((float)_xg,(float)_yg);

   // To debug generation of duplicated points:
   //for ( i=1; i<path.size(); i++ ) if ( path[i-1]==path[i] ) gsout<<"=== DUP! ===\n";

   GS_TRACE2 ( "End..." );
 }

//============================ End of File =================================

