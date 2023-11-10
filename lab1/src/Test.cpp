#include <gtest/gtest.h>
#include "buffer.h"

TEST(CircularBufferTest, CreatesWithInitializerList) {
    CircularBuffer buf = { '1', '2', '3' };
    EXPECT_EQ(buf[0], '1');
    EXPECT_EQ(buf[1], '2');
    EXPECT_EQ(buf[2], '3');
}

TEST(CircularBufferTest, ZeroCapacityConsturctor) {
    CircularBuffer buf;
    EXPECT_TRUE(buf.empty());
    buf.push_back('a');
    buf.push_front('a');
    EXPECT_TRUE(buf.empty());
}

TEST(CircularBufferTest, SpecifiedCapacityConstructor) {
    CircularBuffer buf(5);
    EXPECT_EQ(buf.capacity(), 5);
}

TEST(CircularBufferTest, SpecifiedCapacityAndItemFillConstructor) {
    CircularBuffer buf(5, 'a');
    EXPECT_EQ(buf.capacity(), 5);
    EXPECT_EQ(buf.size(), 5);
    EXPECT_TRUE(buf == (CircularBuffer{'a', 'a', 'a', 'a', 'a'}));
}

TEST(CircularBufferTest, SetCapacityTest) {
    CircularBuffer buf = { 1, 2, 3, 4 };
    buf.push_front(0);
    buf.set_capacity(2);

    EXPECT_EQ(buf.capacity(), 2);
    EXPECT_EQ(buf.size(), 2);
    EXPECT_TRUE(buf == (CircularBuffer{ 0, 1 }));

    buf.set_capacity(5);
    EXPECT_EQ(buf.capacity(), 5);
    EXPECT_EQ(buf.size(), 2);
    EXPECT_TRUE(buf == (CircularBuffer{ 0, 1 }));
}

TEST(CircularBufferTest, UncheckedElementAccess) {
    CircularBuffer buf = { 0, 1, 2, 3, 4 };

    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(buf[i], i);
    }
}

TEST(CircularBufferTest, UncheckedElementAccessConst) {
    const CircularBuffer buf = { 0, 1, 2, 3, 4 };

    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(buf[i], i);
    }
}

TEST(CircularBufferTest, CheckedElementAccess) {
    CircularBuffer buf = { 0, 1, 2, 3, 4 };

    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(buf.at(i), i);
    }

    EXPECT_THROW(buf.at(45), std::out_of_range);
}

TEST(CircularBufferTest, CheckedElementAccessConst) {
    const CircularBuffer buf = { 0, 1, 2, 3, 4 };

    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(buf.at(i), i);
    }

    EXPECT_THROW(buf.at(45), std::out_of_range);
}

TEST(CircularBufferTest, FrontBackTest) {
    CircularBuffer buf(5, 0);
    buf.push_back(9);
    EXPECT_EQ(buf.front(), 0);
    EXPECT_EQ(buf.back(), 9);
}

TEST(CircularBufferTest, FrontBackConstTest) {
    const CircularBuffer buf(5, '0');
    EXPECT_EQ(buf.front(), '0');
    EXPECT_EQ(buf.back(), '0');
}

TEST(CircularBufferTest, ResizeExpandTest) {
    CircularBuffer buf(3, '0');
    buf.resize(5, '1');
    EXPECT_EQ(buf.capacity(), 5);
    EXPECT_EQ(buf.size(), 5);
    EXPECT_TRUE(buf == (CircularBuffer{ '0', '0', '0', '1', '1' }));
}

TEST(CircularBufferTest, ResizeShrinkTest) {
    CircularBuffer buf = { 1, 2, 3, 4 };
    buf.resize(2);
    EXPECT_EQ(buf.capacity(), 4);
    EXPECT_EQ(buf.size(), 2);
    EXPECT_TRUE(buf == (CircularBuffer{ 1, 2 }));
}

TEST(CircularBufferTest, ResizeNonZeroStartTest) {
    CircularBuffer buf = { 1, 2, 3, 4 };
    buf.set_capacity(6);
    buf.pop_front();
    buf.pop_front();
    buf.resize(5, 5);
    EXPECT_EQ(buf.size(), 5);
    EXPECT_TRUE(buf == (CircularBuffer{ 3, 4, 5, 5, 5 }));
}

TEST(CircularBufferTest, PushFrontTest) {
    CircularBuffer buf = { '1', '2' };
    buf.set_capacity(4);
    buf.push_front('3');
    EXPECT_EQ(buf[0], '3');
    EXPECT_EQ(buf[1], '1');
    EXPECT_EQ(buf[2], '2');
    EXPECT_EQ(buf.size(), 3);
}

TEST(CircularBufferTest, PushFrontOverflowTest) {
    CircularBuffer buf(3, '0');
    buf.push_front('1');
    buf.push_front('2');
    EXPECT_EQ(buf[0], '2');
    EXPECT_EQ(buf[1], '1');
    EXPECT_EQ(buf[2], '0');
}

TEST(CircularBufferTest, PushBackTest) {
    CircularBuffer buf = { '1', '2' };
    buf.set_capacity(3);
    buf.push_back('3');
    EXPECT_EQ(buf[0], '1');
    EXPECT_EQ(buf[1], '2');
    EXPECT_EQ(buf[2], '3');
    EXPECT_EQ(buf.size(), 3);
}

TEST(CircularBufferTest, PushBackOverflowTest) {
    CircularBuffer buf(3, '0');
    buf.push_back('1');
    buf.push_back('2');
    EXPECT_EQ(buf[0], '0');
    EXPECT_EQ(buf[1], '1');
    EXPECT_EQ(buf[2], '2');
}

TEST(CircularBufferTest, PopBackTest) {
    CircularBuffer buf = { '1', '2', '3' };
    buf.pop_back();
    EXPECT_EQ(buf.size(), 2);
    buf.pop_back();
    EXPECT_EQ(buf.size(), 1);
    EXPECT_EQ(buf[0], '1');
}

TEST(CircularBufferTest, PopFrontTest) {
    CircularBuffer buf = { '1', '2', '3' };
    buf.pop_front();
    EXPECT_EQ(buf.size(), 2);
    buf.pop_front();
    EXPECT_EQ(buf.size(), 1);
    EXPECT_EQ(buf[0], '3');
}

TEST(CircularBufferTest, LinearizeTest) {
    CircularBuffer buf = { '1', '2', '3', '4' };
    buf.push_front('0');
    buf.push_front('0');
    EXPECT_FALSE(buf.is_linearized());
    buf.linearize();
    EXPECT_TRUE(buf.is_linearized());
    EXPECT_EQ(buf[0], '0');
    EXPECT_EQ(buf[1], '0');
    EXPECT_EQ(buf[2], '1');
    EXPECT_EQ(buf[3], '2');
}

TEST(CircularBufferTest, RotateTest) {
    CircularBuffer buf = { '1', '2', '3', '4' };
    buf.push_front('0');
    buf.push_front('0');
    buf.rotate(3);
    EXPECT_TRUE(buf == (CircularBuffer{ 2, 0, 0, 1 }));
}

TEST(CircularBufferTest, FullFuncTest) {
    CircularBuffer buf = { '1', '2', '3', '4' };
    EXPECT_TRUE(buf.full());
    buf.pop_back();
    EXPECT_FALSE(buf.full());
}

TEST(CircularBufferTest, InsertTest) {
    CircularBuffer buf = { 1, 2, 3, 4 };
    buf.push_front(0);
    buf.push_front(0);

    buf.insert(1, 9);
    EXPECT_EQ(buf.size(), 4);
    EXPECT_TRUE(buf == (CircularBuffer{ 0, 9, 1, 2 }));

    buf.pop_back();
    buf.pop_back();

    buf.insert(1, 2);
    EXPECT_EQ(buf.size(), 2);
    EXPECT_EQ(buf[0], 0);
    EXPECT_EQ(buf[1], 2);
}

TEST(CircularBufferTest, EraseTest) {
    CircularBuffer buf = { 1, 2, 3, 4 };
    buf.push_front(0);
    buf.push_front(0);
    buf.erase(1, 3);
    EXPECT_EQ(buf.size(), 2);

    EXPECT_TRUE(buf == (CircularBuffer{ 0, 2 }));
}

TEST(CircularBufferTest, ClearTest) {
    CircularBuffer buf = { '1', '2', '3', '4' };
    buf.clear();
    EXPECT_EQ(buf.size(), 0);
    EXPECT_EQ(buf.capacity(), 4);

    buf.push_front('0');
    EXPECT_EQ(buf[0], '0');
}

TEST(CircularBufferTest, ReserveTest) {
    CircularBuffer buf(3);
    EXPECT_EQ(buf.reserve(), 3);
    buf.push_front('0');
    EXPECT_EQ(buf.reserve(), 2);
    buf.push_front('0');
    EXPECT_EQ(buf.reserve(), 1);
    buf.push_front('0');
    EXPECT_EQ(buf.reserve(), 0);
}

TEST(CircularBufferTest, EqOpTest) {
    CircularBuffer buf1 = { '1', '2', '3', '4' };
    CircularBuffer buf2 = { '1', '2', '9', '4', '5' };
    CircularBuffer buf3 = { '1', '2', '3', '4' };
    EXPECT_FALSE(buf1 == buf2);
    EXPECT_TRUE(buf1 == buf3);
}

TEST(CircularBufferTest, SwapTest) {
    CircularBuffer buf1 = { '1', '2', '3' };
    CircularBuffer buf2 = { '1', '4', '9' };
    buf1.swap(buf2);
    EXPECT_EQ(buf1[0], '1');
    EXPECT_EQ(buf1[1], '4');
    EXPECT_EQ(buf1[2], '9');
    EXPECT_EQ(buf2[0], '1');
    EXPECT_EQ(buf2[1], '2');
    EXPECT_EQ(buf2[2], '3');
}
TEST(CircularBufferTest, AssignmentOpTest) {
    CircularBuffer buf1 = { '1', '2', '3', '4' };
    CircularBuffer buf2 = { '1', '2', '9', '4' };
    buf1 = buf2;
    EXPECT_TRUE(buf1 == buf2);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
