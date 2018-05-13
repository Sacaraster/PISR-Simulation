/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef SE_DCDT_ELEMENTS_H
# define SE_DCDT_ELEMENTS_H

/** \file se_dcdt_elements.h
 * Specialized DCDT mesh elements
 */

# include <gsim/gs_vec2.h>
# include <gsim/se_mesh.h>

//============================ DCDT Mesh definitions ==================================

class SeDcdtVertex; // contains 2d coordinates
class SeDcdtEdge;   // contains constraints ids
class SeDcdtFace;   // a default element

typedef Se < SeDcdtVertex, SeDcdtEdge, SeDcdtFace > SeDcdtSymEdge;
typedef SeMesh < SeDcdtVertex, SeDcdtEdge, SeDcdtFace > SeDcdtMesh;

class SeDcdtVertex : public SeElement
 { public :
    GsPnt2 p; // 2d coordinates
    gscbool refinement;
    gscbool border;
   public :
    SE_ELEMENT_CASTED_METHODS(SeDcdtVertex,SeDcdtSymEdge);
    SeDcdtVertex () : SeElement() { refinement=0; border=0; }
    SeDcdtVertex ( const SeDcdtVertex& v ) : SeElement(), p(v.p) { refinement=0; border=0; }
    SeDcdtVertex ( const GsPnt2& pnt ) : p(pnt) { refinement=0; border=0; }
    void set ( float x, float y ) { p.x=x; p.y=y; }
    void get_references ( GsArray<int>& ids ); // get all constr edges referencing this vertex
    friend GsOutput& operator<< ( GsOutput& out, const SeDcdtVertex& v );
    friend GsInput& operator>> ( GsInput& inp, SeDcdtVertex& v );
    static inline int compare ( const SeDcdtVertex* /*v1*/, const SeDcdtVertex* /*v2*/ ) { return 0; } // not used
 };

class SeDcdtEdge : public SeElement
 { public :
    GsArray<int> ids; // ids of all constraints sharing this edge
    int nodeid;       // internally used by the optimal search algorithm
    float ca, cb;     // precomputed clearances (stored values have the square of each clearance)
   public :
    SE_ELEMENT_CASTED_METHODS(SeDcdtEdge,SeDcdtSymEdge);
    SeDcdtEdge () : SeElement() { }
    SeDcdtEdge ( const SeDcdtEdge& e ) : SeElement() { ids=e.ids; }
    float cl ( SeDcdtSymEdge* s ) const { return s==se()? ca:cb; }
    bool border () const { return se()->vtx()->border || se()->nvtx()->border? true:false; }
    bool is_constrained() const { return ids.size()>0? true:false; }
    bool free () const { return ids.empty()? true:false; }
    void set_unconstrained () { ids.size(0); }
    bool has_id ( int id ) const;
    bool has_id ( const GsArray<int>& ida ) const;
    bool has_other_id_than ( int id ) const;
    bool remove_id ( int id );
    void add_constraints ( const GsArray<int>& idas );
    friend GsOutput& operator<< ( GsOutput& out, const SeDcdtEdge& e );
    friend GsInput& operator>> ( GsInput& inp, SeDcdtEdge& e );
    static inline int compare ( const SeDcdtEdge* /*e1*/, const SeDcdtEdge* /*e2*/ ) { return 0; } // not used
 };

class SeDcdtFace : public SeElement
 { public :
    SE_ELEMENT_CASTED_METHODS(SeDcdtFace,SeDcdtSymEdge);
    SeDcdtFace () : SeElement() {}
    SeDcdtFace ( const SeDcdtFace& /*f*/ ) : SeElement() {}
    bool junction () const;
    bool border () const;
    friend GsOutput& operator<< ( GsOutput& o, const SeDcdtFace& /*f*/ ) { return o; }
    friend GsInput& operator>> ( GsInput& i, SeDcdtFace& /*f*/ ) { return i; }
    static inline int compare ( const SeDcdtFace* /*f1*/, const SeDcdtFace* /*f2*/ ) { return 0; } // not used
 };

//================================== End of File =========================================

# endif // SE_DCDT_ELEMENTS_H
