/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <math.h>
# include <stdlib.h>
# include <gsim/gs_geo2.h>
# include <gsim/se_mesh.h>
# include <gsim/se_triangulator.h>

# ifdef SETUT_FLGLUT
# include <gsim/gs_ogl.h>
# include "setut_flglut.h"
# else
# include <GL/glut.h>
# endif

//******************************************************************
//  Used Mesh Types
//******************************************************************

# include "setut_vmesh.h"

//******************************************************************
//  Vm Triangulator Manager
//******************************************************************

class VmApManager: public SeTriangulatorManager
 { public :
    virtual void get_vertex_coordinates ( const SeVertex* v, double& x, double & y );
    virtual void set_vertex_coordinates ( SeVertex* v, double x, double y );
 };

void VmApManager::get_vertex_coordinates ( const SeVertex* v, double& x, double & y )
 {
   x = (double) ((VmVertex*)v)->c[0];
   y = (double) ((VmVertex*)v)->c[1];
 }

void VmApManager::set_vertex_coordinates ( SeVertex* v, double x, double y )
 {
   ((VmVertex*)v)->c[0] = (float) x;
   ((VmVertex*)v)->c[1] = (float) y;
 }

//******************************************************************
//  Ready to manipulate triangulation
//******************************************************************

// Lets make life easier with global pointers:
static VmMesh*         TheMesh;
static VmSymEdge*      TheBorder;
static SeTriangulator* TheTriangulator;

#define Radius 0.05  // radius of the agents to insert
#define Lim    0.98  // domain limit coordinate

// Agent placement is controlled here by using the epsilon mechanism available in
// the SeTriangulator class. Each vertex is inserted in a Delaunay triangulation 
// with an epsilon value of 2*r, where r is the radius of each agent.
void insert_agent ( double x, double y )
 { 
   SeBase* s;
   SeTriangulator::LocateResult res;

   printf ( "Vertex %d (%+6.4f,%+6.4f): ", (TheMesh->vertices()-4)+1, x, y );
   res = TheTriangulator->locate_point ( TheBorder->sym()->fac(), x, y, s );

   if ( res==SeTriangulator::NotFound )
    { // within this demo program, this NotFound case should never happen
      printf ( "Error: Not in domain, or faces not ccw!\n" );
    }
   else if ( res==SeTriangulator::VertexFound )
    { printf ( "Another vertex too close, so not inserted.\n" );
      // here the new vertex is detected too close in respect to the given epsilon radius
      // (attention: the current implementation assumes a non-triangular backface)
    }
   else if ( res==SeTriangulator::EdgeFound )
    { TheTriangulator->insert_point_in_edge ( s->edg(), x, y );
      printf ( "Inserted in an edge.\n" );
    }
   else // a face was found
    { TheTriangulator->insert_point_in_face ( s->fac(), x, y );
      printf ( "Inserted in a triangle.\n" );
    }
 }

static double random_coord ()
 {
   double n = double(rand()) / double(RAND_MAX); // n E [0,1]
   n *= (Lim-Radius)*2;
   return n - (Lim-Radius);
 }

//******************************************************************
//  Glut Functions
//******************************************************************

static void GlutResizeWindow ( int w, int h )
 {
   glViewport( 0, 0, w, h );	// View port uses whole window
 }

static void GlutKeyboard ( unsigned char key, int x, int y )
 {
   switch ( key )
    { case 13 : insert_agent ( random_coord(), random_coord() ); break; // enter key
      case 27 : exit(0); // esc key
      default : key=0;
    }
   if ( key ) glutPostRedisplay();
 }

void circle ( float x, float y, float radius  )
 {
   GsVec2 p;
   int nvertices = 128;
   float ang=0, incang = ((float)GS_2PI) / nvertices;
   while ( nvertices>0 )
    { p.set ( radius*sinf(ang), radius*cosf(ang) );
      p += GsVec2(x,y);
      glVertex2f ( p.x, p.y );
      ang += incang;
      nvertices--;
    }
 }

static void GlutDraw ()
 {
   VmSymEdge *s;
   VmEdge *e, *ei;

   if ( TheMesh->empty() ) return;

   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // draw edges:
   glLineWidth ( 1.0f );
   glBegin ( GL_LINES );
   glColor3f ( 0, 0, 0 );
     
   // loop over the circular list of edges:
   e = ei = TheMesh->first()->edg();
   do { s = e->se();
        glVertex2fv ( s->vtx()->c );
        glVertex2fv ( s->nxt()->vtx()->c );
        e = e->nxt();
      } while ( e!=ei );
   glEnd ();

   // loop over the circular list of vertices:
   glLineWidth ( 3.0f );
   glColor3f ( 0.8f, 0, 0 );
   VmVertex* v, *vi;
   v = vi = TheMesh->first()->vtx();
   do { if ( v->c[0]>-Lim && v->c[0]<Lim ) // the corners are not agents
         { glBegin ( GL_LINE_STRIP );
           circle ( v->c[0], v->c[1], (float)Radius );
           glEnd ();
         }
        v = v->nxt();
      } while ( v!=vi );

   glEnd ();
   glutSwapBuffers ();
 }

void ap_main ()
 {
   printf ("Press Enter for a random insertion\n" );

   // Init used mesh and triangulator types:
   TheMesh = new VmMesh;
   TheTriangulator = new SeTriangulator
                      ( SeTriangulator::ModeUnconstrained, // no constraints are used here
                        TheMesh,                           // my mesh to triangulate
                        new VmApManager,                   // my manager says how to access my mesh
                        Radius*2                           // the used epsilon is now the agent radius
                      );

   // But then we have to use a "normal epsilon" for edge tests:
   TheTriangulator->epsilonedg(0.0001);

   // Create an initial triangulated square (sides are larger than the window):
   double xmin, ymin, xmax, ymax;
   xmax=ymax=Lim; xmin=ymin=-Lim;
   TheBorder = (VmSymEdge*) TheTriangulator->init_as_triangulated_square
                            ( xmin, ymin, xmax, ymin, xmax, ymax, xmin, ymax )->sym();

   // initialize glut:
   glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
   glutInitWindowPosition ( 700, 100 );
   glutInitWindowSize( 400, 400 );
   glutCreateWindow ( "GSim Delaunay Triangulator Example - M. Kallmann" );
   glutReshapeFunc  ( GlutResizeWindow );
   glutDisplayFunc  ( GlutDraw );
   glutKeyboardFunc ( GlutKeyboard );

   // configure opengl:
   glDisable ( GL_DEPTH_TEST ); // to make sure the order in drawing is taken!
   glEnable ( GL_LINE_SMOOTH );
   glHint ( GL_LINE_SMOOTH_HINT, GL_NICEST );
   glClearColor ( 1, 1, 1, 0 );

   // run:
   glutMainLoop ();
 }
