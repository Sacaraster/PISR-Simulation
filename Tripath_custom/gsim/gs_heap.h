/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef GS_HEAP_H
# define GS_HEAP_H

/** \file gs_heap.h 
 * Template for a heap based on GsArray. */

# include <gsim/gs_array.h> 

/*! \class GsHeap gs_heap.h
    \brief Heap based on a GsArray 

    GsHeap implements a heap ordered binary tree based on a GsArray object.
    For maximum performance, GsHeap does not honor constructors or 
    destructors of its elements X, so user-managed pointers should be used
    in case of more complex classes, in the same design line as GsArray.
    The heap is ordered according to its associated cost. The remove() 
    method will always remove the minimum cost element (from the top).
    Class X is the element type, and Y is the cost type. Type Y will
    usually be int, float or double. */
template <typename X, typename Y>
class GsHeap
 { private :
    struct Elem { X e; Y c; };
    GsArray<Elem> _heap;
    
   public :

    /*! Default constructor. */
    GsHeap () {}

    /*! Copy constructor. */
    GsHeap ( const GsHeap& h ) : _heap(h._heap) {}

    /*! Set the capacity of the internal array */
    void capacity ( int c ) { _heap.capacity(c); }

    /*! Returns true if the heap is empty, false otherwise. */
    bool empty () const { return _heap.empty(); }

    /*! Returns the number of elements in the queue. */
    int size () const { return _heap.size(); }

    /*! Initializes as an empty heap */
    void init () { _heap.size(0); }

    /*! Compress the internal heap array */
    void compress () { _heap.compress(0); }

    /*! Make the heap have the given size, by removing the worst elements.
        Only applicable when s < size(). */
    void size ( int s )
     { if ( s<=0 ) { init(); return; }
       if ( s>=size() ) return;
       GsArray<Elem> tmp(s,s);
       while ( tmp.size()<s )
        { tmp.push() = _heap.top();
          remove();
        }
       init();
       while ( tmp.size()>0 )
        { insert ( tmp.top().e, tmp.top().c );
          tmp.pop();
        }
     }
    
    /*! Insert a new element with the given cost */
    void insert ( const X& elem, Y cost )
     { // insert at the end:
       _heap.push();
       _heap.top().e = elem;
       _heap.top().c = cost;
       // swim up: (parent of elem k is k/2)
       Elem tmp;
       int k=_heap.size();
       while ( k>1 && _heap[k/2-1].c>_heap[k-1].c )
        { GS_SWAP ( _heap[k/2-1], _heap[k-1] );
          k = k/2;
        }
     }

    /*! Removes the element in the top of the heap, which is always
        the element with lowest cost. */
    void remove ()
     { // put last element in top:
       _heap[0] = _heap.pop();
       // sink down: (children of node k are 2k and 2k+1)
       int j;
       int k=1;
       int n=_heap.size();
       Elem tmp;
       while ( 2*k<=n )
        { j=2*k;
          if ( j<n && _heap[j-1].c>_heap[j].c ) j++;
          if ( !(_heap[k-1].c>_heap[j-1].c) ) break;
          GS_SWAP ( _heap[k-1], _heap[j-1] );
          k=j;
        }
     }

    /*! Get a reference to the top element of the the heap,
        which is always the element with lowest cost. */
    const X& top () const { return _heap[0].e; }

    /*! Get the lowest cost in the heap,
        which is always the cost of the top element. */
    Y lowest_cost () const { return _heap[0].c; }

    /*! Returns elem i (0<=i<size) for inspection */
    const X& elem ( int i ) const { return _heap[i].e; }

    /*! Returns the cost of elem i (0<=i<size) for inspection */
    Y cost ( int i ) const { return _heap[i].c; }

    /*! Assumes elements X are pointers and deletes all
        stored elements, and set the heap to size 0 */
    void delete_all () { while (_heap.size()) delete _heap.pop().e; }

    /*! Assumes elements X are pointers, so it will first delete the
        top() element of the heap and then call remove() */
    void delremove () { delete _heap[0].e; remove(); }

    /*! Output all elements of the heap in an ordered fashion for debugging. */
    friend GsOutput& operator<< ( GsOutput& o, const GsHeap<X,Y>& ch )
     { GsHeap<X,Y> h(ch);
       o << '[';
       while ( h.size()>0 ) { o  << gspc << h.elem(0) << ':' << h.cost(0); h.remove(); }
       return o << ' ' << ']';
     }
 };

//============================== end of file ===============================

#endif // GS_HEAP_H

