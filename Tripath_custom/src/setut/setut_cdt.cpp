/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

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

# include "setut_cmesh.h"

//******************************************************************
//  Cm Triangulator Manager
//******************************************************************

class CmCdtManager: public SeTriangulatorManager
 { public :
    virtual void get_vertex_coordinates ( const SeVertex* v, double& x, double & y );
    virtual void set_vertex_coordinates ( SeVertex* v, double x, double y );
    virtual bool is_constrained ( SeEdge* e );
    virtual void set_unconstrained ( SeEdge* e );
    virtual void get_constraints ( SeEdge* e, GsArray<int>& ids );
    virtual void add_constraints ( SeEdge* e, const GsArray<int>& ids );
    virtual void copy_constraints ( const SeEdge* e1, SeEdge* e2 );
 };

void CmCdtManager::get_vertex_coordinates ( const SeVertex* v, double& x, double & y )
 {
   x = (double) ((CmVertex*)v)->c[0];
   y = (double) ((CmVertex*)v)->c[1];
 }

void CmCdtManager::set_vertex_coordinates ( SeVertex* v, double x, double y )
 {
   ((CmVertex*)v)->c[0] = (float) x;
   ((CmVertex*)v)->c[1] = (float) y;
 }

bool CmCdtManager::is_constrained ( SeEdge* e )
 {
   return ((CmEdge*)e)->is_constrained();
 }

void CmCdtManager::set_unconstrained ( SeEdge* e )
 {
   ((CmEdge*)e)->constraints_ids.size(0);
 }

void CmCdtManager::get_constraints ( SeEdge* e, GsArray<int>& ids )
 {
   ids = ((CmEdge*)e)->constraints_ids;
 }

void CmCdtManager::add_constraints ( SeEdge* e, const GsArray<int>& ids )
 {
   for ( int i=0; i<ids.size(); i++ ) ((CmEdge*)e)->constraints_ids.push(ids[i]);
 }

void CmCdtManager::copy_constraints ( const SeEdge* e1, SeEdge* e2 )
 {
   ((CmEdge*)e2)->constraints_ids = ((CmEdge*)e1)->constraints_ids;
 }

//******************************************************************
//  Ready to insert points in a triangulation
//******************************************************************

// Lets make life easier with global pointers:
CmMesh*         TheMesh;
CmSymEdge*      TheBorder;
SeTriangulator* TheTriangulator;

// Global array saving inserted data to be able to rebuild constrained
// and conforming triangulations for comparison purposes
static bool ChangingMode=false;
static GsArray<float> Vertices;
static GsArray<float> Constraints;

static void insert_vertex ( double x, double y )
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
      Vertices.push()=(float)x;
      Vertices.push()=(float)y;
    }
   else // a face was found
    { TheTriangulator->insert_point_in_face ( s->fac(), x, y );
      printf ( "Inserted in a triangle.\n" );
      Vertices.push()=(float)x;
      Vertices.push()=(float)y;
    }
 }

static SeVertex* insert_or_find_vertex ( float x, double y )
 { 
   SeVertex *v=0;
   SeBase *s;
   SeTriangulator::LocateResult res;

   res = TheTriangulator->locate_point ( TheBorder->sym()->fac(), x, y, s );
   if ( res==SeTriangulator::NotFound )
    { // within this demo program, this NotFound case should never happen
      printf ( "Error: Not in domain, or faces not ccw!\n" );
    }
   else if ( res==SeTriangulator::VertexFound )
    { if ( !ChangingMode ) printf ( "Using existing vertex.\n" );
      v = s->vtx();
    }
   else if ( res==SeTriangulator::EdgeFound )
    { v = TheTriangulator->insert_point_in_edge ( s->edg(), x, y );
    }
   else // a face was found
    { v = TheTriangulator->insert_point_in_face ( s->fac(), x, y );
    }
   return v;
 }

static void insert_constraint ( float x1, float y1, float x2, float y2, int id )
 { 
   SeVertex *v1, *v2;

   v1 = insert_or_find_vertex ( x1, y1 );
   if ( !v1 ) return;
   v2 = insert_or_find_vertex ( x2, y2 );
   if ( !v2 ) return;

   TheTriangulator->insert_line_constraint ( v1, v2, id );

   if ( !ChangingMode )
    { Constraints.push()=x1;
      Constraints.push()=y1;
      Constraints.push()=x2;
      Constraints.push()=y2;
      printf ( "Inserted Constraint %d (%+6.4f,%+6.4f):(%+6.4f,%+6.4f).\n", id, x1, y1, x2, y2 );
    }
 }

// Create an initial triangulated square (sides are larger than the window):
static void init ( SeTriangulator* t )
 {
   double xmin, ymin, xmax, ymax;
   xmax=ymax=1.5; xmin=ymin=-1.5;
   TheBorder = (CmSymEdge*) TheTriangulator->init_as_triangulated_square
                            ( xmin, ymin, xmax, ymin, xmax, ymax, xmin, ymax )->sym();
 }

static void change_triangulator_mode ()
 {
   int i;
   SeTriangulator::Mode mode = TheTriangulator->mode();
   if ( mode==SeTriangulator::ModeConforming )
    { mode = SeTriangulator::ModeConstrained;
      printf("Switching to Constrained mode.\n");
    }
   else
    { mode = SeTriangulator::ModeConforming;
      printf("Switching to Conforming mode.\n");
    }

   TheTriangulator->mode ( mode );
   init ( TheTriangulator );

   ChangingMode = true;

   for ( i=0; i<Vertices.size(); i+=2 )
    insert_vertex ( Vertices[i], Vertices[i+1] );

   for ( i=0; i<Constraints.size(); i+=4 )
    insert_constraint ( Constraints[i], Constraints[i+1], Constraints[i+2], Constraints[i+3], i/4 );

   ChangingMode = false;
 }

static double random_coord ()
 {
   double n = double(rand()) / double(RAND_MAX); // n E [0,1]
   n = n*1.8f -0.9f;

   return n;
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
   CmSymEdge *s;
   CmEdge *e, *ei;

   if ( TheMesh->empty() ) return;

   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // we will draw constrained edges after to ensure they are
   // well visible.
   static GsArray<CmEdge*> cedges;

   // draw non-constrained edges:
   glLineWidth ( 0.5f );
   glColor3f ( 0, 0, 0.8f );     
   glBegin ( GL_LINES );
   e = ei = TheMesh->first()->edg();
   do { if ( e->is_constrained() )  
         { cedges.push()=e; }
        else 
         { s = e->se();
           glVertex2fv ( s->vtx()->c );
           glVertex2fv ( s->nxt()->vtx()->c );
         }
        e = e->nxt();
      } while ( e!=ei );

   // draw constrained edges:
   glLineWidth ( 1.0f );
   glColor3f ( 1, 0, 0 ); 
   while ( cedges.size() )
    { s = cedges.pop()->se();
      glVertex2fv ( s->vtx()->c );
      glVertex2fv ( s->nxt()->vtx()->c );
    }

   glEnd ();
   glutSwapBuffers ();
 }

static void GlutKeyboard ( unsigned char key, int x, int y )
 {
   switch ( key )
    { case 13 : insert_vertex ( random_coord(), random_coord() ); break; // enter key
      case 32 : change_triangulator_mode(); break; // space key
      case 27 : if ( TheMesh->vertices()==4 ) exit(0); // esc key
                init(TheTriangulator);
                Vertices.size(0);
                Constraints.size(0);
                break;
      default : key=0;
    }

   if ( key ) glutPostRedisplay();
 }

static void GlutMouse ( int button, int state, int x, int y )
 {
   if ( state==GLUT_DOWN ) return; // only process button up

   static int count=0;
   static float lpx;
   static float lpy;
   int w = glutGet(GLUT_WINDOW_WIDTH);
   int h = glutGet(GLUT_WINDOW_HEIGHT);
   float px = 2.0f* float(x)/float(w) -1.0f;
   float py = 2.0f* float(y)/float(h) -1.0f;
   py *= -1;

   if ( button==GLUT_RIGHT_BUTTON )
    { insert_vertex ( px, py );
    }
   else if ( button==GLUT_LEFT_BUTTON )
    { count++;
      if ( count%2==0 ) insert_constraint ( lpx, lpy, px, py, count/2 );
      lpx=px; lpy=py;
    }

   glutPostRedisplay();
 }

void cdt_main ()
 {
   printf ("Press:\n");
   printf ("  Enter for a random point insertion\n" );
   printf ("  Esc to init triangulation\n" );
   printf ("  Two consecutive left button clicks to add a constraint\n" );
   printf ("  Right mouse button to insert a point\n" );
   printf ("  Space to switch conforming/constrained mode\n" );
   printf ("    (conforming mode is less robust w/intersections)\n" );

   // Init used mesh and triangulator types:
   TheMesh = new CmMesh;
   TheTriangulator = new SeTriangulator
                      ( SeTriangulator::ModeConstrained, // can be converted to conforming on line
                        TheMesh,                         // my mesh to triangulate
                        new CmCdtManager,             // my manager says how to access my mesh
                        0.0001                           // the used epsilon in geometric primitives
                      );

   // Create an initial triangulated square (sides are larger than the window):
   init ( TheTriangulator );

   // initialize glut:
   glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
   glutInitWindowPosition ( 700, 100 );
   glutInitWindowSize( 400, 400 );
   glutCreateWindow ( "GSim Constrained Delaunay Triangulator Example - M. Kallmann" );
   glutReshapeFunc ( GlutResizeWindow );
   glutDisplayFunc ( GlutDraw );
   glutKeyboardFunc( GlutKeyboard );
   glutMouseFunc   ( GlutMouse );

   // configure opengl:
   glDisable ( GL_DEPTH_TEST ); // to make sure the order in drawing is taken!
   glEnable ( GL_LINE_SMOOTH );
   glHint ( GL_LINE_SMOOTH_HINT, GL_NICEST );
   glClearColor ( 1, 1, 1, 0 );

   // run:
   glutMainLoop ();
 }

