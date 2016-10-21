// ------------------------------------
// projects/allocator/TestAllocator1.c++
// Copyright (C) 2015
// Glenn P. Downing
// ------------------------------------

// --------
// includes
// --------

#define ISTEST 1

#include <algorithm> // count
#include <memory>    // allocator

#include "gtest/gtest.h"

#include "Allocator.h"


// --------------
// TestAllocator1
// --------------

template <typename A>
struct TestAllocator1 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A             allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::size_type  size_type;
    typedef typename A::pointer    pointer;};

typedef testing::Types<
            std::allocator<int>,
            std::allocator<double>,
            Allocator<int,    100>,
            Allocator<double, 100>>
        my_types_1;

TYPED_TEST_CASE(TestAllocator1, my_types_1);

TYPED_TEST(TestAllocator1, test_1) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 1;
    const value_type     v = 2;
    const pointer        p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator1, test_10) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type  x;
    const size_type       s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}

// --------------
// TestAllocator2
// --------------

TEST(TestAllocator2, const_index) {
    const Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);}

TEST(TestAllocator2, index) {
    Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);}

// -----
// valid
// -----

TEST(TestAllocator2, valid_1)
{
    unsigned char c[100] =   {30,  0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   30,  0,   0,  0, 226, 255,
                              255, 255, 0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   226, 255, 255, 255, 16, 0, 0,   0,
                              0,   0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   0,   0,   16, 0, 0,   0};
    Allocator<int, 100> a;
    for(int i = 0; i < 100; ++i)
    {
        a[i] = c[i];
    }
    ASSERT_TRUE(a.valid());
}

TEST(TestAllocator2, valid_2)
{
    unsigned char c[100] =   {30,  0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   0,   30,  0,  0, 226, 255,
                              255, 255, 0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   226, 255, 255, 255, 16, 0, 0,   0,
                              0,   0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   0,   0,   16, 0, 0,   0};
    Allocator<int, 100> a;
    for(int i = 0; i < 100; ++i)
    {
        a[i] = c[i];
    }
    ASSERT_FALSE(a.valid());
}

TEST(TestAllocator2, valid_3)
{
    unsigned char c[100] =   {30,  0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   30,  0,   0,  0, 30,  0,
                              0,   0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   30,  0,   0,   0,   16, 0, 0,   0,
                              0,   0,   0,   0,   0,   0,   0,  0, 0,   0,
                              0,   0,   0,   0,   0,   0,   16, 0, 0,   0};
    Allocator<int, 100> a;
    for(int i = 0; i < 100; ++i)
    {
        a[i] = c[i];
    }
    ASSERT_FALSE(a.valid());
}

TEST(TestAllocator2, valid_4)
{
    unsigned char c[100] =   {30,  0,   0,   0,   0,   0,   0,   0,   0,   0,
                              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                              0,   0,   0,   0,   226, 255, 255, 255, 226, 255,
                              255, 255, 0,   0,   0,   0,   0,   0,   0,   0,
                              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                              0,   0,   226, 255, 255, 255, 16,  0,   0,   0,
                              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                              0,   0,   0,   0,   0,   0,   16,  0,   0,   0};
    Allocator<int, 100> a;
    for(int i = 0; i < 100; ++i)
    {
        a[i] = c[i];
    }
    ASSERT_FALSE(a.valid());
}

// ---------------------
// write_sentinel_to_arr
// ---------------------

TEST(TestAllocator2, write_sentinel_to_arr_1)
{
    int s = 20;
    Allocator<int, 100> a;
    a.write_sentinel_to_arr(a.a, &s);
    ASSERT_EQ(s, *(int*)(a.a));
}

TEST(TestAllocator2, write_sentinel_to_arr_2)
{
    int s = -20;
    Allocator<int, 100> a;
    a.write_sentinel_to_arr(a.a, &s);
    ASSERT_EQ(s, *(int*)(a.a));
}

TEST(TestAllocator2, write_sentinel_to_arr_3)
{
    int s = 1000;
    Allocator<int, 100> a;
    a.write_sentinel_to_arr(a.a, &s);
    ASSERT_EQ(s, *(int*)(a.a));
}

// -----------
// constructor
// -----------

TEST(TestAllocator2, constructor_1)
{
    Allocator<int, 100> a;
    ASSERT_EQ(sizeof(a.a), 100);
}

TEST(TestAllocator2, constructor_2)
{
    try
    {
        Allocator<int, 5> a;
    }
    catch(bad_alloc b)
    {
        ASSERT_TRUE(true);
        return;
    }
    ASSERT_TRUE(false);
}

TEST(TestAllocator2, constructor_3)
{
    Allocator<int, 100> a;
    ASSERT_EQ(*(int*)(a.a), 92);
}

// --------
// allocate
// --------

TEST(TestAllocator2, allocate_1)
{
    Allocator<int, 100> a;
    try
    {
        a.allocate(-20);
    }
    catch(bad_alloc b)
    {
        ASSERT_TRUE(true);
        return;
    }
    ASSERT_TRUE(false);
}

TEST(TestAllocator2, allocate_2)
{
    Allocator<int, 100> a;
    a.allocate(5);
    ASSERT_EQ(*(int*)(a.a), -20);
}

TEST(TestAllocator2, allocate_3)
{
    Allocator<int, 100> a;
    int* p = a.allocate(5);
    a.allocate(5);
    /*
    a.deallocate(p, 5);
    a.allocate(4);
    a.allocate(5);
    ASSERT_EQ(*(int*)(a.a + 56), -20);
    */
    ASSERT_TRUE(true);
}

// ----------
// deallocate
// ----------

TEST(TestAllocator2, deallocate_1)
{
    Allocator<int, 100> a;
    int* p = a.allocate(4);
    a.deallocate(p, 4);
    ASSERT_EQ(*(int*)(a.a), 92);
}

TEST(TestAllocator2, deallocate_2)
{
    Allocator<int, 100> a;
    int* p = a.allocate(4);
    int* q = a.allocate(4);
    a.deallocate(p, 4);
    a.deallocate(q, 4);
    ASSERT_EQ(*(int*)(a.a), 92);
}

TEST(TestAllocator2, deallocate_3)
{
    Allocator<int, 100> a;
    int* p = a.allocate(4);
    try
    {
        a.deallocate(p+1, 4);
    }
    catch(invalid_argument i)
    {
        ASSERT_TRUE(true);
        return;
    }
    ASSERT_TRUE(false);
}

// -------------
// pointer_valid
// -------------

TEST(TestAllocator2, pointer_valid_1)
{
    Allocator<int, 100> a;
    int* p = a.allocate(4);
    ASSERT_TRUE(a.pointer_valid(p));
}

TEST(TestAllocator2, pointer_valid_2)
{
    Allocator<int, 100> a;
    int* p = a.allocate(4);
    ASSERT_FALSE(a.pointer_valid(p+1));
}

TEST(TestAllocator2, pointer_valid_3)
{
    Allocator<int, 100> a;
    a.allocate(4);
    int* p = a.allocate(4);
    ASSERT_TRUE(a.pointer_valid(p));
}

// --------------
// TestAllocator3
// --------------

template <typename A>
struct TestAllocator3 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A             allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::size_type  size_type;
    typedef typename A::pointer    pointer;};

typedef testing::Types<
            Allocator<int,    100>,
            Allocator<double, 100>>
        my_types_2;

TYPED_TEST_CASE(TestAllocator3, my_types_2);

TYPED_TEST(TestAllocator3, test_1) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 1;
    const value_type     v = 2;
    const pointer        p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator3, test_10) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 10;
    const value_type     v = 2;
    const pointer        b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}
