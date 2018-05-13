/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <math.h>
# include <stdlib.h>

# include <gsim/gs_geo2.h>

# include <gsim/se_triangulator_manager.h>
# include <gsim/se_triangulator.h>

//# define GS_USE_TRACE1 // not yet used
# include <gsim/gs_trace.h>

//========================= SeTriangulatorManager ================================

void SeTriangulatorManager::triangulate_face_created_edge ( SeEdge* /*e*/ )
 {
 }

void SeTriangulatorManager::new_intersection_vertex_created ( SeVertex* /*v*/ )
 {
 }

void SeTriangulatorManager::new_steiner_vertex_created ( SeVertex* /*v*/ )
 {
 }

void SeTriangulatorManager::vertex_found_in_constrained_edge ( SeVertex* /*v*/ )
 {
 }

bool SeTriangulatorManager::is_constrained ( SeEdge* /*e*/ )
 {
   gsout.fatal("SeTriangulatorManager::is_constrained() not implemented!");
   return false;
 }

void SeTriangulatorManager::set_unconstrained ( SeEdge* /*e*/ )
 {
   gsout.fatal("SeTriangulatorManager::set_unconstrained() not implemented!");
 }

void SeTriangulatorManager::get_constraints ( SeEdge* /*e*/, GsArray<int>& /*ids*/ )
 {
   gsout.fatal("SeTriangulatorManager::get_constraints() not implemented!");
 }

void SeTriangulatorManager::add_constraints ( SeEdge* /*e*/, const GsArray<int>& /*ids*/ )
 {
   gsout.fatal("SeTriangulatorManager::add_constraints() not implemented!");
 }

void SeTriangulatorManager::copy_constraints ( const SeEdge* /*e1*/, SeEdge* /*e2*/ )
 {
   gsout.fatal("SeTriangulatorManager::copy_constraints() not implemented!");
 }

void SeTriangulatorManager::angles ( SeVertex* v1, SeVertex* v2, SeVertex* v3, float& a1, float& a2, float& a3 )
 {
   double n, x, y, x1, y1, x2, y2, x3, y3;
   
   get_vertex_coordinates ( v1, x1, y1 );
   get_vertex_coordinates ( v2, x2, y2 );
   get_vertex_coordinates ( v3, x3, y3 );

   x=x1; y=y1;
   x1=x2-x1; y1=y2-y1; n=sqrt(x1*x1+y1*y1); x1/=n; y1/=n;
   x2=x3-x2; y2=y3-y2; n=sqrt(x2*x2+y2*y2); x2/=n; y2/=n;
   x3=x-x3;  y3=y-y3;  n=sqrt(x3*x3+y3*y3); x3/=n; y3/=n;
   a1 = (float) acos ( -x1*x3 - y1*y3 ); // ang = acosf ( dot(v1,v2)/|v1||v2| );
   a2 = (float) acos ( -x1*x2 - y1*y2 );
   a3 = gspi-a1-a2;
 }

bool SeTriangulatorManager::ccw ( SeVertex* v1, SeVertex* v2, SeVertex* v3 )
 {
   double x1, y1, x2, y2, x3, y3;
   get_vertex_coordinates ( v1, x1, y1 );
   get_vertex_coordinates ( v2, x2, y2 );
   get_vertex_coordinates ( v3, x3, y3 );
   return gs_ccw ( x1, y1, x2, y2, x3, y3 )>0? true : false;
 }

bool SeTriangulatorManager::ccw ( double x1, double y1, SeVertex* v2, SeVertex* v3 )
 {
   double x2, y2, x3, y3;
   get_vertex_coordinates ( v2, x2, y2 );
   get_vertex_coordinates ( v3, x3, y3 );
   return gs_ccw ( x1, y1, x2, y2, x3, y3 )>0? true : false;
 }

bool SeTriangulatorManager::in_triangle ( SeVertex* v1, SeVertex* v2, SeVertex* v3, SeVertex* v )
 {
   double x1, y1, x2, y2, x3, y3, x, y;
   get_vertex_coordinates ( v1, x1, y1 );
   get_vertex_coordinates ( v2, x2, y2 );
   get_vertex_coordinates ( v3, x3, y3 );
   get_vertex_coordinates ( v, x, y );
   return gs_in_triangle ( x1, y1, x2, y2, x3, y3, x, y );
 }

bool SeTriangulatorManager::in_triangle ( SeVertex* v1, SeVertex* v2, SeVertex* v3,
                                          double x, double y )
 {
   double x1, y1, x2, y2, x3, y3;
   get_vertex_coordinates ( v1, x1, y1 );
   get_vertex_coordinates ( v2, x2, y2 );
   get_vertex_coordinates ( v3, x3, y3 );
   return gs_in_triangle ( x1, y1, x2, y2, x3, y3, x, y );
 }

bool SeTriangulatorManager::in_segment ( SeVertex* v1, SeVertex* v2, SeVertex* v, double eps )
 {
   double x1, y1, x2, y2, x, y;
   get_vertex_coordinates ( v1, x1, y1 );
   get_vertex_coordinates ( v2, x2, y2 );
   get_vertex_coordinates ( v, x, y );
   return gs_in_segment ( x1, y1, x2, y2, x, y, eps );
 }

bool SeTriangulatorManager::is_delaunay ( SeEdge* e )
 {
   SeBase* s1 = e->se();
   SeBase* s2 = s1->sym();
   SeBase* s3 = s2->nxt()->nxt();
   SeBase* s4 = s1->nxt()->nxt();

   double x1, y1, x2, y2, x3, y3, x4, y4;
   get_vertex_coordinates ( s1->vtx(), x1, y1 );
   get_vertex_coordinates ( s2->vtx(), x2, y2 );
   get_vertex_coordinates ( s3->vtx(), x3, y3 );
   get_vertex_coordinates ( s4->vtx(), x4, y4 );

   return gs_in_circle ( x1, y1, x2, y2, x4, y4, x3, y3 )? false:true;
 }

bool SeTriangulatorManager::is_flippable_and_not_delaunay ( SeEdge* e )
 {
   SeBase* s1 = e->se();
   SeBase* s2 = s1->sym();
   SeBase* s3 = s2->nxt()->nxt();
   SeBase* s4 = s1->nxt()->nxt();

   if ( s3->nxt()!=s2 ) return false; // not a triangle !

   double x1, y1, x2, y2, x3, y3, x4, y4;
   get_vertex_coordinates ( s1->vtx(), x1, y1 );
   get_vertex_coordinates ( s2->vtx(), x2, y2 );
   get_vertex_coordinates ( s3->vtx(), x3, y3 );
   get_vertex_coordinates ( s4->vtx(), x4, y4 );

   if ( gs_ccw(x3,y3,x2,y2,x4,y4)<=0 || gs_ccw(x4,y4,x1,y1,x3,y3)<=0 ) return false;

   return gs_in_circle ( x1, y1, x2, y2, x4, y4, x3, y3 );
 }

// a more efficient implementation would use marking instead of the many CCW tests
bool SeTriangulatorManager::sector_vfree ( SeBase* s, double r2, double cx, double cy,
                            double x1, double y1, double x2, double y2, SeBase*& sv )
 {
   double t, d2, x3, y3;

   // check if s is distant enought from c:
   d2 = gs_point_segment_dist2(cx,cy,x1,y1,x2,y2,t);
   if ( d2>=r2 ) return true; // ok enought space

   // test if we're too close to a vertex:
   if ( gs_next2(cx,cy,x1,y1,r2) ) {sv=s; return false;}
   if ( gs_next2(cx,cy,x2,y2,r2) ) {sv=s->nxt(); return false; }

   // recurse if border not reached:
   s=s->sym()->nxt();
   if ( s->nxt()->nxn()!=s ) return true; // attention: this border test assumes non-triangular backface

   get_vertex_coordinates ( s->nvtx(), x3, y3 );
   if ( gs_ccw(cx,cy,x3,y3,x2,y2)>0 ) // c-p3-p2
    { 
      if ( !sector_vfree(s->nxt(),r2,cx,cy,x3,y3,x2,y2,sv) ) return false;
    }
   if ( gs_ccw(cx,cy,x1,y1,x3,y3)>0 ) // c-p1-p3
    { 
      if ( !sector_vfree(s,r2,cx,cy,x1,y1,x3,y3,sv) ) return false;
    }

   return true;
 }

// attention: s must be well positioned
int SeTriangulatorManager::test_boundary ( SeVertex* v1, SeVertex* v2, SeVertex* v3,
                                           double x, double y, double eps, double epsedg, SeBase*& s )
 {
   double x1, y1, x2, y2, x3, y3, eps2=eps*eps;
   get_vertex_coordinates ( v1, x1, y1 );
   get_vertex_coordinates ( v2, x2, y2 );
   get_vertex_coordinates ( v3, x3, y3 );

   if ( epsedg<eps ) // this is for the agent placement test experiments:
    { SeBase* sn = s->nxt();
      SeBase* sp = sn->nxt();
      if ( !sector_vfree(s,eps2,x,y,x1,y1,x2,y2,s) ) return SeTriangulator::VertexFound;
      if ( !sector_vfree(sn,eps2,x,y,x2,y2,x3,y3,s) ) return SeTriangulator::VertexFound;
      if ( !sector_vfree(sp,eps2,x,y,x3,y3,x1,y1,s) ) return SeTriangulator::VertexFound;
      if ( gs_in_segment(x1,y1,x2,y2,x,y,epsedg) ) { return SeTriangulator::EdgeFound; }
      if ( gs_in_segment(x2,y2,x3,y3,x,y,epsedg) ) { s=sn; return SeTriangulator::EdgeFound; }
      if ( gs_in_segment(x3,y3,x1,y1,x,y,epsedg) ) { s=sp; return SeTriangulator::EdgeFound; }
    }
   else // we can do only 5 tests:
    { if ( gs_in_segment(x1,y1,x2,y2,x,y,epsedg) )
       { if ( gs_next2(x1,y1,x,y,eps2) ) { return SeTriangulator::VertexFound; }
         if ( gs_next2(x2,y2,x,y,eps2) ) { s=s->nxt(); return SeTriangulator::VertexFound; }
         return SeTriangulator::EdgeFound;
       }
      if ( gs_in_segment(x2,y2,x3,y3,x,y,epsedg) )
       { s=s->nxt();
         if ( gs_next2(x2,y2,x,y,eps2) ) { return SeTriangulator::VertexFound; }
         if ( gs_next2(x3,y3,x,y,eps2) ) { s=s->nxt(); return SeTriangulator::VertexFound; }
         return SeTriangulator::EdgeFound;
       }
      if ( gs_in_segment(x3,y3,x1,y1,x,y,epsedg) )
       { s=s->nxt()->nxt();
         if ( gs_next2(x3,y3,x,y,eps2) ) { return SeTriangulator::VertexFound; }
         if ( gs_next2(x1,y1,x,y,eps2) ) { s=s->nxt(); return SeTriangulator::VertexFound; }
         return SeTriangulator::EdgeFound;
       }
    }

   return SeTriangulator::NotFound;
 }

bool SeTriangulatorManager::segments_intersect ( SeVertex* v1, SeVertex* v2, 
                                                 SeVertex* v3, SeVertex* v4,
                                                 double& x, double& y )
 {
   double x1, y1, x2, y2, x3, y3, x4, y4;
   get_vertex_coordinates ( v1, x1, y1 );
   get_vertex_coordinates ( v2, x2, y2 );
   get_vertex_coordinates ( v3, x3, y3 );
   get_vertex_coordinates ( v4, x4, y4 );
   return gs_segments_intersect ( x1, y1, x2, y2, x3, y3, x4, y4, x, y );
 }

bool SeTriangulatorManager::segments_intersect ( SeVertex* v1, SeVertex* v2, 
                                                 SeVertex* v3, SeVertex* v4 )
 {
   double x1, y1, x2, y2, x3, y3, x4, y4;
   get_vertex_coordinates ( v1, x1, y1 );
   get_vertex_coordinates ( v2, x2, y2 );
   get_vertex_coordinates ( v3, x3, y3 );
   get_vertex_coordinates ( v4, x4, y4 );
   return gs_segments_intersect ( x1, y1, x2, y2, x3, y3, x4, y4 );
 }

bool SeTriangulatorManager::segments_intersect ( double x1, double y1, double x2, double y2,
                                                 SeVertex* v3, SeVertex* v4 )
 {
   double x3, y3, x4, y4;
   get_vertex_coordinates ( v3, x3, y3 );
   get_vertex_coordinates ( v4, x4, y4 );
   return gs_segments_intersect ( x1, y1, x2, y2, x3, y3, x4, y4 );
 }

void SeTriangulatorManager::segment_midpoint ( SeVertex* v1, SeVertex* v2, double& x, double& y )
 {
   double x1, y1, x2, y2;
   get_vertex_coordinates ( v1, x1, y1 );
   get_vertex_coordinates ( v2, x2, y2 );
   x = (x1+x2)/2.0;
   y = (y1+y2)/2.0;
 }

double SeTriangulatorManager::distance2 ( SeVertex* v1, SeVertex* v2 )
 {
   double x1, y1, x2, y2;
   get_vertex_coordinates ( v1, x1, y1 );
   get_vertex_coordinates ( v2, x2, y2 );
   return gs_dist2 ( x1, y1, x2, y2 );
 }

double SeTriangulatorManager::distance2 ( SeVertex* v1, double x, double y )
 {
   double x1, y1;
   get_vertex_coordinates ( v1, x1, y1 );
   return gs_dist2 ( x1, y1, x, y );
 }

double SeTriangulatorManager::point_segment_dist2 ( double x, double y, SeVertex* v1, SeVertex* v2, 
                                                    double& t, double& qx, double& qy )
 {
   double x1, y1, x2, y2;
   get_vertex_coordinates ( v1, x1, y1 );
   get_vertex_coordinates ( v2, x2, y2 );
   return gs_point_segment_dist2 ( x, y, x1, y1, x2, y2, t, qx, qy );
 }

//============================ End of File =================================

