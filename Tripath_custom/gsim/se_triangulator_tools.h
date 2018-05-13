/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef SE_TRIANGULATOR_TOOLS_H
# define SE_TRIANGULATOR_TOOLS_H

/** \file se_triangulator_tools.h 
 * Triangulator-based tools
 */

# include <gsim/gs_polygon.h>

/* Extracts the "outside countour" of a polygon so that the returned
   polygon is simple and ccw. */
void se_simplify ( const GsPolygon& orig, GsPolygon& res, double epsilon=0.00001 );

/* Inflation of a polygon: first the polygon is inflated with GsPolygon::inflate().
   Then, the result is triangulated and the boundary reconstructed, globally solving intersections.
   Input polygon is expected to be simple and ccw. */
void se_inflate ( const GsPolygon& orig, GsPolygon& res, float radius, float dang, double epsilon=0.00001 );

//============================ End of File =================================

# endif // SE_TRIANGULATOR_TOOLS_H

