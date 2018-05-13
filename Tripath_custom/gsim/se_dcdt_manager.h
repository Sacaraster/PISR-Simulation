/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef SE_DCDT_MANAGER_H
# define SE_DCDT_MANAGER_H

/** \file se_dcdt_manager.h
 * Specilized DCDT manager
 */

# include <gsim/se_triangulator_manager.h>
# include <gsim/se_dcdt_elements.h>

//================================ SeDcdtManager =====================================

/*! Provides the implementation of the required base class virtual methods so
    that SeDcdt can correctly deal with the SeDcdt elements. */
class SeDcdtManager: public SeTriangulatorManager
 { public :
    virtual void get_vertex_coordinates ( const SeVertex* v, double& x, double & y );
    virtual void set_vertex_coordinates ( SeVertex* v, double x, double y );
    virtual void new_steiner_vertex_created ( SeVertex* v );
    virtual bool is_constrained ( SeEdge* e );
    virtual void set_unconstrained ( SeEdge* e );
    virtual void get_constraints ( SeEdge* e, GsArray<int>& ids );
    virtual void add_constraints ( SeEdge* e, const GsArray<int>& ids );
    virtual void copy_constraints ( const SeEdge* e1, SeEdge* e2 );
 };

//================================ End of File ========================================

# endif // SE_DCDT_MANAGER_H
