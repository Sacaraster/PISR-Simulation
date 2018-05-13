/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <math.h>
# include <stdlib.h>

# include <gsim/gs_geo2.h>
# include <gsim/se_triangulator.h>
# include "se_triangulator_internal.h"

//# define GS_USE_TRACE1 // locate point
//# define GS_USE_TRACE2 // ins delaunay point
//# define GS_USE_TRACE3 // ins line constraint
//# define GS_USE_TRACE4 // triangulate face
//# define GS_USE_TRACE6 // remove vertex
# include <gsim/gs_trace.h>

//=========================== debugging tools ====================================

# define PNL printf("\n")

# define PRINTP(x,y)  printf("(%+9.7f,%+9.7f) ", x, y)

# define PRINTV(v)  { double vx, vy; \
                      _man->get_vertex_coordinates ( v, vx, vy ); \
                      PRINTP(vx,vy); }

# define PRINTF(s)  { SeBase* si=s; double vx, vy; int i=1; \
                      do { _man->get_vertex_coordinates ( s->vtx(), vx, vy ); \
                           printf("%d: ",i); PRINTP(vx,vy); PNL; s=s->nxt(); i++; \
                         } while(s!=si); }

//================================================================================
//============================ SeTriangulator ====================================
//================================================================================

static void _setritest ( SeTriangulator* t, const char* s )
{ gsout<<"Checking after "<<s<<"...: "; gsout.flush(); gsout<<(t->checkall(false)?"Ok.":"ERROR!")<<gsnl; }

# define TEST(s) if (_debug_mode) _setritest(this,s);

SeTriangulator::SeTriangulator ( Mode mode, SeMeshBase* m,
                                 SeTriangulatorManager* man, double epsilon )
 {
   _mode = mode;
   _mesh = m;
   _mesh->ref();
   _man = man;
   _man->ref();
   _epsilon = _epsilonedg = epsilon;
   _epsilon2 = epsilon*epsilon;
   _ptree = 0;
   _path_found = false;
   _fdeque = 0;
   _debug_mode = false;
 }

SeTriangulator::~SeTriangulator ()
 {
   delete _ptree;
   delete _fdeque;
   _man->unref();
   _mesh->unref();
 }

static void _error ( bool fatal, bool  out, const char* msg, SeTriangulatorManager* _man, SeBase* s, GsArray<SeBase*>* fa )
 { 
   if ( fatal||out ) { gsout<<"ERROR: "<<msg<<gsnl; PRINTF(s); }
   if ( fatal ) gsout.fatal("se_triangulator.cpp: checkall() found an error.");
   if ( fa ) fa->push()=s;
 }

int SeTriangulator::check ( SeBase* s, bool out, bool fatal, GsArray<SeBase*>* fa )
 {
   double x1, y1, x2, y2, x3, y3, ccw;
   SeVertex *v1, *v2, *v3;
   SeBase *sn, *snn;

   sn=s->nxt(); snn=sn->nxt();
   v1=s->vtx(); v2=sn->vtx(); v3=snn->vtx(); 
   _man->get_vertex_coordinates ( v1, x1, y1 );
   _man->get_vertex_coordinates ( v2, x2, y2 );
   _man->get_vertex_coordinates ( v3, x3, y3 );
   ccw = gs_ccw(x1,y1,x2,y2,x3,y3);
   int errcount=0;

   if ( snn->nxt()!=s ) // check if this is a triangular face
    { errcount++;
      _error ( fatal, out, "non-triangular face found!", _man, s, fa );
    }
   else if ( ccw<0 ) // check if this is a CCW face
    { errcount++;
      _error ( fatal, out, "non-CCW triangle found!", _man, s, fa );
    }
   else if ( ccw/2.0<_epsilon ) // check if triangle is degenerate
    { errcount++;
      _error ( fatal, out, "degenerate (area zero) triangle found!", _man, s, fa );
    }
   else if // check if there are collinear points
       ( gs_point_segment_dist2(x1,y1,x2,y2,x3,y3)<=_epsilon2 ||
         gs_point_segment_dist2(x2,y2,x1,y1,x3,y3)<=_epsilon2 ||
         gs_point_segment_dist2(x3,y3,x1,y1,x2,y2)<=_epsilon2 )
    { errcount++;
      _error ( fatal, out, "collinear vertices found!", _man, s, fa );
    }

   return errcount;
 }

bool SeTriangulator::checkall ( bool out, bool fatal, int* numerrors, GsArray<SeBase*>* fa  )
 {
   SeBase* s = _mesh->first();
   if ( !s ) { if (out) gsout<<"check: empty mesh.\n"; return true; }

   SeFace* border = s->sym()->fac(); // we cannot always assume that the border is this one
   SeFace* f=border;
   int nborder;
   do { nborder = _mesh->vertices_in_face(border->se());
        if ( nborder>3 ) break;
        border=border->nxt();
      } while ( border!=f );

   if (out)
    { gsout<<"Border has "<<nborder<< " vertices\n";
      gsout<<"Checking "<<_mesh->faces()<<" faces...\n";
    }

   if ( fa ) fa->size(0);
   int errcount=0;
   f = border->nxt();
   while ( f!=border )
    { s = f->se();
      errcount += check ( s, out, fatal, fa );
      f = f->nxt();
    }

   if (out) { if (errcount==0 ) gsout<<"Check OK!\n"; else gsout<<"Errors found: "<<errcount<<gsnl; }
   if ( numerrors ) *numerrors=errcount;
   return errcount>0? false:true;
 }

//================================================================================
//============================= init triangulation ===============================
//================================================================================

SeBase* SeTriangulator::init_as_triangulated_square 
                        ( double x1, double y1, double x2, double y2,
                          double x3, double y3, double x4, double y4 )
 {
   SeBase* s = _mesh->init();

   _man->set_vertex_coordinates ( s->vtx(), x3, y3 );
   _man->set_vertex_coordinates ( s->nxt()->vtx(), x4, y4 );
   s = _mesh->mev ( s );
   _man->set_vertex_coordinates ( s->vtx(), x2, y2 );
   s = _mesh->mev ( s );
   _man->set_vertex_coordinates ( s->vtx(), x1, y1 );
   _mesh->mef ( s, s->nxt()->nxt()->nxt() ); // close the square
   _mesh->mef ( s, s->nxt()->nxt() ); // triangulate square

   TEST("init");

   return s;
 }

SeBase* SeTriangulator::init_as_triangle ( double x1, double y1, double x2, double y2,
                                           double x3, double y3 )
 {
   SeBase* s = _mesh->init();

   _man->set_vertex_coordinates ( s->vtx(), x2, y2 );
   _man->set_vertex_coordinates ( s->nxt()->vtx(), x3, y3 );
   s = _mesh->mev ( s );
   _man->set_vertex_coordinates ( s->vtx(), x1, y1 );
   _mesh->mef ( s, s->nxt()->nxt() ); // close the triangle

   TEST("init");

   return s;
 }

//================================================================================
//============================= face triangulation ===============================
//================================================================================

bool SeTriangulator::triangulate_face ( SeFace* f, bool optimize )
 {
   SeBase *s, *x, *xn, *xp, *v, *vp, *vn;
   GsArray<SeBase*>& stack = _buffer;
   bool ok;

   x = s = f->se();

   GS_TRACE4 ( "starting triangulate..." );

   stack.size(0);
   if (optimize)
    { _mesh->begin_marking ();
      do { _mesh->mark(x->edg()); x=x->nxt(); } while ( x!=s );
    }

   GS_TRACE4 ( "entering loop..." );

   while ( true )
    { xn=x->nxt(); xp=x->pri(); v=xn->nxt();

      if ( xp==v ) break; // is a triangle: done.
      ok = true; // assume we can triangulate current corner (xp,x,xn)

      if ( _man->ccw(xp->vtx(),x->vtx(),xn->vtx()) )
       { do { if ( _man->in_triangle(xp->vtx(),x->vtx(),xn->vtx(),v->vtx()) ) // boundary is considered inside
               { ok=false; break; }
              v = v->nxt();
            } while ( v!=xp );
       }
      else ok=false;

      if (ok)
       { s = _mesh->mef ( xp, xn );
         _man->triangulate_face_created_edge ( s->edg() );
         //PRINTF(xp);gsout<<gsnl;PRINTF(xn);gsout<<gsnl;
         if (optimize) stack.push()=s;
         s=xn;
       }
      x=xn;
      if ( !ok && x==s )
       { // Loop occured: this should never happen, but may happen if
         // the triangulation gets into a state with duplicated points,
         // or because of self-intersecting or non CCW faces
         if (optimize) _mesh->end_marking ();
         PRINTF(s);
         gsout.fatal("se_triangulator.cpp: loop occured in triangulate_face.\n");
         return false;
       }
    }

   GS_TRACE4 ( "optimizing..."<<stack.size() );

   // at this point collinear faces could happen, and should
   // be flipped by the optimization below
   while ( stack.size()>0 )
    { GS_TRACE4 ( "optimizing... "<<stack.size() );
      x = stack.pop();
      if ( _man->is_flippable_and_not_delaunay(x->edg()) ) // Flip will optimize triangulation
       { xn=x->nxt(); xp=xn->nxt(); vn=x->sym()->nxt(); vp=vn->nxt();
         if ( !_mesh->marked(vp->edg()) ) stack.push(vp);
         if ( !_mesh->marked(vn->edg()) ) stack.push(vn);
         if ( !_mesh->marked(xp->edg()) ) stack.push(xp);
         if ( !_mesh->marked(xn->edg()) ) stack.push(xn);
         _mesh->flip ( x );
       }
    }

   if (optimize) _mesh->end_marking ();

   GS_TRACE4 ( "ok!" );
   TEST("triangulate_face");

   return true;
 }

//================================================================================
//============================= Locate Point=== ==================================
//================================================================================

SeTriangulator::LocateResult SeTriangulator::locate_point
                             ( const SeFace* iniface, double x, double y, SeBase*& result, bool loctest )
 {
   SeVertex *v1, *v2, *v3;
   SeBase *sn, *snn;
   SeBase *s = iniface->se();
   GsArray<SeBase*>& stack = _buffer;
   int sres = NotFound;
   int triangles = _mesh->faces();
   int visited_count=0;
   int ccws;

   bool walk_failed = false;
   double x1, y1, x2, y2, x3, y3;

   GS_TRACE1 ( "locate_point..." );

   if ( !_mesh->is_triangle(s) )
    { gsout.warning ("NON-TRIANGLE CASE FOUND IN LOCATE_POINT!\n");
      return NotFound;
    }

   _mesh->begin_marking ();
   _mesh->mark ( s->fac() );

   while ( true )
    { 
      sn=s->nxt(); snn=sn->nxt();
      v1=s->vtx(); v2=sn->vtx(); v3=snn->vtx(); 

      stack.size(0);
      ccws=0;
      _man->get_vertex_coordinates ( v1, x1, y1 );
      _man->get_vertex_coordinates ( v2, x2, y2 );
      _man->get_vertex_coordinates ( v3, x3, y3 );

      if ( gs_ccw(x,y,x2,y2,x1,y1)>0 ) { ccws++; if(_mesh->is_triangle(s->sym())) stack.push()=s->sym(); }
      if ( gs_ccw(x,y,x3,y3,x2,y2)>0 ) { ccws++; if(_mesh->is_triangle(sn->sym())) stack.push()=sn->sym(); }
      if ( gs_ccw(x,y,x1,y1,x3,y3)>0 ) { ccws++; if(_mesh->is_triangle(snn->sym())) stack.push()=snn->sym(); }

      if ( ccws!=stack.size() )
       { gsout.warning ("BORDER OR NON TRIANGULAR/CCW FACE ENCOUNTERED IN LOCATE_POINT!\n");
         walk_failed = true;
         break;
       } 

      if ( stack.size()==0 )
       { sres = TriangleFound; // found triangle
         break;
       } 
      else if ( stack.size()==1 )
       { s = stack[0]; // only one option to jump to
       }
      else if ( stack.size()==2 ) // here we use marking, instead of a random choice
       { s = _mesh->marked(stack[0]->fac())? stack[1]:stack[0];
       }
      else
       { gsout.warning ("DEGENERATED CASE FOUND IN LOCATE_POINT!"); // may happen if non triangular face is found
         sres = NotFound;
         break;
       }

      if ( _mesh->marked(s->fac()) ) // this may happen in CDTs, but not in DTs
       { GS_TRACE1 ("Already marked triangle found - will try linear search...");
         walk_failed = true;
         break;
       }

      if ( visited_count++>triangles ) // double security: this case should be covered by the previous case
       { gsout.warning ("WALKED MORE THAN NUMBER OF TRIANGLES - WILL TRY LINEAR SEARCH...\n"); 
         walk_failed = true;
         break;
       }

      _mesh->mark ( s->fac() );
    }

   GS_TRACE1 ( "Triangles Visited: " << visited_count );

   if ( walk_failed ) // do linear search, this should not be considered an error in CDTs...
    { SeFace *f, *fi;
      f = fi = s->fac();
      do { s=f->se(); sn=s->nxt(); snn=sn->nxt();
           v1=s->vtx(); v2=sn->vtx(); v3=snn->vtx(); 
           if ( snn->nxt()==s ) // is triangle
            { if ( _man->in_triangle(v1,v2,v3,x,y) )
               { result = s;
                 sres = TriangleFound;
                 break;
               }
            }
           f = f->nxt();
         } while ( f!=fi );
      if ( _debug_mode ) gsout<<"LINEAR SEARCH NOT SUCCESSFULL.\n";
    }

   if ( sres==TriangleFound && loctest ) // perform extra tests
    { sres = _man->test_boundary ( v1, v2, v3, x, y, _epsilon, _epsilonedg, s );
      if ( sres==NotFound ) sres=TriangleFound;
    }

   GS_TRACE1 ( "Search Result : " << (sres==NotFound?"NotFound" : sres==VertexFound?"VertexFound" : sres==EdgeFound?"EdgeFound":"TriangleFound") );

   if ( _debug_mode ) { if ( sres==NotFound ) gsout << "LocatePt Failed!\n"; }

   _mesh->end_marking ();
   result = s;

   return (LocateResult)sres;
 }

//================================================================================
//============================== Insert Vertex ===================================
//================================================================================

SeVertex* SeTriangulator::insert_point_in_face ( SeFace* f, double x, double y )
 {
   GS_TRACE2 ( "insert_point_in_face..." );

   GsArray<SeBase*>& stack = _buffer;
   SeBase *s, *t;

   // even if the point is in an edge the degenerated triangle will be
   // soon flipped by the circle test (this was the previous used solution)
   stack.size(3);
   s = f->se();
   t = _mesh->mev(s);
   SeVertex* v = t->vtx(); // this is the return vertex
   stack[0]=s;
   _man->set_vertex_coordinates ( v, x, y );

   s = s->nxt();
   t = _mesh->mef ( t, s );
   stack[1]=s; 

   s = s->nxt();
   _mesh->mef ( t, s );
   stack[2]=s; 

   _propagate_delaunay ();

   TEST("insert_point_in_face");
   return v;
 }

SeVertex* SeTriangulator::insert_point_in_edge ( SeEdge* e, double x, double y )
 {
   GS_TRACE2 ( "insert_point_in_edge..." );

   SeBase *s = e->se();

   /* We must project into the edge to ensure correctness, otherwise we may encounter
      cases where the polygon of the neighbors of a vertex v does not contain v */
   double x1, y1, x2, y2;
   _man->get_vertex_coordinates ( s->vtx(), x1, y1 );
   _man->get_vertex_coordinates ( s->nxt()->vtx(), x2, y2 );

   gs_segment_projection ( x1, y1, x2, y2, x, y, x, y, _epsilonedg );
   if ( gs_dist2(x,y,x1,y1)<=_epsilon2 ) return s->vtx();
   else if ( gs_dist2(x,y,x2,y2)<=_epsilon2 ) return s->nxt()->vtx();

   GsArray<SeBase*>& stack = _buffer;
   SeBase *t;
   stack.size(4);

   t = _mesh->mev ( s, s->rot() );
   _man->set_vertex_coordinates ( t->vtx(), x, y );

   stack[0]=s->nxt();
   stack[1]=s->nxt()->nxt();
   stack[2]=t->nxt();
   stack[3]=t->nxt()->nxt();

   _mesh->mef ( s, stack[1] );
   _mesh->mef ( t, stack[3] );

   if ( _mode==ModeConstrained )
    { if ( _man->is_constrained(e) )
       { GS_TRACE2 ( "Point in constrained edge case..." );
         _ibuffer.size(0);
         _man->get_constraints ( e, _ibuffer );
         _man->add_constraints ( t->edg(), _ibuffer ); // constrain the other subdivided part
         _man->vertex_found_in_constrained_edge ( s->vtx() );
       }
    }
   else if ( _mode==ModeConforming )
    { if ( _man->is_constrained(e) )
       { GS_TRACE2 ( "Point in constrained edge case..." );
         GsArray<int> ids; // cannot use buffer because the insertion is recursive
         _man->get_constraints ( e, ids );
         _man->add_constraints ( t->edg(), ids ); // constrain the other subdivided part
         _man->vertex_found_in_constrained_edge ( s->vtx() );
       }
    }

   _propagate_delaunay ();

   TEST("insert_point_in_edge");
   return s->vtx();
 }

void SeTriangulator::_propagate_delaunay ()
 {
   GS_TRACE2 ( "propagate_delaunay..." );

   SeBase *s, *x, *t;

   if ( _mode==ModeConstrained ) // (no recursion)
    { GsArray<SeBase*>& stack = _buffer;

      GS_TRACE2 ( "optimizing in constrained mode..."<<stack.size() );

      while ( stack.size()>0 )
       { x = stack.pop();
         if ( !_man->is_delaunay(x->edg()) )  // Flip will optimize triang
          { s=x->sym()->pri(); t=s->nxt()->nxt();
            if ( !_man->is_constrained(x->edg()) )
             { _mesh->flip(x); stack.push(s); stack.push(t); }
          }
       }
    }
   else if ( _mode==ModeUnconstrained ) // (no recursion)
    { GsArray<SeBase*>& stack = _buffer;

      GS_TRACE2 ( "optimizing in unconstrained mode..."<<stack.size() );

      while ( stack.size()>0 )
       { x = stack.pop();
         // remember that in the delaunay case edges are always flippable;
         // originating from a star polygon
         if ( !_man->is_delaunay(x->edg()) )  // Flip will optimize triang
          { s=x->sym()->pri(); t=s->nxt()->nxt();
            _mesh->flip(x); stack.push(s); stack.push(t);
          }
       }
    }
   else // _mode==ModeConforming: we may have recursion here, so all arrays are local
    { GsArray<SeVertex*> constraints;
      GsArray<GsArray<int>*> constraints_ids;
      GsArray<SeBase*>& stack = _buffer;

      constraints.capacity(32); 
      constraints_ids.capacity(32); 

      GS_TRACE2 ( "optimizing in conforming mode..."<<stack.size() );

      while ( stack.size()>0 )
       { x = stack.pop();
         if ( !_man->is_delaunay(x->edg()) )  // Flip will optimize triang
          { s=x->sym()->pri(); t=s->nxt()->nxt();
            if ( _man->is_constrained(x->edg()) )
             { GS_TRACE2 ( "Prepare to flip constrained edge..." );
               constraints_ids.push() = new GsArray<int>;
               _man->get_constraints ( x->edg(), *constraints_ids.top() );
               constraints.push() = x->vtx();
               constraints.push() = x->nxt()->vtx();
               _man->set_unconstrained ( x->edg() );
             }
            _mesh->flip(x); stack.push(s); stack.push(t);
          }
       }
   
      while ( constraints.size()>0 )
       { GS_TRACE2 ( "Fixing Constraints..." );
         _conform_line ( constraints.pop(), constraints.pop(), *constraints_ids.top() );
         delete constraints_ids.pop();
       }
    }
   GS_TRACE2 ( "Ok." );
 }

SeVertex* SeTriangulator::insert_point ( double x, double y, const SeFace* iniface )
 { 
   SeBase* s;
   LocateResult res;

   if ( iniface ) { if ( !_mesh->is_triangle(iniface->se()) ) iniface=0; }
   if ( !iniface )
    { iniface=_mesh->first()->fac();
      if ( !_mesh->is_triangle(iniface->se()) ) iniface=iniface->nxt();
    }

   res = locate_point ( iniface, x, y, s );

   if ( res==TriangleFound )
    { return insert_point_in_face ( s->fac(), x, y );
    }
   else if ( res==VertexFound )
    { return s->vtx();
    }
   else if ( res==EdgeFound )
    { return insert_point_in_edge ( s->edg(), x, y );
    }
   else // res==NotFound
    { return 0;
    }
 }

bool SeTriangulator::remove_vertex ( SeVertex* v )
 {
   GS_TRACE2 ( "Remove Vertex." );
   SeBase *s = _mesh->delv ( v->se() );
   bool b = triangulate_face ( s->fac() );
   TEST("remove_vertex");
   return b;
 }

//================================================================================
//========================= Insert Line Constraint ===============================
//================================================================================

bool SeTriangulator::insert_line_constraint ( SeVertex *v1, SeVertex *v2, int id )
 {
   GsArray<int>& ids = _ibuffer;
   ids.size(1);
   ids[0]=id;
   if ( v1==v2 ) return true;
   bool res = false;
   switch ( _mode )
    { case ModeConstrained: res=_constrain_line ( v1, v2, ids ); break;
      case ModeConforming: res=_conform_line ( v1, v2, ids ); break;
      default: return false; // case ModeUnconstrained
    }
   TEST("insert_line_constraint");
   return res;
 }

bool SeTriangulator::insert_segment ( double x1, double y1, double x2, double y2, int id, const SeFace* inifac )
 { 
   SeVertex *v1, *v2;

   v1 = insert_point ( x1, y1, inifac );
   if ( !v1 ) return false;
   v2 = insert_point ( x2, y2, inifac );
   if ( !v2 ) return false;

   return insert_line_constraint ( v1, v2, id );
 }

bool SeTriangulator::_conform_line ( SeVertex *v1, SeVertex *v2,
                                     const GsArray<int>& constraints_ids )
 {
   enum Case { LineExist, VertexInEdge, NeedToSubdivide };
   Case c;
   SeEdge* interedge;
   SeVertex *v=0;
   SeBase *x, *s;
   GsArray<SeVertex*> a(0,32);
   double px, py;

   GS_TRACE3 ( "Starting conform_line..." );

   a.push()=v1; a.push()=v2;

   while ( !a.empty() )
    { GS_TRACE3 ( "Stack size:"<<a.size());
      v2=a.pop();
      v1=a.pop();
      interedge=0; // edge intersecting v1,v2 and constrained
      c=NeedToSubdivide;
      x=s=v1->se();
      do { if ( x->nxt()->vtx()==v2 ) // (v1,v2) is there.
            { c=LineExist; break; }
           x=x->rot();
         } while ( x!=s );

      if ( c==NeedToSubdivide )
      do { if ( _man->in_segment(v1,v2,x->nxt()->vtx(),_epsilonedg) )
            { c=VertexInEdge; break; }
           x=x->rot();
         } while ( x!=s );

      if ( c==NeedToSubdivide )
      do { // will subdivide, so just check if we use an intersection point
           if ( _man->is_constrained(x->nxt()->edg()) &&
                _man->segments_intersect 
                    ( x->nxt()->vtx(), x->pri()->vtx(), v1, v2, px, py ) )
                 { interedge = x->nxt()->edg(); break; }
            
           x=x->rot();
         } while ( x!=s );

      //static int i=0; se_out<<i<<"\n"; if ( i++>300 ) while(1);

      if ( c==NeedToSubdivide )
       { GS_TRACE3 ( "NeedToSubdivide"<<(const char*)(interedge? "(with intersection)":"") );
         SeTriangulator::LocateResult res;
         if ( !interedge ) _man->segment_midpoint ( v1, v2, px, py );
         res = locate_point ( s->fac(), px, py, s );
         if ( res==SeTriangulator::NotFound ) 
          { return false; }
         else if ( res==SeTriangulator::VertexFound )
          { v=s->vtx(); }
         else if ( res==SeTriangulator::EdgeFound )
          { v = insert_point_in_edge ( s->edg(), px, py );
            if ( !v ) return false; 
            _man->new_steiner_vertex_created ( v );
          }
         else
          { v = insert_point_in_face ( s->fac(), px, py );
            if ( !v ) return false; 
            _man->new_steiner_vertex_created ( v );
          }
         a.push()=v1; a.push()=v;
         a.push()=v; a.push()=v2;
       }
      else if ( c==VertexInEdge )
       { GS_TRACE3 ( "VertexInEdge");
         v1=x->nxt()->vtx();
         _man->vertex_found_in_constrained_edge ( v1 );
         a.push()=v1; a.push()=v2;
         _man->add_constraints ( x->edg(), constraints_ids );
       }
      else if ( c==LineExist )
       { GS_TRACE3 ( "LineExist");
         _man->add_constraints ( x->edg(), constraints_ids );
       }
    }

   return true;
 }

// handle ok if v1-v2 is already there
// To check: constrain_line doesnt seem to need to receive an array of indices.
bool SeTriangulator::_constrain_line ( SeVertex* v1, SeVertex* v2,
                                       const GsArray<int>& constraints_ids )
 {
   GsArray<SeBase*>& totrig = _buffer;
   GsArray<ConstrElem>& ea = _elembuffer;
   SeBase *s, *e, *s1, *s2 = 0;
   SeVertex* v;
   int i, j, iv1, iv2;
   double px, py;

   GS_TRACE3 ( "Starting constrain_line..." );
   GS_TRACE3 ( "Looking for traversed elements..." );

   // First get lists of traversed elements (ea)
   v = v1;
   e = 0;
   s = v1->se();
   ea.size(0);
   ea.push().set(v,0);

   while ( v!=v2 )
    { //SeBase* ssave=s;
      //SeVertex* vsave=v;
      if ( v ) _v_next_step ( s, v1, v2, v, e );
        else   _e_next_step ( s, v1, v2, v, e );

      if ( e && _man->is_constrained(e->edg()) ) // intersects with another constrained edge
       { 
         _man->segments_intersect ( v1, v2, e->vtx(), e->nxt()->vtx(), px, py );
         s = _mesh->mev ( e, e->rot() );
         v = s->vtx();
         _man->set_vertex_coordinates ( v, px, py );
         _man->new_intersection_vertex_created ( v );
         _man->copy_constraints ( e->edg(), s->edg() );
         e=0; 
         _mesh->mef ( s, s->nxn() ); s=s->sym();
         _mesh->mef ( s->nxt(), s->pri() );
         GS_TRACE3 ( "Vertex added in constraint intersection" ); 
         /* it seems that we would need to call _propagate_delaunay() for
            the created edges between (px,py) and v2, when these edges
            are not going to be optimized with the retriangulation cases later.
            This is rather a special case and right now this is not being treated,
            a fix would: 1)remove totrig reference to _buffer, 2) push edges
            to optmize in _buffer, and 3) call _propagate_delaunay(); */
       }

      if ( v )
       { ea.push().set(v,0);
         s=v->se();
         GS_TRACE3 ( (v==v2?"Found vertex v2.":"Crossed existing vertex.") );
         if ( v!=v2 ) // project point to line for perfect placement
          { double x1, y1, x2, y2;
            _man->get_vertex_coordinates ( v1, x1, y1 );
            _man->get_vertex_coordinates ( v2, x2, y2 );
            _man->get_vertex_coordinates ( v, px, py );
            gs_line_projection ( x1, y1, x2, y2, px, py, px, py );
            _man->set_vertex_coordinates ( v, px, py );
            _man->vertex_found_in_constrained_edge(v);
          }
         v1=v; // advance v1
       }
      else
       { ea.push().set(0,e);
         s=e->sym();
         GS_TRACE3 ( "Crossed edge." );
       }
    }

   // Now kill crossed edges and constrain v1-v2 edge pieces
   totrig.size(0);
   iv1=0;
   for ( i=1; i<ea.size(); i++ ) // first element is always v1
    { if ( ea[i].v ) // 2nd vertex found (can be v2)
       { iv2=i;
         if ( iv2==iv1+1 )
          { GS_TRACE3 ( "Constraining existing edge..." );
            s = ea[iv1].v->se();
            while ( s->nxt()->vtx()!=ea[iv2].v ) s=s->rot();
            _man->add_constraints(s->edg(),constraints_ids);
          }
         else // kill inbetween edges and put new constraint there
          { // here is where we could implement Bernal flips
            GS_TRACE3 ( "Killing crossed edges "<<(iv1+1)<<" to "<<(iv2-1)<<" ..." );
            s1 = ea[iv1+1].e->pri();
            for ( j=iv1+1; j<iv2; j++ ) s2=_mesh->kef(ea[j].e,&s);
            GS_TRACE3 ( "Adding edge constraint..." );
            s = _mesh->mef ( s1, s2->nxt() );
            _man->add_constraints(s->edg(),constraints_ids);
            totrig.push() = s;        // face to be re-triangulated later
            totrig.push() = s->sym(); // face to be re-triangulated later
          }
         iv1=iv2;
       }
    }

   // Finally retriangulate open regions:
   double x2, y2, x2n, y2n;
   GS_TRACE3 ( "Retriangulating "<<totrig.size()<<" regions..." );
   while ( totrig.size()>0 )
    { s2 = totrig.pop();
      s1 = s2->nxt()->nxt();
      if ( s1->nxt()==s2 ) continue; // already a triangle

      _man->get_vertex_coordinates ( s2->vtx(), x2, y2 );
      _man->get_vertex_coordinates ( s2->nxt()->vtx(), x2n, y2n );

      for ( s=s1; s!=s2; s=s->nxt() )
       { if ( _canconnect(s2,s,x2,y2,x2n,y2n) )
          { _maketri ( totrig, s2, s1, s );
            break;
          }
       } 

      // in cases where the face to triangulate has co-circular vertices,
      // the loop above may fail to detect an edge to triangulate. Here
      // we detect this case and just triangulate the first point:
      if ( s==s2 ) _maketri ( totrig, s2, s1, s1 );
    }

   GS_TRACE3 ( "Done." );
   return true;
 }

void SeTriangulator::_v_next_step ( SeBase* s, SeVertex* v1, SeVertex* v2,
                                    SeVertex*& v, SeBase*& e )
 {
   e=0;
   double x1, y1, x2, y2, x3, y3, x4, y4, d2, t;

   // First look if final vertex is found:
   SeBase* x = s;
   do { if ( x->nxt()->vtx()==v2 ) { v=v2; return; }
        x=x->rot();
      } while ( x!=s );

   _man->get_vertex_coordinates ( v1, x1, y1 );
   _man->get_vertex_coordinates ( v2, x2, y2 );
   
   // Project current vertex in the line to get perfect placement
   // and then only consider intersections in the subsegment (v,v2)
   // (cases where v1 was an existing vertex within epsilon distance to the line)
   if ( v!=v1 )
    { _man->get_vertex_coordinates ( v, x3, y3 );
      gs_line_projection ( x1, y1, x2, y2, x3, y3, x1, y1 );
    }

   // Now test the first neighbor vertex:
   v = x->nxt()->vtx();
   _man->get_vertex_coordinates ( v, x3, y3 );
   bool inseg=gs_in_segment(x1,y1,x2,y2,x3,y3,_epsilonedg,d2,t);
   if ( inseg && t>=0 ) return;

   // Then look for intersections in all neighbors:
   do { v = x->nxt()->nxt()->vtx();
        _man->get_vertex_coordinates ( v, x4, y4 );
        bool inseg=gs_in_segment(x1,y1,x2,y2,x4,y4,_epsilonedg,d2,t);
        if ( inseg && t>=0 ) return;
        if ( gs_segments_intersect(x1,y1,x2,y2,x3,y3,x4,y4) ) { v=0; e=x->nxt(); return; }
        x3=x4; y3=y4;
        x=x->rot();
      } while ( x!=s );

   //======== this point should never be reached! ========================
   v=0; e=0;
   gsout.fatal( "se_triangulator.cpp: error in v_next_step()!\n" );
 }

void SeTriangulator::_e_next_step ( SeBase* s, SeVertex* v1, SeVertex* v2,
                                               SeVertex*& v, SeBase*& e )
 {
   v=0;
   e=0;
   SeVertex *va, *vb;
   va = s->nxt()->vtx();
   vb = s->nxt()->nxt()->vtx();

   if ( vb==v2 ) { v=v2; return; }

   double x1, y1, x2, y2, x3, y3, x4, y4;
   _man->get_vertex_coordinates ( v1, x1, y1 );
   _man->get_vertex_coordinates ( v2, x2, y2 );
   _man->get_vertex_coordinates ( va, x3, y3 );
   _man->get_vertex_coordinates ( vb, x4, y4 );

   if ( gs_in_segment(x1,y1,x2,y2,x4,y4,_epsilonedg) ) { v=vb; return; }

   e = gs_segments_intersect(x1,y1,x2,y2,x3,y3,x4,y4)? s->nxt() : s->nxt()->nxt();
 }

bool SeTriangulator::_canconnect ( SeBase* s2, SeBase* sv, double x2, double y2, double x2n, double y2n )
 {
   double xv, yv, x, y;
   _man->get_vertex_coordinates ( sv->vtx(), xv, yv );

   SeBase* s;
   for ( s=s2->nxt()->nxt(); s!=s2; s=s->nxt() )
    { if ( s==sv ) continue;
      _man->get_vertex_coordinates ( s->vtx(), x, y );
      if ( gs_in_circle(x2,y2,x2n,y2n,xv,yv,x,y) ) return false;
    }
   
   return true;
 }

void SeTriangulator::_maketri ( GsArray<SeBase*>& totrig, SeBase* s2, SeBase* s1, SeBase* s )
 {
   if ( s==s1 )
    { GS_TRACE3 ( "MEF case 1" );
      totrig.push() = _mesh->mef ( s2, s );
    }
   else if ( s->nxt()==s2 )
    { GS_TRACE3 ( "MEF case 2" );
      totrig.push() = _mesh->mef ( s, s2->nxt() );
    }
   else
    { GS_TRACE3 ( "MEF case 3" );
      totrig.push() = _mesh->mef ( s, s2->nxt() );
      totrig.push() = _mesh->mef ( s2, s );
    }
 }

//================================================================================
//============================== refine edges ====================================
//================================================================================

int SeTriangulator::refine_edges ( float targetlen, bool markvref )
 {
   SeVertex* v;
   SeEdge* e;
   SeEdge* ei;
   SeBase* se;
   int count=0;

   double x1, y1, x2, y2, len2;
   double tlen2 = double(targetlen)*double(targetlen);

   _buffer.size(0);

   // The following algorithm could be implemented in a single loop by computing in
   // advance the exact number of subdivisions each edge needs, however this method
   // is being used only for experimental purposes so far.
   while ( true )
    { // collect all edges longer than targetlen:
      ei = e = _mesh->first()->edg();
      do { if ( _man->is_constrained(e) )
            { se = e->se();
              _man->get_vertex_coordinates ( se->vtx(), x1, y1 );
              _man->get_vertex_coordinates ( se->nvtx(), x2, y2 );
              len2 = gs_dist2 ( x1, y1, x2, y2 );
              if ( len2>tlen2 ) _buffer.push()=se;
            }
           e = e->nxt();
         } while ( e!=ei );

      // if no edges then done:
      if ( _buffer.empty() ) break;

      // subdivide all edges:
      while ( _buffer.size()>0 )
       { se = _buffer.pop();
         _man->get_vertex_coordinates ( se->vtx(), x1, y1 );
         _man->get_vertex_coordinates ( se->nvtx(), x2, y2 );
         v = insert_point_in_edge ( se->edg(), (x1+x2)/2.0, (y1+y2)/2.0 );
         if ( !v ) { gsout.warning("Insertion failure in refine_edges()!"); count--; }
          else if ( markvref ) { _man->new_steiner_vertex_created(v); }
       }
      count += _buffer.size();
    }

   return count;
 }

//============================ End of File =================================

