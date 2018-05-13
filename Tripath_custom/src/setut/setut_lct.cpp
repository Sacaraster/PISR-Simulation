/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <stdlib.h>
# include <gsim/se_lct.h>
# include <math.h>
# include <time.h>

# ifdef SETUT_FLGLUT
# include <gsim/gs_ogl.h>
# include "setut_flglut.h"
# else
# include <GL/glut.h>
# endif

//******************************************************************
// Define some test examples:
//******************************************************************

# define END 12345.6               // this mark is ok for our examples

static double rtest = 0.9;

static double Example1[] =
       { -3.120, 2.040, 2.680, 2.040, 2.680, -3.600, -3.120, -3.600, END,
              0, -.290, .150, -.300, .150, -1.160, 0, -1.160, END,
          1.710, .310, 2.340, .290, 2.340, -1.010, 1.690, -1.030, END,
           .740, 1.160, .320, 1.160, .260, .370, -.280, .370, -.330, 1.160, -.730, 1.160, -.740, 1.780, .730, 1.780, END,
          1.490, -1.890, -.830, -1.880, -.840, -3.250, 1.490, -3.250, END,
         -2.120, -.510, -2.330, -.260, -2.850, -.180, -3.050, -.500, -2.830, -.810, -2.320, -.720, END,
          END };

static double Example2[] =
       { -3.226, -7.074, -3.226, 7.202, 11.701, 7.202, 11.701, -7.074, END,
          6.200, -7.074, 5.950, -7.074, 5.950, -4.579, 6.200, -4.579, END,
          6.200, -1.798, 5.950, -1.798, 5.950, 4.097, 6.200, 4.097, END,
          6.200, 5.432, 5.950, 5.432, 5.950, 7.202, 6.200, 7.202, END,
         -3.226, 1.250, -3.226, 1.550, -.815, 1.550, -.815, 1.250, END,
          2.127, 1.250, 2.127, 1.550, 8.366, 1.550, 8.366, 1.250, END,
          9.779, 1.250, 9.779, 1.550, 11.701, 1.550, 11.701, 1.250, END,
          3.797, 1.329, 3.797, .086, 3.890, .086, 3.890, 1.329, END,
           .554, 7.202, .554, 1.438, .653, 1.438, .653, 7.202, END,
         -3.226, -1.240, -3.226, -1.041, -1.279, -1.041, -1.279, -1.240, END,
           .203, -1.240, .203, -1.041, 5.970, -1.041, 5.970, -1.240, END,
           .361, 1.349, .820, 1.353, .819, 1.446, .360, 1.442, END,
          1.070, -3.140, 1.060, -3.290, .200, -3.290, .200, -3.140, END,
          4.010, -2.050, 4.030, -1.420, 5.330, -1.420, 5.350, -2.070, END,
          -.420, 5.560, -.840, 5.560, -.900, 4.770, -1.440, 4.770, -1.490, 5.560, -1.890, 5.560, -1.900, 6.180, -.430, 6.180, END,
          5.030, 2.770, 2.710, 2.780, 2.700, 1.410, 5.030, 1.410, END,
          2.800, 4.240, 4.570, 4.240, 4.520, 6.110, 2.800, 6.110, END,
          5.110, .420, 5.360, .630, 5.440, 1.150, 5.120, 1.350, 4.810, 1.130, 4.900, .620, END,
          5.156, -4.808, 2.687, -3.766, 2.247, -4.807, 3.638, -5.394, 3.432, -5.882, 4.510, -6.337, 4.666, -5.969, 5.126, -6.163, 5.476, -5.334, 5.016, -5.140, END,
           .916, 5.588, 1.926, 5.558, 2.109, 5.342, 2.096, 4.892, 1.706, 4.904, 1.697, 4.604, 2.094, 4.512, 2.087, 4.262, 1.693, 4.144, 1.684, 3.834, 2.060, 3.683, 1.836, 2.579, .827, 2.609, END,
         -2.880, -3.170, -2.570, -3.170, -.830, -3.470, -.830, -5.160, -2.570, -5.490, -2.880, -5.490, -2.880, -5.160, -2.880, -3.470, END,
          1.280, -1.770, 1.280, -1.150, 3.200, -1.150, 3.200, -1.770, END,
          END };

static double Example3[] =
       { -8.0, -8.0, -8.0, 8.0, 8.0, 8.0, 8.0, -8.0, END,
          6.029, -.306, 5.706, .629, 4.770, .305, 5.094, -.629, END,
          3.338, 4.487, 2.362, 4.651, 2.198, 3.675, 3.174, 3.511, END,
         -4.895, -3.200, -4.085, -3.768, -3.516, -2.957, -4.327, -2.389, END,
           .240, -6.057, .657, -5.159, -.240, -4.742, -.657, -5.640, END,
          4.775, -5.083, 5.083, -6.024, 6.024, -5.716, 5.716, -4.775, END,
         -3.526, 3.556, -4.323, 2.968, -3.735, 2.172, -2.938, 2.759, END,
         -1.0,-4.0,-1.0,4.0,1.0,4.0,1.0,-4.0, END,
         -7.0,0.0, END,
         -5.0,0.0, END,
          END };
          
static double Example4[] = 
        { -10.0, -10.0, -10.0, 10.0, 10.0, 10.0, 10.0, -10.0, END,
           -7.0, -10.0, -7.0, 6.0, -6.0, 7.0, -6.0, -10.0, END,
           -4.0, -7.0, -4.0, 10.0, -3.0, 10.0, -3.0, -7.0, END,
           -1.0, -10.0, -1.0, 7.0, 0.0, 7.0, 0.0, -10.0, END,
           0.0, 7.0, 8.0, 7.0, 8.0, 5.5, 0.0, 5.5, END,
           3.0, 3.5, 10.0, 3.5, 10.0, 3.0, 3.0, 3.0, END,
           2.0, 1.0, 4.0, 1.0, 4.0, -1.0, 2.0, -1.0, END,
           6.0, 1.0, 8.0, 1.0, 8.0, -1.0, 6.0, -1.0, END,
           3.0, -3.51, 4.99, -3.51, 4.99, -5.0, 3.0, -5.0, END,
           -9.51, -8.5, END,
           -7.49, -8.5, END,
           9.0, -5.99, END,
           9.0, -8.01, END,
           END };
           
static double Example5[] = 
        { -10.0, -10.0, -10.0, 10.0, 10.0, 10.0, 10.0, -10.0, END,
            -6.0, 3.01, END,
            -6.0, -3.01, END,
            2.99, 0.0, END,
            9.01, 0.0, END,
            END };

/*            
static double Example6[] = 
        { -10.0, -10.0, -10.0, 10.0, 10.0, 10.0, 10.0, -10.0, END,
           1.0, 0.0, 3.0, 4.0, 7.0, 7.0, 6.0, 7.2, 4.0, 7.0, 0.0, 5.0, END,
           3.0, 1.0, 3.5, -1.0, 4.0, 1.0, 3.4, 1.5, END,
           -4.0,2.0,-8.0,-4.0,-7.0,-6.0,-2.0,3.0, END,
           -1.5,-4.0,-1.5, -6.0,2.2,-5.0,0.2,-0.9, END,
            END };
*/            

static double Example6[] = 

        { -20.0, -20.0, -20.0, 20.0, 20.0, 20.0, 20.0, -20.0, END,
           2.0, 0.0, 6.0, 8.0, 14.0, 14.0, 12.0, 14.2, 8.0, 14.0, 0.0, 10.0, END,
           3.0, 1.0, 3.5, -1.0, 4.0, 1.0, 3.4, 1.5, END,
           -4.0,2.0,-8.0,-4.0,-7.0,-6.0,-2.0,3.0, END,
           -1.5,-4.0,-1.5, -6.0,2.2,-5.0,0.2,-0.9, END,
            END };
            
static double Example7[] = 
    { -20.0, -20.0, -20.0, 20.0, 20.0, 20.0, 20.0, -20.0, END,
    4.0, 4.0, 4.0, 12.0, 8.0, 12.0, 8.0, 4.0, END,
    4.0, -4.0, 4.0, -12.0, 8.0, -12.0, 8.0, -4.0, END,
    -4.0, 4.0, -8.0, 4.0, -8.0, -8.0, -4.0, -8.0, END,
    
    END };
    
static double Example8[] = 
    { -20.0, -20.0,     -20.0, 20.0,    20.0, 20.0,     20.0, -20.0, END,
    
    END };            

// Some global pointers to make life easier:
static SeLct *TheLct;
static GsPolygon CurPath;
static GsPolygon CurChannel;
static const double* CurExample = Example3;
static float CurX1=0, CurY1=0, CurX2=0, CurY2=0;
static int CurSelection=0; // -2,-1: moving point, >0: moving polygon
static float Radius=0.15f;
static bool CanMoveObst=false;

# define RMin 0.01f
//# define RMax 0.85f
# define RMax 3.00f
# define RInc 0.01f

//******************************************************************
// Create the cdt from data
//******************************************************************

static void create_lct ()
 {
   const double* data = CurExample;
   GsPolygon pol;

   // domain:
   while ( *data!=END ) { pol.push().set((float)data[0],(float)data[1]); data+=2; }
   TheLct->init ( pol, 0.00001f );

   while ( *++data!=END )
    { pol.size(0);
      while ( *data!=END )  { pol.push().set((float)data[0],(float)data[1]); data+=2; }
      int polyID;
      polyID = TheLct->insert_polygon ( pol );
      printf("Polygon ID = %d\n",polyID);
    }
 }
 
 static void create_lct( SeLct *TheLct )
 {
    const double *data = CurExample;
    GsPolygon pol;
    
    
    while( *data != END ){ pol.push().set( (float)data[0], (float)data[1]); data+=2;}
    TheLct->init(pol,0.00001f);
    
    while( *++data!= END )
    { pol.size(0);
        while( *data!=END){ pol.push().set( (float)data[0], (float)data[1]); data+=2;}
        TheLct->insert_polygon(pol);
    }
 }

//******************************************************************
// Retrieve a path
//******************************************************************

static float compute_funnel_path_length( GsPolygon newPath )
{
    float retVal;
    retVal = 0.0;
    for(int i=0;i<CurPath.size()-1;i++)
    {
      //printf("[x,y] = [%f,%f]\n",CurPath[i].x,CurPath[i].y);
      float dx, dy;
      dx = CurPath[i+1].x - CurPath[i].x;
      dy = CurPath[i+1].y - CurPath[i].y;
      retVal += sqrt( dx*dx + dy*dy );
//      printf("[x,y] = [%f,%f]\n",CurPath.x,CurPath.y);
    }
    //printf("Number of waypoints = %d\n",CurPath.size());
    return retVal;
    
}

static void get_path ( float x1, float y1, float x2, float y2 )
 {
   //printf ( "Getting path for %f,%f : %f,%f\n", x1, y1, x2, y2 );
   float retVal;
   time_t tStart, tEnd;
   tStart = clock();
   bool found = TheLct->search_channel ( x1, y1, x2, y2, Radius );

   if ( !found )
    { //printf ( "Endpoints cannot be connected with a free path!\n" );
      CurPath.size(0);
      CurChannel.size(0);
    }
   else
    { TheLct->get_channel_boundary ( CurChannel );
      //TheLct->make_funnel_path ( CurPath, Radius, GS_TORAD(15) );
      // GS_TORAD( XX ) ---- XX is the discretization angle of the arc in DEGREES
      TheLct->make_funnel_path ( CurPath, Radius, GS_TORAD(1) );
      
      retVal = compute_funnel_path_length( CurPath );
      //printf ( "OK.\n" );
    }
    tEnd = clock()-tStart;
    printf("Computation time = %f (ms)\n",(float)tEnd/(float)CLOCKS_PER_SEC*1000.0);
    printf("Total path length = %f\n",retVal);
 }
 
 static void get_path( float x1, float y1, float x2, float y2, float Radius, SeLct *TheLct )
 {
    bool found = TheLct->search_channel(x1, y1, x2, y2, Radius );
    
    if( !found )
    {
        CurPath.size(0);
        CurChannel.size(0);
    }
    else
    {
        TheLct->get_channel_boundary( CurChannel );
        TheLct->make_funnel_path( CurPath, Radius, GS_TORAD(1));
        
    }
 }

static void get_path( float x1, float y1, float x2, float y2, float Radius, SeLct *TheLct,
        GsPolygon *newPath, GsPolygon *newChannel )
{
    bool found = TheLct->search_channel(x1, y1, x2, y2, Radius );
    
    if( !found )
    {
        newPath->size(0);
        newChannel->size(0);
    }
    else
    {
        TheLct->get_channel_boundary( *newChannel );
        TheLct->make_funnel_path( *newPath, Radius, GS_TORAD(15));
    }
}

static int insert_heading_constraint( float xIn, float yIn, float heading, float Radius, SeLct *TheLct, bool direction )
{
    float radiusBuffer = 0.01f;
    float PI = 3.14159;
    

    float x0, y0, x1, y1, x2, y2, x3, y3;
    float xB, yB, xS, yS;
    
    float dir;
    if( direction )
        dir = -1.0;
    else
        dir = 1.0;
        
    
    xS = xIn + dir*(Radius + radiusBuffer)*cos( heading );
    yS = yIn + dir*(Radius + radiusBuffer)*sin( heading );
    
    xB = xIn + -1.0*dir*( radiusBuffer )*cos(heading);
    yB = yIn + -1.0*dir*( radiusBuffer )*sin(heading);
    
    x0 = xB + (Radius + radiusBuffer)*cos( heading + PI*0.5 );
    y0 = yB + (Radius + radiusBuffer)*sin( heading + PI*0.5 );
    
    x1 = xS + (Radius + radiusBuffer)*cos( heading + PI*0.5 );
    y1 = yS + (Radius + radiusBuffer)*sin( heading + PI*0.5 );
    
    x2 = xS + (Radius + radiusBuffer)*cos( heading - PI*0.5 );
    y2 = yS + (Radius + radiusBuffer)*sin( heading - PI*0.5 );
    
    x3 = xB + (Radius + radiusBuffer)*cos( heading - PI*0.5 );
    y3 = yB + (Radius + radiusBuffer)*sin( heading - PI*0.5 );
    
    GsPolygon temp;
    
    
    temp.push().set( x0, y0 );
    temp.push().set( x1, y1 );
    temp.push().set( x2, y2 );
    temp.push().set( x3, y3 );
    temp.push().set( x2, y2 );
    temp.push().set( x1, y1 );
    
    int polyID;
    polyID = TheLct->insert_polygon( temp );
    return polyID;
}



//******************************************************************
//  Glut Functions
//******************************************************************

static void glut_draw_newPath( GsPolygon *newPaths, int nPaths )
{
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    if ( TheLct->num_polygons()<0 ) return;

   // first adapt view matrix to view all the map triangulation:
   float xmin, xmax, ymin, ymax; 
   float mat[16] = { 1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, 1, 0,
                     0, 0, 0, 1 };
   TheLct->get_bounds ( xmin, xmax, ymin, ymax );
   glMatrixMode ( GL_MODELVIEW );
   glLoadIdentity ();

   float w = xmax-xmin;
   float h = ymax-ymin;
   mat[0]=mat[5]=mat[10]= 1.8f * (1/(w>h? w:h));
   glMultMatrixf ( mat );
   mat[0]=mat[5]=mat[10]= 1;

   mat[12]=-(xmin+w/2); mat[13]=-(ymin+h/2);
   glMultMatrixf ( mat );
   mat[12]=mat[13]=0;

   // draw edges:
   static GsArray<GsPnt2> cedges;
   static GsArray<GsPnt2> ucedges;
   TheLct->get_mesh_edges ( &cedges, &ucedges );

   // draw non-constrained edges:
   int i;
   glLineWidth ( 1.0f );
   glBegin ( GL_LINES );
   glColor3f ( 0.9f, 0.9f, 0.9f );     
   for ( i=0; i<ucedges.size(); i+=2 )
    { glVertex2fv ( &ucedges[i].x );
      glVertex2fv ( &ucedges[i+1].x );
    }
   glEnd ();

   // draw constrained edges:
   glLineWidth ( 2.0f );
   glBegin ( GL_LINES );
   glColor3f ( 0, 0, 0 ); 
   for ( i=0; i<cedges.size(); i+=2 )
    { glVertex2fv ( &cedges[i].x );
      glVertex2fv ( &cedges[i+1].x );
    }
   glEnd ();

   // draw the current channel:
   int size = CurChannel.size();
   glColor3f ( 0, 0, 0 );
   glLineWidth ( 1.5f );
   glBegin ( GL_LINE_STRIP );
   for ( i=0; i<size; i++ ) glVertex2f ( CurChannel[i].x, CurChannel[i].y );
   if ( size>0 ) glVertex2f ( CurChannel[0].x, CurChannel[0].y );
   glEnd ();
    
    // draw the current path:
   
   //glColor3f( 0, 1, 0 );
   int k;
   for(k=0;k<nPaths;k++)
   {
   GsPolygon newPath;
   newPath = newPaths[k];
   glLineWidth ( 3.0f );
   glBegin ( GL_LINE_STRIP );
   
   
   float pathColor;
   pathColor = (float)k/nPaths;
   //glColor3f( pathColor, 0, 1.0 - pathColor );
   glColor3f( 0, 0, 0.5f );
   for ( i=0; i<newPath.size(); i++ ) glVertex2f ( newPath[i].x, newPath[i].y );
   glEnd ();
   }
   

   glutSwapBuffers ();
}

static void GlutResizeWindow ( int w, int h )
 {
   glViewport( 0, 0, w, h );	// View port uses whole window
 }

static void GlutDraw ()
 {
   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   if ( TheLct->num_polygons()<0 ) return;

   // first adapt view matrix to view all the map triangulation:
   float xmin, xmax, ymin, ymax; 
   float mat[16] = { 1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, 1, 0,
                     0, 0, 0, 1 };
   TheLct->get_bounds ( xmin, xmax, ymin, ymax );
   glMatrixMode ( GL_MODELVIEW );
   glLoadIdentity ();

   float w = xmax-xmin;
   float h = ymax-ymin;
   mat[0]=mat[5]=mat[10]= 1.8f * (1/(w>h? w:h));
   glMultMatrixf ( mat );
   mat[0]=mat[5]=mat[10]= 1;

   mat[12]=-(xmin+w/2); mat[13]=-(ymin+h/2);
   glMultMatrixf ( mat );
   mat[12]=mat[13]=0;

   // draw edges:
   static GsArray<GsPnt2> cedges;
   static GsArray<GsPnt2> ucedges;
   TheLct->get_mesh_edges ( &cedges, &ucedges );

   // draw non-constrained edges:
   int i;
   glLineWidth ( 1.0f );
   glBegin ( GL_LINES );
   glColor3f ( 0.7f, 0.7f, 0.7f );   
   glColor3f (0.9f,0.9f,0.9f);  
   for ( i=0; i<ucedges.size(); i+=2 )
    { glVertex2fv ( &ucedges[i].x );
      glVertex2fv ( &ucedges[i+1].x );
    }
   glEnd ();

   // draw constrained edges:
   glLineWidth ( 2.0f );
   glBegin ( GL_LINES );
   glColor3f ( 1, 0, 0 ); 
   for ( i=0; i<cedges.size(); i+=2 )
    { glVertex2fv ( &cedges[i].x );
      glVertex2fv ( &cedges[i+1].x );
    }
   glEnd ();

   // draw the current channel:
   int size = CurChannel.size();
   glColor3f ( 0, 0, 0 );
   glLineWidth ( 1.5f );
   glBegin ( GL_LINE_STRIP );
   for ( i=0; i<size; i++ ) glVertex2f ( CurChannel[i].x, CurChannel[i].y );
   if ( size>0 ) glVertex2f ( CurChannel[0].x, CurChannel[0].y );
   glEnd ();

   // draw the current path:
   glLineWidth ( 3.0f );
   glBegin ( GL_LINE_STRIP );
   glColor3f ( 0, 0, 1 );
   //glColor3f( 0, 1, 0 );
   for ( i=0; i<CurPath.size(); i++ ) glVertex2f ( CurPath[i].x, CurPath[i].y );
   glEnd ();

   glutSwapBuffers ();
 }

static bool valid ( float x, float y )
 {
   float xmin, xmax, ymin, ymax; 

   TheLct->get_bounds ( xmin, xmax, ymin, ymax );
   if ( !(x>xmin && x<xmax && y>ymin && y<ymax) ) return false;

   return true;
 }

static void translate_coords ( float& x, float& y, int w, int h )
 {
   // Translate from glut coords:
   float fx = 2.0f* x/w -1.0f;
   float fy = 2.0f* y/h -1.0f;
   fx/=1.8f;
   fy/=1.8f;
   fy = -fy;

   // Clip to avoid sending points outside the domain:
   float xmin, xmax, ymin, ymax; 
   TheLct->get_bounds ( xmin, xmax, ymin, ymax );
   float mw = xmax-xmin;
   float mh = ymax-ymin;
   float s = (mw>mh? mw:mh);
   float tx = (xmin+mw/2);
   float ty = (ymin+mh/2);
   x = fx*s + tx;
   y = fy*s + ty;
   if ( x<xmin ) x=xmin;
   if ( x>xmax ) x=xmax;
   if ( y<ymin ) y=ymin;
   if ( y>ymax ) y=ymax;
 }
 
static int _int_factorial( int n )
{
    return (n==1 || n==0) ? 1 : _int_factorial(n-1) + n;
}

void static get_random_point( float *xI, float *yI, float *psiI, 
    float xLower, float xUpper, float yLower, float yUpper )
{
    *xI = (float)rand()/(float)RAND_MAX*(xUpper - xLower) + xLower;
    *yI = (float)rand()/(float)RAND_MAX*(yUpper - yLower) + yLower;
    *psiI = (float)rand()/(float)RAND_MAX*2.0*3.14159 + 0.0;
    
}

void static run_random_tests( GsPolygon *newPath, GsPolygon *newChannel, int nPaths )
{
    time_t  tStart, tEnd;
    
    
    //int nPaths = numberOfTests;
    //int nChannels = numberOfTests;
    
    
    
    Radius = 1.0f;
    
    float xRand[nPaths], yRand[nPaths], psiRand[nPaths];
    
    int k;
    for(k=0;k<nPaths;k++)
    {
        get_random_point( &xRand[k], &yRand[k], &psiRand[k], -20.0 + Radius, 
        20.0 - Radius, 
        -20.0 + Radius, 
        20.0 - Radius );
    }
    
    /*
    printf("Check points...\n");
    for(k=0;k<nPaths;k++)
    {
        printf("Point %2d - [%f,%f,%f]\n",k,xRand[k],yRand[k],psiRand[k]);
    }
    */
    
    int polyStart, polyFinal;
    int j;
    tStart = clock();
    for(k=0;k<nPaths;k++)
    {
        j = (k+1)%nPaths;
        polyStart = insert_heading_constraint(xRand[k],yRand[k],psiRand[k],Radius,TheLct,1);
        polyFinal = insert_heading_constraint(xRand[j],yRand[j],psiRand[j],Radius,TheLct,0);
        
        get_path( xRand[k], yRand[k], xRand[j], yRand[j], Radius, TheLct, &newPath[k], &newChannel[k] );
        TheLct->remove_polygon( polyStart);
        TheLct->remove_polygon( polyFinal);
    }
    tEnd = clock() - tStart;
    
    float finalTime;
    finalTime = (float)tEnd/(float)CLOCKS_PER_SEC*1000.0;
    
    printf("Computation time for %d paths: %f (ms)\n",nPaths,finalTime);
    
    
}

static void GlutKeyboard ( unsigned char key, int x, int y )
 {
   const double* data = 0;
   bool newpath=false;
   
   bool newpath2=false;
   bool newpath3=false;
   bool newpathG=false;
   bool newpathN=false;
   bool newpathM=false;
   bool newpathO=false;
   bool newpathP=false;
   bool newpathR=false;

   switch ( key )
    { case 27 : exit(0); // esc key
      case '1' : data=Example1; break;
      case '2' : data=Example2; break;
      case '3' : data=Example3; break;
      case '4' : data=Example4; break;
      case '5' : data=Example5; break;
      case '6' : data=Example6; break;
      case '7' : data=Example7; break;
      case 'a' : Radius+=RInc; if (Radius>RMax) Radius=RMax; newpath=true; break;
      case 'z' : Radius-=RInc; if (Radius<RMin) Radius=RMin; newpath=true; break;
      case 'm' : CanMoveObst=!CanMoveObst; 
                 if (CanMoveObst) printf("\nNote: Dynamic updates in LCTs are still in experimental mode!\n");
                  else printf("Dynamic updates off\n");
                 break;
      case 'q' : newpath2=true; break;
      case 'w' : newpath3=true; break;
      case 'g' : newpathG=true; break;
      case 'n' : newpathN=true; break;
      case 'b' : newpathM=true; break;
      case 'o' : newpathO=true; break;
      case 'p' : newpathP=true; break; 
      case 'r' : newpathR=true; break;     
    }

   if ( data )
    { CurExample = data;
      CurX1 = CurY1 = CurX2 = CurY2 = 0;
      CurPath.size(0); CurChannel.size(0);
      create_lct ();
      glutPostRedisplay();
    }

   if ( newpath )
    { get_path ( CurX1, CurY1, CurX2, CurY2 );
      glutPostRedisplay();
      printf ( "Clearance: %f\n", Radius );
    }
    
    if( newpath2 )
    {
        CurX1 = -8.5;   CurY1 = -8.5;
        CurX2 = 9.0;    CurY2 = -7.0;
        Radius = 1.0f;
        get_path( CurX1, CurY1, CurX2, CurY2 );
        glutPostRedisplay();
        
    }
    
    if( newpathR )
    {
        
        int nPaths = 1000;
        
        GsPolygon *newChannel;
        newChannel = new GsPolygon[nPaths];
        GsPolygon *newPath;
        newPath = new GsPolygon[nPaths];
        
        run_random_tests(newPath,newChannel,nPaths);
        
        glut_draw_newPath( newPath, 50 );
        //delete newPath;
        //delete newChannel;
    }
    
    if( newpath3 )
    {
        CurX1 = -6.0;   CurY1 = 0.0;
        CurX2 = 6.0;    CurY2 = 0.0;
        Radius = 3.0f;
        get_path( CurX1, CurY1, CurX2, CurY2 );
        glutPostRedisplay();
    }
    
    if( newpathG )
    {
        CurX1 = -9.0;   CurY1 = -9.0;
        CurX2 = 9.0;    CurY2 = 9.0;
        Radius = 0.5f;
        get_path( CurX1, CurY1, CurX2, CurY2 );
        //glutPostRedisplay();
        
        CurX1 = 9.0;   CurY1 = -9.0;
        CurX2 = 4.2;    CurY2 = 6.0;
        Radius = 0.5f;
        get_path( CurX1, CurY1, CurX2, CurY2 );
        //glutPostRedisplay();
        
        GsPolygon newChannel;
        GsPolygon newPath;
        
        CurX1 = 2.0;   CurY1 = -9.0;
        CurX2 = 3.2;    CurY2 = 3.0;
        Radius = 0.5f;
        
        get_path( CurX1, CurY1, CurX2, CurY2, Radius, TheLct, &newPath, &newChannel);
        
        //glut_draw_newPath( newPath );
        //glutPostRedisplay();
    }
    
    if( newpathM )
    {
        int nPts;
        nPts = 8;
        float xVec[nPts], yVec[nPts];
        xVec[0] = -9.0;     yVec[0] = -5.9;
        xVec[1] = 8.0;      yVec[1] = 8.0;
        xVec[2] = 0.0;      yVec[2] = -7.0;
        xVec[3] = 4.2;      yVec[3] = 8.0;
        xVec[4] = -2.0;     yVec[4] = -2.0;
        xVec[5] = 3.2;      yVec[5] = 3.0;
        xVec[6] = -8.0;     yVec[6] = 0.0;
        xVec[7] = 9.0;      yVec[7] = -3.0;
        
        
        time_t tStart, tEnd;
        tStart = clock();
        int nPaths;
        nPaths = _int_factorial(nPts-1);
        int nChannels;
        
        GsPolygon *newChannel;
        newChannel = new GsPolygon [nPaths];
        GsPolygon *newPath;
        newPath = new GsPolygon [nPaths];
        
        Radius = 0.6f;
        
        int k; int j; int n;
        n = 0;
        for( k = 0; k<nPts;k++)
        {
            for(j = k+1;j<nPts;j++)
            {
                get_path( xVec[k], yVec[k], xVec[j], yVec[j], Radius, TheLct, &newPath[n], &newChannel[n]);
                
                n+=1;
            }
        }
        
        tEnd = clock()-tStart;
        printf("Time to compute %d path is: %f (ms)\n",nPaths,(float)tEnd/(float)CLOCKS_PER_SEC*1000.0);
        glut_draw_newPath( newPath, nPaths );
        
        
    }
    
    if( newpathO )
    {
        double x0, y0, x1, y1;
        x0 = -10.0; y0 = -14.0;
        x1 = 15.0; y1 = 10.0;
        
        GsPolygon *newChannel;
        newChannel = new GsPolygon;
        GsPolygon *newPath;
        newPath = new GsPolygon;
        
        Radius = 4.0f;
        
        double PI;
        PI = 3.14159;
        
        double radiusBuffer = 0.00001;
        
        double psi0, psi1;
        psi0 = 0.0*PI;
        psi1 = 0.5*PI;
        
        double hConX0_L, hConY0_L, hConX1_L, hConY1_L;
        double hConX0_R, hConY0_R, hConX1_R, hConY1_R;
        hConX0_L = x0 + (Radius + radiusBuffer)*cos( psi0 + PI*0.5 );
        hConY0_L = y0 + (Radius + radiusBuffer)*sin( psi0 + PI*0.5 );
        
        hConX1_L = x1 + (Radius + radiusBuffer)*cos( psi1 + PI*0.5 );
        hConY1_L = y1 + (Radius + radiusBuffer)*sin( psi1 + PI*0.5 );
        
        hConX0_R = x0 + (Radius + radiusBuffer)*cos( psi0 - PI*0.5 );
        hConY0_R = y0 + (Radius + radiusBuffer)*sin( psi0 - PI*0.5 );
        
        hConX1_R = x1 + (Radius + radiusBuffer)*cos( psi1 - PI*0.5 );
        hConY1_R = y1 + (Radius + radiusBuffer)*sin( psi1 - PI*0.5 );
        
        GsPolygon constraintPoly;
        int polyIDstart_L, polyIDstart_R;
        
        constraintPoly.push().set((float)hConX0_L,(float)hConY0_L);
        constraintPoly.push().set((float)hConX0_L + radiusBuffer*Radius*cos(psi0),(float)hConY0_L + radiusBuffer*Radius*sin(psi0));
        
        polyIDstart_L = TheLct->insert_polygon ( constraintPoly );
        
        constraintPoly.size(0);
        constraintPoly.push().set((float)hConX0_R,(float)hConY0_R);
        constraintPoly.push().set((float)hConX0_R + radiusBuffer*Radius*cos(psi0),(float)hConY0_R + radiusBuffer*Radius*sin(psi0));
        polyIDstart_R = TheLct->insert_polygon( constraintPoly );
        
        
        int polyIDend_L, polyIDend_R;
        
        constraintPoly.size(0);
        constraintPoly.push().set((float)hConX1_L,(float)hConY1_L);
        constraintPoly.push().set((float)hConX1_L - radiusBuffer*Radius*cos(psi1),(float)hConY1_L - radiusBuffer*Radius*sin(psi1));
        polyIDend_L = TheLct->insert_polygon ( constraintPoly );
        
        constraintPoly.size(0);
        constraintPoly.push().set((float)hConX1_R,(float)hConY1_R);
        constraintPoly.push().set((float)hConX1_R - radiusBuffer*Radius*cos(psi1),(float)hConY1_R - radiusBuffer*Radius*sin(psi1));
        polyIDend_R = TheLct->insert_polygon ( constraintPoly );
        
        TheLct->refine(true);
        TheLct->compute_clearance();
        
        
        get_path( x0, y0, x1, y1, Radius, TheLct, newPath, newChannel );
        
        glut_draw_newPath( newPath, 1 );
    }
    
    if( newpathP )
    {
        double x0, y0, x1, y1;
        x0 = -12.0; y0 = -5.0;
        x1 = 15.0; y1 = 10.0;
        double x2, y2, x3, y3;
        x2 = 13.0; y2 = -10.0;
        x3 = -12.0; y3 = 14.0;
        
        
        GsPolygon *newChannel;
        newChannel = new GsPolygon[4];
        GsPolygon *newPath;
        newPath = new GsPolygon[4];
        
        Radius = 0.25f;
        
        double PI;
        PI = 3.14159;
        
        double radiusBuffer = 0.00001;
        
        double psi0, psi1;
        psi0 = 1.5*PI;
        psi1 = 0.5*PI;
        double psi2, psi3;
        psi2 = 0.5*PI;
        psi3 = 1.25*PI;
        
        time_t tStart, tEnd;
        tStart = clock();
        
        int polyStart, polyFinal;
        polyStart = insert_heading_constraint( x0, y0, psi0, Radius, TheLct, 1 );
        polyFinal = insert_heading_constraint( x1, y1, psi1, Radius, TheLct, 0 );
        
        TheLct->compute_clearance();
        get_path( x0, y0, x1, y1, Radius, TheLct, newPath, newChannel);
        
        TheLct->remove_polygon( polyStart );
        TheLct->remove_polygon( polyFinal );
        
        polyStart = insert_heading_constraint( x0, y0, psi0, Radius, TheLct, 0 );
        polyFinal = insert_heading_constraint( x1, y1, psi1, Radius, TheLct, 1 );
        
        TheLct->compute_clearance();
        get_path( x0, y0, x1, y1, Radius, TheLct, &newPath[1], &newChannel[1]);
        
        TheLct->remove_polygon( polyStart );
        TheLct->remove_polygon( polyFinal );
        
        
        polyStart = insert_heading_constraint( x2, y2, psi2, Radius, TheLct, 1 );
        polyFinal = insert_heading_constraint( x3, y3, psi3, Radius, TheLct, 0 );
        
                TheLct->compute_clearance();
        get_path( x2, y2, x3, y3, Radius, TheLct, &newPath[2], &newChannel[2]);
        
        TheLct->remove_polygon( polyStart );
        TheLct->remove_polygon( polyFinal );
        
        polyStart = insert_heading_constraint( x2, y2, psi2, Radius, TheLct, 0 );
        polyFinal = insert_heading_constraint( x3, y3, psi3, Radius, TheLct, 1 );
        
                TheLct->compute_clearance();
        get_path( x2, y2, x3, y3, Radius, TheLct, &newPath[3], &newChannel[3]);
        
        TheLct->remove_polygon( polyStart );
        TheLct->remove_polygon( polyFinal );
        
        
        
        tEnd = clock()-tStart;
        printf("Time to compute %d path is: %f (ms)\n",2,(float)tEnd/(float)CLOCKS_PER_SEC*1000.0);
        
        glut_draw_newPath( newPath, 4 );
    }
    
    if( newpathN )
    {
//        GsPolygon newChannel[4];
//        GsPolygon newPath[4];
        
//        GsPolygon *newChannel;
//        newChannel = (GsPolygon*)malloc(sizeof(GsPolygon)*4);
//        GsPolygon *newPath;
//        newPath = (GsPolygon*)malloc(sizeof(GsPolygon)*4);

        
        
        time_t tStart, tEnd;
        tStart = clock();
        int nPaths = 4;
        int nChannels;
        
        GsPolygon *newChannel;
        newChannel = new GsPolygon [nPaths];
        GsPolygon *newPath;
        newPath = new GsPolygon [nPaths];
        
        
        
        Radius = 0.25f;
        
        CurX1 = -9.0;   CurY1 = -5.0;
        CurX2 = 8.0;    CurY2 = 8.0;

        
        get_path( CurX1, CurY1, CurX2, CurY2, Radius, TheLct, &newPath[0], &newChannel[0] );
//        get_path( CurX1, CurY1, CurX2, CurY2, Radius, TheLct, newPath+0, newChannel+0 );
        
        CurX1 = 0.0;   CurY1 = -7.0;
        CurX2 = 4.2;    CurY2 = 8.0;
        
        get_path( CurX1, CurY1, CurX2, CurY2, Radius, TheLct, &newPath[1], &newChannel[1] );
//        get_path( CurX1, CurY1, CurX2, CurY2, Radius, TheLct, newPath+1, newChannel+1 );
        
        CurX1 = -2.0;   CurY1 = -2.0;
        CurX2 = 3.2;    CurY2 = 3.0;
        
        get_path( CurX1, CurY1, CurX2, CurY2, Radius, TheLct, &newPath[2], &newChannel[2] );
//        get_path( CurX1, CurY1, CurX2, CurY2, Radius, TheLct, newPath+2, newChannel+2 );
        
        CurX1 = -8.0;   CurY1 = 0.0;
        CurX2 = 9.0;    CurY2 = -3.0;

        
        get_path( CurX1, CurY1, CurX2, CurY2, Radius, TheLct, &newPath[3], &newChannel[3] );
//        get_path( CurX1, CurY1, CurX2, CurY2, Radius, TheLct, newPath+3, newChannel+3 );
        tEnd = clock()-tStart;
        printf("Time to compute %d path is: %f (ms)\n",nPaths,(float)tEnd/(float)CLOCKS_PER_SEC*1000.0);
        glut_draw_newPath( newPath, 4 );
    }
 }
 
 

static bool move_selection ( const GsPnt2& p )
 {
   if ( CurSelection>0 && CanMoveObst ) // move obstacle (note: id 0 is the domain)
    { static GsPolygon pol;
      TheLct->get_polygon ( CurSelection, pol );
      GsPnt2 c = pol.centroid();
      pol.translate ( p-c );
      bool ok=true;
      int i;
      for ( i=0; i<pol.size(); i++ ) // check if polygon can be moved
       if ( !::valid(pol[i].x,pol[i].y) ) { ok=false; break; }
      if ( ok )
       { TheLct->remove_polygon ( CurSelection );
         TheLct->insert_polygon ( pol );
       }
    }
   else if ( CurSelection==-1 )
    { CurX1=p.x; CurY1=p.y;
    }
   else if ( CurSelection==-2 )
    { CurX2=p.x; CurY2=p.y;
    }
   else return false;

   get_path ( CurX1, CurY1, CurX2, CurY2 );
   return true;
 }

static void GlutMouse ( int button, int state, int x, int y )
 {
   if ( state==GLUT_UP ) { CurSelection=0; return; }

   int w = glutGet(GLUT_WINDOW_WIDTH);
   int h = glutGet(GLUT_WINDOW_HEIGHT);
   GsPnt2 p ( (float)x, (float)y );
   translate_coords ( p.x, p.y, w, h );

   CurSelection = TheLct->inside_polygon ( p.x, p.y );
   if ( CurSelection<0 )
    { CurSelection = dist(p,GsPnt2(CurX1,CurY1))<dist(p,GsPnt2(CurX2,CurY2))? -1:-2;
    }
   if ( move_selection(p) ) glutPostRedisplay();
 }

static void GlutDrag ( int x, int y )
 {
   int w = glutGet(GLUT_WINDOW_WIDTH);
   int h = glutGet(GLUT_WINDOW_HEIGHT);
   GsPnt2 p ( (float)x, (float)y );
   translate_coords ( p.x, p.y, w, h );
   if ( move_selection(p) ) glutPostRedisplay();
 }

//******************************************************************
// The Main Function
//******************************************************************

void lct_main ()
 {
   printf ("Keys 1-3 change the example data.\n");
   printf ("Paths are obtained by clicking in valid locations.\n");
   printf ("Keys a,z change the clarance of the current path.\n");
   printf ("The black polygon represents the channel obtained for the path.\n");

   TheLct = new SeLct;
//   create_lct ();
   create_lct( TheLct );

   // initialize glut:
   glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
   glutInitWindowPosition ( 700, 100 );
   //glutInitWindowSize( 640, 480 );
   //glutInitWindowSize( 1280, 640 );
   glutInitWindowSize( 1280, 1280 );
  // glutCreateWindow ( "GSim Local Clearance Triangulation Example - M. Kallmann" );
   glutCreateWindow ( "UAS Path Planning" );
   glutReshapeFunc ( GlutResizeWindow );
   glutDisplayFunc ( GlutDraw );
   glutKeyboardFunc( GlutKeyboard );
   glutMouseFunc   ( GlutMouse );
   glutMotionFunc  ( GlutDrag );

   // configure opengl:
   glDisable ( GL_DEPTH_TEST ); // to make sure the order in drawing is taken!
   glEnable ( GL_LINE_SMOOTH );
   glHint ( GL_LINE_SMOOTH_HINT, GL_NICEST );
   glClearColor ( 1, 1, 1, 0 );

   // run:
   glutMainLoop ();
 }

//******************************************************************
// File Format Example (of example 2)
//******************************************************************
/* 
SeDcdt

# domain:1 polygons:5

epsilon 0.001

domain
-3.12 2.04 2.68 2.04 2.68 -3.6 -3.12 -3.6;

polygon 1
0.0 -0.29 0.15 -0.3 0.15 -1.16 0.0 -1.16;

polygon 2
1.71 0.31 2.34 0.29 2.34 -1.01 1.69 -1.03;

polygon 3
0.74 1.16 0.32 1.16 0.26 0.37 -0.28 0.37 -0.33 1.16 -0.73 1.16 -0.74 1.78 0.73 1.78;

polygon 4
1.49 -1.89 -0.83 -1.88 -0.84 -3.25 1.49 -3.25;

polygon 5
-2.12 -0.51 -2.33 -0.26 -2.85 -0.18 -3.05 -0.5 -2.83 -0.81 -2.32 -0.72;
*/

