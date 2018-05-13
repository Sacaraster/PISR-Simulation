/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef SE_ELEMENTS_H
# define SE_ELEMENTS_H

/** \file se_elements.h
 * Sym Edge elements definitions */

# include <gsim/gs_input.h>
# include <gsim/gs_output.h>

//================================ Types and Constants ===============================

class SeMeshBase;
class SeBase;
class SeElement;

typedef unsigned int semeshindex;    //!< internally used to mark elements (must be unsigned)
typedef SeElement SeVertex;  //!< a vertex is an element
typedef SeElement SeEdge;    //!< an edge is an element
typedef SeElement SeFace;    //!< a face is an element

//=================================== SeElement =======================================

/*! SeElement contains the required information to be attached
    to vertices, edges and faces:
    1. a reference to one (any) adjacent symedge (of type SeBase)
    2. pointers maintaining a circular list of the adjacent elements
       of the same type (vertices, edges or faces)
    3. an index that can be used by SeMeshBase to mark elements
    For attaching user-related information to an element:
    1. SeElement must be derived and all user data is declared
       inside the derived class.
    2. A corresponding GsManager must be derived and
       the required virtual methods must be re-written in order to
       manage the derived SeElement class. The compare method
       is not used. See also sr_class_manager.h. */
class SeElement
 { protected :
    SeElement () { _index=0; _symedge=0; _next=_prior=this; }
   public :
    SeBase*    se () const { return _symedge; }
    SeElement* nxt() const { return _next; }
    SeElement* pri() const { return _prior; }
   private :
    friend class SeMeshBase;
    friend class GsManagerBase;
    SeElement*  _next;
    SeElement*  _prior;
    SeBase*     _symedge;
    semeshindex _index;
    SeElement*  _remove ();
    SeElement*  _insert ( SeElement* n );
 };

/*! The following define can be called in a user derived class of SeElement
    to easily redefine public SeElement methods with correct type casts.
    E stands for the element type, and S for the sym edge type. */
# define SE_ELEMENT_CASTED_METHODS(E,S) \
     S* se() const { return (S*)SeElement::se(); } \
     E* nxt() const { return (E*)SeElement::nxt(); } \
     E* pri() const { return (E*)SeElement::pri(); } 

/*! The following define can be used to fully declare a default user derived
    class of SeElement, which contains no user data but correctly redefines
    public SeElement methods with type casts. This template requires the element
    type E (to be used as a vertex, face or edge data) and the user symedge type S */
# define SE_DEFINE_DEFAULT_ELEMENT(E,S) \
     class E : public SeElement \
      { public : \
        SE_ELEMENT_CASTED_METHODS(E,S); \
        E () : SeElement() {} \
        E ( const E& /*e*/ ) : SeElement() {} \
        friend GsOutput& operator<< ( GsOutput& o, const E& /*e*/ ) { return o; } \
        friend GsInput& operator>> ( GsInput& i, E& /*e*/ ) { return i; } \
        static inline int compare ( const E* /*e1*/, const E* /*e2*/ ) { return 0; } \
      };

//================================== SeBase ========================================

/*! Used to describe all adjacency relations of the mesh topology. The mesh itself is 
    composed of a net of SeBase elements linked together reflecting the vertex and 
    face loops. SymEdge is a short name for symetrical edge, as each SeBase has a
    symetrical one incident to the same edge on the opposite face, and is given by
    sym(). SeBase has local traverse operators permitting to change to any adjacent
    symedge so to access any information stored on a vertex, edge or face. Symedges 
    are also used as parameters to the topological operators of SeMeshBase, which allow
    modifying the mesh. */
class SeBase
 { public :
    /*! Returns the next symedge adjacent to the same face. */
    SeBase* nxt() const { return _next; }

    /*! Returns the result of applying two times the nxt() operator. */
    SeBase* nxn() const { return _next->_next; }

    /*! Returns the prior symedge adjacent to the same face. */
    SeBase* pri() const { return _rotate->_next->_rotate; }

    /*! Returns the next symedge adjacent to the same vertex. */
    SeBase* rot() const { return _rotate; }

    /*! Returns the prior symedge adjacent to the same vertex. */
    SeBase* ret() const { return _next->_rotate->_next; }

    /*! Returns the symmetrical symedge, sharing the same edge. */
    SeBase* sym() const { return _next->_rotate; }

    /*! Returns the element attached to the incident vertex. */
    SeVertex* vtx() const { return _vertex; }

    /*! Convenience operator that returns nxt()->vtx(). */
    SeVertex* nvtx() const { return _next->_vertex; }

    /*! Returns the element attached to the incident edge. */
    SeEdge* edg() const { return _edge; } 

    /*! Returns the element attached to the incident face. */
    SeFace* fac() const { return _face; } 

   private :  
    friend class SeMeshBase;
    friend class SeElement;
    SeBase* _next;
    SeBase* _rotate;
    SeVertex* _vertex;
    SeEdge* _edge;
    SeFace* _face;
 };

/*! This is the template version of the SeBase class. It redefines the methods
    of SeBase in order to perform type casts to the user defined element classes.
    All methods are implemented inline by just calling the corresponding method of
    the base class, but correctly applying the type casts to convert default types
    to user types.
    Important Note: no user data can be stored in a symedge. This template class
    is only used as a technique to correctly perform type casts. */
template <class V, class E, class F>
class Se : public SeBase
 { public :
    Se* nxt() const { return (Se*)SeBase::nxt(); }
    Se* nxn() const { return (Se*)SeBase::nxn(); }
    Se* pri() const { return (Se*)SeBase::pri(); }
    Se* rot() const { return (Se*)SeBase::rot(); }
    Se* ret() const { return (Se*)SeBase::ret(); }
    Se* sym() const { return (Se*)SeBase::sym(); }
    V* nvtx() const { return (V*)SeBase::nvtx(); }
    V* vtx() const { return (V*)SeBase::vtx(); }
    E* edg() const { return (E*)SeBase::edg(); }
    F* fac() const { return (F*)SeBase::fac(); }
 };

//============================ End of File ===============================

# endif // SE_ELEMENTS_H

