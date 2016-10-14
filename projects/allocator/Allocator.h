// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2016
// Glenn P. Downing
// ------------------------------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert>   // assert
#include <cstddef>   // ptrdiff_t, size_t
#include <new>       // bad_alloc, new
#include <stdexcept> // invalid_argument
#include <iostream>  // cout
#include <iomanip>   // iomanip
#include <cstring>   // memcpy
#include <stdint.h>  // uint32_t
#include <cstdlib>   // abs

// ---------
// Allocator
// ---------

using namespace std;

template <typename T, size_t N>
class Allocator {
    public:
        // --------
        // typedefs
        // --------

        typedef T                 value_type;

        typedef size_t       size_type;
        typedef ptrdiff_t    difference_type;

        typedef       value_type*       pointer;
        typedef const value_type* const_pointer;

        typedef       value_type&       reference;
        typedef const value_type& const_reference;

    public:
        // -----------
        // operator ==
        // -----------

        friend bool operator == (const Allocator&, const Allocator&) {
            return true;}                                              // this is correct

        // -----------
        // operator !=
        // -----------

        friend bool operator != (const Allocator& lhs, const Allocator& rhs) {
            return !(lhs == rhs);}

    private:
        // ----
        // data
        // ----

        char a[N];

        // -----
        // valid
        // -----

        /**
         * O(1) in space
         * O(n) in time
         * <your documentation>
         */
        bool valid () const {
            bool can_be_free = true;
            for(const char* i = a; i < &a[N-sizeof(int)];)
            {
                int diff = abs(*(int*)i) + sizeof(int);
                cout << (long)i << "\n";
                cout << diff << "\n";
                cout << *(int*)i << "\n";
                cout << *(int*)(i+diff) << "\n";

                if (*(int*)i != *(int*)(i+diff))    //Does the sentinel have a matching
                                                    //sentinel at the appropriate address?
                    return false;

                diff += sizeof(int);
                cout << diff << "\n";
                if(*(int*)i > 0)                    //If the block is free:
                {
                    if(!can_be_free)                    //Is it ok for the block to be free?
                    {                                   //i.e. was the last block occupied?
                        return false;                   //Otherwise we have two consecutive free blocks
                    }
                    if(*(int*)i < sizeof(T))            //Is the block big enough to fit a T?
                    {                                   //If not we are wasting space
                        return false;
                    }
                    can_be_free = false;                //Tell the next block it is not allowed to be free
                }
                else                                //Otherwise:
                {                       
                    can_be_free = true;                 //Tell the next block it is allowed to be free.
                }
                i += diff;                          //Move on to the next block.
            }
            return true;}

        void write_data_to_arr(char* dest, T const * src)
        {
            char const * by_byte = (char const *)src;
            for(int i = 0; i < sizeof(T); i++)
            {
                dest[i] = by_byte[i];
            }
        }

        void write_sentinel_to_arr(char* dest, int const * src)
        {
            char const * by_byte = (char const *)src;
            for(int i = 0; i < sizeof(int); i++)
            {
                dest[i] = by_byte[i];
            }
        }

        /**
         * O(1) in space
         * O(1) in time
         * https://code.google.com/p/googletest/wiki/AdvancedGuide#Private_Class_Members
         */
         
        #ifdef ISTEST
        FRIEND_TEST(TestAllocator2, index);
        #endif
        int& operator [] (int i) {
            return *reinterpret_cast<int*>(&a[i]);}

    public:
        // ------------
        // constructors
        // ------------

        /**
         * O(1) in space
         * O(1) in time
         * throw a bad_alloc exception, if N is less than sizeof(T) + (2 * sizeof(int))
         */
        Allocator () {
            if(N < sizeof(T) + (2 * sizeof(int)))
            {
                throw bad_alloc();
            }

            int avail = N-2*sizeof(int);
            write_sentinel_to_arr(a, &avail);
            write_sentinel_to_arr(&a[N-sizeof(int)], &avail);

            for(char* j = a; j < a+N; j++)
            {
                cout << setw(4) << (int)*(unsigned char*)j << " ";
            }
            cout << "\n\n";

            assert(valid());}

        // Default copy, destructor, and copy assignment
        // Allocator  (const Allocator&);
        // ~Allocator ();
        // Allocator& operator = (const Allocator&);

        // --------
        // allocate
        // --------

        /**
         * O(1) in space
         * O(n) in time
         * after allocation there must be enough space left for a valid block
         * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
         * choose the first block that fits
         * throw a bad_alloc exception, if n is invalid
         */
        pointer allocate (size_type n) {
            n *= sizeof(T);
            if(n <= 0)      //if n was negative, throw a bad_alloc
            {
                throw bad_alloc();
            }
            for(char* i = a; i < a+N;)  //iterate over blocks
            {
                if((*((int*)i) >= n))   //If we have a free block with enough space
                {
                    if(*((int*)i)-(n+2*sizeof(int)) >= sizeof(T) + 2*sizeof(int))
                    {
                        int old = *(int*)i;
                        *(int*)i = -1*n;
                        *(int*)(i+sizeof(int)+n) = -1*n;
                        *(int*)(i+2*sizeof(int)+n) = old-n-2*sizeof(int);
                        *(int*)(i+old+sizeof(int)) = old-n-2*sizeof(int);
                    }
                    else
                    {
                        int old = *(int*)i;
                        *(int*)i = -1*old;
                        *(int*)(i+sizeof(int)+old) = -1*old;
                        cout << "SUSPECTED ERROR: " << sizeof(int)+(*(int*)i) << "\n";
                    }

                    for(char* j = a; j < a+N; j++)
                    {
                        cout << setw(4) << (int)*(unsigned char*)j << " ";
                    }
                    cout << "\n\n" << (long)(i+sizeof(int)) << ": " << (int)*(i+sizeof(int)) << "\n\n";

                    assert(valid());

                    return (pointer)(i+sizeof(int));
                }
                i += 2*sizeof(int) + *((int*)i);
            }
            throw bad_alloc();
        }

        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         */
        void construct (pointer p, const_reference v) {
            new (p) T(v);                               // this is correct and exempt
            assert(valid());}                           // from the prohibition of new

        // ----------
        // deallocate
        // ----------

        /**
         * O(1) in space
         * O(1) in time
         * after deallocation adjacent free blocks must be coalesced
         * throw an invalid_argument exception, if p is invalid
         * <your documentation>
         */
        void deallocate (pointer p, size_type) {
            // <your code>
            assert(valid());}

        // -------
        // destroy
        // -------

        /**
         * O(1) in space
         * O(1) in time
         */
        void destroy (pointer p) {
            p->~T();               // this is correct
            assert(valid());}

        /**
         * O(1) in space
         * O(1) in time
         */
        const int& operator [] (int i) const {
            return *reinterpret_cast<const int*>(&a[i]);}};

#endif // Allocator_h
