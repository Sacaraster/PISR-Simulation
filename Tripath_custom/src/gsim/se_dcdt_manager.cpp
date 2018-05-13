/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <gsim/se_dcdt_manager.h>

//# define GS_USE_TRACE1
# include <gsim/gs_trace.h>

//============================== SeDcdtManager ==================================

void SeDcdtManager::get_vertex_coordinates ( const SeVertex* v, double& x, double & y )
 {
   x = (double) ((SeDcdtVertex*)v)->p.x;
   y = (double) ((SeDcdtVertex*)v)->p.y;
 }

void SeDcdtManager::set_vertex_coordinates ( SeVertex* v, double x, double y )
 {
   ((SeDcdtVertex*)v)->p.x = (float) x;
   ((SeDcdtVertex*)v)->p.y = (float) y;
 }

void SeDcdtManager::new_steiner_vertex_created ( SeVertex* v )
 {
   ((SeDcdtVertex*)v)->refinement = true;
 }

bool SeDcdtManager::is_constrained ( SeEdge* e )
 {
   return ((SeDcdtEdge*)e)->is_constrained();
 }

void SeDcdtManager::set_unconstrained ( SeEdge* e )
 {
   ((SeDcdtEdge*)e)->set_unconstrained();
 }

void SeDcdtManager::get_constraints ( SeEdge* e, GsArray<int>& ids )
 {
   int i;
   const int size = ((SeDcdtEdge*)e)->ids.size();
   for ( i=0; i<size; i++ ) ids.push() = ((SeDcdtEdge*)e)->ids[i];
 }

void SeDcdtManager::add_constraints ( SeEdge* e, const GsArray<int>& ids )
 {
   ((SeDcdtEdge*)e)->add_constraints ( ids );
 }

void SeDcdtManager::copy_constraints ( const SeEdge* e1, SeEdge* e2 )
 {
   ((SeDcdtEdge*)e2)->ids = ((const SeDcdtEdge*)e1)->ids;
 }

//============================ End of File ===============================

