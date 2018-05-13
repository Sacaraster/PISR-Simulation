/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef SE_DCDT_H
# define SE_DCDT_H

/** \file se_dcdt.h
 * Dynamic Constrained Delaunay Triangulation
 */

# include <gsim/gs_set.h>
# include <gsim/gs_vec2.h>
# include <gsim/gs_polygon.h>
# include <gsim/se_triangulator.h>
# include <gsim/se_dcdt_manager.h>

//=================================== DCDT class ========================================

/*! Mantains a dynamic constrained Delaunay triangulation of given polygons
    for the purpose of path planning and other queries. */
class SeDcdt : public SeTriangulator
 { protected :
    struct InsPol : public GsArray<SeDcdtVertex*> // an "inserted polygon"
    { gscbool open;
      InsPol () { open=0; }
      InsPol ( const InsPol& ob ) : GsArray<SeDcdtVertex*>(ob) { open=0; }
    };
    SeDcdtFace* _backface;         // the CW back face
    SeDcdtFace* _cur_search_face;  // a face near the last change in the mesh, 0 if not valid
    GsSet<InsPol> _polygons;       // inserted polygons, polygon 0 is always the domain
    GsArray<SeDcdtVertex*>  _varray;  // internal buffer
    GsArray<SeDcdtSymEdge*> _earray;  // internal buffer
    GsArray<SeDcdtSymEdge*> _stack;   // internal buffer
    GsArray<SeDcdtVertex*>  _varray2; // internal buffer
    GsArray<int> _ibuffer;            // internal buffer
    bool _dcdt_changed;            // internal flag for clearance operations
    bool _using_domain;            // internal flag
    float _xmin, _xmax, _ymin, _ymax, _radius; // domain info
    void _construct ();

   public :

    /*! The default constructor uses elements declared in se_dcdt_elements.h  */
    SeDcdt ( double epsilon=0.000001 );

    /*! Constructor requiring a mesh and a DcdtManager (both are shared).
        This constructor allows the user to create its own element types, however
        the elements must derive the elements declared in se_dcdt_elements.h,
        and the given mesh (m) must be properly initilized. */
    SeDcdt ( SeMeshBase* m, SeDcdtManager* man, double epsilon=0.000001 );

    /*! Destructor */
    virtual ~SeDcdt ();

    /*! Returns the maintained mesh (to be used only for traversals) */
    SeDcdtMesh* mesh() const { return (SeDcdtMesh*)SeTriangulator::mesh(); }

    /*! Returns the associated (and shared) manager pointer */
    SeDcdtManager* man () const { return (SeDcdtManager*)SeTriangulator::man(); }

    /*! Put in the given arrays the coordinates of the constrained and
        unconstrained edges endpoints. Each two consecutive points in the
        returned arrays give the first and end points of one dge.
        Parameters can be null indicating that no data of that type is requested.
        The two parameters are also allowed to point to the same array */
    void get_mesh_edges ( GsArray<GsPnt2>* constr, GsArray<GsPnt2>* unconstr );

    /*! Extended version of get_mesh_edges() for the different types of edges. 
        (this is a more expensive function due the non-optimized border determination) */
    void get_mesh_edges ( GsArray<GsPnt2>* constr, GsArray<GsPnt2>* unconstr, 
                          GsArray<GsPnt2>* domain, GsArray<GsPnt2>* border );

    /*! Extract and export contours */
    int export_contours ( GsOutput& out, float x, float y );

    /*! Save the current dcdt by saving the list of all inserted obstacles.
        Note that the polygons ids are preserved. */
    bool save ( GsOutput& out );

    /*! Destructs the current map and loads a new one */
    bool load ( GsInput& inp );
    
    /*! Initializes the triangulation with a domain polygon.
        The domain is considered to be the constraint polygon with id 0; and can
        be retrieved again by calling get_polygon(0).
        An external box is automatically computed as an expanded bounding box of
        the domain, where the expansion vector length is 1/5 of the bounding box sides.
        This external box defines the border of the triangulation and its coordinates
        can be retrieved with get_bounds().
        Note that all polygons inserted afterwards must be inside the external box.
        Parameter radius can be used, for instance, to have a security margin in order
        to allow growing polygons without intersecting with the external box.
        If parameter radius is >0, it is used as minimum length for the
        expansion vector used to compute the external box.
        Special Case: If radius is 0, the domain polygon is not inserted, and the
        triangulation is initialized with border equal to the bounding box of domain.
        Parameter epsilon is simply passed to the triangulator. */
    void init ( const GsPolygon& domain, double epsilon, float radius=-1 );

    /*! Internally, the border is generated containing the domain polygon.
        This method allows to retrieve the coordinates of the border rectangle. */
    void get_bounds ( float& xmin, float& xmax, float& ymin, float& ymax ) const;

    /*! Inserts a polygon as a constraint in the CDT, returning its id.
        In case of error, -1 is returned. Polygons can be open.
        The returned id can be used to remove the polygon later on.
        All kinds of intersections and overllapings are handled.
        Collinear vertices are inserted. If not desired, method
        GsPolygon::remove_collinear_vertices() should be called prior insertion. */
    int insert_polygon ( const GsPolygon& polygon );

    /*! Returns the max id currently being used. Note that a GsSet controls
        the ids, so that the max id may not correspond to the number of 
        polygons inserted; ids values are not changed with polygon removal */
    int polygon_maxid () const;

    /*! Returns the number of inserted polygons, including the domain (if inserted). */
    int num_polygons () const;

    /*! Remove a previoulsy inserted polygon. false may be returned if the id is
        not valid or if some internal error occurs. The domain cannot be removed with
        this method. Steiner points may stay as part of other polygons if the
        triangulation is in conforming mode. */
    void remove_polygon ( int polygonid );

    /*! Retrieves the original vertices of the given polygon (without collinear vertices).
        If the given id is invalid, false is returned and the returned polygon is empty.
        Note: returned polygons preserve original orientation therefore can be CCW or CW.*/
    bool get_polygon ( int polygonid, GsPolygon& polygon );

    /*! Returns the vertices and edges used by the polygon in the triangulation.
        Elements may be out of order, specially when they have intersections.
        If an argument is a null pointer, nothing is done with it. */
    void get_triangulated_polygon ( int polygonid, GsArray<SeDcdtVertex*>* vtxs, GsArray<SeDcdtEdge*>* edgs );

    /*! Returns a list with the ids of the polygons having some edge traversed by the
        segment [(x1,y1),(x2,y2)]. The length of the returned array will not be more 
        than depth, corresponding to 'depth' edges being crossed. Note: the id of a 
        polygon will appear both when the ray enters the polygon and when it leaves 
        the polygon. If depth is <0, no depth control is used. In some contexts, this
        routine can also be used to quickly determine if a point is inside a polygon 
        by looking if the number of intersections is odd or even. */
    void ray_intersection ( float x1, float y1, float x2, float y2,
                            GsArray<int>& polygons, int depth,
                            GsArray<GsPnt2>* pts=0 );

    /*! Returns true if given polygon collides with a constrained edge,
        and false otherwise */
    bool polygon_intersection ( const GsPolygon& p );

    /*! Returns true if circle does not touch constrained edge.
        If needed (mainly for improving performance), use search_face(f) for providing
        a specific initial face for point location. */
    bool disc_free ( float cx, float cy, float r );

    /*! Returns all polygons describing the contours of an "eating virus" starting at x,y.
        Array pindices contains, for each contour, the starting and ending vertex index,
        which are sequentially stored in array vertices. */
    void extract_contours ( GsPolygon& vertices, GsArray<int>& pindices, float x, float y );

    /*! Returns all faces traversed by an "eating virus" starting at x,y and not
         crossing obstacles */
    void extract_faces ( GsArray<SeDcdtFace*>& faces, float x, float y );

    /*! Returns the id of the first found polygon containing the given point (x,y),
        or -1 if no polygons are found. The domain polygon, if used in init(), will not
        be considered. The optional parameter allpolys can be sent to return all polygons
        containing the point, and only the first one.
        Note: this method does a linear search over each polygon, alternativelly, the
        ray_intersection() method might also be used to detect polygon containment. */
    int inside_polygon ( float x, float y, GsArray<int>* allpolys=0 );

    /*! Returns the id of one polygon close to the given point (x,y), or -1 otherwise.
        This method locates the point (x,y) in the triangulation and then takes the
        nearest polygon touching that triangle.
        The domain polygon, if used in init(), will not be considered. */
    int pick_polygon ( float x, float y );

    /*! Search for the channel connecting x1,y1 and x2,y2.
        It simply calls SeTriangulator::search_channel(), however here parameter iniface is optional.
        If iniface is not given, an element close to (x1,y1) of the previous call is used.
        For finding paths with r-clearance, inflate polygons before insertion 
        (use GsPolygon methods for that, or the inflate keyword in a tpm file) */
    bool search_channel ( float x1, float y1, float x2, float y2, const SeFace* iniface=0 );

    /*! Returns a reference to the list with the interior edges of the last channel
        determined by a sussesfull call to search_channel */
    const GsArray<SeBase*>& get_channel_interior_edges () const
          { return SeTriangulator::get_channel_interior_edges(); }

    /*! Returns a polygon describing the current channel, and thus, method search_channel() must
        be succesfully called before to determine the channel to consider.
        Parameter radius specifies a desired clearance distance. */
    void get_channel_boundary ( GsPolygon& channel ) { SeTriangulator::get_channel_boundary(channel); }

    /*! Returns the polygonal line passing through the midpoint of the channel interior edges.
        Method search_channel must be succesfully called in advance in order to determine the channel.
        The path is returned as an open polygon in GsPolygon. */
    void get_channel_skeleton ( GsPolygon& path ) { SeTriangulator::get_channel_skeleton(path); }

    /*! Returns the shortest path inside the current channel using the funnel algorithm. */
    void make_funnel_path ( GsPolygon& path ) { SeTriangulator::make_funnel_path(path); }

    /*! Returns the internal "last used face (triangle)" that is used as seed in locate queries. */
    SeDcdtFace* get_search_face() { return _cur_search_face? _cur_search_face:_backface->se()->sym()->fac(); }
    SeDcdtFace* backface() { return _backface; }

    /*! Explicitly set the "last used face" used as seed in locate queries. Use with care!! */
    void set_search_face ( SeDcdtFace* f ) { _cur_search_face=f; }

    /*! Returns the number of junctions, corridors, dead-end, isolated and border triangles; 
        and also constrained and free edges */
    void statistics ( int& nj, int& nc, int& nd, int& ni, int& nb, int& nce, int& nfe );

   protected :
    void _find_intermediate_vertices_and_edges ( SeDcdtVertex* vini, int oid );
    bool _is_intersection_vertex ( SeDcdtVertex* v, int oid, SeDcdtVertex*& v1, SeDcdtVertex*& v2 );
    void _remove_vertex_if_possible ( SeDcdtVertex* v, const GsArray<int>& vids );
    void _add_contour ( SeDcdtSymEdge* s, GsPolygon& vertices, GsArray<int>& pindices );
    SeDcdtSymEdge* _find_one_obstacle ();
 };

//================================== End of File =========================================

# endif // SE_DCDT_H
