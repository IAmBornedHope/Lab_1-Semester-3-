#include <gtest/gtest.h>
#include "AVLTree.hpp"

TEST(BalanceTests, LeftHeavyRotation) {
    AVLTree<int> tree;
    tree.insert(30);
    tree.insert(20);
    tree.insert(10);

    EXPECT_TRUE(tree.contains(30));
    EXPECT_TRUE(tree.contains(20));
    EXPECT_TRUE(tree.contains(10));
    EXPECT_EQ(tree.size(), 3u);
}

TEST(BalanceTests, RightHeavyRotation) {
    AVLTree<int> tree;
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);

    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(20));
    EXPECT_TRUE(tree.contains(30));
    EXPECT_EQ(tree.size(), 3u);
}

TEST(BalanceTests, LeftRightRotation) {
    AVLTree<int> tree;
    tree.insert(30);
    tree.insert(10);
    tree.insert(20);

    EXPECT_TRUE(tree.contains(30));
    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(20));
    EXPECT_EQ(tree.size(), 3u);
}

TEST(BalanceTests, RightLeftRotation) {
    AVLTree<int> tree;
    tree.insert(10);
    tree.insert(30);
    tree.insert(20);

    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(30));
    EXPECT_TRUE(tree.contains(20));
    EXPECT_EQ(tree.size(), 3u);
}

TEST(BalanceTests, StressSequential) {
    AVLTree<int> tree;
    const int N = 1000;
    for (int i = 1; i <= N; ++i) {
        tree.insert(i);
    }

    EXPECT_EQ(tree.size(), static_cast<size_t>(N));
    for (int i = 1; i <= N; ++i) {
        EXPECT_TRUE(tree.contains(i));
    }
}

TEST(EdgeCases, InsertDescendingMany) {
    AVLTree<int> tree;
    for (int i = 100; i >= 1; --i) {
        tree.insert(i);
    }
    EXPECT_EQ(tree.size(), 100u);
    for (int i = 1; i <= 100; ++i) {
        EXPECT_TRUE(tree.contains(i));
    }
}

TEST(EdgeCases, AlternatingInsertions) {
    AVLTree<int> tree;
    for (int i = 0; i < 50; ++i) {
        tree.insert(i);
        tree.insert(100 - i);
    }

    EXPECT_EQ(tree.size(), 100u);

    for (int i = 0; i <= 49; ++i) {
        EXPECT_TRUE(tree.contains(i));
        EXPECT_TRUE(tree.contains(100 - i));
    }

    EXPECT_FALSE(tree.contains(50));
}

TEST(EdgeCases, EraseRootRepeatedly) {
    AVLTree<int> tree;
    for (int i = 1; i <= 10; ++i) {
        tree.insert(i);
    }

    for (int i = 5; i <= 10; ++i) {
        EXPECT_TRUE(tree.contains(i));
        EXPECT_TRUE(tree.erase(i));
    }
    EXPECT_EQ(tree.size(), 4u);
    EXPECT_TRUE(tree.contains(1));
    EXPECT_FALSE(tree.contains(10));
}