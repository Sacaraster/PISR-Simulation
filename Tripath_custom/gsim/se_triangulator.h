/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef SE_TRIANGULATOR_H
# define SE_TRIANGULATOR_H

/** \file se_triangulator.h 
 * A triangulator based on symedges
 */

# include <gsim/gs_polygon.h>
# include <gsim/se_mesh.h>
# include <gsim/se_triangulator_manager.h>

/*! \class SeTriangulator se_triangulator.h
    \brief Delaunay triangulation methods

    SeTriangulator provides several methods to construct and update
    triangulations, including Delaunay triangulations with support 
    to constrained and conforming versions.
    To use it, a SeTriangulatorManager class is required in order
    to tell how the triangulator can access geometric data in the user mesh.
    The triangulator methods were made to be of flexible use for different
    applications, so that they mainly only perform the basic algorithms,
    leaving many decisions to the user. In this way it is left as user
    responsibility to correctly use them. For instance, the associated mesh
    must be in fact a triangulation with counter-clockwise triangles, etc,
    otherwise several algorithms will fail.
    Most of the used techniques are described in the following paper:
       M. Kallmann, H. Bieri, and D. Thalmann, "Fully Dynamic Constrained
       Delaunay Triangulations", In Geometric Modelling for Scientific 
       Visualization, G. Brunnett, B. Hamann, H. Mueller, L. Linsen (Eds.),
       ISBN 3-540-40116-4, Springer-Verlag, Heidelberg, Germany, pp. 241-257, 2003.
    All used epsilon-based geometric primitives are in gs_geo2.h/cpp */
class SeTriangulator : public GsShared
 { public :

    /*! Note: ModeConforming is not always robust when several constraints intersect
        as the used midpoint insertion algorithm may generae many insertions (there 
        is a O(n^3) algorithm that could be used instead). */
    enum Mode { ModeUnconstrained, ModeConforming, ModeConstrained };

    /*! Used by locate_point() method */
    enum LocateResult { NotFound, TriangleFound, EdgeFound, VertexFound };

    /*! Forward declaration of internal classes */
    class FunnelPt;
    class FunnelPath;
    class FunnelDeque;
    class PathNode;
    class PathTree;

   protected :
    Mode _mode;
    double _epsilon, _epsilon2, _epsilonedg;
    SeMeshBase* _mesh;
    SeTriangulatorManager* _man;
    struct ConstrElem { SeVertex* v; SeBase* e;
                        void set(SeVertex* a, SeBase* b) {v=a; e=b;}
                      };
    GsArray<ConstrElem> _elembuffer; // buffer used by _constrain_line
    GsArray<SeBase*> _buffer;        // buffer used in several places
    GsArray<int> _ibuffer;           // buffer used in several places
    PathTree* _ptree;
    GsArray<SeBase*> _channel;
    double _xi, _yi, _xg, _yg;
    FunnelDeque* _fdeque;
    bool _path_found;
    bool _debug_mode;

   public :

    /*! Three modes of triangulations can be created: unconstrained, conforming,
        or constrained. Depending on the mode some internal methods will
        behave differently. In particular, for unconstrained triangulations
        it is not required to reimplement the methods of SeTriangulatorManager
        dealing with is/set/get/add edge constraints.
        The documentation of each method should state which methods are used from
        both the associated manager and geometric primitives.
        Important: both the given mesh and triang manager are considered shared here,
        and therefore their unref() are called by the destructor.
        The epsilon is used in the geometric primitives. */ 
    SeTriangulator ( Mode mode, SeMeshBase* m, SeTriangulatorManager* man, double epsilon );

    /*! The destructor unreferences the associated SeTriangulatorManager,
        but it does not delete the associated mesh. */
    virtual ~SeTriangulator ();

    /*! Set a new epsilon (epsilonedg is also set to be the same) */
    void epsilon ( double eps ) { _epsilon=_epsilonedg=eps; _epsilon2=eps*eps; }

    /*! Get the currently used epsilon */
    double epsilon () const { return _epsilon; }

    /*! Set a different epsilon specifically for vtx-edg tests */
    void epsilonedg ( double epsedg ) { _epsilonedg=epsedg; }

    /*! Returns the associated mesh pointer */
    SeMeshBase* mesh () const { return _mesh; }

    /*! Returns the associated manager pointer */
    SeTriangulatorManager* man () const { return _man; }

    /*! Set the desired mode */
    void mode ( Mode m ) { _mode=m; }

    /*! Get the current triangulator mode */
    Mode mode () const { return _mode; }

    /*! When debug mode is true, several (expensive) validity tests are
        performed by calling checkall() after each operation. Report
        messages are sent to gsout */
    bool debug_mode () { return _debug_mode? true:false; } 

    /*! Change the debug mode status. The default mode is false. */
    void debug_mode ( bool b ) { _debug_mode=b?1:0; }

    /*! Checks the correctness of the triangulation:
        if the back face/border is consistent, if all faces are 
        ccw triangles, non degenerate, etc.
        True is returned if all the tests are succesfull and
        false is returned otherwise. This method is to be used
        for debug purposes only. If out is true (the default)
        messages are sent to gsout. If fatal is true, gsout.fatal()
        is called when the first error is found. */
    bool checkall ( bool out=true, bool fatal=true, int* numerrors=0, GsArray<SeBase*>* fa=0 );

    /*! Checks one triangle and returns the number of errors encountered. */
    int check ( SeBase* s, bool out, bool fatal, GsArray<SeBase*>* fa=0 );

   public :

    /*! This method destroys the associated mesh and initializes it as
        a triangulated square with the given coordinates. Points must
        be passed in counter clockwise order. The returned SeBase
        is adjacent to vertex 1, edge {1,2} and triangle {1,2,4}.
        To construct a square with given maximum and minimum coordinates,
        points should be set as follows (respectivelly):
        xmin, ymin, xmax, ymin, xmax, ymax, xmin, ymax.
        Alternatively, the user may initialize the associated mesh directly
        by calling the construction operators of SeMesh (such as mev and mef).
        Normally the mesh should be initialized as a triangulated convex polygon.
        Initialization is done in such a way that the back face can be recovered with:
        SeFace* backface = mesh()->first()->sym()->fac(); */
    SeBase* init_as_triangulated_square ( double x1, double y1, double x2, double y2,
                                          double x3, double y3, double x4, double y4 );

    /*! This method destroys the associated mesh and initializes it as
        triangle with the given coordinates. Points must be passed in counter
        clockwise order. The returned SeBase is adjacent to vertex 1 and edge {1,2}.
        Initialization is done in such a way that the back face can be recovered with:
        SeFace* backface = mesh()->first()->sym()->fac(); */
    SeBase* init_as_triangle ( double x1, double y1, double x2, double y2,
                               double x3, double y3 );

    /*! Triangulates a counter-clockwise (ccw) oriented face. It is the
        user responsability to provide a face in the correct ccw orientation.
        False is returned in case the face cannot be triangulated. However,
        this migth only happen if the face is not simple.
        It uses the so called "ear" algorithm, that has worst case complexity
        of O(n^2), but has a very simple implementation.
        It uses the geometric primitives gs_ccw() and gs_in_triangle() and
        each time an edge is inserted during the triangulation construction,
        the method new_edge_created() of the associated manager is called.
        If optimization is set to true (the default), the algorithm will
        flip the created edges to satisfy the Delaunay criterion, and for
        this, the geometric primitive gs_in_circle() is also used. */
    bool triangulate_face ( SeFace* f, bool optimize=true );

    /*! This method searches for the location containing the given point.
        The enumerator LocateResult and parameter result are returned.
        Four cases can occur:
        (a) If the point is coincident to an existing vertex, VertexFound
            is returned, and result is adjacent to the vertex.
        (b) If the point is found on an edge, EdgeFound is returned,
            and result is adjacent to the edge.
        (c) If the point is inside a triangle, TriangleFound is returned,
            and result is adjacent to the found triangle.
        (d) In the case the point is not found to be inside the triangulation,
            NotFound is returned.
        The algorithm starts with the given iniface, and continously
        skips to the neighbour triangle which shares an edge separating the
        current triangle and the point to reach in two different semi spaces.
        To avoid possible loops, triangle marking is used.
        The "distance" of the given iniface and the point to search 
        dictates the performance of the algorithm. The agorithm is O(n)
        (n = number of triangles). In general the pratical performance
        is much better than O(n), eg, O(sqrt(n)) for a uniform distribution
        in a square. This algorithm may fail to find the point if the border
        of the triangulation is not convex or if there are errors in the 
        triangulation. It is mainly based on the geometric primitive gs_ccw().
        But it also calls methods gs_in_segment() in order to determine
        if the point is in an existing edge or vertex, up to epsilon.
        If no precise location is needed, for expert use, loctest can be set 
        to false, and with fewer tests only NotFound or TriangleFound is returned. */
    LocateResult locate_point ( const SeFace* iniface,
                                double x, double y, SeBase*& result, bool loctest=true );

    /*! Insert a point in the given triangle, with the given coordinates,
        and then continously flips its edges to ensure the Delaunay criterion.
        The geometric primitive gs_in_circle() is called during this process.
        The new vertex inserted is returned and will be never 0 (null).
        Methods new_vertex_created() and new_edge_created() of the associated
        manager are called for the new three edges and one vertex created.
        If the triangulator is of type conforming, methods for managing the
        constraints ids of edges are called, and to maintain the correctness
        of the triangulation, new vertices might be automatically inserted
        in a recursive mid-point subdivision way. If the triangulator is 
        in constrained mode no Steiner vertices are added. */
    SeVertex* insert_point_in_face ( SeFace* f, double x, double y );

    /*! Insert a point in the given edge, with the given coordinates.
        The given point is automatically projected to the edge to ensure the
        correctness of the insertion. If the projected point is within epsilon to
        the endpoints, no insertion is made and the existing vertex is returned.
        When a new point is inserted, edges are automatically flipped to ensure
        the Delaunay criterion, as in insert_point_in_face() */
    SeVertex* insert_point_in_edge ( SeEdge* e, double x, double y );

    /*! Insert point searches the location of (x,y) and correclty insert it in case it
        is located in an edge or face. If a coincident vertex is found, no insertion
        is done and the coincident vertex is returned.
        If iniface is null, the search starts from mesh()->first()->fac().
        Null is returned in case the point cannot be located. */
    SeVertex* insert_point ( double x, double y, const SeFace* iniface=0 );

    /*! Removes a vertex from the Delaunay triangulation. It is the user responsibility
        to guarantee that the vertex being removed is inside a triangulation, and not
        at the border of a face which is not triangular.
        This method simply calls SeMesh::delv() and then retriangulates the created
        non-triangular polygon, so that no special actions are taken concerning
        constrained edges. */
    bool remove_vertex ( SeVertex* v );

    /*! Inserts a line constraint to the current Delaunay triangulation. The line 
        is defined by two existing vertices. This method has a different behavior
        depending on the actual mode of the triangulator:
        If the triangulator is in unconstrained mode, nothing is done.
        If the mode is conforming, Steiner points are inserted starting with the
        middle point of the missing constraint, and recursively inserted by binary 
        partition until the line becomes present in the triangulation.
        If the triangulator is in constrained mode, Steiner points are only inserted
        at the intersection of existing constraints, if there are any. Hence,
        constrained edges do not respect the Delaunay criterion.
        False is returned if the algorithm fails, what can occur if non ccw or non
        triangular cells are found, or in unconstrained mode.
        The id parameter allows the user to keep track of the created constraints,
        and it is passed to the corresponding manager method.
        Note that edges may be referenced by several
        constrained lines in the case overllap occurs, and thats why each edge
        of the triangulation should maintain an array of constraints ids.
        Methods new_vertex_created() and vertex_found_in_constrained_edge() of
        the manager are called to allow tracking the insertion of Steiner points. */
    bool insert_line_constraint ( SeVertex *v1, SeVertex *v2, int id );

    /*! Inserts the two points with insert_point() and then call insert_line_constraint().
        Returns the success or failure of the operation */
    bool insert_segment ( double x1, double y1, double x2, double y2, int id, const SeFace* inifac=0 );

    /*! Returns true if segment [(x1,y1),(x2,y2)] crosses a constraint and false otherwise.
        Symedge se must be adjacent to the face containing (x1,y1), which should not be
        exactly at edge se->edg(). Point (x2,y2) must be inside the triangulation.
        Intersections controls how many constraint intersections are computed (the default is one).
        If given, arrays constraints, pts and edges will contain constrained edges traversed, 
        intersection points with constrained edges, and all edges traversed. */
    bool ray_intersection ( SeBase* se, double x1, double y1, double x2, double y2,
                            int intersections=1, GsArray<SeBase*>* constraints=0,
                            GsArray<GsPnt2>* pts=0, GsArray<SeBase*>* edges=0 );

    /*! Low level distance test which recursivelly checks sector clearance. */
    bool sector_free ( SeBase* s, double r2, double cx, double cy, 
                       double x1, double y1, double x2, double y2 );

    /*! Returns true if circle interior and boundary do not intersect any constrained edge.
        Parameter s has to be adjacent to the face containing the center point. */
    bool disc_free ( SeBase* s, double cx, double cy, double r );

    /*! Search for a sequence of triangles (e.g. a channel) connecting x1,y1 and x2,y2,
        without crossing edges marked as constrained. A "navigation A* heuristic" is used.
        If true is returned, the channel and paths inside the channel can be retrieved
        with methods get_channel(), get_channel_skeleton(), get_shortest_path().
        Note that the channel may not be the globally shortest one.
        Parameter iniface is required in order to feed the process of finding
        the triangle containing the initial point p1. If it is already the triangle
        containing p1, the search will be solved trivially. */
    bool search_channel ( double x1, double y1, double x2, double y2, const SeFace* iniface );

    /*! Returns a reference to the list with the interior edges of the last channel
        determined by a sussesfull call to search_channel */
    const GsArray<SeBase*>& get_channel_interior_edges () const { return _channel; }

    /*! Returns a polygon describing the current channel, and thus, method search_channel() must
        be succesfully called before to determine the channel to consider. */
    void get_channel_boundary ( GsPolygon& channel );

    /*! Returns the canonical path, which is the path passing through the midpoint of
        the channel interior edges. Method search_channel must be succesfully called before
        in order to determine the channel. The path is returned as an open polygon. */
    void get_channel_skeleton ( GsPolygon& path );

    /*! Returns the shortest path inside the current channel using the funnel algorithm.
        Method search_channel() has to be called before for determining the channel. */
    void make_funnel_path ( GsPolygon& path );

    /*! This refinement function subdivides long edges with recursive bisections.
        Returns the number of subdivisions, ie, the number of midpoints added. */
    int refine_edges ( float targetlen, bool markvref=true );

   protected :
    void _propagate_delaunay ();
    bool _conform_line  ( SeVertex*, SeVertex*, const GsArray<int>& );
    bool _constrain_line ( SeVertex*, SeVertex*, const GsArray<int>& );
    void _v_next_step ( SeBase* s, SeVertex* v1, SeVertex* v2, SeVertex*& v, SeBase*& e );
    void _e_next_step ( SeBase* s, SeVertex* v1, SeVertex* v2, SeVertex*& v, SeBase*& e );
    bool _canconnect ( SeBase* s2, SeBase* sv, double x2, double y2, double x2n, double y2n );
    void _maketri ( GsArray<SeBase*>& totrig, SeBase* s2, SeBase* s1, SeBase* s );
    void _getcostpoint ( PathNode* n, double xn, double yn, double x1, double y1, double x2, double y2, double& x, double& y, double r=0 );
    void _trytoadd ( SeBase* s, int mi, PathNode* n=0, double x1=0, double y1=0, double x2=0, double y2=0 );
    void _ptree_init ( LocateResult res, SeBase* s, float r=0 );
    int  _expand_lowest_cost_leaf ();
    void _funneladd ( bool intop, GsPolygon& path, const GsPnt2& p );
 };

//============================ End of File =================================

# endif // SE_TRIANGULATOR_H

