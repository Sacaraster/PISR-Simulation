/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <gsim/gs_tree.h>

//# define GS_USE_TRACE1   
# include <gsim/gs_trace.h>

# define BLACK(x) ((x)->color==GsTreeNode::Black)
# define RED(x)   ((x)->color==GsTreeNode::Red)
# define NIL      GsTreeNode::null

/*==========================================================================
  Note:
  A binary search tree is a red-black tree if it satisfies : 
  1. Every node is either red or black
  2. Every null leaf is black
  3. If a node is red then both its children are black
  4. Every simple path from a node to a descendant leaf contains the
     same number of black nodes
============================================================================*/

//=============================== GsTreeNode ====================================

static GsTreeNode static_null(GsTreeNode::Black);

GsTreeNode* GsTreeNode::null = &static_null;

GsTreeNode *GsTreeNode::min () const
 {
   GsTreeNode* x = (GsTreeNode*)this;
   if ( x==NIL ) return x;
   while ( x->left!=NIL ) x=x->left;
   return x;
 }

GsTreeNode *GsTreeNode::max () const
 {
   GsTreeNode* x = (GsTreeNode*)this;
   if ( x==NIL ) return x;
   while ( x->right!=NIL ) x=x->right;
   return x;
 }

GsTreeNode *GsTreeNode::next () const
 {
   GsTreeNode* x = (GsTreeNode*)this;
   if ( x->right!=NIL ) return x->right->min();

   GsTreeNode *y = x->parent;
   while ( y!=NIL && x==y->right )
    { x = y;
      y = y->parent;
    }

   return y;
 }

GsTreeNode *GsTreeNode::prior () const
 {
   GsTreeNode* x = (GsTreeNode*)this;
   if ( x->left!=NIL ) return x->left->max();

   GsTreeNode *y = x->parent;
   while ( y!=NIL && x==y->left )
    { x = y;
      y = y->parent;
    }

   return y;
 }

//=============================== GsTreeBase ====================================

//----------------------------- private methods ------------------------------

/*! Returns 0  and leaves _cur pointing to the found matching node if there is one.
    Returns >0 if the key is to be inserted under _cur->right.
    Returns <0 if the key is to be inserted under _cur->left.
    If the tree is empty, _cur will be null and the integer returned is 1. */
int GsTreeBase::_search_node ( const GsTreeNode *key )
 {
   int cmp;
   _cur = _root;

   if ( _cur==NIL ) return 1;

   while ( true )
    {  
      cmp = _man->compare(key,_cur);
      if ( cmp>0 ) 
       { if ( _cur->right!=NIL ) _cur=_cur->right; else return cmp;
       }
      else if ( cmp<0 )
       { if ( _cur->left!=NIL ) _cur=_cur->left; else return cmp;
       }
      else return cmp;
   }
 }

/*! Method for right rotation of the tree about a given node. */
void GsTreeBase::_rotate_right ( GsTreeNode *x )
 {
   GS_TRACE1("Rotate Right");

   GsTreeNode *y = x->left;

   x->left = y->right;
   if ( y->right!=NIL ) y->right->parent=x;
   y->parent = x->parent;

   if ( x->parent!=NIL )
    { if ( x==x->parent->right ) x->parent->right=y;
       else x->parent->left=y;
    }
   else _root = y;

   y->right = x;
   x->parent = y;
 }

/*! Method for left rotation of the tree about a given node. */
void GsTreeBase::_rotate_left ( GsTreeNode *x )
 {
   GS_TRACE1("Rotate Left");

   GsTreeNode *y = x->right;

   x->right = y->left;
   if ( y->left!=NIL ) y->left->parent=x;
   y->parent = x->parent;

   if ( x->parent!=NIL )
    { if ( x==x->parent->left ) x->parent->left=y;
       else x->parent->right=y;
    }
   else _root = y;

   y->left = x;
   x->parent = y;
 }

/*! Rebalance the tree after insertion of a node. */
void GsTreeBase::_rebalance ( GsTreeNode *x )
 {
   GS_TRACE1("Rebalance");

   GsTreeNode *y;

   while ( x!=_root && RED(x->parent) )
    { // if ( !x->parent->parent ) REPORT_ERROR
      if ( x->parent==x->parent->parent->left )
       { y = x->parent->parent->right;
         if ( RED(y) )
          { // handle case 1 (see CLR book, pp. 269)
            x->parent->color = GsTreeNode::Black;
            y->color = GsTreeNode::Black;
            x->parent->parent->color = GsTreeNode::Red;
            x = x->parent->parent;
          }
         else
          { if ( x==x->parent->right )
             { // transform case 2 into case 3 (see CLR book, pp. 269)
               x = x->parent;
               _rotate_left ( x );
             }
            // handle case 3 (see CLR book, pp. 269)
            x->parent->color = GsTreeNode::Black;
            x->parent->parent->color = GsTreeNode::Red;
            _rotate_right ( x->parent->parent );
          }
       }
      else
       { y = x->parent->parent->left;
         if ( RED(y) )
          { // handle case 1 (see CLR book, pp. 269)
            x->parent->color = GsTreeNode::Black;
            y->color = GsTreeNode::Black;
            x->parent->parent->color = GsTreeNode::Red;
            x = x->parent->parent;
          }
         else
          { if ( x==x->parent->left )
             { // transform case 2 into case 3 (see CLR book, pp. 269)
               x = x->parent;
               _rotate_right ( x );
             }
            // handle case 3 (see CLR book, pp. 269)
            x->parent->color = GsTreeNode::Black;
            x->parent->parent->color = GsTreeNode::Red;
            _rotate_left ( x->parent->parent );
          }
       }
    }
 }

/*! Method for restoring red-black properties after deletion. */
void GsTreeBase::_fix_remove ( GsTreeNode *x )
 {
   GS_TRACE1("Fix Remove");

   while ( x!=_root && BLACK(x) )
    {
      if ( x==x->parent->left )
       { GsTreeNode *w = x->parent->right;
         if ( RED(w) )
          { w->color = GsTreeNode::Black;
            x->parent->color = GsTreeNode::Red;
            _rotate_left ( x->parent );
            w = x->parent->right;
          }
         if ( BLACK(w->left) && BLACK(w->right) )
          { w->color = GsTreeNode::Red;
            x = x->parent;
          } 
         else
          { if ( BLACK(w->right) )
             { w->left->color = GsTreeNode::Black;
               w->color = GsTreeNode::Red;
               _rotate_right ( w );
               w = x->parent->right;
             }
            w->color = x->parent->color;
            x->parent->color = GsTreeNode::Black;
            w->right->color = GsTreeNode::Black;
            _rotate_left ( x->parent );
            x = _root;
          }
       }
      else
       { GsTreeNode *w = x->parent->left;
         if ( RED(w) )
          { w->color = GsTreeNode::Black;
            x->parent->color = GsTreeNode::Red;
            _rotate_right ( x->parent );
            w = x->parent->left;
          }
         if ( BLACK(w->left) && BLACK(w->right) )
          { w->color = GsTreeNode::Red;
            x = x->parent;
          }
         else
          { if ( BLACK(w->left) )
             { w->right->color = GsTreeNode::Black;
               w->color = GsTreeNode::Red;
               _rotate_left ( w );
               w = x->parent->left;
             }
            w->color = x->parent->color;
            x->parent->color = GsTreeNode::Black;
            w->left->color = GsTreeNode::Black;
            _rotate_right ( x->parent );
            x = _root;
          }
       }
    }

   x->color = GsTreeNode::Black;
 }

//----------------------------- constructors ------------------------------

GsTreeBase::GsTreeBase ( GsManagerBase* m )
 {
   _root = _cur = NIL;
   _elements = 0;
   _man = m;
   _man->ref();
 }

GsTreeBase::GsTreeBase ( const GsTreeBase& t )
 {
   _root = _cur = NIL;
   _elements = 0;
   _man = t._man;
   _man->ref();
   insert_tree ( t );
 }

GsTreeBase::~GsTreeBase ()
 {
   init ();
   _man->unref();
 }

void GsTreeBase::init ()
 {
   _cur = _root;
   GsTreeNode *curp;

   while ( _cur!=NIL )
    { // 1. descend _cur to a leaf
      while ( _cur->left!=NIL || _cur->right!=NIL ) // while cur is not a leaf 
        _cur = _cur->left!=NIL? _cur->left:_cur->right; 

      // 2. unlink _cur
      curp = _cur->parent;
      if ( _cur!=_root ) 
       { if ( curp->left==_cur ) curp->left=NIL;
          else curp->right=NIL;
       }
      
      // 3. delete and update _cur
      _man->free ( _cur );
      _cur = curp;
    }
    
   _elements = 0;
   _root = _cur = GsTreeNode::null;
 }

GsTreeNode *GsTreeBase::search ( const GsTreeNode *key )
 {
   if ( _root==NIL ) return 0;
   return _search_node(key)==0? _cur:0;
 }

GsTreeNode *GsTreeBase::insert ( GsTreeNode *key )
 {
   int cmp = _search_node ( key );

   if ( _cur!=NIL )
    { if ( cmp>0 ) // key>_cur
       { // if (_cur->right) REPORT_ERROR;
         _cur->right = key;
         key->parent = _cur;
         _rebalance ( key );
         _root->color = GsTreeNode::Black;
         _elements++;
         return key;
       }
      else if ( cmp<0 ) // key<_cur
       { // if (_cur->left) REPORT_ERROR
         _cur->left = key;
         key->parent = _cur;
         _rebalance ( key );
         _root->color = GsTreeNode::Black;
         _elements++;
         return key;
       }
      else return 0; // not inserted, already in the tree
    }
   else // tree empty
    { _root = key;
      _root->init ();
      _root->color = GsTreeNode::Black;
      _elements++;
      return key;
    }
 }

GsTreeNode *GsTreeBase::insert_or_del ( GsTreeNode *key )
 {
   if ( !insert(key) )
    { _man->free ( key );
      return 0;
    }
   return key;
 }

void GsTreeBase::insert_tree ( const GsTreeBase& t )
 { 
   if ( this==&t ) return;

   GsTreeIteratorBase it(t); 
   for ( it.first(); it.inrange(); it.next() )
    { insert_or_del ( (GsTreeNode*)_man->alloc(it.get()) );
    }
 }

GsTreeNode *GsTreeBase::extract ( GsTreeNode *z )
 {
   GsTreeNode *x, *y;

   y = ( z->left==NIL || z->right==NIL )? z : z->next();

   x = ( y->left!=NIL )? y->left : y->right;

   x->parent = y->parent; 
   
   if ( y->parent!=NIL )
    { if ( y==y->parent->left ) y->parent->left=x; 
       else y->parent->right=x;
    }
   else _root = x;

   GsTreeNode::Color ycolor = y->color;

   if ( y!=z ) // make y be z 
    { y->left=z->left; y->right=z->right; y->parent=z->parent; y->color=z->color;
      if ( z->left ) z->left->parent=y;
      if ( z->right ) z->right->parent=y;
      if ( z->parent )
       { if ( z->parent->left==z ) z->parent->left=y; 
          else z->parent->right=y;
       }
      if ( _root==z ) _root=y;
    }

   if ( ycolor==GsTreeNode::Black ) _fix_remove ( x );

   _elements--;
   _cur = z;
   z->init();
   return z;
 }

void GsTreeBase::remove ( GsTreeNode* z )
 {
   extract ( z );
   _man->free ( z );
 }

GsTreeNode* GsTreeBase::search_and_extract ( const GsTreeNode* key )
 {
   int cmp = _search_node ( key );
   if ( cmp!=0 ) return 0; // not found
   return extract ( _cur );
 }

bool GsTreeBase::search_and_remove ( const GsTreeNode* key )
 {
   int cmp = _search_node ( key );
   if ( cmp==0 ) { remove ( _cur ); return true; }
   return false;
 }

void GsTreeBase::adopt ( GsTreeBase& t )
 {
   _root     = t._root;      t._root      = 0;
   _cur      = t._cur;       t._cur       = 0;
   _elements = t._elements;  t._elements  = 0;
 }

void GsTreeBase::operator= ( const GsTreeBase& t )
 {
   init ();
   insert_tree ( t ); // need to write a copy routine instead of inserting all nodes...
 }

GsOutput& operator<< ( GsOutput& o, const GsTreeBase& t )
 { 
   o<<'['<<gspc;
   GsTreeIteratorBase it(t); 
   for ( it.first(); it.inrange(); it.next() )
    { t._man->output ( o, it.cur() );
      o << gspc;
    }
   return o<<']';
 }

//=========================== GsTreeIteratorBase ================================

GsTreeIteratorBase::GsTreeIteratorBase ( const GsTreeBase& t ) : _tree(t)
 { 
   reset ();
 }

void GsTreeIteratorBase::reset ()
 {
   _first = _cur = _tree.first();
   _last = _tree.last();
 }

//============================ End of File =================================
