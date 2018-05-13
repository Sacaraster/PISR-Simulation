/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef GS_STRING_H
# define GS_STRING_H

/** \file gs_string.h 
 * Dynamic string */

# include <gsim/gs_array.h>

/*! \class GsString gs_string.h
    \brief Resizable dynamic string

    GsString uses GsArray to maintain a dynamic buffer containing a string.
    Operator (const char*) is defined and allows to type cast GsString to
    a c string pointer. This type cast will always return a valid pointer;
    in case GsString is empty a pointer to a "" string is returned.
    With this type cast operator defined, any function having
    a const char* parameter will accept GsString as parameter. */
class GsString
 { private:
    GsArray<char> _data; // size=strlen(_data) and capacity saves space for ending 0
    static const char *_empty; // _data will point to empty instead of having a 0 value

   public:
    /*! Default constructor creates an empty string */
    GsString ();

    /*! This constructor connects the string to the given output */
    GsString ( GsOutput& o );

    /*! Constructor from a character. Constructs a string with len()==len
        and capacity()==len+1, containing the given character inside all the
        string. If len<=0, this is the same as the default constructor. */
    GsString ( char c, int len=1 );

    /*! Constructor from a c string. Copies the given string with member
        function set(). */
    GsString ( const char *st );
 
    /*! Copy constructor. Copies the given string with member function set(). */
    GsString ( const GsString& st );

    /*! Obtains a string with length len and filled with c. */
    void set ( char c, int len );

    /*! Same as a copy operator */
    void set ( const GsString& st );

    /*! Set GsString to contain the given string. If the given string pointer
        is 0, GsString will become empty, having the internal buffer freed.
        Otherwise, the internal buffer is only reallocated when more space is
        needed. */
    void set ( const char *st );

    /*! Sets GsString to contain the given formatted string. Note that the
        formatted string is composed using a buffer of 256 bytes, so that
        the result must not exceed this size in any case. The C-library 
        function vsprintf(...) is used and then, set() is called. */
    void setf ( const char *fmt, ... );

    /*! Returns the length of the string */
    int len () const { return _data.size(); }

    /*! Ensures the string has sufficient size to hold a string of len l.
        The string length becomes l and a null character is placed at position l. */
    void len ( int l );

    /*! If the user sets a 0 value in the middle of the string and wants this to
        become the end of the string (as is in C), calclen() will find the 0 char
        and set the length of the GsString to the correct value */
    void calclen ();

    /*! Reserves enought capacity to hold a string with n characters. */
    void reserve ( int n ) { _data.reserve(n+1); }

    /*! Returns the internal buffer size used to store the string. */
    int capacity () const { return _data.capacity(); }

    /*! Frees any non used spaces in the internal buffer.
        After compressing, and if the string is not empty,
        the capacity will be equal to len()+1. */
    void compress ();

    /*! Reads a string from stdin (using fgets()) until the first newline
        character is read or until the number of characters read is equal 
        to max, whichever comes first. The newline character is not included. */
    void input ( int max=63 );

    /*! Appends st to the string. If more space is required, the string capacity
        is doubled in order to speed up consecutive string concatenations. */
    void append ( const char* st );

    /*! Inserts st in the middle of GsString, starting at position i. 
        Index i is not checked and must be correct. If !st or st=="", nothing is done.
        When reallocation is needed the string capacity is multiplied by 2. */
    void insert ( int i, const char* st );

    /*! Inserts n characters starting at position i; if parameter c is 0
        (the default) the new positions are not filed and will contain garbage;
        otherwise the new positions will be filled with character c. */
    void insert ( int i, int n, char c=0 );

    /*! Removes n characters of the string, starting at position i.
        If GsString is empty, or the indices are out of range, nothing is done.
        If the space to remove goes outside the string, the space is clipped. */
    void remove ( int i, int n );

    /*! Type cast operator to access the internal string buffer.
        The internal buffer is always a non-null pointer, but it can point
        to the empty string GsString::_empty. Implemented inline. */
    operator const char* () const { return _data.size()? &_data[0]:_empty; }

    /*! Ok, when type casts do not work, access pointer via this method */
    const char* pt () const { return _data.size()? &_data[0]:_empty; }

    /*! Accesses the character/element number i of the string.
        GsString is indexed as the standard c string, so that valid indices
        are in the range [0,len()-1]. There are no checkings if the index is
        inside the range. Implemented inline. */
    char& operator[] ( int i ) const { return _data[i]; }

    /*! Same as operator[], but returning a const reference to character i.
        Implemented inline. */
    const char& operator() ( int i ) const { return _data[i]; }

    /*! Same as operator[] */
    char& get ( int i ) const { return _data[i]; }

    /*! Same as operator() */
    const char& cget ( int i ) const { return _data[i]; }

    /*! Returns the last char in the string, or 0 if string is empty. */
    char lchar () const { return _data.ctop(); }

    /*! Replaces the last char in the string by c if the string is not empty */
    void lchar ( char c ) { if (c==0) _data.pop()=0; else _data.top()=c; }

    /*! Changes each character to lower case. */
    void lower ();

    /*! Changes each character to upper case. */
    void upper ();

    /*! Converts to an integer, same as the C library function */
    int atoi () const;

    /*! Converts to an integer, same as the C library function */
    long atol () const;

    /*! Converts to a float, same as the C library function. */
    float atof () const;

    /*! Converts to a double, using the atof method. */
    double atod () const { return (double)atof(); }

    /*! Take out leading and ending spaces of GsString. No reallocation is
        done, so that GsString keeps its capacity(), regardless of its len(). */
    void trim ();

    /*! Take out the leading spaces of GsString. No reallocation is done, so
        that GsString keeps its capacity(), regardless of its len(). */
    void ltrim ();

    /*! Take out ending spaces of GsString. No reallocation is used, so that
       GsString keeps its capacity(), regardless of its len(). */
    void rtrim ();

    /*! Puts in xi and xf the indices of the first and last non-white 
        characters. The first argument will contain the position of the first
        non-white char of the string, or -1 if the string is empty. The second
        argument will contain the position of the last non-white char, or -1 
        if the string is empty. If GsString contains only white-space 
        characters, we will have xi>xf, where xi==len() and xf==len()-1. The
        C function isspace() is used to determine if a character is a white-space
        char or not. */
    void bounds ( int &xi, int &xf ) const;

    /*! Makes GsString to becomes its substring enclosed in the coordinates
        [inf,sup]. If sup<0, sup is considerd to be len()-1, ie, the maximum
        valid coordinate. If sup is greater than this maximum, sup becomes 
        this maximum value. If inf<0, inf is considered to be 0. And if 
        inf>sup, GsString becomes empty; already taking into account all 
        checkings for the sup parameter. If GsString is empty, nothing is done.
        In any case, no reallocation is used, so that GsString keeps its 
        same original capacity(). */
    void substring ( int inf, int sup );

    /*! Makes GsString to become a substring of s; s is not changed.
        Same rules as in substring(int,int) apply here. */
    void substring ( const GsString& s, int inf, int sup ) { set(s); substring(inf,sup); }

    /*! Returns the index of the first (case-insensitive) occurrence of c, or -1
        if not found. The search starts at index i, which has default value of 0. */
    int search ( char c, int i=0 ) const;

    /*! Same as search(c,i) but using case-sensitive comparison. */
    int searchcs ( char c, int i=0 ) const;

    /*! Returns the index of the first occurrence of str, or -1 if not found.
        The search starts at index i, which has default value of 0.
        In case of empty strings or st larger than GsString, -1 is returned.
        Case-insensitve comparison is performed. */
    int search ( const char* st, int i=0 ) const;

    /*! Same as search(const char*) but using case-sensitive comparison. */
    int searchcs ( const char* st, int i=0 ) const;

    /*! Replaces n positions, starting at index i, by string newst.
        Returns the index of the first position after the insertion,
        ie, returns i+strlen(newst). If newst is null or empty ("")
        only a remove(i,n) is done. */
    int replace ( int i, int n, const char* newst );

    /*! Replaces the first occurence of oldst with newst, and returns
        the position where newst was inserted (oldst and newst can overlap).
        The search starts at index i, which has default value of 0.
        If oldst was not found, -1 is returned and nothing is done.
        The used comparison function is case-insensitive. */
    int replace ( const char* oldst, const char* newst, int i=0 );

    /*! Same as replace() but performing case-sensitive comparison. */
    int replacecs ( const char* oldst, const char* newst, int i=0 );

    /*! Replaces all ocurrences of oldst by newst.
        Returns the number of replacements done. */
    int replall ( const char* oldst, const char* newst );

    /*! Same as replall() but performing case-sensitive comparison. */
    int replallcs ( const char* oldst, const char* newst );

    /*! Makes GsString be exactly the same object as s, and then makes s an 
        empty string. */
    void adopt ( GsString &s ) { _data.adopt(s._data); }

    /*! Makes GsString be empty, and assigns the internal buffer to s. Returns
        the string s, that will never be 0, and that will never point to the
        internal GsString::_empty. The returned pointer will need to be freed
        with free() by the user when it is no longer needed. */
    char* abandon ( char*& s );

    /*! Appends st to the GsString by calling append(). */
    GsString& operator << ( const char* st ) { append(st); return *this; }

    GsString& operator << ( int i );    //!< converts and appends i (max 64 chars appended)
    GsString& operator << ( float f );  //!< converts and appends f (max 64 chars appended)
    GsString& operator << ( double d ); //!< converts and appends d (max 64 chars appended)
    GsString& operator << ( char c );   //!< converts and appends character c 

    /*! Write the string in the output making sure it can be read again as a string,
        by enclosing the string with double quotes if needed */
    GsOutput& safewrite ( GsOutput& o ) const { o.safewrite(*this); return o; }

    /*! Assignment operator by calling set(). */
    void operator = ( const GsString &s ) { set(s); }

    /*! Copies s into GsString by calling set(). */
    void operator = ( const char* s ) { set(s); }

    /*! Converts integer i to a string and put it in GsString */
    void operator = ( int i ) { len(0); *this<<i; }

    /*! Converts character c to a string and put it in GsString */
    void operator = ( char c ) { len(0); *this<<c; }

    /*! Converts float f to a string and put it in GsString */
    void operator = ( float f ) { len(0); *this<<f; }

    /*! Converts double d to a string and put it in GsString */
    void operator = ( double d ) { len(0); *this<<d; }

    /*! Compares strings using gs_compare(const char*,const char*), that does 
        a case-insensitive comparison. Returns 0 if they are equal, <0 if s1<s2,
        and >0 if s1>s2. */
    static inline int compare ( const GsString* s1, const GsString* s2 ) { return ::gs_compare(s1->pt(),s2->pt()); }
    static inline int compare ( const GsString& s1, const GsString& s2 ) { return ::gs_compare(s1.pt(),s2.pt()); }

    /*! Compares strings using gs_compare_cs(const char*,const char*), that does 
        a case-sensitive comparison. Returns 0 if they are equal, <0 if s1<s2,
        and >0 if s1>s2. */
    static inline int comparecs ( const GsString* s1, const GsString* s2 ) { return ::gs_comparecs(s1->pt(),s2->pt()); }
    static inline int comparecs ( const GsString& s1, const GsString& s2 ) { return ::gs_comparecs(s1.pt(),s2.pt()); }

    // Notice: Operators require at least one class type argument for operators.

    /*! Case-insensitive comparison operator that returns a boolean value. */
    friend inline bool operator == ( const GsString& s1, const char* s2 ) { return ::gs_compare(s1.pt(),s2)==0? true:false; }
    friend inline bool operator == ( const char* s1, const GsString& s2 ) { return ::gs_compare(s1,s2.pt())==0? true:false; }
    friend inline bool operator == ( const GsString& s1, const GsString& s2 ) { return ::gs_compare(s1.pt(),s2.pt())==0? true:false; }

    /*! Case-insensitive comparison operator that returns a boolean value. */
    friend inline bool operator != ( const GsString& s1, const char* s2 ) { return ::gs_compare(s1.pt(),s2)!=0? true:false; }
    friend inline bool operator != ( const char* s1, const GsString& s2 ) { return ::gs_compare(s1,s2.pt())!=0? true:false; }
    friend inline bool operator != ( const GsString& s1, const GsString& s2 ) { return ::gs_compare(s1.pt(),s2.pt())!=0? true:false; }

    /*! Case-insensitive comparison operator that returns a boolean value. */
    friend inline bool operator < ( const GsString& s1, const char* s2 ) { return ::gs_compare(s1.pt(),s2)<0? true:false; }
    friend inline bool operator < ( const char* s1, const GsString& s2 ) { return ::gs_compare(s1,s2.pt())<0? true:false; }
    friend inline bool operator < ( const GsString& s1, const GsString& s2 ) { return ::gs_compare(s1.pt(),s2.pt())<0? true:false; }

    /*! Case-insensitive comparison operator that returns a boolean value. */
    friend inline bool operator <= ( const GsString& s1, const char* s2 ) { return ::gs_compare(s1.pt(),s2)<=0? true:false; }
    friend inline bool operator <= ( const char* s1, const GsString& s2 ) { return ::gs_compare(s1,s2.pt())<=0? true:false; }
    friend inline bool operator <= ( const GsString& s1, const GsString& s2 ) { return ::gs_compare(s1.pt(),s2.pt())<=0? true:false; }

    /*! Case-insensitive comparison operator that returns a boolean value. */
    friend inline bool operator > ( const GsString& s1, const char* s2 ) { return ::gs_compare(s1.pt(),s2)>0? true:false; }
    friend inline bool operator > ( const char* s1, const GsString& s2 ) { return ::gs_compare(s1,s2.pt())>0? true:false; }
    friend inline bool operator > ( const GsString& s1, const GsString& s2 ) { return ::gs_compare(s1.pt(),s2.pt())>0? true:false; }

    /*! Case-insensitive comparison operator that returns a boolean value. */
    friend inline bool operator >= ( const GsString& s1, const char* s2 ) { return ::gs_compare(s1.pt(),s2)>=0? true:false; }
    friend inline bool operator >= ( const char* s1, const GsString& s2 ) { return ::gs_compare(s1,s2.pt())>=0? true:false; }
    friend inline bool operator >= ( const GsString& s1, const GsString& s2 ) { return ::gs_compare(s1.pt(),s2.pt())>=0? true:false; }
 };

//================== functions for filename/path manipulation =====================

/*! Removes the file name of a path. The cut point is determined by
    the last slash character (/ or \) found, which stays in the string.
    The cut point is returned, which can be -1 if no slash is found
    (if no slash character is found, nothing is done). */
int remove_filename ( GsString& path );

/*! Removes the path of a file name. The cut point (returned) is determined
    by the last slash character (/ or \) found. -1 is returned if
    no slash is found, in which case nothing is done. */
int remove_path ( GsString& fname );

/*! Search for an extension at the end of the string and remove it.
    The cut point is the last point character found
    (the point does not remain), and its position
    is returned (or -1 in case no extension is found). */
int remove_extension ( GsString& fname );

/*! Search for a file name at the end of the path and put it in fname.
    The cut point is determined by the last slash character found in the
    path and its position is returned. */
int get_filename ( const GsString& path, GsString& fname );

/*! Search for a path in the given file name and put it in path.
    The cut point is determined by the last slash character found in
    fname and its position is returned. */
int get_path ( const GsString& fname, GsString& path );

/*! Search for an extension at the end of fname and put it in ext.
    The cut point is determined by the last point character present in
    fname and its position is returned (or -1 in case of failure).
    The returned extension does not contain the point character. */
int get_extension ( const GsString& fname, GsString& ext );

/*! Search for an extension at the end of fname and returns a pointer
    to the first character of the extension, or null if no extension found */
const char* get_extension ( const GsString& fname );

/*! Same as get_filename(), but path is modified to not contain the
    file name anymore. The slash character stays in path (not in fname).
    If no filename is found -1 is returned and nothing is changed. */
int extract_filename ( GsString& path, GsString& fname );

/*! Search for an extension at the end of fname, put it in ext, and
    erase it from the fname. The cut point is the last point character
    found, its position is returned (or -1 in case of failure).
    The point character will be no more present in the string and neither
    in ext. -1 is returned if not found, and in this case nothing is
    changed. */
int extract_extension ( GsString& fname, GsString& ext );

/*! Checks if ".ext" exists at the end of fname (case insensitive).
    If ext is a null pointer, only checks if a '.' exists.
    Note: 'ext' must not contain the '.' character. */
bool has_extension ( const GsString& fname, const char* ext=0 );

/*! Case-sensitive version of has_extension. */
bool has_extensioncs ( const GsString& fname, const char* ext=0 );

/*! Ensures a slash exists at the end, and replaces all '\' with '/'.
    Returns true if a non empty path (>=2 chars) is found and false otherwise. */
bool validate_path ( GsString& path );

//============================== end of file ===============================

# endif // GS_STRING_H

