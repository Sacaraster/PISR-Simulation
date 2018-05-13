/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <stdlib.h>
# include <gsim/se_dcdt.h>

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
# define FIRST_EXAMPLE  Example5

static double Example1[] =
       { -10, -10, 10, -10, 10, 10, -10, 10, END,
           1, 1, 7, 3, 3, 8, END,
         END };

static double Example2[] =
       { -3.120, 2.040, 2.680, 2.040, 2.680, -3.600, -3.120, -3.600, END,
              0, -.290, .150, -.300, .150, -1.160, 0, -1.160, END,
          1.710, .310, 2.340, .290, 2.340, -1.010, 1.690, -1.030, END,
           .740, 1.160, .320, 1.160, .260, .370, -.280, .370, -.330, 1.160, -.730, 1.160, -.740, 1.780, .730, 1.780, END,
          1.490, -1.890, -.830, -1.880, -.840, -3.250, 1.490, -3.250, END,
         -2.120, -.510, -2.330, -.260, -2.850, -.180, -3.050, -.500, -2.830, -.810, -2.320, -.720, END,
          END };

static double Example3[] =
       { -3.226, -7.074, -3.226, 7.202, 11.701, 7.202, 11.701, -7.074, END,
          1.710, .310, 2.340, .290, 2.340, -1.010, 1.690, -1.030, END,
          1.490, -1.890, -.830, -1.880, -.840, -3.250, 1.490, -3.250, END,
          6.200, 5.432,  5.950, 5.432, 5.950, 7.202, 6.200, 7.202, END,
          6.200, -1.798, 5.950, -1.798, 5.950, 4.097, 6.200, 4.097, END,
          6.200, -7.074, 5.950, -7.074, 5.950, -4.579, 6.200, -4.579, END,
          5.950, -6.629, 5.950, 7.006, 6.200, 7.006, 6.200, -6.629, END,
          END };

static double Example4[] =
       { -3.226, -7.074, -3.226, 7.202, 11.701, 7.202, 11.701, -7.074, END,
          6.200, -7.074, 5.950, -7.074, 5.950, -4.579, 6.200, -4.579, END,
          6.200, -1.798, 5.950, -1.798, 5.950, 4.097, 6.200, 4.097, END,
          6.200, 5.432, 5.950, 5.432, 5.950, 7.202, 6.200, 7.202, END,
         -3.226, 1.250, -3.226, 1.550, -.815, 1.550, -.815, 1.250, END,
          2.127, 1.250, 2.127, 1.550, 8.366, 1.550, 8.366, 1.250, END,
          9.779, 1.250, 9.779, 1.550, 11.701, 1.550, 11.701, 1.250, END,
          3.797, 1.329, 3.797, .086, 3.890, .086, 3.890, 1.329, END,
          0.554, 7.202, .554, 1.438, .653, 1.438, .653, 7.202, END,
         -3.226, -1.240, -3.226, -1.041, -1.279, -1.041, -1.279, -1.240, END,
          0.203, -1.240, .203, -1.041, 5.970, -1.041, 5.970, -1.240, END,
          0.361, 1.349, .820, 1.353, .819, 1.446, .360, 1.442, END,
          END };

static double Example5[] =
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

static double Example6[] =
       { -8, -8, -8, 8, 8, 8, 8, -8, END,
          6.029, -.306, 5.706, .629, 4.770, .305, 5.094, -.629, END,
          3.338, 4.487, 2.362, 4.651, 2.198, 3.675, 3.174, 3.511, END,
         -4.895, -3.200, -4.085, -3.768, -3.516, -2.957, -4.327, -2.389, END,
           .240, -6.057, .657, -5.159, -.240, -4.742, -.657, -5.640, END,
          4.775, -5.083, 5.083, -6.024, 6.024, -5.716, 5.716, -4.775, END,
         -3.526, 3.556, -4.323, 2.968, -3.735, 2.172, -2.938, 2.759, END,
          END };

static double Example7[] = 
       { -10, -10, 10, -10, 10, 10, -10, 10, END,
         -4.098, -8.630, 8.832, -8.688, 7.389, -5.109, -0.404, -7.245, 4.041, -7.360, -3.175, -5.051, END,
         -5.888, 7.995, -6.004, 2.973, 6.234, 2.915, 5.715, 8.573, 0, 8.5, 0, 7, 3.925, 6.725, 4.503, 4.416, -4.387, 4.416, -4.387, 7.014, 0, 7, 0, 8.5, END,
         -8.140, 0.432, -5.253, 0.432, -6.927, -1.876, END,
         -6.927, -1.876, -8.255, -4.589, -5.542, -4.358, END,
         -1.551, 0.556, -1.841, -4.054, 5.449, -4.088, 4.902, 0.607, 1.9, -0.5, 3.861, -2.637, 0.019, -2.602, 1.9, -0.5, END,
          1.9, -0.5, 0.241, 0.846, 0.446, 1.580, 1.180, 1.956, 2.119, 1.887, 3.058, 1.495, 3.229, 0.812, END,
         -3.867, 1.298, -4.445, -0.894, -2.886, -0.779, -2.886, 1.241, END,
         -3.463, -0.144, -3.579, -1.760, -2.309, -1.645, -2.424, 0.086, END,
          0.0, 9.265, 0.0, 5.917, -2.193, 5.802, -2.482, 9.092, END,
         -9, -6, -6, -6, -8, -8, END,
         -7, -6, -4, -6, -6, -8, END,
          END };

// Some global pointers to make life easier:
static SeDcdt *TheDcdt;
static GsPolygon CurPath;
static GsPolygon CurChannel;
static const double* CurExample = FIRST_EXAMPLE;
static float CurX1=0, CurY1=0, CurX2=0, CurY2=0;
static int   CurSelection=0; // -2,-1: moving point, >0: moving polygon

//******************************************************************
// Create the cdt from data
//******************************************************************

static void create_dcdt ()
 {
   const double* data = CurExample;
   GsPolygon pol;

   // domain:
   while ( *data!=END ) { pol.push().set((float)data[0],(float)data[1]); data+=2; }
   TheDcdt->init ( pol, 0.00001f );

   while ( *++data!=END )
    { pol.size(0);
      while ( *data!=END )  { pol.push().set((float)data[0],(float)data[1]); data+=2; }
      TheDcdt->insert_polygon ( pol );
    }
 }

//******************************************************************
// Retrieve a path
//******************************************************************

static void get_path ( float x1, float y1, float x2, float y2 )
 {
   //printf ( "Getting path for %f,%f : %f,%f\n", x1, y1, x2, y2 );

   bool found = TheDcdt->search_channel ( x1, y1, x2, y2, 0/*iniface*/ );

   if ( !found )
    { //printf ( "Endpoints cannot be connected with a free path!\n" );
      CurPath.size(0);
      CurChannel.size(0);
    }
   else
    { TheDcdt->get_channel_boundary ( CurChannel );
      TheDcdt->make_funnel_path ( CurPath );
      //printf ( "OK.\n" );
    }
 }

//******************************************************************
//  Glut Functions
//******************************************************************

static void GlutResizeWindow ( int w, int h )
 {
   glViewport( 0, 0, w, h );	// View port uses whole window
 }

static void GlutDraw ()
 {
   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   if ( TheDcdt->num_polygons()<0 ) return;

   // first adapt view matrix to view all the map triangulation:
   float xmin, xmax, ymin, ymax; 
   float mat[16] = { 1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, 1, 0,
                     0, 0, 0, 1 };
   TheDcdt->get_bounds ( xmin, xmax, ymin, ymax );
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
   TheDcdt->get_mesh_edges ( &cedges, &ucedges );

   // draw non-constrained edges:
   int i;
   glLineWidth ( 1.0f );
   glBegin ( GL_LINES );
   glColor3f ( 0.7f, 0.7f, 0.7f );     
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
   for ( i=0; i<CurPath.size(); i++ ) glVertex2f ( CurPath[i].x, CurPath[i].y );
   glEnd ();

   glutSwapBuffers ();
 }

static bool valid ( float x, float y )
 {
   float xmin, xmax, ymin, ymax; 

   TheDcdt->get_bounds ( xmin, xmax, ymin, ymax );
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
   TheDcdt->get_bounds ( xmin, xmax, ymin, ymax );
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

static void GlutKeyboard ( unsigned char key, int x, int y )
 {
   const double* data = 0;
   switch ( key )
    { case 27 : exit(0); // esc key
      case '1' : data=Example1; break;
      case '2' : data=Example2; break;
      case '3' : data=Example3; break;
      case '4' : data=Example4; break;
      case '5' : data=Example5; break;
      case '6' : data=Example6; break;
      case '7' : data=Example7; break;
    }

   if ( data )
    { CurExample = data;
      CurX1 = CurY1 = CurX2 = CurY2 = 0;
      CurPath.size(0); CurChannel.size(0);
      create_dcdt ();
      glutPostRedisplay();
    }
 }

static bool move_selection ( const GsPnt2& p )
 {
   if ( CurSelection>0 ) // move obstacle (note: id 0 is the domain)
    { static GsPolygon pol;
      TheDcdt->get_polygon ( CurSelection, pol );
      GsPnt2 c = pol.centroid();
      pol.translate ( p-c );
      bool ok=true;
      int i;
      for ( i=0; i<pol.size(); i++ ) // check if polygon can be moved
       if ( !::valid(pol[i].x,pol[i].y) ) { ok=false; break; }
      if ( ok )
       { TheDcdt->remove_polygon ( CurSelection );
         TheDcdt->insert_polygon ( pol );
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

   CurSelection = TheDcdt->inside_polygon ( p.x, p.y );
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

void dcdt_main ()
 {
   printf ("Keys 1-7 change the example data.\n");
   printf ("Obstacles can be dragged with the mouse.\n");
   printf ("Paths are obtained by clicking in valid locations.\n");
   printf ("The black polygon represents the channel obtained for the path.\n");

   TheDcdt = new SeDcdt;
   CurExample=Example7;
   create_dcdt ();

   // initialize glut:
   glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
   glutInitWindowPosition ( 700, 100 );
   glutInitWindowSize( 640, 480 );
   glutCreateWindow ( "GSim Constrained Delaunay Triangulation Example - M. Kallmann" );
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

