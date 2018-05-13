/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <gsim/gs_tree.h>
# include <gsim/se_mesh_import.h>

//# define GS_USE_TRACE1 // basic messages
//# define GS_USE_TRACE2 // more messages
//# define GS_USE_TRACE4 // active contour
//# define GS_USE_TRACE5 // tree
# include <gsim/gs_trace.h>

//======================= static data =============================================

// Question: Could I substitute ImpEdgeTree by an ImpEdgeHashTable ...?

class ImpEdge : public GsTreeNode
 { public :
    int a, b;      // ImpEdge vertices, not oriented
    int v;         // The other vertex of the adjacent face
    int f;         // The index of the edge's face
    ImpEdge *sym;     // The other edge, belonging to the other face sharing this edge
    ImpEdge *e2, *e3; // The other two edges of this face
   public :
    ImpEdge ( int i1=0, int i2=0, int i3=0 ) : f(0), sym(0), e2(0), e3(0) { a=i1; b=i2; v=i3; }
    void set_fac_elems ( ImpEdge *b, ImpEdge *c, int fac ) { e2=b; e3=c; f=fac; }
    void swap() { int tmp=a; a=b; b=tmp; }

    friend GsOutput& operator<< ( GsOutput& o, const ImpEdge& e )
     { return o<<'['<<e.a<<' '<<e.b<<' '<<e.v<<"]"; }

    friend GsInput& operator>> ( GsInput& i, ImpEdge& /*e*/ ) { return i; }

    static int compare ( const ImpEdge* e1, const ImpEdge* e2 );
 };

int ImpEdge::compare ( const ImpEdge *e1, const ImpEdge *e2 )
 {
   if ( e1->a<e2->a ) return -1;
   if ( e1->a>e2->a ) return  1;
   if ( e1->b<e2->b ) return -1;
   if ( e1->b>e2->b ) return  1;
   return 0;
 }

/*
# ifdef SE_UGS_TRACE4
struct PFace { SeBase *e; SeMeshBase *m; } PFaceInst;
FgOutput &operator << ( FgOutput &o, PFace &f )
 {
   o << '[';
   SeBase *e=f.e;
   while ( true )
    { o<<(int)f.m->index(e->vtx());
      e = e->nxt();
      if ( e!=f.e ) o<<' '; else break;
    }
   return o<<']';
 }
# define GS_TRACE_CONTOUR(x,s) { PFaceInst.e=x; PFaceInst.m=s; GS_TRACE4 ( "Active Contour = "<< PFaceInst ); }
# else
# define GS_TRACE_CONTOUR(x,s)
# endif
*/

# define GS_TRACE_CONTOUR(x,s)

//=========================== ImpEdgeTree ========================================

class ImpEdgeTree : public GsTree<ImpEdge>
 { public :
//    ImpEdgeTree()  {}
   public :
    ImpEdge *insert_edge ( int a, int b, int v );
    bool  insert_face ( int a, int b, int c, int f );
    ImpEdge* search_edge ( int a, int b );
    void  remove_face ( ImpEdge *e );
 };

ImpEdge *ImpEdgeTree::insert_edge ( int a, int b, int v )
 {
   ImpEdge *e = new ImpEdge ( a, b, v );
   if ( !insert(e) )
    { ImpEdge *x = cur();
      e->swap();
      if ( !insert(e) ) 
       { GS_TRACE2 ( "Could not insert edge ("<<a<<','<<b<<")!!" );
         delete e; 
         return 0; 
       }
      x->sym = e;
      e->sym = x;
    }
   else // search if the sym edge is there and set sym pointers
    {   // (to try: sort (a,b) to avoid checking if the sym exists...)
      ImpEdge eba ( b, a, v );
      ImpEdge *esym = search ( &eba );
      if ( esym )
       { esym->sym = e;
         e->sym = esym;
       }
    }
   return e;
 }

bool ImpEdgeTree::insert_face ( int a, int b, int c, int f )
 {
   ImpEdge *e1 = insert_edge ( a, b, c );
   ImpEdge *e2 = insert_edge ( b, c, a );
   ImpEdge *e3 = insert_edge ( c, a, b );
   
   if (e1) e1->set_fac_elems ( e2, e3, f );
   if (e2) e2->set_fac_elems ( e1, e3, f );
   if (e3) e3->set_fac_elems ( e1, e2, f );
 
   return e1&&e2&&e3? true:false;
 }

ImpEdge* ImpEdgeTree::search_edge ( int a, int b )
 {
   ImpEdge key(a,b,0);

   ImpEdge *e = search(&key);
   if ( !e )
    { key.swap();
      e=search(&key);
    }

   return e;
 }

void ImpEdgeTree::remove_face ( ImpEdge *e )
 {
   if (!e) return;

   // e2 and/or e3 can be null in case non-manifold edges were found during the
   // construction of the edge tree with start()
   ImpEdge *e2 = e->e2;
   ImpEdge *e3 = e->e3;

   extract ( e );
   if ( e->sym  ) e->sym->sym=0;

   if (e2) 
    { if ( e2->sym ) e2->sym->sym=0;
      remove ( e2 );
    }

   if (e3)
    { if ( e3->sym ) e3->sym->sym=0;
      remove ( e3 );
    }
 } 

//======================= internal data ==================================

static SeBase* init ( SeMeshImport *self, SeMeshBase* m, int a, int b )
 {
   SeBase *s = m->init ();
   m->index(s->vtx(),(semeshindex)b);
   m->index(s->nxt()->vtx(),(semeshindex)a);
   self->attach_vtx_info ( s->vtx(), b );        
   self->attach_vtx_info ( s->nxt()->vtx(), a );
   self->attach_edg_info ( s->edg(), b, a );
   return s;
 }

static SeBase* mev ( SeMeshImport *self, SeMeshBase* m, SeBase* s, int ev, int v )
 {
   s = m->mev ( s );
   self->attach_edg_info ( s->edg(), ev, v );
   self->attach_vtx_info ( s->vtx(), v );
   m->index ( s->vtx(), (semeshindex)v );
   return s;
 }

static SeBase* mef ( SeMeshImport *self, SeMeshBase* m, SeBase* s1, SeBase* s2, int a, int b, int f )
 {
   s2 = m->mef ( s1, s2 );
   self->attach_edg_info ( s2->edg(), a, b );
   self->attach_fac_info ( s2->fac(), f );
   return s2;
 }

class SeMeshImport::Data
 { public :
    SeMeshImport *self;
    ImpEdgeTree tree;
    GsArray<SeBase*> active_contours;    // stack of contours
    GsArray<SeBase*> contours_to_solve;  // keep contours that will result in genus or holes
    GsArray<SeBase*> vtxsymedges;        // keep pointer for vertices already inserted
   public :
    Data ( SeMeshImport *x );
    SeMeshBase *init_shell ();
    int start ( const int *triangles, int numtris, int numvtx );
    void solve_lasting_contours ( SeMeshBase *m );
    void glue_face_on_one_edge ( SeMeshBase *m, SeBase *&s, ImpEdge *e );
    void glue_face_on_two_edges ( SeMeshBase *m, SeBase *&s, ImpEdge *e );
    void glue_face_on_three_edges ( SeMeshBase *m, SeBase *s, ImpEdge *e );
 };

SeMeshImport::Data::Data ( SeMeshImport *x ) 
 {
   vtxsymedges.size(0);
   vtxsymedges.capacity(256);
   active_contours.size(0);
   active_contours.capacity(32);
   self = x;
 }

SeMeshBase *SeMeshImport::Data::init_shell ()
 {  
   GS_TRACE1 ( "Getting new shell..." );
   SeMeshBase *m = self->get_new_shell ();
   if ( !m ) return 0;

   GS_TRACE1 ( "Getting seed face from the root of the tree..." );

   ImpEdge *e = tree.root();
   int a=e->a; int b=e->b; int c=e->v; // the order of the vertices is random
   int f=e->f;
   tree.remove_face ( e );
   delete e;

   GS_TRACE1 ( "Creating seed face = ["<<a<<" "<<b<<" "<<c<<"]" );

   m->begin_indexing ();

   SeBase *s;

   s = init ( self, m, a, b );
   s = mev ( self, m, s, b, c );
   mef ( self, m, s, s->nxt()->nxt(), a, c, f );

   vtxsymedges[c] = s;
   vtxsymedges[b] = s->nxt();
   vtxsymedges[a] = s->nxt()->nxt();

   active_contours.push() = s;

   GS_TRACE2 ("seed face info: "<<(int)s->sym()->fac()<<", active face info: "<<(int)s->fac() );

   return m;
 }

int SeMeshImport::Data::start ( const int *triangles, int numtris, int numvtx )
 {  
   int i;

   active_contours.size(0);

   GS_TRACE1 ( "Initializing vertices table with size "<<numvtx<<"..." );
   vtxsymedges.size(numvtx);
   for ( i=0; i<vtxsymedges.size(); i++ )  vtxsymedges[i]=0;

   GS_TRACE1 ( "Creating edge tree..." );

   const int *tri=triangles;
   int faces_with_edges_not_inserted=0;
   for ( i=0; i<numtris; i++ ) 
    { if ( !tree.insert_face(tri[0],tri[1],tri[2],i) ) 
        faces_with_edges_not_inserted++;
      tri += 3;
    }

   return faces_with_edges_not_inserted;
 }

static SeBase* coincident_contour ( SeBase *s1, SeBase *s2 )
 {
   SeBase *x = s2;
   while ( s1->vtx() != x->nxt()->vtx() )
    { x = x->nxt();
      if ( x==s2 ) return 0; // not coincident 
    }

   // a common starting vertex was found, so that the contours will be coincident
   // or we'll have non-manifold vertices. Let's verify :
   s2 = x;
   do { if ( s1->vtx() != s2->nxt()->vtx() )
         { printf("Non-manifold vertex detected!\n"); return 0; }
        s1 = s1->nxt();
        s2 = s2->pri();
      } while ( s2!=x );

   return x;
 }

// should finished the contours by detecting loops to join or holes info to attach
void SeMeshImport::Data::solve_lasting_contours ( SeMeshBase *m )
 {
   int i;
   bool genus_increased;
   SeBase *s, *c;

   while ( !contours_to_solve.empty() )
    { genus_increased = false;
      s = contours_to_solve.pop();
      GS_TRACE_CONTOUR(s,m);
      for ( i=0; i<contours_to_solve.size(); i++ )
       { c = coincident_contour(s,contours_to_solve[i]);
         if ( c )
          { GS_TRACE1 ( "Making one genus..." );
            GS_TRACE_CONTOUR(c,m);
            contours_to_solve.remove(i);//[i]=contours_to_solve.pop();
            genus_increased = true;
            m->mg ( s, c );
            break;
          }
       }
      if ( !genus_increased )
       { GS_TRACE1 ( "Closing last contour as a hole..." );
         self->attach_hole_info ( s->fac() );
       }
    }
 }

void SeMeshImport::Data::glue_face_on_one_edge ( SeMeshBase *m, SeBase *&s, ImpEdge *e )
 {
   tree.remove_face ( e );

   SeBase *snxt = s->nxt();

   s = mev ( self, m, s, e->a, e->v );
   s = mef ( self, m, snxt, s, e->b, e->v, e->f );

   vtxsymedges[e->a] = s->nxt()->sym();
   vtxsymedges[e->b] = snxt;
   vtxsymedges[e->v] = snxt->pri();
   s = snxt;

   delete e;
 }

void SeMeshImport::Data::glue_face_on_two_edges ( SeMeshBase* m, SeBase*& s, ImpEdge* e )
 {
   tree.remove_face ( e );
   SeElement *ve  = vtxsymedges[e->v]->vtx();
   SeBase *s1, *s2;

   if ( s->pri()->vtx()==ve ) 
    { s1=s->nxt(); s2=s->pri(); }
   else
    { s1=s->nxt()->nxt(); s2=s; }

   mef ( self, m, s1, s2, m->index(s1->vtx()), m->index(s2->vtx()), e->f );
   s = s1->pri();

   vtxsymedges[(int)m->index(s->vtx())] = s;
   vtxsymedges[(int)m->index(s1->vtx())] = s1;
   delete e;
 }

void SeMeshImport::Data::glue_face_on_three_edges ( SeMeshBase* /*m*/, SeBase* s, ImpEdge* e )
 {
   tree.remove_face ( e );
   self->attach_fac_info ( s->fac(), e->f );   
   delete e;
 }

//======================== SeMeshImport ========================================

SeMeshImport::SeMeshImport ()
 {
   _data = new Data ( this );
 }

SeMeshImport::~SeMeshImport () 
 {
   for ( int i=0; i<shells.size(); i++ ) delete shells[i];
   delete _data;
 }

void SeMeshImport::compress_buffers ()
 {
   shells.compress();
   _data->vtxsymedges.capacity(0);
   _data->active_contours.capacity(0);
 }

SeMeshImport::Msg SeMeshImport::start (  const int *triangles, int numtris, int numvtxs )
 {  
   GS_TRACE1 ( "Start :" );

   int not_inserted = _data->start ( triangles, numtris, numvtxs );

   shells.size(0);
   SeMeshBase *m = _data->init_shell (); // will push something in active_contours
   if ( !m ) return MsgNullShell;
   shells.push()=m;

   GS_TRACE_CONTOUR(_data->active_contours.top(),m);
   GS_TRACE4 ( "Face info of active contour = " << (int)(_data->active_contours.top()->fac()) );
   GS_TRACE5 ( "Tree = "<<_data->tree );
   GS_TRACE1 ( "Start finished !" );
   GS_TRACE1 ( "Faces Lost: "<<not_inserted );

   return not_inserted>0? MsgNonManifoldFacesLost : MsgOk;
 }

static SeBase* in_same_face ( SeBase *e, SeElement *v )
 {
   SeBase *x = e;
   do { if ( x->vtx()==v ) return x;
        x = x->nxt();
      } while ( x!=e );
   return 0;
 }

SeMeshImport::Msg SeMeshImport::next_step ()
 {
   int a, b;
   ImpEdge *e;
   SeBase *ini, *s, *sv;

   int edges_found = 0;

   GS_TRACE2 ( " " );
   GS_TRACE2 ( "Next step :" );

   ImpEdgeTree &tree = _data->tree;
   GsArray<SeBase*> &active_contours = _data->active_contours;
   GsArray<SeBase*> &contours_to_solve = _data->contours_to_solve;
   GsArray<SeBase*> &vtxsymedges = _data->vtxsymedges;

   GS_TRACE2 ( "Active contours : "<<active_contours.size() );

   if ( tree.empty() )
    { GS_TRACE1 ( "Empty tree: no more faces to glue." );
      if ( !active_contours.empty() ) 
       { while ( !active_contours.empty() ) contours_to_solve.push()=active_contours.pop();
         GS_TRACE1 ( "Solving "<<contours_to_solve.size()<<" open contour(s)..." );
         _data->solve_lasting_contours ( shells.top() );
       }
      for ( int i=0; i<shells.size(); i++ ) shells[i]->end_indexing();
      GS_TRACE1 ( "Finished : "<<shells.size()<<" shell(s), last shell V-E+F="<<shells.top()->euler() );
      return MsgFinished;
    }
   else if ( active_contours.empty() ) 
    { GS_TRACE1 ( "Starting new shell "<<shells.size()<<"... " );
      shells.push() = _data->init_shell();
      return MsgOk; // return now to cope with degenerated data (a shell with a single triangle).
    }

   s = ini = active_contours.top();

   while ( true )
    { 
      a = (int) shells.top()->index(s->vtx());
      b = (int) shells.top()->index(s->nxt()->vtx());
      GS_TRACE2 ( "Searching edge "<<a<<" "<<b<<"... " );
      e = tree.search_edge ( a, b ); // will search {a,b} or {b,a}

      if ( e )
       { edges_found++;

         sv = 0;
         if ( vtxsymedges[e->v] ) // the other vertex is already in the mesh
	     sv = in_same_face ( s, vtxsymedges[e->v]->vtx() );

         if ( !vtxsymedges[e->v] || !sv ) // if the other vertex is free or is in another contour
          { GS_TRACE1 ( "Gluing face "<<a<<" "<<b<<" "<<e->v<<" on one edge... " );
            _data->glue_face_on_one_edge ( shells.top(), s, e );
            active_contours.top() = s;
            GS_TRACE_CONTOUR(s,shells.top());
            GS_TRACE5 ( "Tree = "<<tree );
            return MsgOk;
          }
         else if ( s->nxt()->nxt()->nxt()==s ) // trying to close the active contour that is a triangle
          { GS_TRACE1 ( "Gluing last face "<<a<<" "<<b<<" "<<e->v<<" of current active contour..." );
            _data->glue_face_on_three_edges ( shells.top(), s, e );
            active_contours.pop();
            GS_TRACE1 ( "Active contour popped... " );
            GS_TRACE5 ( "Tree = "<<tree );
            return MsgOk;
	      }
         else if ( s->nxt()->nxt()->vtx()==vtxsymedges[e->v]->vtx() ||
                   s->pri()->vtx()==vtxsymedges[e->v]->vtx() )
	      { GS_TRACE1 ( "Gluing face "<<a<<" "<<b<<" "<<e->v<<" on two edges... " );
            _data->glue_face_on_two_edges ( shells.top(), s, e );
            active_contours.top() = s;
            GS_TRACE_CONTOUR(s,shells.top());
            GS_TRACE5 ( "Tree = "<<tree );
            return MsgOk;
	      }
         else if ( sv )
          { GS_TRACE1 ( "Connecting one edge, and pushing one active contour... " );
            shells.top()->mef ( s, sv );
            attach_edg_info ( s->edg(), shells.top()->index(s->vtx()), shells.top()->index(sv->vtx()) );
            active_contours.push()=sv;
            return MsgOk;
          }
         else
          { GS_TRACE1 ( "XXXXXXXXXXXXXXXXXX undef situation!! XXXXXXXXXXXXXXXXXXXXXXXXXXXX" );
            GS_TRACE_CONTOUR(active_contours.top(),shells.top());
            GS_TRACE1 ( "s = ["<<a<<","<<b<<"] v="<<e->v);
            GS_TRACE1 ( "s.fac="<<(int)s->fac()<<", vfac="<<(int)vtxsymedges[e->v]->fac() );
          }
        }
       else
        { // e not in the edge tree: input data is not fully connected!
        }

      s = s->nxt();
      if ( s==ini ) break;// already passed all edges of the active contour
    }

   // At this point we looked into all edges of the active_contour,
   // but no faces were glued and the tree is not empty.

   GS_TRACE1 ( "No more faces could be glued to the actual contour!" );
   contours_to_solve.push() = active_contours.pop();
   
   return MsgOk;
 }

//============================== virtuals ==================================

void SeMeshImport::attach_vtx_info ( SeVertex* /*v*/, int /*vi*/ )
 { 
 }

void SeMeshImport::attach_edg_info ( SeEdge* /*e*/, int /*a*/, int /*b*/ )
 { 
 }

void SeMeshImport::attach_fac_info ( SeFace* /*f*/, int /*fi*/ )
 {
 }

void SeMeshImport::attach_hole_info ( SeFace* /*f*/ )
 { 
 }

//============================== end of file ===============================

