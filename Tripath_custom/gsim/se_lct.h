/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef SE_LCT_H
# define SE_LCT_H

/** \file se_lct.h
 * Computation of paths with clearance from DCDTs
 */

# include <gsim/se_dcdt.h>

//================================== DcdtClear class ========================================

/*! Mantains a DCDT class for the purpose of extracting paths with clearance */
class SeLct : public SeDcdt
 { protected :
    FunnelPath*      _fpath;    // internal buffer;
    SeFace* _fi;                // triangle containing initial point in current query
    int  _finalsearchnode;      // stores the index of the node containing the goal point
    int  _maxfronts;            // stores the max number of nodes in the front queue
    bool _pre_clearance;        // automatic update and use of precomputed clearance information
    bool _auto_refinement;      // automatic refinement flag
    SeDcdtSymEdge* _secblockse;
    double         _secblockd2;
    GsPnt2         _secblockpt;

    enum PathResult { NoPath, TrivialPath, LocalPath, GlobalPath };
    PathResult _path_result;

    enum EntranceType { EntBlocked, EntTrivial, EntNotTrivial };
    struct Entrance { EntranceType type;     // entrance type of the last query
                      SeDcdtSymEdge* s;      // entrance edge
                      bool top;              // top or bottom entrance type
                      int fps1, fps2;        // number of points in the main side and the other side
                      GsArray<SeVertex*> fv; // disturbance vertices, size will be fps1+fps2-2
                      GsArray<GsPnt2> fp;    // coordinates of the funnel side, including circle tangent and entrance corner
                    } _ent[4]; // 0-2: departure, 1:arrival

    struct ExtCl { char l; float r; };
    GsArray<ExtCl> _extcl;

    struct Capsule { double r2, ax, ay, bx, by; };
    struct Sector { double r2, cx, cy, ax, ay, bx, by; };
    void* _fudata;
    void (*_funnelcb) ( void* udata );
    void* _sudata;
    void (*_searchcb) ( void* udata );
    void _construct ();

   public :

    /*! The default constructor just calls the default construcotr of SeDcdt */
    SeLct ( double epsilon=0.000001 );

    /*! Constructor for custom element types, which calls the equivalent constructor of SeDcdt */
    SeLct ( SeMeshBase* m, SeDcdtManager* man, double epsilon=0.000001 );

    /*! Destructor */
    virtual ~SeLct ();

    /*! Search for a sequence of free triangles (e.g. a channel) connecting x1,y1 and x2,y2,
        with guaranteed clearance of radius. The A* heuristic is used.
        If true is returned, a path inside the channel can be then retrieved
        with make_funnel_path(). Note that the channel may not be the globally shortest one. */
    bool search_channel ( float x1, float y1, float x2, float y2, float radius, const SeFace* iniface=0 );

    /*! Use the funnel algorithm to get the shortest path of given clearance radius in the channel
        found by search_channel(). Parameter extclear is in development, leave it zero. */
    void make_funnel_path ( GsPolygon& path, float radius, float dang, float extclear=0 );

    /*! This is the global refinement method which will enforce the local clearance property.
        If parameter force is true, refinements will be checked independently of the internal
        up-to-date flag. The number of refinements performed is returned.
        This method will also precompute clearances automatically. 
        Method search_channel() will automatically call this method when needed. 
        A situation where the user may want to call this method explicitly is before drawing
        the triangulation, to ensure that the refined LCT triangulation is shown. */
    int refine ( bool force=false );

   public : // the methods below this point are for expert users only

    void auto_refinement ( bool b ) { _auto_refinement=b; }
    bool auto_refinement () const { return _auto_refinement; }
    void pre_clearance ( bool b ) { _pre_clearance=b; }
    bool pre_clearance ( bool b ) const { return _pre_clearance; }

    enum RefinementType { LocalClearanceRef, LocalClearanceMidRef, CorridorRef, JunctionRef };

    /*! This refinement method subdivides all non-trivially passages according to the refinement type.
        If parameter maxiter is -1, iterations will not be limited and the refinement will
        stop only when all refinements are completed (usually only few O(n) iterations are needed).
        Returns the number of refinements performed. */
    int refine ( RefinementType type, int maxiter=-1, int* niter=0 );

    /*! Will precompute maximum clearances per triangle instead of computing them during path search. */
    void compute_clearance ();

    /*! Get edges showing vertex-segment pairs requiring refinement */
    void get_refinements ( GsArray<GsPnt2>& ea, RefinementType type=LocalClearanceRef );

    /*! returns all junction triangles (the ones with three transparent (unconstrained) edges) */
    void get_junctions ( GsArray<SeDcdtFace*>& fa );

    /*! Returns true if disturbances were found for the given case: 0-2: departure, 3:arrival, 4:capsule */
    bool disturbances_found ( int i );

    /*! Returns the disturbance points for the given case: 0-2: departure, 3:arrival, 4:capsule */
    GsArray<GsPnt2>& disturbances ( int i );

    /*! Returns the entrance edge of each given case: 0-2: departure, 3:arrival */
    SeBase* entrance ( int i );

    void funnel_callback ( void (*cb)(void* udata), void* udata );
    void search_callback ( void (*cb)(void* udata), void* udata );
    void get_path_corners ( GsArray<GsVec2>& points, GsArray<char>& topdown );
    void get_funnel_corners ( GsArray<GsVec2>& points, GsArray<char>& topdown, FunnelDeque* funnel=0 );
    void get_search_nodes ( GsArray<SeBase*>& e );
    int  get_search_nodes () const;
    void get_search_metric ( GsArray<GsPnt2>& pnts );
    void get_search_front ( GsArray<SeBase*>& e );
    int  get_max_fronts () const { return _maxfronts; }
    void get_extcl ( int i, char& l, float& r ) const { l=_extcl[i].l; r=_extcl[i].r; }

   protected : // internal methods:
    void _clear_path ();
    // refinement methods:
    SeDcdtSymEdge* _needs_refinement ( SeDcdtSymEdge* s, RefinementType type, SeDcdtSymEdge*& disturb );
    // search methods:
    bool _canpass ( SeDcdtSymEdge* nen, SeDcdtSymEdge* nex, SeDcdtSymEdge* en, SeDcdtSymEdge* ex, const GsPnt2& p1, const GsPnt2& p2, const GsPnt2& p3, float r, float d2 );
    void _trytoadd ( SeDcdtSymEdge* en, SeDcdtSymEdge* ex, int mi, const GsPnt2& p1, const GsPnt2& p2, const GsPnt2& p3 );
    int  _expand_lowest_cost_leaf ();
    void _ptreeaddent ( SeDcdtSymEdge* s, bool top, bool edge );
    bool _search_channel ( double x1, double y1, double x2, double y2, float radius, const SeFace* iniface );
    // sector tests:
    bool _local_transition_free ( SeDcdtSymEdge* e, SeDcdtSymEdge* s, double d2, double px, double py );
    double _sector_clearance ( SeDcdtSymEdge* s, double maxr2, const GsVec2& c, const GsVec2& p1, const GsVec2& p2 );
    bool _sector_clear ( SeDcdtSymEdge* s, double r2, const GsVec2& c, const GsVec2& p1, const GsVec2& p2, const GsVec2* c2=0 );
    bool _entsector_clear ( const Sector& sc, SeBase* s, double x1, double y1, double x2, double y2, SeVertex* v );
    bool _entsector_clear ( SeBase* s, double r2, double cx, double cy, double ax, double ay, double bx, double by, SeVertex* v );
    // entrance tests:
    void _collect_entrance_disturbances ( int i, int fst, bool top, SeBase* s, SeVertex* v, double x1, double y1, double x2, double y2, const Capsule& c );
    bool _analyze_entrance_side ( SeBase* s, int i, bool main, bool top, double cx, double cy, double r );
    void _analyze_entrance ( SeBase* s, int i, double cx, double cy, double r );
    void _analyze_entrances ( SeBase* s, double cx, double cy, double r );
    // arrival test:
    void _analyze_arrival ( SeBase* s, int i, double r, float d2, SeDcdtSymEdge* ne, SeDcdtSymEdge* ns );
    // local path tests:
    void _collect_capsule_disturbances ( SeBase* s, double x1, double y1, double x2, double y2, const Capsule& c, const Sector& sec );
    PathResult _analyze_local_path ( SeBase* s, double r );
    // funnel methods:
    void _funneladd ( FunnelDeque* funnel, char side, FunnelPath* fpath, const GsPnt2& p, float radius );
    void _funnelstart ( FunnelDeque* funnel, const GsPnt2& apex, SeBase* ent, FunnelPath* fpath, float radius, float rextra=0 );
    void _funnelclose ( FunnelDeque* funnel, FunnelPath* fpath, int ei, float radius );
    void _fpathpush ( FunnelPath* fpath, const FunnelPt& fp3, float radius );
    void _fpathmake ( FunnelPath* fpath, GsPolygon& path, float radius, float dang );
    void _finitextcl ( float radius, float extclear );
    float _fgetextr ( int c, char m, float radius, float extclear );
 };

//================================== End of File =========================================

# endif // SE_LCT_H
