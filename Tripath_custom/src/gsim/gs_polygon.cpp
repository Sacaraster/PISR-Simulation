/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/
 
# include <math.h>
# include <gsim/gs_geo2.h>
# include <gsim/gs_string.h>
# include <gsim/gs_polygon.h>

//# define GS_USE_TRACE1 
//# define GS_USE_TRACE2 // arc
//# define GS_USE_TRACE3 // convex hull
# include <gsim/gs_trace.h>
 
//=================================== GsPolygon =================================================

GsPolygon::GsPolygon ( int s, int c ) : GsArray<GsVec2> ( s, c )
 {
   _open = false;
 }

GsPolygon::GsPolygon ( const GsPolygon& p ) : GsArray<GsVec2> ( p )
 { 
   _open = p._open;
 }

GsPolygon::GsPolygon ( GsVec2* pt, int s, int c ) : GsArray<GsVec2> ( pt, s, c )
 {
   _open = false;
 }

void GsPolygon::setpoly ( const float* pt, int numv )
 {
   if ( numv<0 ) return;
   size ( numv );
   int i, numf=numv*2;
   for ( i=0; i<numf; i+=2 )
     get(i).set( pt[i], pt[i+1] );
   _open = false;
 }

bool GsPolygon::simple () const
 {
   int i, j, j1, end;

   if ( size()<3 ) return false; // degenerated polygon with 1 or 2 edges

   for ( i=0; i<size(); i++ )
    { end = i+size()-1;
      for ( j=i+2; j<size(); j++ )
       { j1 = (j+1)%size();
         if ( j1!=i && segments_intersect(get(i),get(i+1),get(j),get(j1)) )
          return false; // non adjacent edges crossing
       }
    }
   return true; // is simple
 }

bool GsPolygon::convex () const
 {
   int n, i, i1, i2;

   if ( size()<3 ) return false; // degenerated polygon with 1 or 2 edges

   float o, ordering;

   for ( i=0; i<size(); i++ )
    { i1 = (i+1)%size();
      i2 = (i+2)%size();
      ordering = ::ccw ( get(i), get(i1), get(i2) );
      if ( ordering!=0 ) break;
    }

   if ( ordering==0 ) return false; // not even a simple polygon

   for ( n=0; n<size(); n++ )
    { i1 = (i+1)%size();
      i2 = (i+2)%size();
      o = ::ccw ( get(i), get(i1), get(i2) );
      if ( o*ordering<0 ) return false; // not a convex angle
      i++;
    }

   return true;
 }

float GsPolygon::area () const // oriented: >0 if ccw
 {
   float sum=0;
   int i, j;
  
   if ( size()<=2 ) return 0; // degenerated polygon with 1 or 2 edges

   for ( i=0; i<size(); i++ )
    { j = vid(i+1);
      sum += get(i).x*get(j).y - get(j).x*get(i).y;
    }

   return sum/2.0f;
 }

bool GsPolygon::contains ( const GsVec2& p ) const
 {
   int i, n=size();
   float angle=0;
   GsVec2 p1, p2;

   for ( i=0; i<n; i++ )
    { p1 = get(i) - p;
      p2 = get((i+1)%n) - p;
      p2.y = get((i+1)%n).y - p.y;
      angle += oriangle(p1,p2);;
   }

   return GS_ABS(angle)<gspi? false : true;
}

bool GsPolygon::contains ( const GsPolygon& pol ) const
 {
   int i;
   for ( i=0; i<pol.size(); i++ )
    { if ( !contains(pol[i]) ) return false; }
   return true;
 }

GsPnt2 GsPolygon::sample ( char mode ) const
 {
   GsPnt2 p;
   float ax, ay, bx, by;
   get_bounding_box ( ax, ay, bx, by );

   p.x = gs_random ( ax, bx );
   p.y = gs_random ( ay, by );
   if ( mode=='b' ) return p;

   while ( !contains(p) )
    { p.x = gs_random ( ax, bx );
      p.y = gs_random ( ay, by );
    }
   return p;
 }

int GsPolygon::has_in_boundary ( const GsVec2& p, float ds ) const
 {
   int i1, i2;
   for ( i1=0; i1<size(); i1++ )
    { i2 = (i1+1)%size();
      if ( in_segment ( get(i1), get(i2), p, ds ) ) return i1;
    }
   return -1;
 }

void GsPolygon::circle_approximation ( const GsVec2& center, float radius, int nvertices )
 {
   GsVec2 p;
   float ang=0, incang = ((float)GS_2PI) / nvertices;
   size(nvertices); // reserve memory
   size(0);
   while ( nvertices>0 )
    { p.set ( radius*sinf(ang), radius*cosf(ang) );
      p += center;
      push ( p );
      ang += incang;
      nvertices--;
    }
   _open = 0;
 }

void GsPolygon::square ( const GsPnt2& center, float radius )
 {
   size(0);
   push().set ( center.x-radius, center.y-radius );
   push().set ( center.x+radius, center.y-radius );
   push().set ( center.x+radius, center.y+radius );
   push().set ( center.x-radius, center.y+radius );
 }

float GsPolygon::perimeter () const
 {
   if ( size()<2 ) return 0;
   int i;
   float len=0;
   for ( i=1; i<size(); i++ ) len += dist ( cget(i-1), cget(i) );
   if ( !_open ) len += dist ( cget(i), cget(0) );
   return len;
 }

GsPnt2 GsPolygon::interpolate_along_edges ( float t ) const
 {
   int ilast, i1, i2=0;
   float len1=0, len2=0;

   GsVec2 v;
   if ( size()==0 ) return v;
   if ( size()==1 || t<0 ) return cget(0);
   ilast = _open? size()-1:0;

   v = cget(0);
   if ( t<=0 ) return v;

   for ( i1=0; i1<size(); i1++ )
    { if ( i1==size()-1 )
       { if ( _open ) break;
         i2 = 0;
       }
      else i2 = i1+1;
      len1 = len2;
      len2 += dist ( cget(i1), cget(i2) );
      if ( t<len2 ) break;
    }
   
   if ( _open )
    { if ( i1==ilast ) return cget(ilast); }
   else
    { if ( i2==0 ) return cget(0); }

   len2 -= len1;
   t -= len1;
   t /= len2;
   v = lerp ( cget(i1), cget(i2), t );
   return v;
 }

void GsPolygon::resample ( float maxlen )
 {
   if ( maxlen<=0 ) return;
   int i, nsub;
   GsVec2 v1, v2;
   float len;
   GsPolygon obsamp;
   obsamp.size(size());
   obsamp.size(0);

   for ( i=0; i<size(); i++ )
    { if ( i+1==size() && _open ) { obsamp.push()=top(); break; }
      v1 = get(i);
      v2 = get ( (i+1)%size() );
      obsamp.push() = v1;
      len = dist(v1,v2);
      if ( len>maxlen )
       { nsub = (int)(len/maxlen);
         len = len/(nsub+1);
         v2 = v2 - v1;
         v2.len(len);
         while ( nsub>0 )
          { v1+=v2; obsamp.push()=v1; nsub--; }
       }
    }
   adopt ( obsamp );
 }

void GsPolygon::remove_duplicated_vertices ( float epsilon )
 {
   int i1=0;
   epsilon *= epsilon;
   while ( i1<size() )
    { if ( _open && i1+1==size() ) break;
      if ( dist2(get(i1),get((i1+1)%size()))<=epsilon ) remove(i1);
       else i1++;
    }
 }

void GsPolygon::remove_collinear_vertices ( float epsilon )
 {
   int i=0, i1, i2;
   while ( i<size() )
    { if ( _open && i>=size()-2 ) break;
      i1 = (i+1)%size();
      i2 = (i+2)%size();
      if ( gs_point_line_dist ( get(i).x, get(i).y,
                                get(i1).x, get(i1).y,
                                get(i2).x, get(i2).y )<=epsilon )
       remove(i1);
      else
       i++;
    }
 }

GsVec2 GsPolygon::centroid () const
 {
   GsVec2 c;
   int i;
   for ( i=0; i<size(); i++ ) c += get(i);
   c /= (float)size();
   return c;
 }

void GsPolygon::translate ( const GsVec2& dv )
 {
   int i;
   for ( i=0; i<size(); i++ ) set ( i, get(i)+dv );
 }

void GsPolygon::rotate ( const GsVec2& center, float radians )
 {
   int i;
   float s = sinf ( radians );
   float c = cosf ( radians );

   for ( i=0; i<size(); i++ )
    { GsVec2& v = (*this)[i];
      v.rot ( center, s, c );
    }
 }

void GsPolygon::scale ( float s )
 {
   int i;
   for ( i=0; i<size(); i++ ) set ( i, get(i)*s );
 }

GsVec2 GsPolygon::south_pole ( int* index ) const
 {
   int i;
   GsVec2 p;
   if ( size()==0 ) return p;
   
   p=get(0); if (index) *index=0; 

   for ( i=1; i<size(); i++ )
    if ( get(i).y<p.y )
     { p=get(i); if (index) *index=i; }

   //for ( i=0; i<size(); i++ ) gsout<<dist(get(i),p)<<gsnl;

   return p;
 }

struct WV { int i; float a; void set ( int n, float f ) { i=n; a=f; } };

static int sWrap ( GsPolygon& data, int ini, const GsPnt2& o, const GsVec2& v, GsArray<WV>& a )
 {
   int i, imin;
   float angmin = 100.0f;
   float ang;

   a.size(1);
   a[0].set(0,angmin);
   int n = data.size();
   for ( i=ini; i<n; i++ )
    { ang = angle ( v, data[i]-o );
      if ( ang<=angmin )
	   { // gsout<<ang<<gsnl;
         //if ( ang<0) ang=0;
		 angmin=ang; imin=i;
	     if ( GS_NEXT(a.top().a,angmin,0.00001f) )
		  { a.push().set(imin,angmin); }
		 else
		  { a.size(1); a[0].set(imin,angmin); }
	   }
    }

   // if there are collinear points, pick the farthest one:
   if ( a.size()>1 )
    { imin = a[0].i;
      if ( imin==0 ) return 0;
      float d, dmax = dist2(o,data[imin]);
      for ( i=1; i<a.size(); i++ )
       { if ( a[i].i==0 ) return 0;
         d = dist2 ( o, data[a[i].i] );
         if ( d>dmax ) { dmax=d; imin=a[i].i; }
       }
    }

   GS_TRACE3 ( "Min index:"<<imin<<" ang:"<<angmin<<" point:"<<p[imin] );
   return imin;
 }

void GsPolygon::convex_hull ( GsPolygon& hull ) const
  {
    if ( size()<=3 ) { hull=*this; return; }

    GS_TRACE3("Convex Hull Starting");

    hull.reserve(size());
    hull.size(0);

    int i0;
    GsPolygon data ( *this );
    GsPnt2 sp = data.south_pole(&i0);
    if ( i0>0 ) { data[i0]=data[0]; data[0]=sp; i0=0; } // sp is now at 0 pos
    GS_TRACE3("South pole: "<<sp);
    hull.push(sp);

    GsVec2 v = GsVec2::i;
    int nexti, ini=1;
    GsArray<WV> buffer;
    GsPnt2 o = sp;
    
    int count = size()+1;
    while ( count-- ) // this is a protection, the loop should break when i==i0
     {
       //gsout<<pol<<gsnl;getchar();
       nexti = sWrap ( data, ini, o, v, buffer );
       if ( nexti==i0 ) break; // back to south pole
       ini = 0;
       v = data[nexti]-o; v.normalize();
       o = data[nexti];
       if ( dist2(hull.top(),o)>gsmall ) hull.push()=o;
       data[nexti] = data.pop(); // remove point from data set
       if ( data.size()==1 ) break; // only south pole left
       //gsout<<hull<<gsnl;
     }
    if ( count==0 ) gsout.warning ( "Forced loop break in GsPolygon::convex_hull()!" );

    // remove col pnts here !

    GS_TRACE3("Result: "<<pol);
  }

int GsPolygon::pick_vertex ( const GsPnt2& p, float epsilon, bool first ) const
 {
   int i, imin=-1;
   if ( size()==0 ) return imin;

   float dist, distmin;
   distmin = dist2(get(0),p);
   imin = 0;
   if ( first ) epsilon*=epsilon;

   for ( i=1; i<size(); i++ )
    { dist = dist2(get(i),p);
      if ( first ) { if (dist<=epsilon) return i; }
      if ( dist<distmin ) { distmin=dist; imin=i; }
    }

   if ( distmin<=epsilon )
    return imin;
   else
    return -1;
 }

int GsPolygon::pick_edge ( const GsVec2& p, float epsilon, float& dist2 ) const
 { 
   int i, i2, iresult;
   double d, t;

   iresult=-1;
   dist2=-1;

   int s = size();
   for ( i=0; i<s; i++ )
    { i2 = (i+1)%s;
      if ( i2==0 && open() ) break;
      //gsout<<cget(i)<<gspc<<cget((i+1)%size())<<gspc<<p<<gsnl;
      if ( in_segment(cget(i),cget(i2),p,epsilon,d,t) )
       { if ( dist2<0 || d<dist2 )
          { iresult = i;
            dist2 = (float)d;
          }
       }
    }
   //gsout<<"result: "<<iresult<<gsnl;
   return iresult;
 }

void GsPolygon::ear_triangulation ( GsArray<GsPnt2>& tris, bool knowntobeccw ) const
 {
   bool ok;
   int a1, a2, a3, b, size;
   float prec = 0.000005f;
   //double ang, maxang; int maxangi;
   double ccw;
   tris.size(0); 

   if ( GsPolygon::size()<3 ) return;
   GsPolygon pol(*this);
   if (!knowntobeccw) { if ( !pol.ccw() ) pol.revert(); }
   pol.remove_collinear_vertices ( prec );

   while ( pol.size()>3 )
    { size = pol.size();
      //maxang = 0;
      //maxangi=-1;
      for ( a2=pol.size()-1; a2>=0; a2-- )
       { a1 = pol.vid(a2-1);
         a3 = pol.vid(a2+1);

         if ( next(pol[a1],pol[a2],prec) || next(pol[a2],pol[a3],prec) )
          { pol.remove(a2); /*maxangi=-2;*/ break; }

         ccw = gs_ccw ( pol[a1].x, pol[a1].y, pol[a2].x, pol[a2].y, pol[a3].x, pol[a3].y );
         if ( ccw<=0 ) continue; // not ccw

         ok = true;
         for ( b=0; b<pol.size(); b++ )
          { if ( b==a1 || b==a2 || b==a3 ) continue;
            if ( gs_in_triangle_interior ( pol[a1].x, pol[a1].y, pol[a2].x, pol[a2].y,
                                           pol[a3].x, pol[a3].y, pol[b].x, pol[b].y ) )
             { ok=false; break; }
          }
         
         if ( ok )
          { tris.push()=pol[a1]; tris.push()=pol[a2]; tris.push()=pol[a3];
            //gsout<<pol[a1]<<gspc<<pol[a2]<<gspc<<pol[a3]<<gspc<<pol[b]<<gsnl; 
            pol.remove(a2);
            break;
            //ang = gs_triangle_min_angle ( pol[a1].x, pol[a1].y, pol[a2].x, pol[a2].y, pol[a3].x, pol[a3].y );
            //if ( ang>maxang ) { maxang=ang; maxangi=a2; }
          }
       }
      if ( size==pol.size() ) break; // LOOP!!!
      /*if ( maxangi==-1 ) break; // LOOP!!!
      if ( maxangi>=0 )
       { a2 = maxangi;
         a1 = pol.vid(a2-1);
         a3 = pol.vid(a2+1);
         tris.push()=pol[a1]; tris.push()=pol[a2]; tris.push()=pol[a3];
         //gsout<<pol[a1]<<gspc<<pol[a2]<<gspc<<pol[a3]<<gspc<<pol[b]<<gsnl; 
         pol.remove(a2);
       }*/
   }

  tris.push()=pol[0]; tris.push()=pol[1]; tris.push()=pol[2];
 }

void GsPolygon::get_bounding_box ( float& minx, float& miny, float& maxx, float& maxy ) const
 { 
   int s = size();
   if ( s==0 )
    { minx=miny=1.0f; maxx=maxy=0; // define an "empty box"
      return;
    }
   
   int i;
   minx = maxx = get(0).x;
   miny = maxy = get(0).y;
   for ( i=1; i<s; i++ )
    { GS_UPDMIN ( minx, get(i).x ); GS_UPDMAX ( maxx, get(i).x );
      GS_UPDMIN ( miny, get(i).y ); GS_UPDMAX ( maxy, get(i).y );
    }
 }

//================================ configs =================================================

void GsPolygon::get_configuration ( float& x, float& y, float& a ) const
 {
   GsVec2 c = centroid();
   x=c.x; y=c.y;
   a = oriangle ( GsVec2::i, get(0)-c );
   if ( a<0 ) a += gs2pi;
 }

void GsPolygon::set_configuration ( float x, float y, float a )
 {
   GsVec2 c = centroid();
   GsVec2 nc(x,y);

   translate ( nc-c );

   float b = oriangle ( GsVec2::i, get(0)-nc );
   if ( b<0 ) b += gs2pi;
   a = a-b;

   rotate ( nc, a );
 }

bool GsPolygon::intersects ( const GsPolygon& p ) const
 {
   int i, i2, s = p.size();

   for ( i=0; i<s; i++ )
    { i2 = (i+1)%s;
      if ( i2==0 && p.open() ) break;
      if ( intersects(p[i],p[i2]) ) return true;
    }

   return false;
 }

bool GsPolygon::intersects ( const GsVec2& p1, const GsVec2& p2 ) const
 {
   int i, i2, s = size();

   for ( i=0; i<s; i++ )
    { i2 = (i+1)%s;
      if ( i2==0 && open() ) break;
      if ( segments_intersect(p1,p2,get(i),get(i2)) ) return true;
    }

   return false;
 }

void GsPolygon::arc ( const GsPnt2& c, const GsVec2& v1, const GsVec2& v2, float radius, float dang, const GsVec2* limv )
 {
   float lccw=0;
   GsVec2 la, lb;
   if ( limv ) { la=c+v1; lb=c+*limv; lccw=::ccw(la,lb,c); }

   GsVec2 n1=v1; n1.len(radius);
   GsVec2 n2=v2; n2.len(radius);
   float ang = angle(n1,n2);
   bool inv=false;
   if ( dang<0 ) { inv=true; dang=-dang; }

   int i, n = 1+(int)(ang/dang);
   if ( n==1 )
    { dang=ang; }
   else
    { dang += (ang-(float(n)*dang))/float(n); }

   GS_TRACE2 ( "Arc: n="<<n<<" dang="<<GS_TODEG(dang)<<"degs" );

   float f = cosf(dang/2.0f);
   n1/=f; n2/=f; // new len: r/cos(a/2)
   push() = c+n1;
   if (inv) dang=-dang;;
   float Sa = sinf(dang);
   float Ca = cosf(dang);
   for ( i=1; i<=n; i++ )
    { if ( i==n )
       { push() = c+n2; } // make last one exact
      else
       { n1.rot(Sa,Ca);
         push() = c+n1;
       }
      if (limv) 
       if ( ::ccw(la,lb,top())*lccw<0 ) // passed limit, correct!
        { n2=pop();
          n1=top();
          segments_intersect ( la, lb, n1, n2, top() );
          break;
        }
    }
 }

static void addconv ( int i, GsPolygon& p, const GsPnt2& a, const GsPnt2& b, const GsPolygon& path, float radius, float dang )
 {
   GsPnt2 lp = p.top();
   //p.push ( lp ); p.push ( path[i-1] ); p.push ( a ); // to debug
   p.arc ( path[i-1], lp-path[i-1], a-path[i-1], radius, dang );
   p.push() = b;
 }

static void addconc ( int /*i*/, GsPolygon& p, const GsPnt2& a, const GsPnt2& b, const GsPolygon& /*path*/, float /*radius */)
 {
   GsVec2 x;
   if ( lines_intersect ( p.top(1), p.top(), a, b, x ) )
    { p.top() = x;
      p.push() = b;
    }
   else // lines are parallel
    { p.top() = b; }
 }

// method SnPolyed::add_polygon_selection() can be easilly used to test this method
void GsPolygon::inflate ( const GsPolygon& p, float radius, float dang )
 {
   size ( 0 );
   open ( false );

   if ( radius<=0 ) { *this=p; return; }

   if ( p.size()<=1 )
    { if ( p.size()<=0 ) return;
      circle_approximation ( p[0], radius, int(gs2pi/dang)+1 );
      return;
    }

   double t1x, t1y, t2x, t2y, t3x, t3y, t4x, t4y, res;
   GsPnt2 t1, t2, t3, t4;

   int i;
   char convex;

   if ( p.open() ) 
    { GsPolygon  top; // the top part of the polygon
      GsPolygon& bot = *this; // the bottom part of the polygon

      for ( i=1; i<p.size(); i++ )
       { res = gs_external_tangents ( p[i-1].x, p[i-1].y, radius, p[i].x, p[i].y, radius,
                                      t1x, t1y, t2x, t2y, t3x, t3y, t4x, t4y );
         if ( res<0 ) continue; // will happen in case of duplicated points

         t1.set ( float(t1x), float(t1y) );
         t2.set ( float(t2x), float(t2y) );
         t3.set ( float(t3x), float(t3y) );
         t4.set ( float(t4x), float(t4y) );


         convex = 0;
         if ( i>1 ) convex = ::ccw(p[i-2],p[i-1],p[i])>=0? 'b':'t'; 

         if ( convex=='t' )
          {
            addconc ( i, bot, t1, t2, p, radius );
            addconv ( i, top, t3, t4, p, radius, -dang );
          }
         else if ( convex=='b' )
          {
            addconv ( i, bot, t1, t2, p, radius, dang );
            addconc ( i, top, t3, t4, p, radius );
          }
         else
          {
            bot.push()=t1; bot.push()=t2;
            top.push()=t3; top.push()=t4;
          }
       }

      // add the end half-circle:
      GsPnt2 x = bot.pop();
      GsPnt2 y = top.pop();
      bot.arc ( p.top(), x-p.top(), y-p.top(), radius, dang );

      // add the top part in reverse order:
      while ( top.size()>1 ) bot.push()=top.pop();

      // add the end half-circle:
      bot.arc ( p[0], top.top()-p[0], bot[0]-p[0], radius, dang );
   }
  else // inflate polygon
   { int im1, im2;
     for ( i=0; i<p.size(); i++ )
      { im1 = p.vid(i-1);
        im2 = p.vid(i-2);
        res = gs_external_tangents ( p[im1].x, p[im1].y, radius, p[i].x, p[i].y, radius,
                                     t1x, t1y, t2x, t2y, t3x, t3y, t4x, t4y );
        if ( res<0 ) continue;
        t1.set ( float(t1x), float(t1y) );
        t2.set ( float(t2x), float(t2y) );

        if ( i==0 ) { push()=t1; push()=t2; continue; }

        if ( ::ccw(p[im2],p[im1],p[i])>=0 )
         addconv ( i, *this, t1, t2, p, radius, dang );
        else
         addconc ( i, *this, t1, t2, p, radius );
      }

     if ( ::ccw(p.top(1),p.top(),p[0])>0 )
      { arc ( p.top(), top()-p.top(), get(0)-p.top(), radius, dang );}
     else
      { GsVec2 x;
        lines_intersect ( get(0), get(1), top(1), top(), x );
        get(0)=x; pop();
      }
    }
 }

int GsPolygon::compare ( const GsPolygon* p1, const GsPolygon* p2 )
 {
   return p1->size()-p2->size();
 }

GsOutput& operator<< ( GsOutput& out, const GsPolygon& p )
 {
   if ( p.open() ) out<<"open ";
   return out << (const GsArray<GsPnt2>&)p;
 }

GsInput& operator>> ( GsInput& inp, GsPolygon& p )
 {
   if ( inp.check()==GsInput::String )
    { inp.get();
      if ( inp.ltoken()=="open" ) p.open ( true );
    }

   return inp >> (GsArray<GsPnt2>&)p;
 }

float dist ( const GsPolygon& pa, const GsPolygon& pb )
 {
   int pasize = pa.size();
   int pbsize = pb.size();
   if ( pasize==0 || pbsize==0 ) return 0;

   double d, dmin = dist2 ( pa(0), pb(0) );
   int i, i1, j, j1;

   for ( i=0; i<pasize; i++ )
    { i1 = (i+1)%pasize;
      for ( j=0; j<pbsize; j++ )
       { j1 = (j+1)%pbsize;
         d = gs_segment_segment_dist2 ( pa(i).x, pa(i).y, pa(i1).x, pa(i1).y, 
                                        pb(j).x, pb(j).y, pb(j1).x, pb(j1).y );
         if ( d<dmin ) dmin=d;
       }
    }

   return (float) sqrt(dmin);
 }

//================================ End of File =================================================

//using now revert instead of reverse
    /*! Reverse the order of the elements */
  //  void reverse ();
/*void GsPolygon::reverse ()
 {
   GsVec2 v;
   int i, j, m, s;
   s = size();
   m = s/2;
   for ( i=0; i<m; i++ )
    { j = s-1-i;
      v = get(i);
      (*this)[i] = (*this)[j];
      (*this)[j] = v;
    }
 }
*/

