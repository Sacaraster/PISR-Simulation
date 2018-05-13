/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

//=== internal header file ===//

# include <gsim/gs_array.h>
# include <gsim/gs_heap.h>
# include <gsim/gs_deque.h>
# include <gsim/se_triangulator.h>

//================================================================================
//================================== tools =======================================
//================================================================================

# define PNL printf("\n")

# define PRINTP(x,y)  printf("(%+9.7f,%+9.7f) ", x, y)

# define PRINTV(v)  { double vx, vy; \
                      _man->get_vertex_coordinates ( v, vx, vy ); \
                      PRINTP(vx,vy); }

inline bool pt1circfree ( SeTriangulator* t, SeBase* s, double x, double y, double radius )
 { if ( radius>0 ) 
    { if ( !t->disc_free(s,x,y,radius) ) return false; }
   return true;
 }

inline bool pt2circfree ( SeTriangulator* t, SeBase* s, double x, double y, double radius )
 { if ( radius>0 )
    { if ( !t->disc_free(s,x,y,radius) ) return false; }
   return true;
 }

//================================================================================
//=============================== PathNode =======================================
//================================================================================

class SeTriangulator::PathNode // a node can be seen as one edge
 { public :
    int parent;    // index of the parent node, -1 if root
    SeBase* en;    // the entrance edge of the node
    SeBase* ex;    // the exit edge of the node
    float ncost;   // accumulated cost until touching s->edg()
    float hcost;   // heuristic cost from s->edg() to the goal
    float x, y;    // point at s->edg() used for the heuristic cost

   public :
    void set ( int pnode, SeBase* s, SeBase* t, float nc, float hc, float a, float b )
         { parent=pnode; en=s; ex=t; ncost=nc; hcost=hc; x=a; y=b; }
 };

typedef SeTriangulator::PathNode SePathNode;

//================================================================================
//=============================== PathTree =======================================
//================================================================================
class SeTriangulator::PathTree
 { public :
    GsArray<SePathNode> nodes;
    GsHeap<int,float> leafs;
    float radius, radius2, diam2;

   public :
    PathTree ()
     { nodes.capacity(64); leafs.capacity(64); }

    void setradius ( float r ) 
     { radius=r; radius2=r*r; diam2=4*radius2; }

    void init ( float r ) 
     { nodes.size(0); leafs.init(); setradius(r); }

    void add_child ( int pleaf, SeBase* e, SeBase* s, float ncost, float hcost, double x, double y )
     { nodes.push().set ( pleaf, e, s, ncost, hcost, (float)x, (float)y );
       int ni = nodes.size()-1;
       leafs.insert ( ni, cost(ni) );
     }

    float cost ( int i )
     { return nodes[i].ncost + nodes[i].hcost; }

    int lowest_cost_leaf ()
     { if ( leafs.empty() ) return -1;
       int i = leafs.top();
       leafs.remove();
       return i;
     }

    void print ()
     { int i;
       gsout << "NParents: ";
       for ( i=0; i<nodes.size(); i++ ) gsout<<nodes[i].parent<<gspc;
       gsout << "\nLeafs: ";
       for ( i=0; i<leafs.size(); i++ ) gsout<<leafs.elem(i)<<gspc;
       gsout<<gsnl;
     }
 };

//================================================================================
//================================ FunnelPt ======================================
//================================================================================

class SeTriangulator::FunnelPt : public GsVec2
 { public :
    char side;  // side is t:top, b:bottom, p:init/final vertex
    char apex;  // apex is 0 or 1
    float dist; // dist to closest object: for extra clearance computation
   public :
    FunnelPt () {}
    FunnelPt ( float a, float b, char s, char ap, float d=0 ) : GsVec2(a,b) { side=s; apex=ap; dist=d; }
    void set ( float a, float b ) { x=a; y=b; apex=0; } // used only for radius 0 paths
    void set ( float a, float b, char s ) { x=a; y=b; apex=0; side=s; dist=0; }
    void set ( float a, float b, char s, char ap, float d=0 ) { x=a; y=b; side=s; apex=ap; dist=d; }
    bool Pnt () const { return side=='p'? true:false; }
    bool Bot () const { return side=='b'? true:false; }
    bool Top () const { return side=='t'? true:false; }
    char optop () const { return side=='t'? 'b':'t'; }
    char opside () const { return side=='p'? side:optop(); }
    //void operator = ( const FunnelPt& p ) { x=p.x; y=p.y; side=p.side; apex=p.apex; dist=p.dist; }
    friend GsOutput& operator<< ( GsOutput& out, const FunnelPt& fp )
      { if (fp.apex==1) out<<'*'; return out<<fp.side<<':'<<fp.x<<','<<fp.y; }
 };

typedef SeTriangulator::FunnelPt SeFunnelPt;

//================================================================================
//============================== FunnelPath ======================================
//================================================================================

class SeTriangulator::FunnelPath : public GsArray<SeFunnelPt>
 { public :
    bool autolen;
    float len;
   public :
    FunnelPath ( const FunnelPath& fp ) : GsArray<SeFunnelPt>(fp) { autolen=fp.autolen; len=fp.len; }
    FunnelPath ( bool al ) : autolen(al) { len=0; }
    bool sameas ( const FunnelPath& fp, int i1, int i2 )
     { for ( int i=i1; i<=i2; i++ ) { if ( get(i)!=fp[i] ) return false; }
       return true;
     }
 };

typedef SeTriangulator::FunnelPath SeFunnelPath;

//================================================================================
//============================== FunnelDeque =====================================
//================================================================================

class SeTriangulator::FunnelDeque : public GsDeque<SeFunnelPt>
 { public :
    GsPnt2 apex;
    FunnelDeque ( int cap=32 ) : GsDeque<SeFunnelPt>(cap) {}
    FunnelDeque ( const FunnelDeque& f ) : GsDeque<SeFunnelPt>(f), apex(f.apex) {}

   public :

    void init () { GsDeque<SeFunnelPt>::init(); }

    FunnelPt& push () { return GsDeque<SeFunnelPt>::push(); }

    void push ( float x, float y, char side, char ap, float dist )
     { GsDeque<SeFunnelPt>::push().set ( x, y, side, ap, dist );
       if ( ap ) apex = get();
     }

    void newapex ()
     { get().apex=1;
       apex = get();
     }

    void print ()
     { for ( int i=0; i<size(); i++ ) gsout<<(get(i).Top()?"T":get(i).Bot()?"B":"P")<<gspc;
       gsout<<gsnl;
     }
 };

typedef SeTriangulator::FunnelDeque SeFunnelDeque;

//============================ End of File =================================

