/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <stdlib.h>
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

class VmTriFaceManager: public SeTriangulatorManager
 { public :
    virtual void get_vertex_coordinates ( const SeVertex* v, double& x, double & y );
    virtual void set_vertex_coordinates ( SeVertex* v, double x, double y );
 };

void VmTriFaceManager::get_vertex_coordinates ( const SeVertex* v, double& x, double & y )
 {
   x = (double) ((VmVertex*)v)->c[0];
   y = (double) ((VmVertex*)v)->c[1];
 }

void VmTriFaceManager::set_vertex_coordinates ( SeVertex* v, double x, double y )
 {
   ((VmVertex*)v)->c[0] = (float) x;
   ((VmVertex*)v)->c[1] = (float) y;
 }

//******************************************************************
//  Ready to triangulate a face
//******************************************************************

# define POLYSIZE 9

float poly[] = { -0.5f, -0.7f,
                 -0.7f, -0.2f,
                 -0.8f, 0.4f,
                 -0.1f, 0.7f,
                  0.8f, 0.6f,
                  0.8f, 0.0f,
                  0.1f, 0.3f,
                 -0.3f, -0.4f,
                  0.8f, -0.4f }; // a non-convex polygon

static VmMesh* TheMesh; // a global pointer to our mesh, to make life easier

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

   glEnable ( GL_LINE_SMOOTH );
   glHint ( GL_LINE_SMOOTH_HINT, GL_NICEST );
   glClearColor ( 1, 1, 1, 0 );

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
   glutSwapBuffers();
 }

static void GlutKeyboard ( unsigned char key, int x, int y )
 {
   switch ( key )
    { case 27 : exit(0); // esc key
    }
 }

void triface_main ()
 {
   // Init used mesh and triangulator types:
   TheMesh = new VmMesh;
   SeTriangulator* tri = new SeTriangulator
                      ( SeTriangulator::ModeUnconstrained, // no constraints are used here
                        TheMesh,                           // my mesh to triangulate
                        new VmTriFaceManager,              // my manager says how to access my mesh
                        0.0001                             // the used epsilon in geometric primitives
                      );

   // Create the face and triangulate it:
   VmSymEdge *s = TheMesh->init();
   VmSymEdge *x = s->nxt();
   x->vtx()->set ( poly[0], poly[1] );
   s->vtx()->set ( poly[2], poly[3] );
   for ( int i=2; i<POLYSIZE; i++ )
    { s = TheMesh->mev ( s );
      s->vtx()->set ( poly[i*2], poly[i*2+1] );
    }
   TheMesh->mef ( s, x );

   // Attention: the face must be in ccw orientation!
   tri->triangulate_face ( s->fac() );

   // initialize glut:
   glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
   glutInitWindowPosition ( 700, 100 );
   glutInitWindowSize( 400, 400 );
   glutCreateWindow ( "GSim Face Triangulation Example - M. Kallmann" );
   glutReshapeFunc ( GlutResizeWindow );
   glutDisplayFunc ( GlutDraw );
   glutKeyboardFunc ( GlutKeyboard );

   // configure opengl:
   glEnable ( GL_DEPTH_TEST );
   glEnable ( GL_LINE_SMOOTH );
   glHint ( GL_LINE_SMOOTH_HINT, GL_NICEST );
   glClearColor ( 1, 1, 1, 0 );

   // run:
   glutMainLoop ();
 }
