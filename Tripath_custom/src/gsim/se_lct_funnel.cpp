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
//# define GS_USE_TRACE1 // shortest path
//# define GS_USE_TRACE2 // funnel point addition
//# define GS_USE_TRACE3 // funnel algorithm
//# define GS_USE_TRACE4 // funnel algorithm details
# include <gsim/gs_trace.h>

//================================================================================
//========================= disc-funnel shortest path ============================
//================================================================================

static bool isgoalopening ( const SeFunnelPt& fa, const SeFunnelPt& fb, GsPnt2 c, float /*radius*/ )
 {
   GsPnt2 a=fa;
   GsPnt2 b=fb;
   GsPnt2 p=b;

   tangents ( fb.side, fa.side, b, a, fb.dist, fa.dist );
   tangents (   'p',   fb.side, c, p, 0,       fb.dist );

   c.x = b.x+(c.x-p.x);
   c.y = b.y+(c.y-p.y);

   if ( fb.side=='t' )
    { return gs_ccw(a.x,a.y,b.x,b.y,c.x,c.y)>0? true:false;
	}
   else
    { return gs_ccw(b.x,b.y,a.x,a.y,c.x,c.y)>0? true:false;
	}
 }

static bool isopening ( const SeFunnelPt& fa, const SeFunnelPt& fb, const SeFunnelPt& fc, float /*r*/, int& fcase )
 {
   GsPnt2 a = fa;
   GsPnt2 b = fb;
   GsPnt2 p = b;
   GsPnt2 c = fc;
   char sori;

   if ( fb.side==fc.side && !fb.apex ) // same as 1st
    { tangents ( fc.side, fb.side, c, p, fc.dist, fb.dist );
      tangents ( fb.side, fa.side, b, a, fb.dist, fa.dist );
      fcase = 1;
      sori = fc.side;
    }
   else if ( fb.side==fc.side ) // here fb.apex is 1
    { tangents ( fc.side, fb.side, c, p, fc.dist, fb.dist );
      tangents ( fa.side, fc.side, b, a, fa.dist, fc.dist );
      fcase = 2;
      sori = fa.side;
    }
   else if ( fb.Pnt() )
    { tangents ( fc.side, fb.side, c, p, fc.dist, fb.dist );
      tangents ( fa.side, fb.side, a, b, fa.dist, fb.dist );
      fcase = 3;
      sori = fa.side;
    }
   else // fc.side!=fb.side
    { tangents ( fc.side, fa.side, c, p, fc.dist, fa.dist );
      tangents ( fa.side, fa.side, a, b, fa.dist, fa.dist );
      fcase = 4;
      sori = fa.side;
    }
   
   c.x = b.x+(c.x-p.x);
   c.y = b.y+(c.y-p.y);

   if ( sori=='t' )
    { return gs_ccw(a.x,a.y,b.x,b.y,c.x,c.y)>0? true:false;
	}
   else
    { return gs_ccw(b.x,b.y,a.x,a.y,c.x,c.y)>0? true:false;
	}
 }

# ifdef GS_USE_TRACE4
static const char* sFunnelDesc ( GsDeque<SeFunnelPt>& dq )
 { static GsString s;
   s.len(0);
   for ( int i=0; i<dq.size(); i++ ){ char c=dq[i].side; if(dq[i].apex)c=GS_UPPER(c); s<<c; }
   return (const char*)s;
 }
#endif

void SeLct::_funneladd ( FunnelDeque* funnel, char side, FunnelPath* fpath, const GsPnt2& p, float radius )
 {
   int fcase;
   bool bapex = false;
   funnel->topmode ( side=='t'? true:false );

   GS_TRACE4 ( "Adding point to funnel side: "<<side );
   GS_TRACE4 ( "Starting funnel: "<<sFunnelDesc(dq) );

   // point to be added to the funnel:
   SeFunnelPt c; c.set ( p.x, p.y, side, 0, radius );
   
   // the funnel will only have less than 2 elements at the beginning (during set-up), check that:
   if ( funnel->size()<2 )
    { GS_TRACE4 ( "Trivial addition" );
      funnel->push ( c.x, c.y, c.side, c.apex, c.dist );
      return;
    }

   // update the funnel according to the point to be inserted:
   while (true)
    { 
      SeFunnelPt& b = funnel->get();
      SeFunnelPt& a = funnel->get(1);

      if ( b.apex )
       { GS_TRACE3 ( "Funnel apex reached" );
         bapex=true; // if the apex is popped, it will go to the path
       }

      if ( isopening ( a, b, c, radius, fcase ) ) 
       { GS_TRACE3 ( "Funnel opening: add" );
         if ( bapex ) funnel->newapex();
         funnel->push ( c.x, c.y, c.side, c.apex, c.dist );
         break;
       }

      GS_TRACE3 ( "Funnel not opening: pop" );
      funnel->pop();

      if ( funnel->size()==1 )
       { GS_TRACE3 ( "Funnel size is 1: select apex" );
         if ( fcase>1 && dist(b,p)<dist(b,a) )
          { funnel->get().apex=0;
            funnel->push ( c.x, c.y, c.side, 1, c.dist );
            if (bapex) _fpathpush ( fpath, funnel->get(), radius );
          }
         else 
          { funnel->newapex();
            if (bapex) _fpathpush ( fpath, funnel->get(), radius );
            funnel->push ( c.x, c.y, c.side, 0, c.dist );
          }
         break;
       }

      if ( bapex )
       { GS_TRACE3 ( "fpath push: "<<a );
         _fpathpush ( fpath, a, radius );
       }
    }

   //to debug deque:
   GS_TRACE4 ( "Resulting funnel:"<<sFunnelDesc(dq) );
   GS_TRACE4 ( "Done.\n" );
 }

void SeLct::_funnelstart ( FunnelDeque* funnel, const GsPnt2& apex, SeBase* ent, FunnelPath* fpath, float radius, float extclear )
 {
   // add the first apex:
   GS_TRACE1 ( "Adding funnel apex..." );
   funnel->pusht().set ( apex.x, apex.y, 'p', 1 ); // initial point becomes the funnel apex
   funnel->apex = apex;
   fpath->push() = funnel->top();

   // check the entrance type:
   int i = ent==_ent[0].s? 0: ent==_ent[1].s? 1:2;
   GS_TRACE1 ( "Used entrance "<<i<<" is "<<(_ent[i].type==EntTrivial?"trivial":"not trivial") );

   // add entrance correction vertices if they exist:
   if ( _ent[i].type==EntNotTrivial )
    { GS_TRACE1 ( "Main entrance side: " << (_ent[i].top?"top":"bottom") );
      GS_TRACE1 ( (_ent[i].top?"Top":"Bottom") << " disturbances: "<<(_ent[i].fps1-2) );
      GS_TRACE1 ( (_ent[i].top?"Bottom":"Top") << " disturbances: "<<(_ent[i].fps2-2) );
      Entrance& e = _ent[i];
      if ( e.fps1>2 )
       { int max = e.fps1-1;
         GS_TRACE1 ( "Adding "<<e.fps1-2<<" disturbance(s) to the funnel..." );
         for ( i=1; i<max; i++ ) 
          _funneladd ( funnel, e.top? 't':'b', fpath, e.fp[i], radius );
       }
      if ( e.fps2>2 )
       { int max = e.fp.size()-1;
         GS_TRACE1 ( "Adding "<<e.fps2-2<<" disturbance(s) to the funnel..." );
         for ( i=e.fps1+1; i<max; i++ ) 
          _funneladd ( funnel, e.top? 'b':'t', fpath, e.fp[i], radius );
       }
    }

   // add the two vertices of the channel entrance:
   GS_TRACE1 ( "Adding channel first edge to the funnel..." );
   double x, y;
   GsPnt2 a, b;
   _man->get_vertex_coordinates ( ent->vtx(), x, y ); a.set((float)x,(float)y);
   _man->get_vertex_coordinates ( ent->nxt()->vtx(), x, y ); b.set((float)x,(float)y);

   if ( extclear>0 ) // extra clearance is to be completed
    { _funneladd ( funnel, 'b', fpath, a, radius );
      _funneladd ( funnel, 't', fpath, b, radius );
    }
   else
    { _funneladd ( funnel, 'b', fpath, a, radius );
      _funneladd ( funnel, 't', fpath, b, radius );
    }
 }

void SeLct::_funnelclose ( FunnelDeque* funnel, FunnelPath* fpath, int ei, float radius )
 {
   // 1. check if additional funnel vertices are needed due corrections detected
   // with arrival tests:
   int i;
   Entrance& ent = _ent[ei];
   if ( ent.type==EntNotTrivial )
    { GS_TRACE1 ( "Arrival has "<<ent.fp.size()-4<<" disturbance(s)..." );
      GS_TRACE1 ( "Main arrival side: " << (ent.top?"top":"bottom") );
      GS_TRACE1 ( (ent.top?"Top":"Bottom") << " disturbances: "<<ent.fps1-2 );
      GS_TRACE1 ( (ent.top?"Bottom":"Top") << " disturbances: "<<ent.fps2-2 );
      bool intop = !ent.top; // top entrance means bottom arrival
      if ( ent.fps1>2 )
       { //int max = ent.fps1-1;
         GS_TRACE1 ( "Adding "<<ent.fps1-2<<" disturbance(s) to the funnel..." );
         for ( i=ent.fps1-2; i>0; i-- ) 
          _funneladd ( funnel, intop? 't':'b', fpath, ent.fp[i], radius );
       }
      if ( ent.fps2>2 )
       { //int max = ent.fp.size()-2;
         GS_TRACE1 ( "Adding "<<ent.fps2-2<<" disturbance(s) to the funnel..." );
         for ( i=ent.fp.size()-2; i>ent.fps1; i-- ) 
          _funneladd ( funnel, intop? 'b':'t', fpath, ent.fp[i], radius );
       }
    }

   // 2. check where is the zone in the funnel that the goal is:
   GsPnt2 pg ( (float)_xg,(float)_yg );
   FunnelDeque& dq = *funnel;
   bool intop=true;
   SeFunnelPt fb;
   dq.topmode ( intop );
   while ( dq.size()>1 )
    { if ( dq.get().apex ) { intop=!intop; dq.topmode(intop); }
      if ( isgoalopening(dq.get(1),dq.get(),pg,radius) ) { fb=dq.get(); break; }
      dq.pop();
    }

   // 3. add the needed portion of the funnel side to the funnel path:
   for ( i=0; dq.get(i).apex==0; i++ ); // stop at the apex
   while ( --i>=0 ) _fpathpush ( fpath, dq.get(i), radius );

   // 4. add the final point:
   _fpathpush ( fpath, FunnelPt(pg.x,pg.y,'p',0,radius), radius );
 }

inline float ArcAndTanLen ( const GsVec2& a, const GsVec2& b, const GsVec2& cent, const GsVec2& c, float radius )
 {
   return ( radius*angle(a-cent,b-cent) ) + dist(b,c);
 }

# define PATH_CORRECTION // path correction is needed - but can be disabled for testing
void SeLct::_fpathpush ( FunnelPath* fpath, const FunnelPt& fp3, float radius )
 {
   char c3 = fp3.opside();
   GsPnt2 p1, p2, p3, q2, q3;

   if ( fpath->size()==0 )
    { fpath->push()=fp3;
      if ( fpath->autolen ) fpath->len=0;
      return;
    }

   // The following test is rarely needed but examples needing it have been found, even in LCTs.
   // The r-funnel algorithm remains linear since the overall number of pops is bounded by n.
   #ifdef PATH_CORRECTION
   while ( fpath->size()>=2 )
    { SeFunnelPt& fp1 = fpath->top(1);
      SeFunnelPt& fp2 = fpath->top();
      p1=fp1; p2=fp2; p3=fp3; q2=p2;
      char c1 = fp1.opside();
      char c2 = fp2.optop();

      tangents ( c1, c2, p1, p2, fp1.dist, fp2.dist );
      tangents ( c2, c3, q2, p3, fp2.dist, fp3.dist );
      q3 = p2 + (p3-q2);

      double ccw = gs_ccw ( p1.x,p1.y,p2.x,p2.y,q3.x,q3.y );
      if ( GS_NEXT(ccw,0,1.0E-9) || (c2=='b' && ccw<=0) || (c2=='t' && ccw>=0) )
       { if ( fpath->autolen && fpath->size()>2 ) // discount lenght of the region being popped
          { SeFunnelPt& fp = fpath->top(2);
            GsPnt2 p = fp;
            GsPnt2 p0 = fp1;
            tangents ( fp.opside(), c1, p, p0, fp.dist, fp1.dist );
            fpath->len -= ArcAndTanLen ( p0, p1, fp1, p2, radius );
          }
         fpath->pop();
       }
      else
       { if ( fpath->autolen ) fpath->len += ArcAndTanLen ( p2, q2, fp2, p3, radius );
         fpath->push()=fp3;
         break;
       }
    }
   #else
   if ( fpath->size()>=2 )
    { SeFunnelPt& fp1 = fpath->top(1);
      SeFunnelPt& fp2 = fpath->top();
      p1=fp1; p2=fp2; p3=fp3; q2=p2;
      if ( fpath->autolen ) fpath->len += ArcAndTanLen ( p2, q2, fp2, p3, radius );
      fpath->push()=fp3;
      return;
    }
   #endif

   if ( fpath->size()==1 )
    { if ( fpath->autolen )
       { p3 = fp3;
         tangents ( 'p', fp3.opside(), fpath->get(0), p3, radius, radius );
         fpath->len = dist ( fpath->get(0), p3 );
       }
      fpath->push()=fp3;
    }
 }
# undef PATH_CORRECTION

void SeLct::_fpathmake ( FunnelPath* fpath, GsPolygon& path, float radius, float dang )
 {
   // init path:
   path.open ( true );
   path.size ( 0 );

   // check trivial case:
   if ( fpath->size()==2 ) // only endpoints inserted
    { path.push() = fpath->get(0);
      path.push() = fpath->top();
      return;
    }
   path.push() = fpath->get(0);

   GsPnt2 lp ( fpath->get(1) );
   tangents ( 'p', fpath->get(1).optop(), fpath->get(0), lp, 0, fpath->get(1).dist );
   
   int size = fpath->size();
   char code[2];
   float ra, rb;
   GsPnt2 a, b;
   for ( int i=2; i<size; i++ )
    {
      a=fpath->get(i-1); b=fpath->get(i);
      ra=fpath->get(i-1).dist; rb=fpath->get(i).dist;
      code[0]=fpath->get(i-1).optop();
      code[1]=fpath->get(i).opside();
      tangents ( code[0], code[1], a, b, ra, rb );
      GsPnt2& cent = fpath->get(i-1);
      path.arc ( cent, lp-cent/*v1*/, a-cent/*v2*/, ra, code[0]=='b'? dang:-dang );
      lp=b;
    }
   path.push() = fpath->top();
 }

void SeLct::_finitextcl ( float radius, float extclear )
 {
   SeDcdtSymEdge *s1, *s2;
   int i, j, max = _channel.size()-1;

   _extcl.size ( _channel.size() ); // the last two positions hold first two vertices
   _extcl[0].l = 'x';
   _extcl[0].r = radius;

   if ( _channel.size()==1 ) { }

   for ( i=0; i<max; i++ )
    { j=i+1;
      s1 = (SeDcdtSymEdge*)_channel[i];
      s2 = (SeDcdtSymEdge*)_channel[j];

      if ( s1->vtx()==s2->vtx() ) // bot rot
       { _extcl[j].l = 't'; // top location
         _extcl[j].r = _fgetextr ( j, 't', radius, extclear );
       }
      else // top rot
       { _extcl[j].l = 'b'; // bot location
         _extcl[j].r = _fgetextr ( j, 'b', radius, extclear );
       }
    }
 }

float SeLct::_fgetextr ( int c, char m, float radius, float extclear )
 {
   SeDcdtSymEdge *s1, *s2, *s;
   s1 = s2 = (SeDcdtSymEdge*)_channel[c];

   int i=c, max=_channel.size();

   if ( m=='t' ) // top vertex max clearance requested
    { 
      if ( i>=max-1 )
       { s1=0; }
      else
       { s2 = (SeDcdtSymEdge*)_channel[++i];
         if ( s1->vtx()!=s2->vtx() ) // top rotation
          { while ( ++i<max )
             { s2 = (SeDcdtSymEdge*)_channel[i];
               if ( s1->nvtx()!=s2->nvtx() ) break; // end of top rotation
             }
            s1 = s1->sym();
            s2 = i<max? s2->nxn()->ret():s2->sym();
          }
         else // bot rotation
          { s1=s2; 
          }
       }
    }
   else // bot vertex max clearance requested
    { 
      if ( i>=max-1 )
       { s1=0; }
      else
       { s2 = (SeDcdtSymEdge*)_channel[++i];
         if ( s1->vtx()==s2->vtx() ) // bot rotation
          { while ( ++i<max )
             { s2 = (SeDcdtSymEdge*)_channel[i];
               if ( s1->vtx()!=s2->vtx() ) break; // end of bot rotation
             }
            s1 = s1->ret();
            s2 = i<max? s2->nxt()->sym():s2;
            GS_SWAPT(s1,s2,s);
          }
         else // top rotation
          { s1=s2=s2->nxt(); 
          }
       }
    }

   if ( s1==0 ) return radius;

   s = s1;
   float newr, cl, mincl;
   cl = mincl = s->edg()->cl(s);
   while ( s!=s2 )
    { s=s->rot();
      cl = s->edg()->cl(s);
      if ( cl<mincl ) mincl=cl;
    }

   float maxcl = sqrtf(mincl); // edges store the square of the clearances
   float diam = radius*2.0f;
   float avail = maxcl-diam;

   if ( avail<=0 )
    { newr = radius; }
   else if ( avail/2 > extclear )
    { newr = (diam+extclear)/2.0f; }
   else
    { newr = (diam+(avail/2.0f))/2.0f; }

   return newr;
 }

void SeLct::make_funnel_path ( GsPolygon& path, float radius, float dang, float extclear )
 {
   GS_TRACE1 ( "Entering funnel path..." );

   if ( radius<=0 ) { SeDcdt::make_funnel_path(path); return; }

   path.open ( true );
   path.size ( 0 );

   if ( _path_result==NoPath )
    { GS_TRACE1 ( "No path to compute." );
      return;
    }

   if ( _path_result==TrivialPath )
    { path.push().set((float)_xi,(float)_yi);
      path.push().set((float)_xg,(float)_yg);
      GS_TRACE1 ( "Trivial path computed." );
      return;
    }

   if ( _path_result==LocalPath )
    { GS_TRACE1 ( "Computing local path..." );
      path.push().set((float)_xi,(float)_yi);
      GsArray<GsPnt2>& fp = _ent[0].fp;
      bool top = _ent[0].top;
      if ( !top ) dang=-dang;
      GsVec2 c, v1, v2;
      int k, max = fp.size()-2;
      for ( k=0; k<max; k++ )
       { c = fp[k+1];
         if ( top )
          { v1 = (fp[k]-c).ortho();
            v2 = (c-fp[k+2]).ortho();
          }
         else
          { v1 = (c-fp[k]).ortho();
            v2 = (fp[k+2]-c).ortho();
          }
         path.arc ( c, v1, v2, radius, dang );
       }
      path.push().set((float)_xg,(float)_yg);
      GS_TRACE1 ( "Local path has "<<path.size()<<" vertices." );
      return;
    }

   // ok from now on we are treating the global path search case:
   GS_TRACE1 ( "Computing global path..." );

   // allocate/access used buffers:
   // (we reset autolen to false since this is shared with the optimal search)
   if ( !_fpath ) _fpath = new FunnelPath(false); else { _fpath->size(0); _fpath->autolen=false; }
   if ( !_fdeque ) _fdeque = new FunnelDeque; else _fdeque->init();

   // init auxiliary structures if extra clearance is asked:
   if ( extclear>0 ) _finitextcl ( radius, extclear );

   // check entrances and start funnel:
   _funnelstart ( _fdeque, GsPnt2(_xi,_yi), _channel[0], _fpath, radius, extclear );
   if (_funnelcb) _funnelcb(_fudata);

   // now add remaining vertices of the funnel:
   GS_TRACE1 ( "Running funnel algorithm..." );
   int i, j, max = _channel.size()-1;

   if ( extclear>0 )
    { SeDcdtSymEdge *s;
      float r=radius;
      for ( i=0; i<max; i++ )
       { j=i+1;
         s = (SeDcdtSymEdge*)_channel[j];
         if ( _extcl[j].l=='t' ) // add top vertex
          { r = _extcl[j].r;
            _funneladd ( _fdeque, 't', _fpath, s->nvtx()->p, r ); // add top edge vertex
          }
         else // add bot vertex
          { r = _extcl[j].r;
            _funneladd ( _fdeque, 'b', _fpath, s->vtx()->p, r ); // add bottom edge vertex
          }
        if (_funnelcb) _funnelcb(_fudata);
      }
    }
   else
    { SeDcdtSymEdge *s1, *s2;
      for ( i=0; i<max; i++ )
       { s1 = (SeDcdtSymEdge*)_channel[i];
         s2 = (SeDcdtSymEdge*)_channel[i+1];
         if ( s1->vtx()==s2->vtx() ) // add top vertex
          { GS_TRACE2 ( "Updating funnel top with channel edge "<<(i+1) );
            _funneladd ( _fdeque, 't', _fpath, s2->nvtx()->p, radius ); // add top edge vertex
          }
         else // add bot vertex
          { GS_TRACE2 ( "Updating funnel bottom with channel edge "<<(i+1) );
            _funneladd ( _fdeque, 'b', _fpath, s2->vtx()->p, radius ); // add bottom edge vertex
          }
        if (_funnelcb) _funnelcb(_fudata);
      }
    }

   GS_TRACE1 ( "Path has " << _fpath->size() << " vertices before closure." );
   GS_TRACE1 ( "Processing closure..." );
   _funnelclose ( _fdeque, _fpath, 3, radius );

   // build path approximation based on tangents:
   GS_TRACE1 ( "Building curved path approximation..." );
   _fpathmake ( _fpath, path, radius, dang );

   GS_TRACE1 ( "Done." );
 }

void SeLct::get_path_corners ( GsArray<GsVec2>& points, GsArray<char>& topdown )
 {
   points.size(0);
   topdown.size(0);
   if ( !_fpath ) return;
   for ( int i=0; i<_fpath->size(); i++ )
    { points.push() = _fpath->get(i);
      topdown.push() = _fpath->get(i).side;
    }
 }

void SeLct::get_funnel_corners ( GsArray<GsVec2>& points, GsArray<char>& sides, FunnelDeque* funnel )
 {
   char s;
   points.size(0);
   sides.size(0);
   if ( !funnel ) funnel=_fdeque;
   if ( !funnel ) return;
   for ( int i=0; i<funnel->size(); i++ )
    { points.push() = (*funnel)[i];
      s = (*funnel)[i].side;
      sides.push() = s;
      if ( (*funnel)[i].apex ) sides.top() = GS_UPPER(s);
    }
 }

//============================ End of File =================================

