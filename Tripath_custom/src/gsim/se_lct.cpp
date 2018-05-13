/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <math.h>
# include <gsim/gs_array.h>
# include <gsim/gs_geo2.h>
# include <gsim/se_lct.h>
# include "se_triangulator_internal.h"

//# define GS_USE_TRACE1  // Not used
//# define GS_USE_TRACE2  // Refinement Method
//# define GS_USE_TRACE3  // Local Clearance Properties
# include <gsim/gs_trace.h>

# define CONSTR(s) s->edg()->is_constrained()

//=============================== SeLct ==================================

void SeLct::_construct ()
 {
   _fpath = 0;
   _fudata = _sudata = 0;
   _funnelcb = _searchcb = 0;
   _dcdt_changed = true;
   _auto_refinement = true;
   _pre_clearance = true;
   _clear_path ();
 }

SeLct::SeLct ( double epsilon )
      :SeDcdt ( epsilon )
 {
   _construct ();
 }

SeLct::SeLct ( SeMeshBase* m, SeDcdtManager* man, double epsilon )
      :SeDcdt ( m, man, epsilon )

 { 
   _construct ();
 }

SeLct::~SeLct () 
 {
   delete _fpath;
 }

//================================================================================
//=============================== search path ====================================
//================================================================================

bool SeLct::search_channel ( float x1, float y1, float x2, float y2, float radius, const SeFace* iniface )
 {
   // (we do not handle radius==0 and extclear>0)
   if ( radius<=0 ) return SeDcdt::search_channel ( x1, y1, x2, y2, iniface );

   // fast security test to ensure at least that points are not outside the border limits:
   if ( x1<_xmin || x1>_xmax || x2<_xmin || x2>_xmax ) return false;

   // check if refinment is needed:
   if ( _dcdt_changed ) refine ();

   // search for channel:
   if ( !iniface ) iniface = get_search_face();
   bool found = _search_channel ( (double)x1, (double)y1, (double)x2, (double)y2, radius, iniface );

   // to optimize searching for next queries around the same point,
   // we set the next starting search face to the first channel face:
   if ( _channel.size()>0 )
     _cur_search_face = (SeDcdtFace*)SeTriangulator::_channel[0]->fac();

   return found;
 }

//================================================================================
//============================ refinement methods ================================
//================================================================================

int SeLct::refine ( bool force )
 {
   if ( !_dcdt_changed && !force ) return 0;

   int nref=0;
   if ( _auto_refinement )
    { 
      nref += refine ( LocalClearanceRef );
    }
   if ( _pre_clearance  ) 
    {
      compute_clearance ();
    }

   _dcdt_changed = false;

   return nref;
 }

# ifdef GS_USE_TRACE3

static bool InCirc ( double p1x, double p1y, double p2x, double p2y, double p3x, double p3y, double px, double py, bool strict=true )
 {
   if ( gs_in_circle ( p1x, p1y, p2x, p2y, p3x, p3y, px, py ) ) return true;
   double cx, cy;
   gs_circle_center ( p1x, p1y, p2x, p2y, p3x, p3y, cx, cy );
   double r = sqrt(gs_dist2(cx,cy,p1x,p1y));
   double d = sqrt(gs_dist2(cx,cy,px,py));
   d = GS_DIST(d,r);
   gsout<< "("<<d<<")"<<gspc;
   if ( strict ) return false;
   if ( d<0.00001 ) return true; // usual epsilon is 0.000001
   return false;
 }

# define NotInBvcCirc(px,py)   ( rside && !InCirc ( b.x, b.y, v.x, v.y, c.x, c.y, px, py ) ) || \
                               (!rside && !InCirc ( b.x, b.y, a.x, a.y, v.x, v.y, px, py ) )
# define NotInPvcCirc(px,py)   ( rside && !InCirc ( prev->x, prev->y, v.x, v.y, c.x, c.y, px, py ) ) || \
                               (!rside && !InCirc ( prev->x, prev->y, a.x, a.y, v.x, v.y, px, py ) )
# define NotInAbcCirc(px,py)   !InCirc ( a.x, a.y, b.x, b.y, c.x, c.y, px, py )
# define PRNP(p) gsout<<(p.x/10.0f)<<','<<(p.y/10.0f)<<" , ";
# define PRINT { PRNP(a); PRNP(b); PRNP(c); PRNP(v); PRNP(vp); gsout<<gsnl; }

# define SUBT 0.25f

static void TestDisturbTrav ( SeDcdtSymEdge* ca, SeDcdtSymEdge* dv, SeDcdtSymEdge* sub )
 {
   const GsPnt2& s1 = sub->vtx()->p;
   const GsPnt2& s2 = sub->nvtx()->p;
   const GsPnt2& v = dv->vtx()->p;
   const GsPnt2& c = ca->vtx()->p;
   const GsPnt2& a = ca->nvtx()->p;
   const GsPnt2& b = ca->nxt()->nvtx()->p;

   bool rside = dv->nxt()->nvtx()==ca->vtx()? true:false;
   const GsPnt2* prev;
   if ( rside ) // get the "previous apex" to the disturb pt
     prev = &(dv->rot()->nxn()->vtx()->p);
   else
     prev = &(dv->nvtx()->p);

   double bpx, bpy;
   gs_line_projection ( s1.x, s1.y, s2.x, s2.y, b.x, b.y, bpx, bpy );
   GsPnt2 bp(bpx,bpy); // corner projection

   double vpx, vpy;
   gs_line_projection ( s1.x, s1.y, s2.x, s2.y, v.x, v.y, vpx, vpy );
   GsPnt2 vp(vpx,vpy); // disturbance projection

   // Test b' in abc circ
   //=> bp may be slightly outside abc, by E-5
   //if ( NotInAbcCirc(bpx,bpy) ) gsout<<"bp not in abc circ!\n";

   // Test v' in abc circ
   //=> vp may be slightly outside abc, by E-5
   //if ( NotInAbcCirc(vpx,vpy) ) gsout<<"vp not in abc circ!\n";

   // Test b' in bvc or bva circ
   //=> bp may be slightly outside bvc, by E-5
   //if ( NotInBvcCirc(bpx,bpy) ) gsout<<"bp not in bvc/bva circ!\n";

   // Test v' in bvc or bva circ
   //=> vp may be slightly outside bvc, by E-5
   //if ( NotInBvcCirc(vpx,vpy) ) gsout<<"vp not in bvc/bva circ!\n";

   // Test midpoint between vp and bp in abc circ
   GsPnt2 o = (vp+bp)/2.0f; // this midpoint also is
   if ( NotInAbcCirc(o.x,o.y) ) gsout<<"(vp+bp)/2 not in abc circ!\n";

   // Test midpoint between vp and bp in bvc or bva circ
   if ( NotInBvcCirc(o.x,o.y) ) gsout<<"(vp+bp)/2 not in bvc circ!\n";

   // Test midpoint between vp and bp in pvc or pva circ
   //=> (vp+bp)/2 may not be in PVC
   //if ( NotInPvcCirc(o.x,o.y) ) gsout<<"(vp+bp)/2 not in PVC circ!\n"; 

   // Test circ intersection with s:
   double cx, cy, r, t1, t2; bool ok;
   if ( rside )
    ok = gs_circle_center ( prev->x, prev->y, v.x, v.y, c.x, c.y, cx, cy ); // pvc circle
   else
    ok = gs_circle_center ( prev->x, prev->y, a.x, a.y, v.x, v.y, cx, cy ); // pva circle
   if ( !ok ) gsout.fatal("No center!");
   r = sqrt ( gs_dist2(cx,cy,v.x,v.y) );
   int n = gs_line_circle_intersect ( s1.x, s1.y, s2.x, s2.y, cx, cy, r, t1, t2 );
   if ( n<2 ) gsout<<"No crossing!\n";

   // Test bvc/bav circle intersection with s
   GsPnt2 c1 = s1 + (s2-s1)*(float)t1;
   GsPnt2 c2 = s1 + (s2-s1)*(float)t2;
   o = (c1+c2)/2.0f;
 
   if ( rside ) 
    { //o = (c1+vp)/2.0f; 
      //o = c1*(1.0f-SUBT) + c2*SUBT;
      if ( dist(s1,vp)>dist(s1,o) ) gsout<<"R-NNNNNOOOOO!\n";
    }
   else
    { //o = (c2+vp)/2.0f; 
      //o = c2*(1.0f-SUBT) + c1*SUBT;
      if ( dist(s2,vp)>dist(s2,o) ) gsout<<"L-NNNNNOOOOO!\n";
    }

   //if ( NotInAbcCirc(c1.x,c1.y) ) { gsout<<"c1 not in abc circ!\n"; PRINT; }
   //if ( NotInAbcCircEp(c2.x,c2.y) ) gsout<<"c2 not in abc circ!\n";
   //if ( NotInBvcCircEp(c1.x,c1.y) ) gsout<<"c1 not in bvc circ!\n";
   //if ( NotInBvcCircEp(c2.x,c2.y) ) gsout<<"c2 not in bvc circ!\n";
   if ( NotInAbcCirc(o.x,o.y) ) gsout<<"pref not in abc circ!\n";
   if ( NotInBvcCirc(o.x,o.y) ) gsout<<"pref not in bvc circ!\n";
   if ( NotInPvcCirc(o.x,o.y) ) gsout<<"pref not in PVC circ!\n";

   static float minvang=360;
   float vang = rside? GS_TODEG(angle(b-v,c-v)) : GS_TODEG(angle(b-v,a-v));
   if ( vang<minvang ) minvang=vang;
   if ( vang<90 ) { gsout<<"V ANGLE < 90! ANG:" << vang<<  "  MIN="<<minvang<<gsnl; PRINT; }
   //if ( vang<91 ) { gsout<<"v angle:" << vang<<  "  min="<<minvang<<gsnl; PRINT; }
   
 }
# endif // GS_USE_TRACE3

// ca is the traversal base, ca->pri()->vtx() is the traversal corner
// dv->vtx() is the disturbance
// sub is the closest constraint, which will be subdivided
static GsPnt2 _subdivpt ( SeDcdtSymEdge* ca, SeDcdtSymEdge* dv, SeDcdtSymEdge* sub, SeLct::RefinementType type )
 {
   const GsPnt2& s1 = sub->vtx()->p;
   const GsPnt2& s2 = sub->nvtx()->p;
   const GsPnt2& v = dv->vtx()->p;

   if ( type==SeLct::LocalClearanceRef )
    { 
      const GsPnt2& c = ca->vtx()->p;
      const GsPnt2& a = ca->nvtx()->p;
      const GsPnt2& b = ca->nxt()->nvtx()->p;

      # ifdef GS_USE_TRACE3
      TestDisturbTrav ( ca, dv, sub );
      # endif

      double cx, cy, r, t1, t2; bool ok;
      bool rside = dv->nxt()->nvtx()==ca->vtx()? true:false;

      const GsPnt2* p;
      if ( rside ) // get the "previous apex" to the disturb pt
        p = &(dv->rot()->nxn()->vtx()->p);
      else
        p = &(dv->nvtx()->p);
   
      if ( rside )
       ok = gs_circle_center ( p->x, p->y, v.x, v.y, c.x, c.y, cx, cy ); // bvc circle
      else
       ok = gs_circle_center ( p->x, p->y, a.x, a.y, v.x, v.y, cx, cy ); // bva circle
      if ( !ok ) gsout.fatal("No center!");

      r = sqrt ( gs_dist2(cx,cy,v.x,v.y) );
      int n = gs_line_circle_intersect ( s1.x, s1.y, s2.x, s2.y, cx, cy, r, t1, t2 );
      if ( n<2 ) gsout.fatal("No crossing!");

      GsPnt2 c1 = s1 + (s2-s1)*(float)t1;
      GsPnt2 c2 = s1 + (s2-s1)*(float)t2;

      return (c1+c2)*0.5f; // this can be proved to always work

      /*Another option is the midpoint between c1 and v', but not proved yet
      float t=SUBT;
      return rside? c1*(1.0f-t) + c2*t  :  c2*(1.0f-t) + c1*t;

      double vpx, vpy; // disturbance projection v'
      gs_line_projection ( s1.x, s1.y, s2.x, s2.y, v.x, v.y, vpx, vpy );
      GsVec2 vp(vpx,vpy);
      return rside? (c1+vp)/2.0f  :  (c2+vp)/2.0f;
      */
    }
   else if ( type==SeLct::LocalClearanceMidRef )
    { 
      const GsPnt2& c = ca->vtx()->p;
      const GsPnt2& a = ca->nvtx()->p;
      const GsPnt2& b = ca->nxt()->nvtx()->p;

      bool rside = dv->nxt()->nvtx()==ca->vtx()? true:false;

      const GsPnt2* p;
      if ( rside ) // get the "previous apex" to the disturb pt
        p = &(dv->rot()->nxn()->vtx()->p);
      else
        p = &(dv->nvtx()->p);

      double pdx, pdy; // previous disturbance point projection p'
      gs_line_projection ( s1.x, s1.y, s2.x, s2.y, p->x, p->y, pdx, pdy );

      double vpx, vpy; // disturbance projection v'
      gs_line_projection ( s1.x, s1.y, s2.x, s2.y, v.x, v.y, vpx, vpy );

      double t = 0.5;
      return GsPnt2 ( vpx*(1.0-t)+pdx*(t), vpy*(1.0-t)+pdy*(t) );
    }

   double x, y, t;
   gs_line_projection ( s1.x, s1.y, s2.x, s2.y, v.x, v.y, x, y );
   if ( GS_DIST(s1.x,s2.x)>GS_DIST(s1.y,s2.y) )
    t = GS_PARAM(s1.x,s2.x,x);
   else
    t = GS_PARAM(s1.y,s2.y,y);

   if ( type==SeLct::CorridorRef )
    { 
      GS_CLIP(t,0,1);
    }
   else if ( type==SeLct::JunctionRef )
    {
      const double dt = 0.25;
      if ( t<dt ) t=dt; else if ( t>1.0-dt ) t=1.0-dt; else t=0.5;
    }

   return lerp(s1,s2,float(t));
 }

static bool isdisturbance ( SeLct* self, SeDcdtVertex* vtx, SeDcdtSymEdge* ca, SeDcdtSymEdge* can, SeDcdtSymEdge* cap,
                            double clear2, SeDcdtSymEdge* s, bool rside )
 {
   const GsPnt2& c = ca->vtx()->p;
   const GsPnt2& a = can->vtx()->p;
   const GsPnt2& b = cap->vtx()->p;
   const GsPnt2& v = vtx->p;
   GsPnt2& s1 = s->vtx()->p;
   GsPnt2& s2 = s->nvtx()->p;

   //if ( ca->edg()->border() || vtx->border ) return false;
   if ( vtx==ca->vtx() || vtx==can->vtx() || vtx==cap->vtx() ) return false;

   // if v is a refinement, it is in the middle of a straight line constraint, therefore one of its
   // neighbor vertices will either already be a disturbance or prevent v from being a disturbance
   if ( vtx->refinement ) return false; 

   double t, dvs2, vpx, vpy; // v prime is projection of v at s
   dvs2 = gs_point_segment_dist2 ( v.x,v.y, s1.x,s1.y,s2.x,s2.y, t, vpx,vpy );
   if ( dvs2>=clear2 ) return false; // clearance test
   if ( t<=0 || t>=1 ) return false; // test if v orthogonal projection is inside s (this is redundant)

   double bacx, bacy; // projection of b in ac segment
   gs_point_segment_dist2 ( b.x,b.y, a.x,a.y,c.x,c.y, t, bacx,bacy );

   if ( rside ) 
    { if ( dist2(v,c)<=dvs2 ) return false; // test if adjacent traversal is closing
      if ( gs_ccw(bacx,bacy,b.x,b.y,v.x,v.y)<=0 ) return false; // test if it is right of b-bac
    }
   else
    { if ( dist2(v,a)<=dvs2 ) return false; // test if adjacent traversal is closing
      if ( gs_ccw(bacx,bacy,b.x,b.y,v.x,v.y)>=0 ) return false; // test if it is left of b-bac
    }

   if ( ca!=s ) // ac itself is very often the closest constraint
    {
      if ( !gs_segments_intersect ( v.x,v.y,vpx,vpy, a.x,a.y,c.x,c.y ) ) return false; // test if v-vprime crosses ac
      gs_point_segment_dist2(v.x,v.y,a.x,a.y,c.x,c.y,t);
      if ( t<=0 || t>=1 ) return false;  // test if v has ortho vis to ac
    }

   return true;
 }

// if a refinement is needed, it will return the blocking edge, otherwise null
SeDcdtSymEdge* SeLct::_needs_refinement ( SeDcdtSymEdge* s, RefinementType type, SeDcdtSymEdge*& disturb )
 {
   SeDcdtSymEdge * sn = s->nxt();
   SeDcdtSymEdge * sp = sn->nxt();

   if ( s->vtx()->border || sn->vtx()->border || sp->vtx()->border ) return 0;

   if ( type==LocalClearanceRef || type==LocalClearanceMidRef )
    { const GsPnt2& c = s->vtx()->p;
      const GsPnt2& a = sn->vtx()->p;
      const GsPnt2& b = sp->vtx()->p;

      if ( CONSTR(sn) || CONSTR(sp) ) return 0; // not a valid traversal

      float h = (float)gs_point_segment_dist ( b.x, b.y, a.x, a.y, c.x, c.y );
      GsPnt2 vec = (a-c).ortho(); vec.len(h);
      GsPnt2 ra = a + vec;
      GsPnt2 rc = c + vec;

      double d2ab=dist2(a,b);
      double d2bc=dist2(b,c);
      double l2=GS_MIN(d2ab,d2bc);

      float delta = dist(a,c) - ( 2.0f * (float) sqrt(l2-(h*h)) );
      vec = d2ab<d2bc? c-a : a-c;
      vec.len ( delta );
      GsPnt2 p = b + vec; // second corner at right

      if ( !_sector_clear ( s, l2, b, c, a, &p ) )
       {
         double clear2 = _secblockd2;
         SeDcdtVertex* v;
         SeDcdtSymEdge* ei = s;
         SeDcdtSymEdge* e;
         for ( e=ei->rot()->rot(); e!=ei; e=e->rot() ) // right side
          { v = e->nvtx();
            if ( CONSTR(e) ) break;
            if ( ccw(v->p,rc,c)<=0 ) break; 
            if ( isdisturbance ( this, v, s, sn, sp, clear2, _secblockse, true ) )
             { disturb = e->nxt(); return _secblockse;
             }
          }
         ei = s->sym();
         for ( e=sp->rot()->nxt(); e!=ei; e=e->ret() ) // left side
          { v = e->nvtx();
            if ( CONSTR(e) ) break;
            if ( ccw(v->p,a,ra)<=0 ) break; 
            if ( isdisturbance ( this, v, s, sn, sp, clear2, _secblockse, false ) )
             { disturb = e->nxt(); return _secblockse;
             }
          }
       }
      return 0;
    }
   else if ( type==CorridorRef )
    {
      if ( !CONSTR(s) || CONSTR(sn) || CONSTR(sp) ) return 0; // s has to be the only constraint of a corridor triangle
      const GsPnt2& a = s->vtx()->p;
      const GsPnt2& b = sn->vtx()->p;
      const GsPnt2& c = sp->vtx()->p;
      float ang = angle(a-c,b-c);
      if ( ang>gspidiv2 )
       { disturb = sp;
         return s;
       }
      return 0;
    }
   else if ( type==JunctionRef )
    { 
      if ( CONSTR(s) || CONSTR(sn) || CONSTR(sp) ) return 0; // not a junction
      const GsPnt2& a = s->vtx()->p;
      const GsPnt2& b = sn->vtx()->p;
      const GsPnt2& c = sp->vtx()->p;
      double ang = angle(a-c,b-c);
      if ( ang<gspidiv2 ) return 0; // prunes a little with MIN but a lot (half) with MAX
      float d2ca=dist2(c,a);
      float d2cb=dist2(c,b);
      float d2=GS_MIN(d2ca,d2cb);
      if ( _sector_clear ( s, d2, c, a, b ) ) return 0;
      disturb = sp;
      return _secblockse;
    }
   return 0;
 }

int SeLct::refine ( RefinementType type, int maxiter, int* niter )
 {
   SeEdge* e;
   SeEdge* ei;
   SeDcdtSymEdge *s;
   SeDcdtSymEdge *sub;
   SeDcdtSymEdge *dv;
   int nref=0, i=maxiter;

   _dcdt_changed=true; // mark it as changed so that clearances are recomputed

   #ifdef GS_USE_TRACE2
   double t0=gs_time();
   #endif GS_USE_TRACE2

   if ( niter ) *niter = 0;
   # define NITER (maxiter<0 ? (-i-1) : (maxiter-i))

   while ( i!=0 ) // if maxiter<0 the loop will only stop when refinement is concluded
    { _earray.size(0);
      ei = e = _backface->se()->edg();

      // collect all symedges to be fixed:
      do { s = (SeDcdtSymEdge*)e->se();
           sub = _needs_refinement(s,type,dv);
           if ( sub ) { _earray.push()=s; _earray.push()=sub; _earray.push()=dv; }
           else
            { s = s->sym();
              sub = _needs_refinement(s,type,dv);
              if ( sub ) { _earray.push()=s; _earray.push()=sub; _earray.push()=dv; }
            }
           e = e->nxt();
         } while ( e!=ei );

      // if traversed all edges without subdivisions needed: done.
      if ( _earray.empty() ) break;

      i--;
      GS_TRACE2 ( "Iteration: "<<NITER<<" Disturbs: "<<(_earray.size()/3) );

      // subdivide edges:
      while ( _earray.size()>0 )
       {
         dv = _earray.pop();
         sub = _earray.pop();
         s = _earray.pop();
         sub = _needs_refinement(s,type,dv);
         if ( sub )
          { GsPnt2 p = _subdivpt ( s, dv, sub, type ); // we need to re-check since triangles may have been flipped
            SeVertex* v = insert_point_in_edge ( sub->edg(), p.x, p.y );
            if ( !v ) { gsout.warning("failure in refinement"); return false; }
            ((SeDcdtVertex*)v)->refinement = 1; 
            nref++;
          }
       }
    }

   if ( niter ) { *niter = NITER; }
   # undef NITER

   #ifdef GS_USE_TRACE2
   gsout<<"Time: "<<(gs_time()-t0)<<"s\n";
   #endif GS_USE_TRACE2

   return nref;
 }

void SeLct::compute_clearance ()
 {
   SeDcdtFace* f;
   SeDcdtSymEdge* s;

   #define SETC(s,v) if(s->edg()->se()==s) s->edg()->ca=(float)v; else s->edg()->cb=(float)v

   for ( f=_backface->nxt(); f!=_backface; f=f->nxt() )
    { s = f->se();
      SeDcdtSymEdge * sn = s->nxt();
      SeDcdtSymEdge * sp = sn->nxt();
      if ( s->vtx()->border || sn->vtx()->border || sp->vtx()->border ) continue;

      const GsPnt2& a = s->vtx()->p;
      const GsPnt2& b = sn->vtx()->p;
      const GsPnt2& c = sp->vtx()->p;

      double dac=dist2(a,c);
      double dab=dist2(a,b);
      double dbc=dist2(b,c);

      double ma = GS_MIN(dab,dac);
      double mb = GS_MIN(dab,dbc);
      double mc = GS_MIN(dac,dbc);

      ma = _sector_clearance ( sn, ma, a, b, c );
      mb = _sector_clearance ( sp, mb, b, c, a );
      mc = _sector_clearance ( s,  mc, c, a, b );

      SETC(s,ma);
      SETC(sn,mb);
      SETC(sp,mc);
    }

   #undef SETC
 }

//================================================================================
//=============================== get functions ==================================
//================================================================================

void SeLct::get_refinements ( GsArray<GsPnt2>& ea, RefinementType type )
 {
   int i;
   SeEdge* e;
   SeEdge* ei;
   SeDcdtSymEdge* s;
   SeDcdtSymEdge* sub;
   SeDcdtSymEdge* dv;
   ea.size ( 0 );
   ei = e = _backface->se()->edg();
   do { s = (SeDcdtSymEdge*)e->se();
        for ( i=0; i<2; i++ )
         { sub = _needs_refinement(s,type,dv);
           if ( sub )
            { ea.push()=dv->vtx()->p;
              ea.push()=_subdivpt(s,dv,sub,type);
            }
           s = s->sym();
         }
        e = e->nxt();
      } while ( e!=ei );
 }

void SeLct::get_junctions ( GsArray<SeDcdtFace*>& fa )
 {
   SeDcdtFace* f;
   fa.size ( 0 );

   for ( f=_backface->nxt(); f!=_backface; f=f->nxt() )
    { if ( f->junction() && !f->border() )
       fa.push()=f;
    }
 }

//================================================================================
//================================= extras =======================================
//================================================================================

bool SeLct::disturbances_found ( int i )
 { 
   if (i<0) return false;
   if (i>=4) return _path_result==LocalPath? true:false;
   return _ent[i].type==EntNotTrivial? true:false;
 }

GsArray<GsPnt2>& SeLct::disturbances ( int i )
 { 
   if ( i<0 || i>3 ) i=0; // capsule disturbances are saved in _ent[0]
   return _ent[i].fp;
 }

SeBase* SeLct::entrance ( int i )
 {
   if ( i<0 || i>3 ) i=0;
   return _ent[i].s;
 }

void SeLct::funnel_callback ( void (*cb)(void* udata), void* udata )
 {
   _funnelcb = cb;
   _fudata = udata;
 }

void SeLct::search_callback ( void (*cb)(void* udata), void* udata )
 {
   _searchcb = cb;
   _sudata = udata;
 }

void SeLct::_clear_path ()
 {
   _ent[0].type = EntBlocked;
   _ent[1].type = EntBlocked;
   _ent[2].type = EntBlocked;
   _ent[3].type = EntBlocked;
   _path_result = NoPath;
   _maxfronts = 0;
   _fi = 0;
 }

//============================ End of File ===============================
