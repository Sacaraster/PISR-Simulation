/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <stdio.h>
# include <math.h>

# include <gsim/gs_geo2.h>

// This file is designed to be as most as possible independent of other gs types.
// It groups 2d geometrical primitives which have been carefully tested.

//================================= Macros =========================================

# define gPARAM(a,b,x)            ((x)-(a))/((b)-(a)) // return t : x = a(1-t) + bt
# define gDIST(a,b)               (a>b? (a-b):(b-a))
# define gABS(x)                  (x>0? (x):-(x))
# define gMAX(a,b)                (a>b? (a):(b))
# define gMIN3(a,b,c)             ((a)<(b)? ((a)<(c)?(a):(c)):((b)<(c)?(b):(c)))
# define gMAX3(a,b,c)             ((a)>(b)? ((a)>(c)?(a):(c)):((b)>(c)?(b):(c)))
# define gSWAP(a,b)               { tmp=a; a=b; b=tmp; }
# define gOUTXY(a,b)              printf("(%+6.4f,%+6.4f) ", a, b )
# define gOUTL                    printf("\n");
# define gINCIRCLEEPS             1.0E-11
# define gEPS                     1.0E-12 // E-14 was used before (doubles have 15 decimals)
# define gPI                      3.1415926535897932
# define gNEXTZERO(a)             ( (a)>-(gEPS) && (a)<(gEPS) )

// The following macro can be defined to activate some extra operations that
// try to get more precise results in the functions of this file. Normally
// these extra operations are not worth to activate
// # define gMAXPREC 

//================================= funcs =====================================

bool gs_segments_intersect ( double p1x, double p1y, double p2x, double p2y,
                             double p3x, double p3y, double p4x, double p4y )
 {
   double d = (p4y-p3y)*(p1x-p2x)-(p1y-p2y)*(p4x-p3x);
   if ( gNEXTZERO(d) ) return false; // they are parallel
   double t = ((p4y-p3y)*(p4x-p2x)-(p4x-p3x)*(p4y-p2y)) / d;
   if ( t<0.0 || t>1.0 ) return false; // outside [p1,p2]
   double s = ((p4y-p2y)*(p1x-p2x)-(p1y-p2y)*(p4x-p2x)) / d;
   if ( s<0.0 || s>1.0 ) return false; // outside [p3,p4]
   return true;
}

bool gs_segments_intersect ( double p1x, double p1y, double p2x, double p2y,
                             double p3x, double p3y, double p4x, double p4y,
                             double& qx, double &qy )
 {
   double d = (p4y-p3y)*(p1x-p2x)-(p1y-p2y)*(p4x-p3x);
   if ( gNEXTZERO(d) ) return false; // they are parallel
   double t = ((p4y-p3y)*(p4x-p2x)-(p4x-p3x)*(p4y-p2y)) / d;
   if ( t<0.0 || t>1.0 ) return false; // outside [p1,p2]
   double s = ((p4y-p2y)*(p1x-p2x)-(p1y-p2y)*(p4x-p2x)) / d;
   if ( s<0.0 || s>1.0 ) return false; // outside [p3,p4]
   qx = t*p1x+(1-t)*p2x;
   qy = t*p1y+(1-t)*p2y;
   # ifdef gMAXPREC 
   qx += s*p3x+(1-s)*p4x;
   qy += s*p3y+(1-s)*p4y;
   qx /= 2;
   qy /= 2;
   # endif
   return true;
}

bool gs_lines_intersect ( double p1x, double p1y, double p2x, double p2y,
                          double p3x, double p3y, double p4x, double p4y )
 {
   double d = (p4y-p3y)*(p1x-p2x)-(p1y-p2y)*(p4x-p3x);
   if ( gNEXTZERO(d) ) return false; // they are parallel
   return true;
}

bool inline gslinesintersect ( double p1x, double p1y, double p2x, double p2y,
                               double p3x, double p3y, double p4x, double p4y,
                               double& qx, double& qy, double& t )
 {
   double d = (p4y-p3y)*(p1x-p2x)-(p1y-p2y)*(p4x-p3x);
   if ( gNEXTZERO(d) ) return false; // they are parallel
   t = ((p4y-p3y)*(p4x-p2x)-(p4x-p3x)*(p4y-p2y)) / d;
   qx = t*p1x+(1.0-t)*p2x;
   qy = t*p1y+(1.0-t)*p2y;
   # ifdef gMAXPREC // 1000 random tests reveal E-10 order error between t and s results
   double s = ((p4y-p2y)*(p1x-p2x)-(p1y-p2y)*(p4x-p2x)) / d;
   qx += s*p3x+(1.0-s)*p4x;
   qy += s*p3y+(1.0-s)*p4y;
   qx /= 2.0;
   qy /= 2.0;
   # endif
   return true;
}

bool gs_lines_intersect ( double p1x, double p1y, double p2x, double p2y,
                          double p3x, double p3y, double p4x, double p4y,
                          double& qx, double& qy )
 {
   double t;
   return gslinesintersect(p1x,p1y,p2x,p2y,p3x,p3y,p4x,p4y,qx,qy,t);
 }

bool gs_lines_intersect ( double p1x, double p1y, double p2x, double p2y,
                          double p3x, double p3y, double p4x, double p4y,
                          double& qx, double& qy, double& t )
 {
   return gslinesintersect(p2x,p2y,p1x,p1y,p3x,p3y,p4x,p4y,qx,qy,t); // note p1-p2 are reversed on purpose
 }

bool gs_segment_line_intersect ( double p1x, double p1y, double p2x, double p2y,
                                 double p3x, double p3y, double p4x, double p4y,
                                 double& qx, double& qy )
 {
   double t;
   if ( !gslinesintersect(p1x,p1y,p2x,p2y,p3x,p3y,p4x,p4y,qx,qy,t) ) return false;
   if ( t<0 || t>1 ) return false;
   return true;
 }

int gs_line_circle_intersect ( double p1x, double p1y, double p2x, double p2y, 
                               double cx, double cy, double r, double& t1, double& t2 )
 {
   // x = p1x + t(p2x-p1x), y = p1y + t(p2y-p1y)
   // (x-cx)^2 + (y-cy)^2 = r^2
   // => x^2 - 2.x.cx + cx^2 + y^2 - 2.y.cy + cy^2 = r^2
   // => [ p1x^2 + 2.p1x.t(p2x-p1x) + t^2(p2x-p1x)^2 ] - 2.cx.[p1x + t(p2x-p1x)] + cx^2 + 
   //    [ p1y^2 + 2.p1y.t(p2y-p1y) + t^2(p2y-p1y)^2 ] - 2.cy.[p1y + t(p2y-p1y)] + cy^2 = r^2
   // => t^2 (p2x-p1x)^2 + t 2[p1x(p2x-p1x)-cx(p2x-p1x)] + p1x^2 - 2.cx.p1x + cx^2 + 
   //    t^2 (p2y-p1y)^2 + t 2[p1y(p2y-p1y)-cy(p2y-p1y)] + p1y^2 - 2.cy.p1y + cy^2 = r^2
   // leads to a.t^2 + b.t + c = 0, where:
   double dx=(p2x-p1x), dy=(p2y-p1y);
   double a = dx*dx + dy*dy;
   double b = 2.0 * ( (p1x-cx)*dx + (p1y-cy)*dy );
   double c = p1x*p1x + p1y*p1y + cx*cx + cy*cy - 2.0*((cx*p1x)+(cy*p1y)) - r*r;

   // normal form: x^2 + px + q = 0
   double p = b / (2*a);
   double q = c / a;
   double D = p*p - q;

   if ( gNEXTZERO(D) )
    { t1 = t2 = -p;
      return 1;
    }
   else if ( D<0 )
    { t1 = t2 = 0;
      return 0;
    }
   else // if (D > 0)
    { double sqrt_D = sqrt(D);
      t1 =   sqrt_D - p;
      t2 = - sqrt_D - p;
      return 2;
    }
}

void gs_line_projection ( double p1x, double p1y, double p2x, double p2y, double px, double py, double& qx, double& qy )
 {
   qx = px;
   qy = py;
   double vx = -(p2y-p1y);
   double vy = p2x-p1x; // v = (p2-p1).ortho(), ortho==(-y,x)
   gs_lines_intersect ( p1x, p1y, p2x, p2y, px, py, px+vx, py+vy, qx, qy );
 }

int gs_segment_projection ( double p1x, double p1y, double p2x, double p2y, double px, double py, double& qx, double& qy, double epsilon )
 {
   gs_line_projection ( p1x, p1y, p2x, p2y, px, py, qx, qy );

   double t;
   if ( gDIST(p1x,p2x)>gDIST(p1y,p2y) )
    t = gPARAM(p1x,p2x,qx);
   else
    t = gPARAM(p1y,p2y,qy);

   if ( t<0.0 ) // check dist(q,p1)
    { px = qx-p1x;
      py = qy-p1y;
      return (px*px + py*py)>epsilon*epsilon? 0:1;
    }
   else if ( t>1.0 ) // check dist(q,p2)
    { px = qx-p2x;
      py = qy-p2y;
      return (px*px + py*py)>epsilon*epsilon? 0:2;
    }
   else return 3; // inside segment, regardless of the epsilon
 }

inline double gsdist2 ( double p1x, double p1y, double p2x, double p2y )
 {
   double dx = p2x-p1x;
   double dy = p2y-p1y;
   return dx*dx + dy*dy;
 }

double gs_dist2 ( double p1x, double p1y, double p2x, double p2y )
 {
   return gsdist2(p1x,p1y,p2x,p2y);
 }

double gs_angle ( double v1x, double v1y, double v2x, double v2y )
 {
   double n = sqrt(v1x*v1x+v1y*v1y) * sqrt(v2x*v2x+v2y*v2y);
   // n will be 0 if a given vector is null, so check that here:
   if ( n==0 ) return 0;
   // get the cosine of the angle:
   n = (v1x*v2x + v1y*v2y) / n;
   // n has to be in [-1,1] and numerical aproximations may result in values
   // slightly outside the interval; so the test below is really needed:
   if ( n<-1.0 ) n=-1.0; else if ( n>1.0 ) n=1.0;
   return acos ( n );
 }

double gs_len ( double& vx, double& vy, double len )
 {
   double l = sqrt ( vx*vx+vy*vy);
   if ( l>0 ) { len = len/l; vx*=len; vy*=len; }
   return l;
 }

double gs_anglen ( double v1x, double v1y, double v2x, double v2y )
 {
   return acos ( v1x*v2x+v1y*v2y );
 }

inline double gspsegdist2 ( double px, double py, double p1x, double p1y, double p2x, double p2y, double& t, double& qx, double& qy )
 {
   gs_line_projection ( p1x, p1y, p2x, p2y, px, py, qx, qy );

   if ( gDIST(p1x,p2x)>gDIST(p1y,p2y) )
    t = gPARAM(p1x,p2x,qx);
   else
    t = gPARAM(p1y,p2y,qy);

   if ( t<0.0 ) // get dist(p,p1)
    { px = px-p1x;
      py = py-p1y;
    }
   else if ( t>1.0 ) // get dist(p,p2)
    { px = px-p2x;
      py = py-p2y;
    }
   else // get dist(p,q)
    { px = px-qx;
      py = py-qy;
    }

   return (px*px + py*py);
 }

double gs_point_segment_dist ( double px, double py, double p1x, double p1y, double p2x, double p2y )
 {
   double t, qx, qy;
   return sqrt ( gspsegdist2( px, py, p1x, p1y, p2x, p2y, t, qx, qy ) );
 }

double gs_point_segment_dist2 ( double px, double py, double p1x, double p1y, double p2x, double p2y )
 {
   double qx, qy, t;
   return gspsegdist2 ( px, py, p1x, p1y, p2x, p2y, t, qx, qy );
 }

double gs_point_segment_dist2 ( double px, double py, double p1x, double p1y, double p2x, double p2y, double& t )
 {
   double qx, qy;
   return gspsegdist2 ( px, py, p1x, p1y, p2x, p2y, t, qx, qy );
 }

double gs_point_segment_dist2 ( double px, double py, double p1x, double p1y, double p2x, double p2y, double& t, double& qx, double& qy )
 {
   return gspsegdist2 ( px, py, p1x, p1y, p2x, p2y, t, qx, qy );
 }

inline double gssegsegd2 ( double p1x, double p1y, double p2x, double p2y,
                           double p3x, double p3y, double p4x, double p4y )
 {
   double t, d, dmin, qx, qy;
   dmin = gspsegdist2( p1x, p1y, p3x, p3y, p4x, p4y, t, qx, qy );
   d = gspsegdist2( p2x, p2y, p3x, p3y, p4x, p4y, t, qx, qy );
   if ( d<dmin ) dmin=d;
   d = gspsegdist2( p3x, p3y, p1x, p1y, p2x, p2y, t, qx, qy );
   if ( d<dmin ) dmin=d;
   d = gspsegdist2( p4x, p4y, p1x, p1y, p2x, p2y, t, qx, qy );
   if ( d<dmin ) dmin=d;
   return dmin;
 }

double gs_segment_segment_dist ( double p1x, double p1y, double p2x, double p2y,
                                 double p3x, double p3y, double p4x, double p4y )
 {
   return sqrt ( gssegsegd2(p1x,p1y,p2x,p2y,p3x,p3y,p4x,p4y) );
 }


double gs_segment_segment_dist2 ( double p1x, double p1y, double p2x, double p2y,
                                 double p3x, double p3y, double p4x, double p4y )
 {
   return gssegsegd2(p1x,p1y,p2x,p2y,p3x,p3y,p4x,p4y);
 }

double gs_point_line_dist ( double px, double py, double p1x, double p1y, double p2x, double p2y )
 {
   double qx, qy;
   gs_line_projection (p1x,p1y,p2x,p2y,px,py,qx,qy);
   return sqrt( gsdist2(px,py,qx,qy) );
 }

bool gs_next ( double p1x, double p1y, double p2x, double p2y, double epsilon )
 { 
   return gsdist2(p1x,p1y,p2x,p2y)<=epsilon*epsilon? true:false; // only inline calls
 }

bool gs_next2 ( double p1x, double p1y, double p2x, double p2y, double epsilon2 )
 { 
   return gsdist2(p1x,p1y,p2x,p2y)<=epsilon2? true:false; // only inline calls
 }

double gs_ccw ( double p1x, double p1y, double p2x, double p2y, double p3x, double p3y )
 {
   return GS_CCW(p1x,p1y,p2x,p2y,p3x,p3y);
 }

bool gs_in_segment ( double p1x, double p1y, double p2x, double p2y, double px, double py, double epsilon )
 {
   double t, qx, qy;
   return gspsegdist2( px, py, p1x, p1y, p2x, p2y, t, qx, qy ) > epsilon*epsilon? false:true;
 }

bool gs_in_segment ( double p1x, double p1y, double p2x, double p2y, double px, double py,
                     double epsilon, double& dist2, double& t )
 {
   double qx, qy;
   dist2 = gspsegdist2( px, py, p1x, p1y, p2x, p2y, t, qx, qy );
   return dist2>epsilon*epsilon? false:true;
 }

bool gs_in_triangle ( double p1x, double p1y, double p2x, double p2y, double p3x, double p3y, double px, double py )
 {
   return GS_CCW(px,py,p1x,p1y,p2x,p2y)>=0 && 
          GS_CCW(px,py,p2x,p2y,p3x,p3y)>=0 && 
          GS_CCW(px,py,p3x,p3y,p1x,p1y)>=0 ? true:false;
 }

bool gs_in_triangle_interior ( double p1x, double p1y, double p2x, double p2y, double p3x, double p3y, double px, double py )
 {
   return GS_CCW(px,py,p1x,p1y,p2x,p2y)>0 && 
          GS_CCW(px,py,p2x,p2y,p3x,p3y)>0 && 
          GS_CCW(px,py,p3x,p3y,p1x,p1y)>0 ? true:false;
 }

double gs_triangle_min_angle ( double p1x, double p1y, double p2x, double p2y, double p3x, double p3y )
 {
   double a = gs_angle ( p2x-p1x, p2y-p1y, p3x-p1x, p3y-p1y );
   double b = gs_angle ( p3x-p2x, p3y-p2y, p3x-p2x, p3y-p2y );
   double c = gPI-a-b;
   return gMIN3(a,b,c);
 }

double gs_triangle_max_angle ( double p1x, double p1y, double p2x, double p2y, double p3x, double p3y )
 {
   double a = gs_angle ( p2x-p1x, p2y-p1y, p3x-p1x, p3y-p1y );
   double b = gs_angle ( p3x-p2x, p3y-p2y, p3x-p2x, p3y-p2y );
   double c = gPI-a-b;
   return gMAX3(a,b,c);
 }

// circle_test :
// p1, p2, p3 must be in ccw order
// Calculates the following determinant :
//   | p1x   p1y   p1x*p1x + p1y*p1y   1.0 | 
//   | p2x   p2y   p2x*p2x + p2y*p2y   1.0 | 
//   | p3x   p3y   p3x*p3x + p3y*p3y   1.0 | 
//   | px    py    px*px   + py*py     1.0 | 
// This may not be the most accurate calculation, but is the fastest.
bool gs_in_circle ( double p1x, double p1y, double p2x, double p2y, double p3x, double p3y, double px, double py )
 {
   double p1z = p1x*p1x + p1y*p1y;
   double p2z = p2x*p2x + p2y*p2y;
   double p3z = p3x*p3x + p3y*p3y;
   double pz  = px*px   + py*py;

   double m12 = p2x*p3y - p2y*p3x;
   double m13 = p2x*p3z - p2z*p3x;
   double m14 = p2x     - p3x;
   double m23 = p2y*p3z - p2z*p3y;
   double m24 = p2y     - p3y;
   double m34 = p2z     - p3z;
   double d1  = p1y*m34 - p1z*m24 + m23;
   double d2  = p1x*m34 - p1z*m14 + m13;
   double d3  = p1x*m24 - p1y*m14 + m12;
   double d4  = p1x*m23 - p1y*m13 + p1z*m12;

   double det = d4 - px*d1 + py*d2 - pz*d3;

   const double prec = gINCIRCLEEPS; // Using an epsilon is very important
   return det>prec? true:false;
 }

bool gs_circle_center ( double p1x, double p1y, double p2x, double p2y, double p3x, double p3y,
                        double& cx, double& cy )
 {
   double v1x = (p2x-p1x)*0.5;
   double v1y = (p2y-p1y)*0.5;
   double v2x = (p3x-p1x)*0.5;
   double v2y = (p3y-p1y)*0.5;

   #define o1x -v1y // orthogonal vectors
   #define o1y v1x
   #define o2x -v2y
   #define o2y v2x

   return gs_lines_intersect ( p1x+v1x, p1y+v1y, p1x+v1x+o1x, p1y+v1y+o1y,
                               p1x+v2x, p1y+v2y, p1x+v2x+o2x, p1y+v2y+o2y,
                               cx, cy );
   #undef o1x
   #undef o1y
   #undef o2x
   #undef o2y
 }

bool gs_in_circle ( double p1x, double p1y, double p2x, double p2y, double p3x, double p3y,
                    double px, double py, double epsilon )
 {
   double cx, cy;
   if ( !gs_circle_center(p1x,p1y,p2x,p2y,p3x,p3y,cx,cy) ) return false;
   double r = sqrt(gsdist2(cx,cy,p1x,p1y))-epsilon;
   if ( r<0 ) return false;
   return gsdist2(cx,cy,px,py)<=r*r? true:false;
 }

double gs_tangent ( double px, double py, double cx, double cy, double r, 
                    double& t1x, double& t1y, double& t2x, double& t2y ) // (p,c,t1):CCW, (p,c,t2):CW
 {
   double d2 = gsdist2(px,py,cx,cy);
   double d = d2-r*r;
   if ( d<=0 ) return -1;
   double s = sqrt ( d );
   d = sqrt ( d2 );
   
   if ( d==0 ) return -1;
   cx = (cx-px)/d; // c-p unit vector 
   cy = (cy-py)/d;
   
   double sa = r/d; // get sinus and cosinus
   double ca = s/d;
   t1x = (cx*ca-cy*sa) * s + px; // rotate and scale
   t1y = (cx*sa+cy*ca) * s + py;
   sa=-sa;
   t2x = (cx*ca-cy*sa) * s + px; // now rotate to the other side
   t2y = (cx*sa+cy*ca) * s + py;

   /* code fragment to test this function:
   double tx1, ty1, tx2, ty2, px=1, py=1, cx=-5, cy=5, r=3;
   gs_tangent(px,py,cx,cy,r,tx1,ty1,tx2,ty2);
   gsout<<tx1<<","<<ty1<<gsnl;
   gsout<<tx2<<","<<ty2<<gsnl; */

   return s;
 }

double gs_external_tangents ( double c1x, double c1y, double r1, double c2x, double c2y, double r2, 
                              double& t1x, double& t1y, double& t2x, double& t2y,
                              double& t3x, double& t3y, double& t4x, double& t4y )
 {
   double s, r, vx, vy;
   if ( r1==r2 )
    {
      s = sqrt(gsdist2(c1x,c1y,c2x,c2y));
      if ( s==0 ) return -1;
      vy =  ((c2x-c1x)/s)*r1; // ortho vector
      vx = -((c2y-c1y)/s)*r1; 
      t1x=c1x-vx; t1y=c1y-vy; t2x=c2x-vx; t2y=c2y-vy;
      t3x=c1x+vx; t3y=c1y+vy; t4x=c2x+vx; t4y=c2y+vy;
    }
   else if ( r1>r2 )
    {
      r = r1-r2;
      s = gs_tangent ( c2x, c2y, c1x, c1y, r, t1x, t1y, t3x, t3y ); // (p,c,t1):CCW, (p,c,t2):CW
      if ( s<0 ) return -1;
      vx = ((t1x-c1x)/r)*r2;
      vy = ((t1y-c1y)/r)*r2;
      t1x+=vx; t1y+=vy; t2x=c2x+vx; t2y=c2y+vy; // (c1,t1,c2) is CCW
      vx = ((t3x-c1x)/r)*r2;
      vy = ((t3y-c1y)/r)*r2;
      t3x+=vx; t3y+=vy; t4x=c2x+vx; t4y=c2y+vy; // (c1,t3,c2) is CW
    }
   else // r2>r1
    {
      r = r2-r1;
      s = gs_tangent ( c1x, c1y, c2x, c2y, r, t4x, t4y, t2x, t2y ); // (p,c,t1):CCW, (p,c,t2):CW
      if ( s<0 ) return -1;
      vx = ((t4x-c2x)/r)*r1;
      vy = ((t4y-c2y)/r)*r1;
      t4x+=vx; t4y+=vy; t3x=c1x+vx; t3y=c1y+vy;
      vx = ((t2x-c2x)/r)*r1;
      vy = ((t2y-c2y)/r)*r1;
      t2x+=vx; t2y+=vy; t1x=c1x+vx; t1y=c1y+vy;
    }
   return s;
 }

double gs_internal_tangents ( double c1x, double c1y, double r1, double c2x, double c2y, double r2, 
                              double& t1x, double& t1y, double& t2x, double& t2y,
                              double& t3x, double& t3y, double& t4x, double& t4y )
 {
   double s, r, vx, vy;
   r = r1+r2;
   s = gs_tangent ( c2x, c2y, c1x, c1y, r, t1x, t1y, t3x, t3y ); // (p,c,t1):CCW, (p,c,t2):CW
   if ( s<0 ) return -1;
   vx = ((c1x-t1x)/r)*r2;
   vy = ((c1y-t1y)/r)*r2;
   t1x+=vx; t1y+=vy; t2x=c2x+vx; t2y=c2y+vy;
   vx = ((c1x-t3x)/r)*r2;
   vy = ((c1y-t3y)/r)*r2;
   t3x+=vx; t3y+=vy; t4x=c2x+vx; t4y=c2y+vy;
   return s;
 }

void gs_barycentric ( double p1x, double p1y, double p2x, double p2y, double p3x, double p3y,
                      double px, double py, double& u, double& v, double& w )
 {
   # define DET3(a,b,c,d,e,f,g,h,i) a*e*i +b*f*g +d*h*c -c*e*g -b*d*i -a*f*h
   double A  = DET3 ( p1x, p2x, p3x, p1y, p2y, p3y, 1, 1, 1 );
   double A1 = DET3 (  px, p2x, p3x,  py, p2y, p3y, 1, 1, 1 );
   double A2 = DET3 ( p1x,  px, p3x, p1y,  py, p3y, 1, 1, 1 );
   //double A3 = DET3 ( p1x, p2x,  px, p1y, p2y,  py, 1, 1, 1 );
   # undef DET3
   u = A1/A;
   v = A2/A;
   w = 1.0-u-v; // == A3/A;
 }

//=============================== Documentation ======================================

/*
Intersection of segments math:

t p1 + (1-t)p2 = p      (1)
s p3 + (1-s)p4 = p      (2)

=> Making (1)=(2) :
   t p1 + (1-t)p2 = s p3 + (1-s)p4 
   t(p1-p2) + s(p4-p3) = p4-p2
   t(p1x-p2x) + s(p4x-p3x) = p4x-p2x    (3)
   t(p1y-p2y) + s(p4y-p3y) = p4y-p2y    (4)

=> Putting t from (3) to (4) :
  t = [(p4x-p2x) - s(p4x-p3x)] / (p1x-p2x)
  [(p4x-p2x) - s(p4x-p3x)] / (p1x-p2x) = [(p4y-p2y) - s(p4y-p3y)] / (p1y-p2y)
  (p1y-p2y)(p4x-p2x) - s(p1y-p2y)(p4x-p3x) = (p4y-p2y)(p1x-p2x) - s(p4y-p3y)(p1x-p2x)
  s(p4y-p3y)(p1x-p2x) - s(p1y-p2y)(p4x-p3x) = (p4y-p2y)(p1x-p2x) - (p1y-p2y)(p4x-p2x)
  s = [(p4y-p2y)(p1x-p2x)-(p1y-p2y)(p4x-p2x)] / [(p4y-p3y)(p1x-p2x)-(p1y-p2y)(p4x-p3x)]
  Let d = (p4y-p3y)(p1x-p2x)-(p1y-p2y)(p4x-p3x)       (5)
  s = [(p4y-p2y)(p1x-p2x)-(p1y-p2y)(p4x-p2x)] / d     (6)

=> Putting s from (3) to (4) :
  s = [(p4x-p2x) - t(p1x-p2x)] / (p4x-p3x)
  [(p4x-p2x) - t(p1x-p2x)] / (p4x-p3x) =  [(p4y-p2y) - t(p1y-p2y)] / (p4y-p3y)
  (p4y-p3y)(p4x-p2x) - t(p4y-p3y)(p1x-p2x) = (p4x-p3x)(p4y-p2y) - t(p4x-p3x)(p1y-p2y)
  t(p4x-p3x)(p1y-p2y) - t(p4y-p3y)(p1x-p2x) = (p4x-p3x)(p4y-p2y) - (p4y-p3y)(p4x-p2x)
  t = [(p4x-p3x)(p4y-p2y)-(p4y-p3y)(p4x-p2x)] / [(p4x-p3x)(p1y-p2y)-(p4y-p3y)(p1x-p2x)]
  t = -1*[(p4x-p3x)(p4y-p2y)-(p4y-p3y)(p4x-p2x)] / -1*[(p4x-p3x)(p1y-p2y)-(p4y-p3y)(p1x-p2x)]
  t = [(p4y-p3y)(p4x-p2x)-(p4x-p3x)(p4y-p2y)] / [(p4y-p3y)(p1x-p2x)-(p4x-p3x)(p1y-p2y)]
  Using (5) :
  t = [(p4y-p3y)(p4x-p2x)-(p4x-p3x)(p4y-p2y)] / d     (7)

=> From (6) and (7), t and s determines p: 
  p = t p1 + (1-t)p2 = s p3 + (1-s)p4  */

//=============================== End of File ======================================
