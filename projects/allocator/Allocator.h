// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2016
// Glenn P. Downing
// ------------------------------

/**
* @file Allocator.h
* This file contains the Allocator class
*/

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

/**
 * An allocator class capable of allocating memory
 * directly on the stack.
 * 
 * @tparam T The type of object for which this allocator
 * will allocate space.
 * 
 * @tparam N The size, in bytes, of the memory region
 * this allocator will manage.  This memory is allocated
 * on the stack when the Allocator is constructed.
 */
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

        /**
         * An equality comparator operator.
         *
         * @return true; all allocators are equal to one another by this operator.
         */
        friend bool operator == (const Allocator&, const Allocator&) {
            return true;}                                              // this is correct

        // -----------
        // operator !=
        // -----------

        /**
         * An inequality comparator operator.
         *
         * @return false; all allocators are equal to one another by this operator.
         */

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
         * This function ensures that the memory region
         * managed by this allocator is well-formed.
         * That is, each block is of the length denoted
         * by its sentinels, and no consecutive free
         * blocks exist.
         *
         * @return true if the region is well-formed.
         */
        bool valid () const {
            bool can_be_free = true;
            for(const char* i = a; i < &a[N-sizeof(int)];)
            {
                int diff = abs(*(int*)i) + sizeof(int);

                if (*(int*)i != *(int*)(i+diff))    //Does the sentinel have a matching
                                                    //sentinel at the appropriate address?
                    return false;

                diff += sizeof(int);
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

        /**
         * This function writes a sentinel into the memory region
         * managed by this allocator.
         *
         * @param dest The location in the memory region to which
         * the sentinel shall be written
         *
         * @param src The location of the integer which will be
         * written as a sentinel
         */
        void write_sentinel_to_arr(char* dest, int const * src)
        {
            char const * by_byte = (char const *)src;
            for(int i = 0; i < sizeof(int); i++)
            {
                dest[i] = by_byte[i];
            }
        }
         
        #ifdef ISTEST
        FRIEND_TEST(TestAllocator2, index);
        FRIEND_TEST(TestAllocator2, valid_1);
        FRIEND_TEST(TestAllocator2, valid_2);
        FRIEND_TEST(TestAllocator2, valid_3);
        FRIEND_TEST(TestAllocator2, valid_4);
        FRIEND_TEST(TestAllocator2, write_sentinel_to_arr_1);
        FRIEND_TEST(TestAllocator2, write_sentinel_to_arr_2);
        FRIEND_TEST(TestAllocator2, write_sentinel_to_arr_3);
        FRIEND_TEST(TestAllocator2, constructor_1);
        FRIEND_TEST(TestAllocator2, constructor_3);
        FRIEND_TEST(TestAllocator2, allocate_2);
        FRIEND_TEST(TestAllocator2, allocate_3);
        FRIEND_TEST(TestAllocator2, deallocate_1);
        FRIEND_TEST(TestAllocator2, deallocate_2);
        #endif
        /**
         * Allows random access to the memory region managed by this allocator.
         *
         * @param i The offset of the location
         * to be accessed from the beginning of the region
         *
         * @returns an integer representation of the contents of the requested
         * location
         */
        int& operator [] (int i) {
            return *reinterpret_cast<int*>(&a[i]);}

    public:
        // ------------
        // constructors
        // ------------

        /**
         * Constructs a new allocator
         *
         * @throw bad_alloc Thrown if the memory region
         * cannot fit, at minimum, one object of size T
         * and two sentinels.  That is, N must be at
         * least sizeof(T)+2*sizeof(int).
         */
        Allocator () {
            if(N < sizeof(T) + (2 * sizeof(int)))
            {
                throw bad_alloc();
            }

            int avail = N-2*sizeof(int);
            write_sentinel_to_arr(a, &avail);
            write_sentinel_to_arr(&a[N-sizeof(int)], &avail);

            assert(valid());}

        // Default copy, destructor, and copy assignment
        // Allocator  (const Allocator&);
        // ~Allocator ();
        // Allocator& operator = (const Allocator&);

        // --------
        // allocate
        // --------

        /**
         * Allocates space for n objects of type T.
         * 
         * @param n The number of objects of type T
         * which shall fit in the space allocated by
         * this call to allocate.
         *
         * @return a pointer to the beginning of the
         * region allocated by this call to allocate.
         *
         * @throw bad_alloc Thrown if n is less than 0
         * or if there is not enough room for the requested
         * allocation.
         */
        pointer allocate (size_type n) {
            n *= sizeof(T);
            if(n <= 0)      //if n was negative, throw a bad_alloc
            {
                throw bad_alloc();
            }
            for(char* i = a; i < a+N;)  //iterate over blocks
            {
                if(*(int*)i >= n && *(int*)i > 0)   //If we have a free block with enough space
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
                    }

                    assert(valid());

                    return (pointer)(i+sizeof(int));
                }
                i += 2*sizeof(int) + abs(*((int*)i));
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
         * This function deallocates a region
         * indicated by p
         * 
         * @param p The beginning of the region
         * to be deallocated
         *
         * @param n the size, in units of the size
         * of T, of the region to be deallocated.
         * n should be equal to the value
         * passed to the allocate() call which
         * originally returned p.
         */
        void deallocate (pointer p, size_type n) {
            char* pc = (char*) p;
            if(!pointer_valid(p) || *(int*)(pc-sizeof(int)) > 0)
            {
                throw invalid_argument("p");
            }
            int size = -1*(*(int*)(pc-sizeof(int)));
            *(int*)(pc-sizeof(int)) = size;
            pc += size;
            *(int*)pc = size;
            pc -= size + 2*sizeof(int);
            if(*(int*)pc > 0 && pc >= a)
            {
                int sum = *(int*)pc + size + 2*sizeof(int);
                pc -= sum-size-sizeof(int);
                *(int*)pc = sum;
                pc += sum+sizeof(int);
                *(int*)pc = sum;
                size = sum;
                pc -= size + 2*sizeof(int);
            }

            pc += size + 3*sizeof(int);
            if(*(int*)pc > 0 && pc < a+N)
            {
                int sum = *(int*)pc + size + 2*sizeof(int);
                pc += sum-size-sizeof(int);
                *(int*)pc = sum;
                pc -= sum+sizeof(int);
                *(int*)pc = sum;
                pc -= size + 2*sizeof(int);
            }            

            assert(valid());}

        /**
         * This function determines whether a pointer
         * points to the beginning of a block in the region
         * managed by this allocator.
         *
         * @param p The pointer whose validity shall be
         * determined.
         *
         * @return true if this pointer is valid; that is,
         * if it points to the beginning of a block in the
         * region of memory managed by this allocator.
         */ 
        bool pointer_valid(pointer p)
        {
            for(char* i = a; i < a+N;)
            {
                int size = abs(*(int*)i);
                i += sizeof(int);
                if(i == (char*)p)
                    return true;
                else if(i > (char*)p)
                {
                    return false;
                }
                i += size + sizeof(int);
            }
            return false;
        }

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
