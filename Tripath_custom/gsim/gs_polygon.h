/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef GS_POLYGON_H
# define GS_POLYGON_H

/** \file gs_polygon.h
 * A polygon described by an array of 2d points
 */

# include <gsim/gs_vec2.h>
# include <gsim/gs_array.h>

/*! \class GsPolygon gs_polygon.h
    \brief Array of 2d points

    GsPolygon derives GsArray<GsPnt2> and provides methods for operations with
    polygons. */
class GsPolygon : public GsArray<GsPnt2>
 { private :
    bool _open;

   public :
    /*! Constructor with a given size and capacity, whith default values of 0 */
    GsPolygon ( int s=0, int c=0 );

    /*! Copy constructor */
    GsPolygon ( const GsPolygon& p );

    /*! Constructor from a given buffer */
    GsPolygon ( GsPnt2* pt, int s, int c );

    /*! An open polygon is equivalent to a polygonal line */
    void open ( bool b ) { _open=b; }

    /*! Returns true if the polygon is open, and false otherwise */
    bool open () const { return _open; }
  
    /*! Sets the polygon from a float list. numv is the number of vertices, each
        vertex being represented by two floats, ie numv = 2*num floats. */
    void setpoly ( const float* pt, int numv );

    /*! Returns true if the polygon is simple by testing if it is degenerated
       or if non adjacent edges intersect */
    bool simple () const;

    /*! Returns true if the polygon is convex */
    bool convex () const;

    /*! Returns true if the polygon has orientation counter-clockwise */
    bool ccw () const { return area()>0? true:false; }

    /*! Returns the oriented area, it will be >0 if the polygon is ccw */
    float area () const;

    /*! Point in polygon test */
    bool contains ( const GsPnt2& p ) const;

    /*! Check if all points of pol are inside the polygon */
    bool contains ( const GsPolygon& pol ) const;

    /*! Sample a point inside the polygon. Possible modes are:
        'p': ensures the sample is inside the polygon,
        'b': only ensures the sample is inside the bounding box.
        The polygon has to be simple. Method contains is used in mode 'p' */
    GsPnt2 sample ( char mode='p' ) const;

    /*! Check if p is in the boundary of the polygon according to the precision ds.
        the function segment_contains_point() is used for each polygon edge. If p is not
        in the boundary, -1 is returned, othersise the returned index says which edge of
        the polygon contains the point. */
    int has_in_boundary ( const GsPnt2& p, float ds ) const;

    /*! Makes the polygon be a circle approximation with given center, radius, and 
        number of vertices. */
    void circle_approximation ( const GsPnt2& center, float radius, int nvertices );

    /*! Makes the polygon be a square with given center and radius. */
    void square ( const GsPnt2& center, float radius );

    /*! Calculates the total length along edges */
    float perimeter () const;

    /*! Parameter t must be between 0 and perimeter */
    GsPnt2 interpolate_along_edges ( float t ) const;

    /*! Resample the polygon by subdividing edges to ensure that each edge has at 
        most maxlen as length */
    void resample ( float maxlen );

    /*! Remove adjacent vertices which are duplicated, according to epsilon */
    void remove_duplicated_vertices ( float epsilon );

    /*! Remove adjacent vertices which are collinear, according to epsilon */
    void remove_collinear_vertices ( float epsilon );

    /*! Get centroid of polygon */
    GsPnt2 centroid () const;

    /*! Reverse the order of the elements */
    void reverse () { GsArray<GsPnt2>::revert(); }

    /*! Adds dv to each vertex of the polygon */
    void translate ( const GsVec2& dv );

    /*! Rotates the polygon around center about angle radians */
    void rotate ( const GsPnt2& center, float radians );

    /*! Scale vertices by given s factor */
    void scale ( float s );

    /*! Returns the lowest vertex of the polygon. If a non-null int pointer
        is passed, it will contain the index of the found south pole */
    GsPnt2 south_pole ( int* index=0 ) const;

    /*! Compute the convex hull and return it in hull, uses the gift-wrapping algorithm */
    void convex_hull ( GsPolygon& hull ) const;

    /*! Returns the index of the vertex which is closer to p, and within
        an epsilon distance of p, or -1 if there is not such a vertex */
    int pick_vertex ( const GsPnt2& p, float epsilon, bool first=false ) const;

    /*! Returns the index i of the edge (i,i+1) that is the closer edge
        to p, and within a distance of epsilon to p. The square of that
        distance is returned in dist2. -1 is returned if no edges are found */
    int pick_edge ( const GsPnt2& p, float epsilon, float& dist2 ) const;

    /*! Divides the polygon in triangles */
    void ear_triangulation ( GsArray<GsPnt2>& tris, bool knowntobeccw=false ) const;

    /*! Returns the min,max coords of the bounding square of the polygon.
        If this polygon is empty, (1,1),(0,0) is returned. */
    void get_bounding_box ( float& minx, float& miny, float& maxx, float& maxy ) const;

    /*! Same as the other get_bounding_box() method, but with GsVec2 arguments */
    void get_bounding_box ( GsVec2& min, GsVec2& max ) const
         { get_bounding_box(min.x,min.y,max.x,max.y); } 

    /*! Get the polygon configuration. x,y is the centroid, and a is the angle
        between the centroid and the vertex 0, in the range [0,2pi) */
    void get_configuration ( float& x, float& y, float& a ) const;

    /*! Puts the polygon in the configuration (x,y,a): x,y is the centroid, and
        a is the angle between the centroid and the vertex 0, in the range [0,2pi) */
    void set_configuration ( float x, float y, float a );

    /*! Tests if the polygon intersects with the given segment. */
    bool intersects ( const GsPnt2& p1, const GsPnt2& p2 ) const;

    /*! Test if the two polygons intersect */
    bool intersects ( const GsPolygon& p ) const;

    /*! Computes vertices aproximating a circle arc and push the vertices to the
        current polygon. The arc is computed by interpolating from vector v1 to v2,
        around center c. It is assumed that v2 is counter clockwise in respect to v1
        around c, and dang (radians) is the angle increment for the aproximation.
        The sign of dang may be set negative to invert the orientation of the arc.
        Sampled points are farther away from the center than the ideal circle radius,
        in order to ensure that the approximation is no smaller than the ideal circle; 
        If limv is given, it will limit the arc to not pass vector limv. */
    void arc ( const GsPnt2& c, const GsVec2& v1, const GsVec2& v2, float radius, float dang, const GsVec2* limv=0 );

    /*! Makes this polygon to delimit the region of distance radius to p;
        dang (radians) is the angle increment to aproximate curved sections.
        The given polygon must be in ccw orientation.
        Closed and open polygons are correctly handled.
        Note: self-intersections are not handled here, for precise inflation
        routines, check se_triangulator_tools.h. */
    void inflate ( const GsPolygon& p, float radius, float dang );

    /*! Comparison function that just compares the number of vertices,
        ie, it returns p1->size()-p2->size() */
    static int compare ( const GsPolygon* p1, const GsPolygon* p2 );

    /*! Outputs the open flag and calls GsArray::operator<< */
    friend GsOutput& operator<< ( GsOutput& out, const GsPolygon& p );

    /*! Check for the open flag and calls GsArray::operator>> */
    friend GsInput& operator>> ( GsInput& inp, GsPolygon& p );

    /*! Returns the minum distance between 2 polygons by a edge-edge distance between edges */
    friend float dist ( const GsPolygon& pa, const GsPolygon& pb );
 };

//================================ End of File =================================================

# endif // GS_POLYGON_H

