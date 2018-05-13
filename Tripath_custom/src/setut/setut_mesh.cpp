/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <stdlib.h>
# include <gsim/se_mesh.h>

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
// Ok, now we can use the mesh and draw it with OpenGL
//******************************************************************

static VmMesh* TheMesh; // a global pointer to our mesh, to make life easier

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
   glColor3f ( 0, 0, 1.0f );
   glBegin ( GL_LINES );

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

//******************************************************************
// The Main Function
//******************************************************************

void mesh_main ()
 {
   float xmin, xmax, ymin, ymax;

   // Create the mesh object we have defined:
   TheMesh = new VmMesh;

   // Construct a simple planar mesh with two triangles and a square border:
   xmin = ymin = -0.8f;
   xmax = ymax =  0.8f;
   VmSymEdge* s = TheMesh->init();
   s->vtx()->set ( xmax, ymax );
   s->nxt()->vtx()->set ( xmin, ymax );
   s = TheMesh->mev ( s );
   s->vtx()->set ( xmax, ymin );
   s= TheMesh->mev ( s );
   s->vtx()->set ( xmin, ymin );
   TheMesh->mef ( s, s->nxt()->nxt()->nxt() ); // close the square
   TheMesh->mef ( s, s->nxt()->nxt() ); // triangulate square

   // print it to the console:
   gsout<<gsnl;
   TheMesh->save ( gsout );

   // initialize glut:
   glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
   glutInitWindowPosition ( 700, 100 );
   glutInitWindowSize( 400, 400 );
   glutCreateWindow ( "GSim SeMesh Example - M. Kallmann" );
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

/* You should see in your shell the folowing mesh description:

SYMEDGE MESH DESCRIPTION

SymEdges 10
3 6 1 0 2
6 8 0 0 0
8 5 2 1 1
5 1 0 1 2
2 9 3 2 1
7 2 2 2 2
9 0 1 3 0
0 4 3 3 2
4 3 0 4 1
1 7 3 4 0

Vertices 4
1 0.800000 0.800000
0 -0.800000 0.800000
2 0.800000 -0.800000
4 -0.800000 -0.800000

Edges 5
0 
2 
4 
6 
8 

Faces 3
9 
8 
7 
*/
