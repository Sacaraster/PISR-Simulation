/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

/** \file gs_tree.h 
 * A red-black balanced search tree */

# ifndef GS_TREE_H
# define GS_TREE_H

# include <gsim/gs_manager.h>
# include <gsim/gs_output.h>

/*! \class GsTreeNode gs_tree.h
    \brief A red-black node for GsTree

    GsTreeNode is the node that classes should derive in order to be
    inserted in GsTree. This class has all its data members as public, 
    so that the user can use them as needed. There are no methods for 
    manipulation of the nodes inside this class, all such manipulation 
    methods are in GsTreeBase. */
class GsTreeNode
 { public :
    /*! The color type of the node. */
    enum Color { Red, Black };

    /*! GsTreeNode::null is initialized in gs_tree_node.cpp, and serves
        as a sentinel indicating a null node. GsTreeNode::null is a black
        node where its pointers are initialized as pointing to itself, but
        the parent pointer may keep arbitrary values during manipulation
        inside GsTreeBase. */
    static GsTreeNode* null;
   public :
    Color color;        //!> Color of the node.
    GsTreeNode* parent; //!> Pointer to node's parent.
    GsTreeNode* left;   //!> Pointer to node's left child.
    GsTreeNode* right;  //!> Pointer to node's righ child.
   public :
    /*! Default Constructor. */
    GsTreeNode ( Color c=Red ) : color(c), parent(null), left(null), right(null) {}
   public :
    /*! Sets all links to GsTreeNode::null, but leaves color unchanged */
    void init () { parent=left=right=null; }

    /*! Returns the minimum node of the subtree rooted at this node */
    GsTreeNode* min () const;

    /*! Returns the maximum node of the subtree rooted at this node */
    GsTreeNode* max () const;

    /*! Returns the successor node of this node in the tree */
    GsTreeNode* next () const;

    /*! Returns the predecessor node of this node in the tree */
    GsTreeNode* prior () const;
 };

/*! \class GsTreeBase gs_tree.h
    \brief red-black tree base class

    This class contains all methods for tree manipulation. The user should
    however use the template class GsTree for an implementation that
    includes automatic type casts for the user type. A manager to the
    user data is required. User data must be a class deriving GsTreeNode.
    For details about red black trees, see:
    T. H. Cormen, C. E. Leiserson, and R. L. Rivest, "Introduction to Algorithms"
    1990, MIT, chapter 14, ISBN 0-262-03141-8
    <br><br>
    Note:
    A binary search tree is a red-black tree if it satisfies : <br>
    1. Every node is either red or black <br>
    2. Every null leaf is black <br>
    3. If a node is red then both its children are black <br>
    4. Every simple path from a node to a descendant leaf contains the same number of black nodes <br> */
class GsTreeBase
 { private :
    GsTreeNode *_root;        // the root of the tree
    GsTreeNode *_cur;         // the current element of the tree
    GsManagerBase* _man; // manager of user data, that derives GsListNode
    int _elements;            // number of elements of the tree
    int  _search_node ( const GsTreeNode* key );
    void _rotate_right ( GsTreeNode* x );
    void _rotate_left ( GsTreeNode* x );
    void _rebalance ( GsTreeNode* x );
    void _fix_remove ( GsTreeNode* x );

   public :

    /*! Initiates an empty tree. The class manager must manage a user class
        deriving from GsTreeNode. */
    GsTreeBase ( GsManagerBase* m );

    /*! Copy constructor. The class manager of t is shared. */
    GsTreeBase ( const GsTreeBase& t );

    /*! Destructor. */
   ~GsTreeBase ();

    /*! Deletes all elements of the tree. */
    void init ();

    /*! Returns true iff there is no nodes in the tree. */
    bool empty () const { return _root==GsTreeNode::null? true:false; } 
  
    /*! Returns the number of elements of the tree. */
    int elements () const { return _elements; }

    /*! Returns the current element being pointed, that will be GsTreeNode::null
        if the tree is emptyis returned. */
    GsTreeNode* cur () const { return _cur; }

    /*! Sets the current element to be c, which must be a node of the tree. */
    void cur ( GsTreeNode* c ) { _cur = c; }

    /*! Returns the root of the tree, that will be GsTreeNode::null if the tree is empty. */
    GsTreeNode* root () { return _root; }

    /*! Returns the first element of the tree, ie, the minimum according
        to the comparison function. */
    GsTreeNode* first () const { return _root->min(); }

    /*! Returns the last element of the tree, ie, the maximum. */
    GsTreeNode* last () const { return _root->max(); }

    /*! Will put the current position cur() pointing to the node with minimum value.
        If the list is empty, cur will point to GsTreeNode::null. */
    void gofirst () { _cur = _root->min(); }

    /*! Will put the current position cur() pointing to the node with maximum value.
        If the list is empty, cur will point to GsTreeNode::null. */
    void golast () { _cur = _root->max(); }

    /*! Will put the current position cur() pointing to the next
        node curnext(), cur can become GsTreeNode::null. */
    void gonext () { _cur = _cur->next(); }

    /*! Will put the current position cur() pointing to the prior
        link curprior(), cur can become GsTreeNode::null. */
    void goprior () { _cur = _cur->prior(); }

    /*! Returns a pointer to the item that is equal to the given key, or 0
        if it could not find the key in the tree. cur will be the last node
        visited during the search. */
    GsTreeNode* search ( const GsTreeNode *key );

    /*! If inserted, will return key, otherwise will return 0, and cur will 
        point to key. Duplication is not allowed. */
    GsTreeNode* insert ( GsTreeNode* key );

    /*! Tries to insert key using method insert(). In case of duplication
        key is not inserted, key is deleted, and 0 is returned. The returned
        node will be pointing to key in case of sucees, or pointing to the
        node in the tree that is equal to key in case of failure. In all cases,
        cur will be the last node visited during the search for key. */
    GsTreeNode* insert_or_del ( GsTreeNode* key );

    /*! Duplicates and inserts all elements of t in the tree. */
    void insert_tree ( const GsTreeBase& t );

    /*! Extracts node z that must be inside the tree; z is returned. */
    GsTreeNode* extract ( GsTreeNode* z );

    /*! Removes and delete node z, which must be inside the tree. */
    void remove ( GsTreeNode* z );

    /*! Extracts the item equal to the key and return it or 0 if the 
        key was not found. */
    GsTreeNode* search_and_extract ( const GsTreeNode* key );

    /*! Removes and deletes the item equal to the key. Returns true if a node
        is found and deleted, otherwise false is returned. */
    bool search_and_remove ( const GsTreeNode* key );

    /*! Adopt the data in tree t, and set t to an empty tree. Both
        trees must manage the same derived class of GsTreeNode. */
    void adopt ( GsTreeBase& t );

    /*! Copy operator */
    void operator= ( const GsTreeBase& t );

    /*! Outputs the tree in the format: [e1 e2 en] */
    friend GsOutput& operator<< ( GsOutput& o, const GsTreeBase& t );
 };

/*! \class GsTree gs_tree.h
    \brief red-black balanced tree

    GsTree defines automatic type casts to the user type, which must
    derive GsTreeNode. To traverse the tree, the first, last, next,
    and prior keywords are related to the order defined by the comparison
    method in the class manager. For documentation of the methods
    see the documentation of the base class GsTreeBase methods. */
template <class X>
class GsTree : public GsTreeBase
 { public:

    /*! Default constructor that automatically creates a GsManager<X>. */
    GsTree () : GsTreeBase ( new GsManager<X> ) {}

    /*! Constructor with a given class manager. */
    GsTree ( GsManagerBase* m ) : GsTreeBase ( m ) {}

    /*! Copy constructor with class manager sharing. */
    GsTree ( const GsTree& t ) : GsTreeBase ( t ) {}

    X* cur () { return (X*)GsTreeBase::cur(); }
    void cur ( X* c ) { GsTreeBase::cur((GsTreeNode*)c); }
    X* root () { return (X*)GsTreeBase::root(); }
    X* first () { return (X*) GsTreeBase::first(); }
    X* last () { return (X*) GsTreeBase::last(); }
    X* curnext () const { return (X*)GsTreeBase::cur()->next(); }
    X* curprior () const { return (X*)GsTreeBase::cur()->prior(); }
    X* search ( const X* key ) { return (X*)GsTreeBase::search(key); }
    X* insert ( X* key ) { return (X*)GsTreeBase::insert(key); } 
    X* insert_or_del ( X* key ) { return (X*)GsTreeBase::insert_or_del(key); }
    X* extract ( X* n ) { return (X*) GsTreeBase::extract(n); }
    void remove ( X* n ) { GsTreeBase::remove(n); }
    X* search_and_extract ( const X* key ) { return (X*) GsTreeBase::search_and_extract(key); }
    bool search_and_remove ( const X* key ) { return GsTreeBase::search_and_remove(key); }
 };

/*! Base class for iterating over trees. */
class GsTreeIteratorBase
 { private :
    GsTreeNode* _cur;
    GsTreeNode* _first;
    GsTreeNode* _last;
    const GsTreeBase& _tree;

   public :
    /*! Constructor */
    GsTreeIteratorBase ( const GsTreeBase& t );

    /*! Returns the current node being pointed by the iterator */
    GsTreeNode* cur () const { return _cur; }

    /*! Returns the first node in the associated tree */
    GsTreeNode* getfirst () const { return _first; }

    /*! Returns the last node in the associated tree */
    GsTreeNode* getlast () const { return _last; }

    /*! Must be called each time the associate tree is changed */
    void reset ();

    /*! Points the iterator to the first element. */
    void first () { _cur=_first; }

    /*! Points the iterator to the last element. */
    void last () { _cur=_last; }

    /*! Advances the current position of the iterator to the next one */
    void next () { _cur=_cur->next(); }

    /*! Walk back the current position of the iterator of one position */
    void prior () { _cur=_cur->prior(); }

    /*! Returns true if get() points to a valid position */
    bool inrange () { return _cur==GsTreeNode::null? false:true; }

    /*! Returns the current element, can return GsTreeNode::null */
    GsTreeNode* get () { return _cur; }

    /*! Returns true if the current position is pointing to the last element. */
    bool inlast () const { return _cur==_last? true:false; }

    /*! Returns true if the current position is pointing to the first element */
    bool infirst () const { return _cur==_first? true:false; }
 };

/*! Derives GsTreeIteratorBase providing correct type casts for the user type */
template <class X>
class GsTreeIterator : public GsTreeIteratorBase
 { public :
    GsTreeIterator ( const GsTree<X>& s ) : GsTreeIteratorBase(s) {}
    X* get () { return (X*)GsTreeIteratorBase::get(); }
    X* operator-> () { return (X*)GsTreeIteratorBase::get(); }
 };



//============================ End of File =================================

# endif // GS_TREE_H
