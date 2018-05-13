/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <gsim/se_triangulator_tools.h>
# include <gsim/se_mesh.h>
# include <gsim/se_dcdt.h>

//# define GS_USE_TRACE1 // 
# include <gsim/gs_trace.h>

void se_simplify ( const GsPolygon& orig, GsPolygon& res, double epsilon )
 {
   float ax, ay, bx, by;
   orig.get_bounding_box ( ax, ay, bx, by );

   SeDcdt tri;
   tri.init ( orig, epsilon );

   GsArray<int> pindices;
   tri.extract_contours ( res, pindices, bx, by ); 
 }

void se_inflate ( const GsPolygon& orig, GsPolygon& res, float radius, float dang, double epsilon )
 {
   res.inflate ( orig, radius, dang );

   float ax, ay, bx, by;
   orig.get_bounding_box ( ax, ay, bx, by );

   SeDcdt tri;
   tri.init ( res, epsilon );

   GsArray<int> pindices;
   tri.extract_contours ( res, pindices, bx, by ); 
 }

//============================ End of File =================================

