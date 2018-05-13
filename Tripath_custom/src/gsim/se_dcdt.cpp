/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <gsim/gs_string.h>
# include <gsim/gs_geo2.h>
# include <gsim/se_dcdt.h>
# include <gsim/se_triangulator_tools.h>

//# define GS_USE_TRACE1  // insert polygon
//# define GS_USE_TRACE2  // init
//# define GS_USE_TRACE3  // insert polygon
//# define GS_USE_TRACE4  // remove polygon
//# define GS_USE_TRACE5  // funnel
//# define GS_USE_TRACE6  // translate
//# define GS_USE_TRACE7  // ray
//# define GS_USE_TRACE8  // tri manager
//# define GS_USE_TRACE9  // extract contours
# include <gsim/gs_trace.h>

//=============================== SeDcdt ==================================

void SeDcdt::_construct ()
 {
   _backface = 0;
   _cur_search_face = 0;
   _radius = -1;
   _using_domain = false; // only used internally in some operations
   _xmin = _xmax = _ymin = _ymax = 0;
   _dcdt_changed = true;
 }

SeDcdt::SeDcdt ( double epsilon )
       :SeTriangulator ( SeTriangulator::ModeConstrained,
                         new SeDcdtMesh, new SeDcdtManager, epsilon )
 { 
   _construct();
 }

SeDcdt::SeDcdt ( SeMeshBase* m, SeDcdtManager* man, double epsilon )
       :SeTriangulator ( SeTriangulator::ModeConstrained, m, man, epsilon )

 { 
   _construct();
 }

SeDcdt::~SeDcdt () 
 {
 }

void SeDcdt::get_mesh_edges ( GsArray<GsPnt2>* constr, GsArray<GsPnt2>* unconstr )
 {
   SeDcdtEdge *e, *ei;
   SeDcdtSymEdge *s;
   GsArray<GsPnt2>* pa;

   if ( constr ) constr->size(0);
   if ( unconstr ) unconstr->size(0);
   
   e = ei = mesh()->first()->edg();

   do { if ( e->is_constrained() ) pa=constr; else pa=unconstr;
        if ( pa )
         { s = e->se();  pa->push() = s->vtx()->p;
           s = s->nxt(); pa->push() = s->vtx()->p;
         }
        e = e->nxt();
      } while ( e!=ei );
 }

void SeDcdt::get_mesh_edges ( GsArray<GsPnt2>* constr, GsArray<GsPnt2>* unconstr, GsArray<GsPnt2>* domain, GsArray<GsPnt2>* border )
 {
   SeDcdtEdge *e, *ei;
   SeDcdtSymEdge *s;
   GsArray<GsPnt2>* pa;

   if ( constr ) constr->size(0);
   if ( unconstr ) unconstr->size(0);
   if ( domain ) domain->size(0);
   if ( border ) border->size(0);
   
   e = ei = mesh()->first()->edg();

   do { if ( e->is_constrained() )
         { if ( e->has_id(0) ) pa=domain; else pa=constr; }
        else 
         { if ( e->border() ) pa=border; else pa=unconstr; }
        if ( pa )
         { s = e->se();  pa->push() = s->vtx()->p;
           s = s->nxt(); pa->push() = s->vtx()->p;
         }
        e = e->nxt();
      } while ( e!=ei );
 }

//================================================================================
//================================ save ==========================================
//================================================================================

int SeDcdt::export_contours ( GsOutput& out, float x, float y )
 {
   GsPolygon V;
   GsArray<int> I;
   extract_contours ( V, I, x, y );

   int elems = I.size()/2;

   out << "SeDcdt\n\n";
   out << "# domain:" << (elems>0?1:0) << " polygons:" << (elems-1) << gsnl << gsnl;
   out << "epsilon " << SeTriangulator::epsilon() << gsnl << gsnl;

   if ( elems==0 ) return true;

   int i, j;
   out << "domain\n";
   InsPol& p = *_polygons[0];
   for ( i=0; i<p.size(); i++ )
    { out << p[i]->p;
      if ( i==p.size()-1 ) out<<';'; else out<<gspc;
    }
   out << gsnl << gsnl;

   for ( i=0; i<I.size(); i+=2 )
    { out << "polygon " << ((i/2)+1);
      out << gsnl;

      for ( j=I[i]; j<=I[i+1]; j++)
       { out << V[j];
         if ( j==I[i+1] ) out<<';'; else out<<gspc;
       }
      out << gsnl << gsnl;
    }

   return true;
 }

bool SeDcdt::save ( GsOutput& out )
 {
   int elems = _polygons.elements();

   out << "SeDcdt\n\n";
   out << "# domain:" << (elems>0?1:0) << " polygons:" << (elems-1) << gsnl << gsnl;
   out << "epsilon " << SeTriangulator::epsilon() << gsnl << gsnl;

   if ( _radius!=-1 )
     out << "init_radius " << _radius << gsnl << gsnl;

   if ( elems==0 ) return true;

   int i, id, psize;
   int maxid = _polygons.maxid();
   for ( id=0; id<=maxid; id++ )
    { if ( !_polygons[id] ) continue;

      InsPol& p = *_polygons[id];
   
      if ( id==0 )
       { out << "domain\n"; }
      else
       { out << "polygon " << id;
         if ( p.open ) out << " open";
         out << gsnl;
       }

      psize = p.size();
      for ( i=0; i<psize; i++ )
       { out << p[i]->p;
         if ( i==psize-1 ) out<<';'; else out<<gspc;
       }
      out << gsnl << gsnl;
    }

   return true;
 }

//================================================================================
//================================ load ==========================================
//================================================================================

static void _read_pol ( GsInput& inp, GsPolygon& pol, float maxlen )
 {
   pol.size ( 0 );
   while ( 1 )
    {
      if ( inp.check()==GsInput::Delimiter )
       { if ( inp.getc()==';' ) break;
          else inp.unget();
       }
      inp >> pol.push();
    }
   if ( maxlen>0 ) pol.resample ( maxlen );
 }

bool SeDcdt::load ( GsInput& inp )
 {
   GsPolygon pol;
   pol.capacity ( 64 );

   float epsilon = 0.00001f;
   float epsedg = -1.0f;
   float radius = -1.0f;
   float maxlen = -1.0f;
   float inflate = 0;
   float inflatedang = 0;
   int simplify = 0; // false
   int id, nextid=1;

   // signature:
   inp.commentchar ( '#' );
   inp.get();
   if ( inp.ltoken()!="SeDcdt") return false;

   while ( 1 )
    { inp.get();
      if ( inp.end() ) break;
      const GsString& s = inp.ltoken();
 
      if ( s=="epsilon" )
       { inp >> epsilon;
       }
      else if ( s=="epsilonedg" )
       { inp >> epsedg;
       }
      else if ( s=="init_radius" )
       { inp >> radius;
       }
      else if ( s=="maxlen" )
       { inp >> maxlen;
       }
      else if ( s=="simplify" )
       { inp >> simplify;
       }
      else if ( s=="inflate" )
       { inp >> inflate >> inflatedang;
         inflatedang = GS_TORAD(inflatedang);
         if ( inflate<0 || inflatedang<0 ) gsout.warning("Wrong parameters in inflate keyword: [radius][dang]");
       }
      else if ( s=="domain" )
       { _read_pol ( inp, pol, maxlen );
         pol.open ( false );
         init ( pol, epsilon, radius );
         if ( epsedg>0 ) epsilonedg(epsedg);
       }
      else if ( s=="polygon" )
       { if ( num_polygons()==0 ) return false;
         inp >> id;
         while ( id>nextid ) { _polygons.insert(0); nextid++; }
         inp.get();
         if ( inp.ltoken()=="open" )
          { pol.open(true); }
         else
          { inp.unget();
            pol.open(false);
          }
         _read_pol ( inp, pol, maxlen );

         if ( simplify )
          { GsPolygon p(pol);
            se_simplify ( p, pol );
          }
         if ( inflate>0 )
          { GsPolygon p(pol);
            if ( !simplify ) { if ( !p.ccw() ) p.revert(); }
            //pol.inflate ( p, inflate, inflatedang );
            se_inflate ( p, pol, inflate, inflatedang );
          }
         insert_polygon ( pol );
         nextid++;
       }
    }

   return true;
 }

//================================================================================
//================================ init ==========================================
//================================================================================

void SeDcdt::init ( const GsPolygon& domain , double epsilon, float radius )
 {
   GS_TRACE2 ( "Starting init()..." );

   // Reset cur searched face:
   _cur_search_face = 0;

   _dcdt_changed = true;

   // Clear structures if needed:
   if ( _backface ) 
    { mesh()->destroy(); _backface=0; _polygons.init(); }

   // Checks parameters:
   if ( domain.size()<3 || domain.open() )
     gsout.fatal ( "se_dcdt.cpp: domain polygon must be simple and closed in init().");

   // Calculates an external polygon (the border) :
   GS_TRACE2 ( "Calculating External Polygon for domain with "<<domain.size()<<" points..." );
   GsVec2 min, max;
   domain.get_bounding_box ( min, max );
   _xmin=min.x; _xmax=max.x; _ymin=min.y; _ymax=max.y;
   float dx = (_xmax-_xmin)/5.0f;
   float dy = (_ymax-_ymin)/5.0f;
   if ( radius>0 )
    { if ( dx<radius ) dx=radius;
      if ( dy<radius ) dy=radius;
    }
   if ( radius!=0 )
    { _xmax+=dx; _xmin-=dx; _ymax+=dy; _ymin-=dy; }

   // Creates the triangulated external square (the border):
   GS_TRACE2 ( "Creating External Polygon..." );
   SeTriangulator::epsilon ( epsilon );
   _backface = ((SeDcdtSymEdge*)SeTriangulator::init_as_triangulated_square
               ( _xmin, _ymin, _xmax, _ymin, _xmax, _ymax, _xmin, _ymax ))->sym()->fac();

   SeDcdtSymEdge* s = _backface->se();

   GS_TRACE2 ( "Backface v1: " << s->vtx()->p );
   s->vtx()->border = 1;
   s=s->nxt(); 
   GS_TRACE2 ( "Backface v2: " << s->vtx()->p );
   s->vtx()->border = 1;
   s=s->nxt(); 
   GS_TRACE2 ( "Backface v3: " << s->vtx()->p );
   s->vtx()->border = 1;
   s=s->nxt(); 
   GS_TRACE2 ( "Backface v4: " << s->vtx()->p );
   s->vtx()->border = 1;

   _radius = radius;

   // Inserts the vertices of the domain according to radius:
   if ( radius==0 )
    { _using_domain = false;
    }
   else
    { _using_domain = true;
      insert_polygon ( domain );
    }
 }

void SeDcdt::get_bounds ( float& xmin, float& xmax, float& ymin, float& ymax ) const
 {
   xmin=_xmin; xmax=_xmax; ymin=_ymin; ymax=_ymax;
 }

//================================================================================
//=========================== insert polygon ====================================
//================================================================================

int SeDcdt::insert_polygon ( const GsPolygon& pol )
 {
   int i, i1, id;
   SeVertex* v;
   SeFace* sface;

   _dcdt_changed = true;

   GS_TRACE1 ( "Inserting entry in the polygon set..." ); // put in _polygons
   id = _polygons.insert();
   InsPol& ip = *_polygons[id];
   ip.open = pol.open();

   GS_TRACE1 ( "Inserting polygon points..." ); // insert vertices
   sface = get_search_face();
   for ( i=0; i<pol.size(); i++ )
    { v = SeTriangulator::insert_point ( pol[i].x, pol[i].y, sface );
      if ( !v ) gsout.fatal ( "se_dcdt.cpp: search failure in _insert_polygon()." );
      ip.push() = (SeDcdtVertex*)v;
      sface = v->se()->fac();
    }

   // should we keep here collinear vertices (which are not corners)? 
   // they can be removed as Steiner vertices when removing another intersecting polygon
 
   _cur_search_face=0; // Needed because edge constraint may call kef

   GS_TRACE1 ( "Inserting polygon edges constraints..." ); // insert edges
   for ( i=0; i<ip.size(); i++ )
    { i1 = (i+1)%ip.size();
      if ( i1==0 && ip.open ) break; // do not close the polygon
      if ( !SeTriangulator::insert_line_constraint ( ip[i], ip[i1], id ) ) 
        gsout.fatal ( "se_dcdt.cpp: unable to insert constraint in _insert_polygon()." );
    }

   return id;
 }


int SeDcdt::polygon_maxid () const
 {
   return _polygons.maxid();
 }

int SeDcdt::num_polygons () const
 {
   return _polygons.elements();
 }

//================================================================================
//=========================== remove polygon ====================================
//================================================================================

void SeDcdt::_find_intermediate_vertices_and_edges ( SeDcdtVertex* vini, int oid )
 {
   SeDcdtEdge *e;
   SeDcdtVertex *v;
   SeDcdtSymEdge *s, *si;

   mesh()->begin_marking ();
   _varray.size(0); _earray.size(0); _stack.size(0);

   // initialize stack with all constrained edges from v:
   _varray.push() = vini;
   mesh()->mark ( vini );
   s = si = vini->se();
   do { e = s->edg();
        if ( e->has_id(oid) )
         { _stack.push() = s;
           _earray.push() = s;
           mesh()->mark ( e );
         }
        s = s->rot();
      } while ( s!=si );

   // advance untill all edges are reached:
   while ( _stack.size() )
    { s = si = _stack.pop()->nxt();
      v = s->vtx();
      if ( !mesh()->marked(v) )
       { _varray.push() = v;
         mesh()->mark ( v );
       }
      do { e = s->edg();
           if ( !mesh()->marked(e) && e->has_id(oid) )
            { _stack.push() = s;
              _earray.push() = s;
              mesh()->mark ( e );
            }
           else
            { SeDcdtVertex* v = s->sym()->vtx();
              if ( v->refinement && !mesh()->marked(v) )
               { mesh()->mark(v);
                 _varray.push() = v;
               }
            }
           s = s->rot();
         } while ( s!=si );
    }

   mesh()->end_marking ();
 }

bool SeDcdt::_is_intersection_vertex ( SeDcdtVertex* v, int id, SeDcdtVertex*& v1, SeDcdtVertex*& v2 )
 {
   SeDcdtSymEdge *si, *s;
   v1 = v2 = 0;

   // at this point, it is guaranteed that only two constraints with index id
   // are adjacent to v, because there is a test that ensures that only two
   // constrained edges are incident to v in _remove_vertex_if_possible().
   // (however these two constrained edges may have more than one id)

   si = s = v->se();
   do { if ( s->edg()->has_id(id) )
         { if ( v1==0 ) v1=s->nxt()->vtx();
            else if ( v2==0 ) { v2=s->nxt()->vtx(); break; }
         }
        s = s->rot();
      } while ( s!=si );

   if ( v1==0 || v2==0 ) return false; // v is an extremity of an open constraint

   double d = gs_point_segment_dist ( v->p.x, v->p.y, v1->p.x, v1->p.y, v2->p.x, v2->p.y );
   return d<=SeTriangulator::epsilon()? true:false;
 }

void SeDcdt::_remove_vertex_if_possible ( SeDcdtVertex* v, const GsArray<int>& vids )
 {
   GS_TRACE4 ( "Try to remove vertex with ref="<<vids.size()<<"..." );

   // Easier case, vertex is no more used:
   if ( vids.size()==0 ) // dangling vertex
    { GS_TRACE4 ( "Removing dangling vertex" );
      SeTriangulator::remove_vertex(v);
      return;
    }

   int i;
   int num_of_incident_constrained_edges=0;
   SeDcdtSymEdge *s;
   s = v->se();
   do { if ( s->edg()->is_constrained() ) num_of_incident_constrained_edges++;
        s = s->rot();
      } while ( s!=v->se() );

   if ( num_of_incident_constrained_edges!=2 ) return;

   // test if all polygons using the vertex no more need it:
   SeDcdtVertex *v1, *v2;
   GsArray<SeDcdtVertex*>& va = _varray2; // internal buffer
   va.size ( 0 );

   // (note that we can have vids.size()>1 at this point)
   for ( i=0; i<vids.size(); i++ )
    { if ( !_is_intersection_vertex(v,vids[i],v1,v2) )
       { GS_TRACE4 ( "Not an intersection vertex!" );
         return;
       }
      va.push()=v1; va.push()=v2;
    }

   GS_TRACE4 ( "Removing one intersection vertex..." );
   SeTriangulator::remove_vertex(v);

   // and recover all deleted constraints:
   for ( i=0; i<vids.size(); i++ )
    { SeTriangulator::insert_line_constraint ( va[i*2], va[i*2+1], vids[i] );
    }
 }

void SeDcdt::remove_polygon ( int polygonid )
 {
   int i;
   GS_TRACE4 ( "Entering remove_polygon..." );

   _dcdt_changed = true;

   if ( polygonid==0 )
     gsout.fatal("se_dcdt.cpp: domain cannot be removed by remove_polygon().");

   if ( polygonid<0 || polygonid>_polygons.maxid() )
     gsout.fatal("se_dcdt.cpp: invalid id sent to remove_polygon().");

   if ( !_polygons[polygonid] )
     gsout.fatal("se_dcdt.cpp: remove_polygon(): polygon already removed.");

   // search_face can be invalidated so make it unavailable:
   _cur_search_face = 0;

   // Recuperate intermediate vertices inserted as Steiner points:
   GS_TRACE4 ( "Recuperating full polygon..." );
   _find_intermediate_vertices_and_edges ( _polygons[polygonid]->get(0), polygonid ); 
   GS_TRACE4 ( "polygon has "<<_varray.size()<<" vertices " << "and "<<_earray.size()<<" edges." ); 

   // Remove all ids which are equal to polygonid in the edge constraints
   GS_TRACE4 ( "Updating edge constraints..." );
   for ( i=0; i<_earray.size(); i++ )
    { _earray[i]->edg()->remove_id ( polygonid ); // remove all occurences
    }

   GS_TRACE4 ( "Removing free vertices..." );
   for ( i=0; i<_varray.size(); i++ )
    { _varray[i]->get_references ( _ibuffer ); //vobs
      GS_TRACE4 ( "Vertex "<<i<<": (" << _varray[i]->p << ")" );
      _remove_vertex_if_possible ( _varray[i], _ibuffer );
    }

   _polygons.remove(polygonid);
   GS_TRACE4 ( "Finished." );
 }

//================================================================================
//============================= get polygon ======================================
//================================================================================

bool SeDcdt::get_polygon ( int polygonid, GsPolygon& polygon )
 { 
   polygon.size(0);

   if ( polygonid<0 || polygonid>_polygons.maxid() ) return false;
   if ( !_polygons[polygonid] ) return false;

   InsPol& p = *_polygons[polygonid];

   int i;
   polygon.size ( p.size() );
   polygon.open ( p.open? true:false );
   for ( i=0; i<p.size(); i++ )
    polygon[i] = p[i]->p;
   return true;
 }

void SeDcdt::get_triangulated_polygon ( int polygonid, GsArray<SeDcdtVertex*>* vtxs, GsArray<SeDcdtEdge*>* edgs )
 { 
   if ( polygonid<0 || polygonid>_polygons.maxid() ) return;
   if ( !_polygons[polygonid] ) return;

   _find_intermediate_vertices_and_edges ( _polygons[polygonid]->get(0), polygonid ); 

   if ( vtxs ) *vtxs = _varray;

   if ( edgs ) // need to convert symedges to edges
    { int i;
      edgs->size ( _earray.size() );
      for ( i=0; i<edgs->size(); i++ ) (*edgs)[i] = _earray[i]->edg();
    }
 }

//================================================================================
//=========================== ray intersection ===================================
//================================================================================

void SeDcdt::ray_intersection ( float x1, float y1, float x2, float y2,
                                GsArray<int>& polygons, int depth,
                                GsArray<GsPnt2>* pts )
 {
   SeTriangulator::LocateResult res;
   SeBase* ses;

   polygons.size(0);
   if ( pts ) pts->size(0);
   if ( depth==0 ) return;

   double p1x=x1, p1y=y1, p2x=x2, p2y=y2;

   res = SeTriangulator::locate_point ( get_search_face(), p1x, p1y, ses );
   if ( res==SeTriangulator::NotFound )
    { GS_TRACE7 ( "First point not found!" );
      return;
    }

    GsArray<SeDcdtSymEdge*> _earray;  // internal buffer
    _earray.size(0);

    SeTriangulator::ray_intersection ( ses, p1x, p1y, p2x, p2y, depth, (GsArray<SeBase*>*)&_earray, pts );
   int i, j;
   for ( i=0; i<_earray.size(); i++ )
    { GsArray<int>& a = _earray[i]->edg()->ids;
      for ( j=0; j<a.size(); j++ ) polygons.push()=a[j];
    }
 }

bool SeDcdt::polygon_intersection ( const GsPolygon& p )
 {
   GsArray<int> pols;
   int i1, i2;
   for ( i1=0; i1<p.size(); i1++ )
    { i2 = (i1+1)%p.size();
      ray_intersection ( p[i1].x, p[i1].y, p[i2].x, p[i2].y, pols, 1 );
      if ( pols.size()>0 ) return true;
    }
   return false;
 }

bool SeDcdt::disc_free ( float cx, float cy, float r )
 {
   SeBase *s;
   SeTriangulator::LocateResult res;

   res = SeTriangulator::locate_point ( get_search_face(), cx, cy, s );
   if ( res==SeTriangulator::NotFound )
    { GS_TRACE7 ( "Circle Free: center not located!" );
      return false;
    }

   _cur_search_face = ((SeDcdtSymEdge*)s)->fac();

   return SeTriangulator::disc_free ( s, cx, cy, r );
 }

void SeDcdt::_add_contour ( SeDcdtSymEdge* s, GsPolygon& vertices, GsArray<int>& pindices )
 {
   SeDcdtSymEdge* si=s;
   pindices.push() = vertices.size();

   GS_TRACE9 ( "Begin contour: "<<pindices.top() );
   do { vertices.push() = s->vtx()->p;
        mesh()->mark ( s->edg() );         
        do { s=s->rot(); } while ( !s->edg()->is_constrained() );
        s = s->sym();
      } while ( s!=si );

   pindices.push() = vertices.size()-1;
   GS_TRACE9 ( "End contour: "<<pindices.top() );
 }

SeDcdtSymEdge* SeDcdt::_find_one_obstacle ()
 {
   SeDcdtSymEdge *s;

   while ( 1 )
    { if ( _earray.empty() ) break;

      s = _earray.pop();
      if ( mesh()->marked(s->edg()) ) continue;
      if ( s->edg()->is_constrained() ) return s;
      mesh()->mark(s->edg());

      s = s->sym()->nxt();
      if ( !mesh()->marked(s->edg()) ) { _earray.push()=s; }
      s = s->nxt();
      if ( !mesh()->marked(s->edg()) ) { _earray.push()=s; }
    }

   return 0;
 }

void SeDcdt::extract_contours ( GsPolygon& vertices, GsArray<int>& pindices, float x, float y )
 {
   SeTriangulator::LocateResult res;
   SeBase *ses;

   vertices.size(0);
   pindices.size(0);

   GS_TRACE9 ( "Begin Extract Contours" );

   res = SeTriangulator::locate_point ( get_search_face(), x, y, ses );
   if ( res==SeTriangulator::NotFound ) return;

   mesh()->begin_marking();
   _earray.size(0);
   SeDcdtSymEdge *s, *si;
   s = si = (SeDcdtSymEdge*)ses;
   do { _earray.push() = s;
        s = s->nxt();
      } while ( s!=si );

   while (1)
    { s = _find_one_obstacle ();
      if ( !s ) break;
      _add_contour ( s, vertices, pindices );
    }

   mesh()->end_marking();
   GS_TRACE9 ( "End Extract" );
 }

void SeDcdt::extract_faces ( GsArray<SeDcdtFace*>& faces, float x, float y )
 {
   SeTriangulator::LocateResult res;
   SeBase *ses;

   faces.size(0);

   GS_TRACE9 ( "Begin Extract Faces" );

   res = SeTriangulator::locate_point ( get_search_face(), x, y, ses );
   if ( res==SeTriangulator::NotFound ) return;

   mesh()->begin_marking();
   _earray.size(0);
   SeDcdtSymEdge *s = (SeDcdtSymEdge*)ses;
   mesh()->mark(s->fac());
   faces.push() = s->fac();
   _earray.push() = s;
   _earray.push() = s->nxt();
   _earray.push() = s->pri();

   while (_earray.size())
    { s = _earray.pop()->sym();
      if ( !mesh()->marked(s->fac()) && !s->edg()->is_constrained() )
       { mesh()->mark(s->fac());
         faces.push() = s->fac();
         _earray.push() = s->nxt();
         _earray.push() = s->pri();
       }
    }

   mesh()->end_marking();
   GS_TRACE9 ( "End Extract" );
 }

//================================================================================
//=========================== inside polygon =====================================
//================================================================================

static int interhoriz ( float px, float py, float p1x, float p1y, float p2x, float p2y )
 {
   if ( p1y>p2y ) { float tmp; GS_SWAP(p1x,p2x); GS_SWAP(p1y,p2y); } // swap
   if ( p1y>=py ) return false; // not intercepting
   if ( p2y<py  ) return false; // not intercepting or = max 
   float x2 = p1x + (py-p1y) * (p2x-p1x) / (p2y-p1y);
   return (px<x2)? true:false;
 }

int SeDcdt::inside_polygon ( float x, float y , GsArray<int>* allpolys )
 {
   if ( _polygons.elements()<=1 ) return -1; // if there is only the domain, return.

   int cont=0, i, size;
   InsPol* pol;
   SeDcdtVertex *v1, *v2;

   if ( allpolys ) allpolys->size(0);

   int polid=0;
   if ( _using_domain ) polid++;
   for ( ; polid<=_polygons.maxid(); polid++ )
    { pol = _polygons[polid];
      if ( !pol ) continue;

      size = pol->size();
      for ( i=0; i<size; i++ )
       { v1 = pol->get(i);
         v2 = pol->get( (i+1)%size );
         cont ^= interhoriz ( x, y, v1->p.x, v1->p.y, v2->p.x, v2->p.y );
       }
      if (cont) 
       { if ( allpolys ) allpolys->push()=polid;
          else return polid;
       }
    }

   if ( allpolys ) if ( allpolys->size()>0 ) return allpolys->get(0);
   return -1;
 }

int SeDcdt::pick_polygon ( float x, float y )
 {
   if ( _polygons.elements()==0 ) return -1;
   if ( _using_domain && _polygons.elements()==1 ) return -1; // if there is only the domain, return.

   SeTriangulator::LocateResult res;
   SeBase *sse;

   res = SeTriangulator::locate_point ( get_search_face(), x, y, sse );
   if ( res==SeTriangulator::NotFound ) return -1;

   SeDcdtSymEdge *s = (SeDcdtSymEdge*)sse;
   GsPnt2 pt(x,y);
   float d1 = dist2 ( pt, s->vtx()->p );
   float d2 = dist2 ( pt, s->nxt()->vtx()->p );
   float d3 = dist2 ( pt, s->nxt()->nxt()->vtx()->p );
   if ( d2<=d1 && d2<=d3 ) s=s->nxt();
   if ( d3<=d1 && d3<=d2 ) s=s->nxt()->nxt();

   SeDcdtSymEdge* k=s;
   do { GsArray<int>& ids = k->edg()->ids;
        if ( ids.size()>0 ) { if ( !_using_domain || ids[0]>0 ) return ids[0]; } // 0 is the domain
        k=k->rot();
      } while ( k!=s );

   return -1;
 }

void SeDcdt::statistics ( int& nj, int& nc, int& nd, int& ni, int& nb, int& nce, int& nfe )
 {
   SeFace* f;
   SeFace* fi;
   SeDcdtSymEdge* s;
   SeDcdtSymEdge* sn;
   SeDcdtSymEdge* sp;
   int n;
   nj=0; nc=0; nd=0; ni=0; nb=0;

   fi = f = _backface;
   do { s = (SeDcdtSymEdge*)f->se();
        sn = s->nxt();
        sp = sn->nxt(); 
        if ( s->vtx()->border || sn->vtx()->border || sp->vtx()->border )
         { if ( f!=_backface ) nb++;
         }
        else
         { n=0;
           if ( s->edg()->is_constrained() ) n++;
           if ( sn->edg()->is_constrained() ) n++;
           if ( sp->edg()->is_constrained() ) n++;
           switch ( n )
            { case 1: nc++; break;
              case 2: nd++; break;
              case 3: ni++; break;
              default: nj++; break;
            }
         }
        f = f->nxt();
      } while ( f!=fi );

   nce=0; nfe=0;

   SeDcdtEdge* e;
   SeDcdtEdge* ei;
   ei = e = _backface->se()->edg();
   do { if ( !e->border() )
         { if ( e->is_constrained() )
            { nce++; }
           else
            { nfe++; }
         }
        e = e->nxt();
      } while ( e!=ei );
 }

//================================================================================
//=============================== search path ====================================
//================================================================================

bool SeDcdt::search_channel ( float x1, float y1, float x2, float y2, const SeFace* iniface )
 {
   // fast security test to ensure at least that points are not outside the border limits:
   if ( x1<_xmin || x1>_xmax || x2<_xmin || x2>_xmax ) return false;

   if ( !iniface ) iniface = get_search_face();

   bool found = SeTriangulator::search_channel ( x1, y1, x2, y2, iniface );

   // to optimize searching for next queries around the same point,
   // we set the next starting search face to the first channel face:
   if ( SeTriangulator::get_channel_interior_edges().size()>0 )
     _cur_search_face = (SeDcdtFace*)SeTriangulator::get_channel_interior_edges()[0]->fac();

   return found;
 }

//============================ End of File ===============================

