#include <algorithm>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <array>
#include <random>
#include "../include/stack.hpp"
#include "../include/custom_memory_resource.hpp"

// Вспомогательные структуры

struct Heavy {
    int id;
    std::string name;
    double weight;

    bool operator==(Heavy const& o) const {
        return id == o.id && name == o.name && weight == o.weight;
    }
};

struct Heavy2 {
    int a;
    double b;
    std::string c;

    bool operator==(Heavy2 const& o) const {
        return a == o.a && b == o.b && c == o.c;
    }
};

struct MoveOnly {
    int x;

    MoveOnly(int v) : x(v) {}
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;

    MoveOnly(MoveOnly&& other) noexcept : x(other.x) {
        other.x = -999;
    }
    MoveOnly& operator=(MoveOnly&& other) noexcept {
        x = other.x;
        other.x = -999;
        return *this;
    }
};

// БАЗОВЫЕ ТЕСТЫ 

TEST(StackBasic, PushPopSimple) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    st.push(1);
    st.push(2);

    EXPECT_EQ(st.top(), 2);
    st.pop();
    EXPECT_EQ(st.top(), 1);
}

TEST(StackBasic, EmptyPop) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    EXPECT_TRUE(st.empty());
    st.pop();
    EXPECT_TRUE(st.empty());
}

TEST(StackBasic, Clear) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    st.push(5);
    st.push(6);
    st.clear();
    EXPECT_TRUE(st.empty());
}

// ИТЕРАТОРЫ 

TEST(StackIter, IterateSimple) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    st.push(1);
    st.push(2);
    st.push(3);

    std::vector<int> out;
    for (int x : st) out.push_back(x);

    EXPECT_EQ(out, (std::vector<int>{3,2,1}));
}

TEST(StackIter, IterateEmpty) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    int cnt = 0;
    for (int x : st) cnt++;
    EXPECT_EQ(cnt, 0);
}

TEST(StackIter, ConstIter) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    st.push(7);
    st.push(9);

    const Stack<int>& cst = st;
    int sum = 0;
    for (int x : cst) sum += x;
    EXPECT_EQ(sum, 16);
}

TEST(StackIter, PostfixIter) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    st.push(10);
    st.push(20);

    auto it = st.begin();
    auto old = it++;

    EXPECT_EQ(*old, 20);
    EXPECT_EQ(*it, 10);
}

TEST(StackIter, MultipleIndependentIterators) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    st.push(1);
    st.push(2);
    st.push(3);

    auto it1 = st.begin();
    auto it2 = st.begin();

    ++it1;
    EXPECT_NE(*it1, *it2);
}

// СЛОЖНЫЕ ТИПЫ

TEST(StackComplex, HeavyStruct) {
    VectorTrackingMemoryResource mem;
    Stack<Heavy> st(&mem);

    st.push({1,"A",3.14});
    st.push({2,"B",2.71});

    EXPECT_EQ(st.top(), (Heavy{2,"B",2.71}));
    st.pop();
    EXPECT_EQ(st.top(), (Heavy{1,"A",3.14}));
}

TEST(StackComplex, Heavy2Struct) {
    VectorTrackingMemoryResource mem;
    Stack<Heavy2> st(&mem);

    st.push({10,5.5,"hi"});
    st.push({20,7.7,"yo"});

    EXPECT_EQ(st.top(), (Heavy2{20,7.7,"yo"}));
}

TEST(StackComplex, MoveOnlyTypeWorks) {
    VectorTrackingMemoryResource mem;
    Stack<MoveOnly> st(&mem);

    st.push(MoveOnly(5));
    st.push(MoveOnly(10));

    EXPECT_EQ(st.top().x, 10);
    st.pop();
    EXPECT_EQ(st.top().x, 5);
}

TEST(StackComplex, PointerType) {
    VectorTrackingMemoryResource mem;
    Stack<const char*> st(&mem);

    st.push("hello");
    st.push("world");

    EXPECT_STREQ(st.top(), "world");
}

TEST(StackComplex, HugeArrayType) {
    VectorTrackingMemoryResource mem;
    Stack<std::array<int,100>> st(&mem);

    std::array<int,100> a{};
    a[10] = 42;
    st.push(a);

    EXPECT_EQ(st.top()[10], 42);
}

// МНОГО ОПЕРАЦИЙ PUSH/POP

TEST(StackOps, MixedOps) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    st.push(10);
    st.push(20);
    st.pop();
    st.push(30);

    EXPECT_EQ(st.top(), 30);
}

TEST(StackOps, PopAfterEveryPush) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    for (int i = 0; i < 200; i++) {
        st.push(i);
        EXPECT_EQ(st.top(), i);
        st.pop();
        EXPECT_TRUE(st.empty());
    }
}

TEST(StackOps, ManyPushPop) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    for (int i = 0; i < 5000; i++) st.push(i);
    for (int i = 4999; i >= 0; i--) {
        EXPECT_EQ(st.top(), i);
        st.pop();
    }

    EXPECT_TRUE(st.empty());
}

// СРАВНЕНИЕ СО std::vector 

TEST(StackCompare, CompareWithVector) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);
    std::vector<int> v;

    for (int i = 0; i < 1000; i++) {
        st.push(i);
        v.push_back(i);
    }

    std::vector<int> out;
    for (int x : st) out.push_back(x);

    std::reverse(v.begin(), v.end());
    EXPECT_EQ(out, v);
}

// RANDOM TESTS 

TEST(StackRandom, RandomOperations) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    std::vector<int> mirror;
    std::mt19937 rng(123);

    for (int i = 0; i < 5000; i++) {
        if (mirror.empty() || (rng() % 2 == 0)) {
            int val = rng() % 10000;
            st.push(val);
            mirror.push_back(val);
        } else {
            st.pop();
            mirror.pop_back();
        }
    }

    std::vector<int> out;
    for (int x : st) out.push_back(x);

    std::reverse(mirror.begin(), mirror.end());
    EXPECT_EQ(out, mirror);
}

// STRESS 

TEST(StackStress, HugeStress100k) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    for (int i = 0; i < 100000; i++) st.push(i);
    EXPECT_EQ(st.top(), 99999);
}

TEST(StackStress, MemoryNoLeaksAfterScope) {
    VectorTrackingMemoryResource mem;

    size_t before = mem.allocated_count();
    {
        Stack<int> st(&mem);
        for (int i = 0; i < 20000; i++) st.push(i);
    }
    EXPECT_EQ(mem.allocated_count(), before);
}

// ITERATOR EDGE CASES 

TEST(StackIteratorEdge, IteratorAfterPop) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    st.push(1);
    st.push(2);
    st.push(3);

    auto it = st.begin();
    st.pop();
    EXPECT_NE(it, st.begin());
}

TEST(StackIteratorEdge, IteratorMultiplePop) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    st.push(5);
    st.push(6);
    st.push(7);

    auto it = st.begin();
    st.pop();
    st.pop();

    EXPECT_EQ(st.top(), 5);
}

// PMR TESTS

TEST(StackPMR, MultipleStacksOneAllocator) {
    VectorTrackingMemoryResource mem;

    {
        Stack<int> a(&mem);
        Stack<int> b(&mem);

        a.push(1);
        b.push(2);
    }
    EXPECT_EQ(mem.allocated_count(), 0);
}

TEST(StackPMR, PMRWorksWithComplex) {
    VectorTrackingMemoryResource mem;
    Stack<std::string> st(&mem);

    st.push("hello");
    st.push("world");

    EXPECT_EQ(st.top(), "world");
}

// CONST TESTS

TEST(StackConst, ConstBeginEnd) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    st.push(1);
    st.push(3);

    const Stack<int>& cst = st;

    auto it = cst.begin();
    EXPECT_EQ(*it, 3);
}

TEST(StackConst, ConstForRange) {
    VectorTrackingMemoryResource mem;
    Stack<int> st(&mem);

    st.push(10);
    st.push(20);

    const Stack<int>& cst = st;

    int sum = 0;
    for (int x : cst) sum += x;

    EXPECT_EQ(sum, 30);
}
