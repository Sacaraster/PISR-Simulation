/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <math.h>
# include <gsim/gs_vec2.h>
# include <gsim/gs_geo2.h>

//============================= static data ================================

const GsVec2 GsVec2::null(0.0f,0.0f);
const GsVec2 GsVec2::minusone(-1.0f,-1.0f);
const GsVec2 GsVec2::one(1.0f,1.0f);
const GsVec2 GsVec2::i(1.0f,0);
const GsVec2 GsVec2::j(0,1.0f);

//============================== GsVec2 ====================================

void GsVec2::rot ( float radians )
 {
   rot ( sinf(radians), cosf(radians) );
 }

void GsVec2::rot ( const GsVec2& cent, float sa, float ca )
 {
   x-=cent.x; y-=cent.y;
   rot ( sa, ca );
   x+=cent.x; y+=cent.y;
 }

void GsVec2::rot ( const GsVec2& cent, float radians )
 {
   x-=cent.x; y-=cent.y;
   rot ( sinf(radians), cosf(radians) );
   x+=cent.x; y+=cent.y;
 }

void GsVec2::normalize ()
 {
   float f = x*x + y*y;
   if ( f==1.0 || f==0.0 ) return;
   f = sqrt ( f );
   x/=f; y/=f;
 }

float GsVec2::len ( float n )
 {
   float f = sqrt (x*x + y*y);
   if ( f>0 ) { n/=f; x*=n; y*=n; }
   return f;
 }

float GsVec2::norm () const
 {
   float f = x*x + y*y;
   if ( f==1.0 || f==0.0 ) return f;
   return sqrt ( f );
 }


float GsVec2::normax () const
 {
   float a = GS_ABS(x);
   float b = GS_ABS(y);
   return GS_MAX ( a, b );
 }

float GsVec2::angle () const 
 {
   float ang=atan2f(y,x);
   if ( ang<0 ) ang += gs2pi;
   return ang;
 }

float GsVec2::angmax () const 
 {
   float comp, b, a;
   a=GS_ABS(x); b=GS_ABS(y);
   if (b==0.0 && x>=0.0) return 0.0;
   comp = b>=a? ((float)2.0)-(a/b) : (b/a);
   if (x<0.0) comp = ((float)4.0)-comp;
   if (y<0.0) comp = ((float)8.0)-comp;
   return comp;
 }

//=================================== Friend Functions ===================================

void swap ( GsVec2 &v1, GsVec2 &v2 )
 {
   float tmp;
   GS_SWAP(v1.x,v2.x);
   GS_SWAP(v1.y,v2.y);
 }

float distmax ( const GsVec2 &v1, const GsVec2 &v2 )
 {
   float a = v1.x-v2.x;
   float b = v1.y-v2.y;
   a = GS_ABS(a);
   b = GS_ABS(b);
   return GS_MAX(a,b);
 }

float dist ( const GsVec2 &v1, const GsVec2 &v2 )
 {
   float dx, dy;

   dx=v1.x-v2.x; dy=v1.y-v2.y;

   return sqrt (dx*dx + dy*dy);
 }

float dist2 ( const GsVec2 &v1, const GsVec2 &v2 )
 {
   float dx, dy;
   dx=v1.x-v2.x; dy=v1.y-v2.y;
   return dx*dx + dy*dy;
 }

float angle ( const GsVec2 &v1, const GsVec2 &v2 )
 {
   return (float) gs_angle ( v1.x, v1.y, v2.x, v2.y );
 }

float anglenorm ( const GsVec2 &v1, const GsVec2 &v2 )
 {
   return acos ( dot(v1,v2) );
 }

// angle from vector 1 to vector 2, positive anticlockwise, between -pi -> pi
float oriangle ( const GsVec2& v1, const GsVec2& v2 ) // (-pi,pi]
{
   double theta1 = atan2(v1.y,v1.x);
   double theta2 = atan2(v2.y,v2.x);
   float dtheta = float(theta2-theta1);
   while ( dtheta>gspi ) dtheta -= gs2pi;
   while ( dtheta<-gspi ) dtheta += gs2pi;
   return dtheta;
}

/* this implementation is perfectly fine but a bit slower than the other one
float oriangle ( const GsVec2& v1, const GsVec2& v2 ) // (-pi,pi]
 {
   float a = acos ( dot(v1,v2)/(v1.norm()*v2.norm()) );
   if ( GS_CCW(v1.x,v1.y,0,0,v2.x,v2.y)>0 ) a=-a;
   return a;
 }*/

float angmax ( const GsVec2 &v1, const GsVec2 &v2 ) // [0,4]
 {
   float a1 = v1.angmax();
   float a2 = v2.angmax();
   float a = a1>a2? a1-a2:a2-a1;
   if ( a>4.0f ) a = 8.0f-a;
   return a;
 }

float oriangmax ( const GsVec2 &v1, const GsVec2 &v2 ) // (-4,4]
 {
   float a = v2.angmax() - v1.angmax();
   if ( a>4.0f ) a -= 8.0f;
   if ( a<=-4.0f ) a += 8.0f;
   return a;
 }

int compare ( const GsVec2 &v1, const GsVec2 &v2 )
 {
   if ( v1.x > v2.x ) return  1;
   if ( v1.x < v2.x ) return -1;
   if ( v1.y > v2.y ) return  1;
   if ( v1.y < v2.y ) return -1;
   return 0;
 }

int compare ( const GsVec2* v1, const GsVec2* v2 )
 {
   return compare ( *v1, *v2 );
 }

int comparepol ( const GsVec2 &v1, const GsVec2 &v2 )
 {
   float a1 = angmax(v1,GsVec2::i);
   float a2 = angmax(v2,GsVec2::i);
   if ( a1 > a2 ) return  1;
   if ( a1 < a2 ) return -1;
   return 0;
 }

int comparepol ( const GsVec2* v1, const GsVec2* v2 )
 {
   return comparepol ( *v1, *v2 );
 }

void barycentric ( const GsPnt2& p1, const GsPnt2& p2, const GsPnt2& p3, const GsPnt2& p,
                   float& u, float& v, float& w )
 {
   # define DET3(a,b,c,d,e,f,g,h,i) a*e*i +b*f*g +d*h*c -c*e*g -b*d*i -a*f*h
   float A  = DET3 ( p1.x, p2.x, p3.x, p1.y, p2.y, p3.y, 1, 1, 1 );
   float A1 = DET3 (  p.x, p2.x, p3.x,  p.y, p2.y, p3.y, 1, 1, 1 );
   float A2 = DET3 ( p1.x,  p.x, p3.x, p1.y,  p.y, p3.y, 1, 1, 1 );
   //float A3 = DET3 ( p1.x, p2.x,  p.x, p1.y, p2.y,  p.y, 1, 1, 1 );
   # undef DET3
   u = A1/A;
   v = A2/A;
   w = 1.0f-u-v; // == A3/A;
 }

float ccw ( const GsVec2& p1, const GsVec2& p2, const GsVec2& p3 )
 {
   return GS_CCWD(p1.x,p1.y,p2.x,p2.y,p3.x,p3.y);
 }

bool segments_intersect ( const GsVec2& p1, const GsVec2& p2, const GsVec2& p3, const GsVec2& p4 )
 {
   return gs_segments_intersect ( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y );
 }

bool segments_intersect ( const GsVec2& p1, const GsVec2& p2, const GsVec2& p3, const GsVec2& p4, GsVec2& p )
 {
   double x, y;
   bool b = gs_segments_intersect ( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, x, y );
   p.set ( float(x), float(y) );
   return b;   
 }

bool lines_intersect ( const GsVec2& p1, const GsVec2& p2, const GsVec2& p3, const GsVec2& p4 )
 {
   return gs_lines_intersect ( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y );
 }

bool lines_intersect ( const GsVec2& p1, const GsVec2& p2, const GsVec2& p3, const GsVec2& p4, GsVec2& p )
 {
   double x, y;
   bool b = gs_lines_intersect ( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, x, y );
   p.set ( float(x), float(y) );
   return b;   
 }

void line_projection ( const GsVec2& p1, const GsVec2& p2, const GsVec2& p, GsVec2& q )
 {
   double x, y;
   gs_line_projection ( p1.x, p1.y, p2.x, p2.y, p.x, p.y, x, y );
   q.set ( float(x), float(y) );
 }

int segment_projection ( const GsVec2& p1, const GsVec2& p2, const GsVec2& p, GsVec2& q, double epsilon )
 {
   double x, y;
   int res = gs_segment_projection ( p1.x, p1.y, p2.x, p2.y, p.x, p.y, x, y, epsilon );
   q.set ( float(x), float(y) );
   return res;   
 }

bool in_segment ( const GsVec2& p1, const GsVec2& p2, const GsVec2& p, double epsilon )
 {
   return gs_in_segment ( p1.x, p1.y, p2.x, p2.y, p.x, p.y, epsilon );
 }

bool in_segment ( const GsVec2& p1, const GsVec2& p2, const GsVec2& p, double epsilon, double& dist2, double& t )
 {
   return gs_in_segment ( p1.x, p1.y, p2.x, p2.y, p.x, p.y, epsilon, dist2, t );
 }

bool in_triangle ( const GsVec2& p1, const GsVec2& p2, const GsVec2& p3, const GsVec2& p )
 {
   return gs_in_triangle ( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p.x, p.y );
 }

bool in_circle ( const GsVec2& p1, const GsVec2& p2, const GsVec2& p3, const GsVec2& p )
 {
   return gs_in_circle ( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p.x, p.y );
 }

bool in_circle ( const GsVec2& p1, const GsVec2& p2, const GsVec2& p3, const GsVec2& p, double epsilon )
 {
   return gs_in_circle ( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p.x, p.y, epsilon );
 }

bool circle_center ( const GsVec2& p1, const GsVec2& p2, const GsVec2& p3, GsVec2& c )
 {
   double x, y;
   bool res = gs_circle_center ( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, x, y );
   c.set ( float(x), float(y) );
   return res;
 }

// code: t:top, b:bottom, p:point
void tangents ( char type1, char type2, GsPnt2& p1, GsPnt2& p2, float radius1, float radius2 )
 {
   double t1x, t1y, t2x, t2y, t3x, t3y, t4x, t4y;
   # define SETP1(x,y) p1.set((float)x,(float)y)
   # define SETP2(x,y) p2.set((float)x,(float)y)
   if ( type1=='p' )
    { if ( type2=='p' ) return;
      gs_tangent ( p1.x, p1.y, p2.x, p2.y, radius2, t1x, t1y, t2x, t2y ); // (p,c,t1):CCW, (p,c,t2):CW 
      if ( type2=='t' ) { SETP2(t1x,t1y); } else { SETP2(t2x,t2y); }
    }
   else if ( type2=='p' )
    {
      gs_tangent ( p2.x, p2.y, p1.x, p1.y, radius1, t1x, t1y, t2x, t2y ); // (p,c,t1):CCW, (p,c,t2):CW 
      if ( type1=='t' ) { SETP1(t2x,t2y); } else { SETP1(t1x,t1y); }
    }
   else if ( type1=='t' && type2=='t' )
    {
      gs_external_tangents ( p1.x, p1.y, radius1, p2.x, p2.y, radius2, 
                             t1x, t1y, t2x, t2y, t3x, t3y, t4x, t4y ); // (c1,t1,c2):CCW, (c1,t3,c2):CW 
      SETP1(t3x,t3y); SETP2(t4x,t4y);
    }
   else if ( type1=='b' && type2=='b' )
    {
      gs_external_tangents ( p1.x, p1.y, radius1, p2.x, p2.y, radius2, 
                             t1x, t1y, t2x, t2y, t3x, t3y, t4x, t4y ); // (c1,t1,c2):CCW, (c1,t3,c2):CW 
      SETP1(t1x,t1y); SETP2(t2x,t2y);
    }
   else if ( type1=='b' && type2=='t' )
    {
      gs_internal_tangents ( p1.x, p1.y, radius1, p2.x, p2.y, radius2, 
                             t1x, t1y, t2x, t2y, t3x, t3y, t4x, t4y ); // (c1,t1,c2):CCW, (c1,t3,c2):CW 
      SETP1(t1x,t1y); SETP2(t2x,t2y);
    }
   else if ( type1=='t' && type2=='b' )
    {
      gs_internal_tangents ( p1.x, p1.y, radius1, p2.x, p2.y, radius2, 
                             t1x, t1y, t2x, t2y, t3x, t3y, t4x, t4y ); // (c1,t1,c2):CCW, (c1,t3,c2):CW 
      SETP1(t3x,t3y); SETP2(t4x,t4y);
    }
   # undef SETP1
   # undef SETP2
 }

//================================== End of File ===========================================
