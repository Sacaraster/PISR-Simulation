/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef CMESH_H
# define CMESH_H

//******************************************************************
// Here is an example of the types required for defining a mesh
// containing 2d coordinates in the vertices, and an array of ids
// in edges in order to store the ids of constrained edges.
//******************************************************************

//******************************************************************
//  First, we define the types we will use
//******************************************************************

class CmVertex; // will be defined later containing my own data
class CmEdge;   // will be defined later containing constraints ids
class CmFace;   // this will be a default element

// Define CmSymEdge to have nice type casts defined:
typedef Se < CmVertex, CmEdge, CmFace > CmSymEdge;

// Define CmMesh also to have nice type casts defined:
typedef SeMesh < CmVertex, CmEdge, CmFace > CmMesh;

//******************************************************************
//  Second, we define the face as a default element
//******************************************************************

SE_DEFINE_DEFAULT_ELEMENT(CmFace,CmSymEdge);

//******************************************************************
//  Then we define the vertex class with the 2d coordinates
//******************************************************************

class CmVertex : public SeElement
 { public :
    float c[2]; // 2d coordinates
   public :
    SE_ELEMENT_CASTED_METHODS(CmVertex,CmSymEdge);
    CmVertex () { c[0]=c[1]=0; }
    CmVertex ( const CmVertex& v ) : SeElement() { c[0]=v.c[0]; c[1]=v.c[1]; }

    void set ( float x, float y ) { c[0]=x; c[1]=y; }

    friend GsOutput& operator<< ( GsOutput& out, const CmVertex& v )
           { return out << v.c[0] << gspc << v.c[1]; }

    friend GsInput& operator>> ( GsInput& inp, CmVertex& v )
           { return inp >> v.c[0] >> v.c[1]; }

    static inline int compare ( const CmVertex* v1, const CmVertex* v2 ) { return 0; } // not used
 };

//******************************************************************
//  And finally we define the edge class with the ids
//******************************************************************

class CmEdge : public SeElement
 { public :
    GsArray<int> constraints_ids; // ids of all constraints sharing this edge
   public :
    SE_ELEMENT_CASTED_METHODS(CmEdge,CmSymEdge);
    CmEdge () { }
    CmEdge ( const CmEdge& e ) : SeElement() { constraints_ids=e.constraints_ids; }

    bool is_constrained() { return constraints_ids.size()>0? true:false; }

    friend GsOutput& operator<< ( GsOutput& out, const CmEdge& e )
           { return out << e.constraints_ids; }

    friend GsInput& operator>> ( GsInput& inp, CmEdge& e )
           { return inp >> e.constraints_ids; }

    static inline int compare ( const CmEdge* e1, const CmEdge* e2 ) { return 0; } // not used
 };

# endif // CMESH_H
