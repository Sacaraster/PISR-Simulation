/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef SE_TRIANGULATOR_MANAGER_H
# define SE_TRIANGULATOR_MANAGER_H

/** \file se_triangulator_manager.h 
 * Interfaces user data with the triangulator
 */

# include <gsim/se_mesh.h>

/*! SeTriangulatorManager contains virtual methods that the user needs to provide
    to the triangulator, allowing the triangulator to:
    1.access the coordinates of a vertex, which is a user-denided type.
    2.notify the user of some events: vertices, edges created, etc.
    3.access the constrained information of edges. This information should be
      stored as an array with the constraints ids sharing the constrained edge.
      However, if constraints are guaranteed to not overlap, the user can
      simply maintain a boolean variable. Or if no constraints are used, then
      the respective virtual methods do not need to be provided.
    Other utility methods are available in the class. */
class SeTriangulatorManager : public GsShared
 { public :

    /*! Allows retrieving the coordinates of a vertex. This method is pure 
        virtual, so it needs to be implemented in all cases. */
    virtual void get_vertex_coordinates ( const SeVertex* v, double& x, double& y )=0;

    /*! Allows setting the coordinates of a vertex. This method is pure 
        virtual, so it needs to be implemented in all cases. */
    virtual void set_vertex_coordinates ( SeVertex* v, double x, double y )=0;

    /*! This method is called by SeTriangulator::triangulate_face() to notify
        new edges created during the triangulation process, before the optimization
        phase, which is based on edge swap. The default implementation simply does nothing.*/
    virtual void triangulate_face_created_edge ( SeEdge* e );

    /*! This method is called by the triangulator to notify when vertices which
        are intersection of two constrained edges are inserted in the triangulation.
        The default implementation simply does nothing. */
    virtual void new_intersection_vertex_created ( SeVertex* v );

    /*! This method is called by the triangulator to notify when steiner vertices are
        inserted when recovering conforming line constraints. The default implementation
        simply does nothing. */
    virtual void new_steiner_vertex_created ( SeVertex* v );

    /*! This method is called to track vertices which are found when inserting
        edge constraints with insert_line_constraint(). Meaning that the found
        vertices v which are in the segment v1, v2 are passed here. The default
        implementation simply does nothing. */
    virtual void vertex_found_in_constrained_edge ( SeVertex* v );

    /*! Should return true if the passed edge is constrained. The default
        implementation always returns false. */
    virtual bool is_constrained ( SeEdge* e );

    /*! Requires that the edge becomes unconstrained. This is called during 
        manipulation of the triangulation, when edges need to be temporary
        unconstrained for consistent edge flipping. The default implementation
        does nothing. */
    virtual void set_unconstrained ( SeEdge* e );

    /*! Used to get the constraints shared by one edge. If the passed array of
        ids is returned as empty, it means that the edge is not constrained;
        and this is the case of the default implementation. */
    virtual void get_constraints ( SeEdge* e, GsArray<int>& ids );

    /*! This method is called when the edge needs to be set as constrained.
        The passed array contains the ids to be added to the edge. So that
        the user can keep track of many constraints sharing the same constrained
        edge. The passed array will never be empty. The default implementation 
        does nothing. */
    virtual void add_constraints ( SeEdge* e, const GsArray<int>& ids );

    /*! Copy constraints of e1 to e2 without changing e1. */
    virtual void copy_constraints ( const SeEdge* e1, SeEdge* e2 );

    /*! Returns the internal angles relative to each of the triangle vertices */
    void angles ( SeVertex* v1, SeVertex* v2, SeVertex* v3, float& a1, float& a2, float& a3 );

    /*! Retrieves the coordinates and tests gs_ccw()>0 */
    bool ccw ( SeVertex* v1, SeVertex* v2, SeVertex* v3 );

    /*! Retrieves the coordinates and tests gs_ccw()>0 */
    bool ccw ( double x1, double y1, SeVertex* v2, SeVertex* v3 );

    /*! Retrieves the coordinates and call gs_in_triangle() */
    bool in_triangle ( SeVertex* v1, SeVertex* v2, SeVertex* v3, SeVertex* v );

    /*! Retrieves the coordinates and call gs_in_triangle() */
    bool in_triangle ( SeVertex* v1, SeVertex* v2, SeVertex* v3, double x, double y );

    /*! Retrieves the coordinates and call gs_in_segment() */
    bool in_segment ( SeVertex* v1, SeVertex* v2, SeVertex* v, double eps );

    /*! Retrieves the coordinates and call gs_in_circle() */
    bool is_delaunay ( SeEdge* e );

    /*! Uses gs_ccw() and gs_in_circle() */
    bool is_flippable_and_not_delaunay ( SeEdge* e );

    /*! Recursive test of vertex proximity, called by test_boundary() */
    bool sector_vfree ( SeBase* s, double r2, double cx, double cy,
                        double x1, double y1, double x2, double y2, SeBase*& sv );

    /*! Check if (x,y) lies in the interior, edge, or vertex of (v1,v2,v3).
        Parameter s must be in the triangle and adjacent to v1.
        A SeTriangulator::LocateResult enum is returned, and s will be adjacent to
        the found element, if any. */
    int test_boundary ( SeVertex* v1, SeVertex* v2, SeVertex* v3,
                        double x, double y, double eps, double epsedg, SeBase*& s );

    /*! Retrieves the coordinates and call gs_segments_intersect() */
    bool segments_intersect ( SeVertex* v1, SeVertex* v2, 
                              SeVertex* v3, SeVertex* v4, double& x, double& y );

    /*! Retrieves the coordinates and call gs_segments_intersect() */
    bool segments_intersect ( SeVertex* v1, SeVertex* v2, SeVertex* v3, SeVertex* v4 );

    /*! Retrieves the coordinates and call gs_segments_intersect() */
    bool segments_intersect ( double x1, double y1, double x2, double y2,
                              SeVertex* v3, SeVertex* v4 );

    /*! Returns the midpoint of segment (v1,v2) */
    void segment_midpoint ( SeVertex* v1, SeVertex* v2, double& x, double& y );

    /*! Returns the square of the distance between vertices v1 and v2 */
    double distance2 ( SeVertex* v1, SeVertex* v2 );

    /*! Returns the square of the distance between vertices v1 and (x,y) */
    double distance2 ( SeVertex* v1, double x, double y );

    /*! Returns the square of the distance between point (x,y) and segment (v1,v2).
        Parameter t will contain the parametric location of the projection point returned in q. */
    double point_segment_dist2 ( double x, double y, SeVertex* v1, SeVertex* v2,
                                 double& t, double& qx, double& qy );
 };

//============================ End of File =================================

# endif // SE_TRIANGULATOR_MANAGER_H

