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

class VmDtManager: public SeTriangulatorManager
 { public :
    virtual void get_vertex_coordinates ( const SeVertex* v, double& x, double & y );
    virtual void set_vertex_coordinates ( SeVertex* v, double x, double y );
 };

void VmDtManager::get_vertex_coordinates ( const SeVertex* v, double& x, double & y )
 {
   x = (double) ((VmVertex*)v)->c[0];
   y = (double) ((VmVertex*)v)->c[1];
 }

void VmDtManager::set_vertex_coordinates ( SeVertex* v, double x, double y )
 {
   ((VmVertex*)v)->c[0] = (float) x;
   ((VmVertex*)v)->c[1] = (float) y;
 }

//******************************************************************
//  Ready to insert points in a triangulation
//******************************************************************

// Lets make life easier with global pointers:
static VmMesh*         TheMesh;
static VmSymEdge*      TheBorder;
static SeTriangulator* TheTriangulator;

void insert ( double x, double y )
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
    { printf ( "Duplicated vertex, so not inserted.\n" );
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
   return n*1.8f -0.9f;
 }

// This function examplifies agent insertion with a given radius, but by inserting each vertex 
// and then removing it if nearby vertices are too close - this works well for varied distances.
// Example file setut_ap.cpp shows how to do it using the epsilon distance, withtout removals.
static void insert_spaced_points ( int n, double radius )
 {
   SeBase *sr;
   SeTriangulator::LocateResult res;
   VmVertex* v;
   int nr=0;
   double px, py, vx, vy, dist;
   while ( n-->0 )
    { px = random_coord();
      py = random_coord();
      res = TheTriangulator->locate_point ( TheBorder->sym()->fac(), px, py, sr );

      v = 0;
      if ( res==SeTriangulator::TriangleFound )
        v = (VmVertex*) TheTriangulator->insert_point_in_face ( sr->fac(), px, py );
      else if ( res==SeTriangulator::EdgeFound )
        v = (VmVertex*) TheTriangulator->insert_point_in_edge ( sr->edg(), px, py );

      if ( v )
       { vx = v->c[0];
         vy = v->c[1];
         VmSymEdge *si, *s, *x;
         si = s = v->se();
         bool pointok=true;
         do { x = s->nxt();
              px=x->vtx()->c[0]; py=x->vtx()->c[1];
              dist = sqrt(gs_dist2(px,py,vx,vy));
              if ( dist<radius ) { pointok=false; break; }
              s = s->rot();
            } while ( s!=si );
         if (!pointok) 
          { TheTriangulator->remove_vertex ( v );
            nr++;
          }
       }
    }
   printf ( "Vertices in the mesh=%d  Rejected=%d\n", TheMesh->vertices(), nr );
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
   VmSymEdge *s;
   VmEdge *e, *ei;

   if ( TheMesh->empty() ) return;

   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // draw edges:
   glLineWidth ( 1.0f );
   glBegin ( GL_LINES );
   glColor3f ( 0, 0, 1.0f );
     
   // loop over the circular list of edges:
   e = ei = TheMesh->first()->edg();
   do { s = e->se();
        glVertex2fv ( s->vtx()->c );
        glVertex2fv ( s->nxt()->vtx()->c );
        e = e->nxt();
      } while ( e!=ei );

   glEnd ();
   glutSwapBuffers ();
 }

static void GlutKeyboard ( unsigned char key, int x, int y )
 {
   switch ( key )
    { case 13 : insert ( random_coord(), random_coord() ); break; // enter key
      case 32 : insert_spaced_points ( 10000, 0.05 ); break; // space key
      case 27 : exit(0); // esc key
      default : key=0;
    }
   if ( key ) glutPostRedisplay();
 }

static void GlutMouse ( int button, int state, int x, int y )
 {
   if ( state==GLUT_DOWN ) return; // only process button up
   double w = glutGet(GLUT_WINDOW_WIDTH);
   double h = glutGet(GLUT_WINDOW_HEIGHT);
   double fx = 2.0f* double(x)/w -1.0f;
   double fy = 2.0f* double(y)/h -1.0f;
   insert ( fx, fy*-1.0f );
   glutPostRedisplay();
 }

void dt_main ()
 {
   printf ("Press Enter for a random insertion\n" );
   printf ("Mouse click for point insertion\n" );
   printf ("Space for inserting several points w/ minimal distance\n" );

   // Init used mesh and triangulator types:
   TheMesh = new VmMesh;
   TheTriangulator = new SeTriangulator
                      ( SeTriangulator::ModeUnconstrained, // no constraints are used here
                        TheMesh,                           // my mesh to triangulate
                        new VmDtManager,                   // my manager says how to access my mesh
                        0.0001                             // the used epsilon in geometric primitives
                      );

   // Create an initial triangulated square (sides are larger than the window):
   double xmin, ymin, xmax, ymax;
   xmax=ymax=1.5; xmin=ymin=-1.5;
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
   glutMouseFunc    ( GlutMouse );

   // configure opengl:
   glDisable ( GL_DEPTH_TEST ); // to make sure the order in drawing is taken!
   glEnable ( GL_LINE_SMOOTH );
   glHint ( GL_LINE_SMOOTH_HINT, GL_NICEST );
   glClearColor ( 1, 1, 1, 0 );

   // run:
   glutMainLoop ();
 }
