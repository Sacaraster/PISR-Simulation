/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <stdlib.h>
# include <gsim/se_mesh.h>
# include <gsim/se_mesh_import.h>

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
//  Example Data to Import a "Triangle List" to SeMesh
//******************************************************************

static float Vertices[] = 
 { -0.5,  0.0,
    0.0, -0.5,
    0.0,  0.5,
    0.5,  0.0  };

static int Triangles[] =
 { 0, 1, 2,
   2, 3, 1 };

static int NumTriangles = 2;
static int NumVertices = 4;

//******************************************************************
//  Definition of VmImport class
//******************************************************************

class VmImport : public SeMeshImport
 { public :
    virtual SeMeshBase* get_new_shell ();
    virtual void attach_vtx_info ( SeVertex* v, int vi );
 };

SeMeshBase* VmImport::get_new_shell ()
 {
   return new VmMesh;
 }

void VmImport::attach_vtx_info ( SeVertex* v, int vi )
 {
   VmVertex* mv = (VmVertex*)v;
   mv->c[0] = Vertices[vi*2];
   mv->c[1] = Vertices[vi*2+1];
 }

//******************************************************************
//  Glut functions
//******************************************************************

static VmMesh* TheMesh; // a global pointer to our mesh, to make life easier

static void GlutResizeWindow ( int w, int h )
 {
   glViewport( 0, 0, w, h ); // View port uses whole window
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
   glutSwapBuffers();
 }

static void GlutKeyboard ( unsigned char key, int x, int y )
 {
   switch ( key )
    { case 27 : exit(0); // esc key
    }
 }

//******************************************************************
//  The main function
//******************************************************************

void import_main ()
 {
   // Init VmImport:
   VmImport* imp = new VmImport;

   // Do the import:
   VmImport::Msg msg = imp->start ( Triangles, NumTriangles, NumVertices );
   while ( msg!=VmImport::MsgFinished )
    { msg = imp->next_step ();
    }

   // Get result:
   if ( imp->shells.size()!=1 )
    { printf("Error: shell Not Found!\n"); exit(1); }

   TheMesh = (VmMesh*)imp->shells.pop();
   printf ( "Mesh found:\n" );
   printf ( "vertices: %d\n" , TheMesh->vertices() );
   printf ( "edges: %d\n" , TheMesh->edges() );
   printf ( "faces: %d\n" , TheMesh->faces() );

   // initialize glut:
   glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
   glutInitWindowPosition ( 700, 100 );
   glutInitWindowSize( 400, 400 );
   glutCreateWindow ( "GSim SeMesh Import Example - M. Kallmann" );
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
