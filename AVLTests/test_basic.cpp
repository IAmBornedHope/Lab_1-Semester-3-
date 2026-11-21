#include <gtest/gtest.h>
#include "AVLTree.hpp"

TEST(BasicTests, EmptyTree) {
    AVLTree<int> tree;
    EXPECT_TRUE(tree.is_empty());
    EXPECT_EQ(tree.size(), 0u);
    EXPECT_FALSE(tree.contains(5));
}

TEST(BasicTests, InsertAndContains) {
    AVLTree<int> tree;
    EXPECT_TRUE(tree.insert(10));
    EXPECT_TRUE(tree.insert(5));
    EXPECT_TRUE(tree.insert(15));

    EXPECT_EQ(tree.size(), 3u);
    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(15));
    EXPECT_FALSE(tree.contains(99));
}

TEST(BasicTests, DuplicateInsert) {
    AVLTree<int> tree;
    EXPECT_TRUE(tree.insert(42));
    EXPECT_FALSE(tree.insert(42));
    EXPECT_EQ(tree.size(), 1u);
}

TEST(BasicTests, Erase) {
    AVLTree<int> tree;
    for (int i = 1; i <= 5; ++i) {
        tree.insert(i);
    }

    EXPECT_TRUE(tree.erase(3));
    EXPECT_FALSE(tree.contains(3));
    EXPECT_EQ(tree.size(), 4u);
}

TEST(BasicTests, EraseNonExistentThrows) {
    AVLTree<int> tree;
    tree.insert(1);
    EXPECT_THROW(tree.erase(99), InvalidArgumentException);
}

TEST(BasicTests, Clear) {
    AVLTree<int> tree;
    tree.insert(1);
    tree.insert(2);
    tree.clear();
    EXPECT_TRUE(tree.is_empty());
    EXPECT_EQ(tree.size(), 0u);
}

TEST(BasicTests, StringTree) {
    AVLTree<std::string> tree;
    tree.insert("apple");
    tree.insert("mephi");
    tree.insert("lol");

    EXPECT_TRUE(tree.contains("apple"));
    EXPECT_FALSE(tree.contains("kaf22"));
    EXPECT_EQ(tree.size(), 3u);
}

TEST(EdgeCases, InsertNegativeNumbers) {
    AVLTree<int> tree;
    EXPECT_TRUE(tree.insert(-10));
    EXPECT_TRUE(tree.insert(-20));
    EXPECT_TRUE(tree.insert(0));
    EXPECT_TRUE(tree.insert(10));

    EXPECT_EQ(tree.size(), 4u);
    EXPECT_TRUE(tree.contains(-10));
    EXPECT_TRUE(tree.contains(0));
    EXPECT_TRUE(tree.contains(10));
    EXPECT_FALSE(tree.contains(-5));
}

TEST(EdgeCases, InsertOneElementAndEraseIt) {
    AVLTree<int> tree;
    tree.insert(42);
    EXPECT_EQ(tree.size(), 1u);

    EXPECT_TRUE(tree.erase(42));
    EXPECT_TRUE(tree.is_empty());
    EXPECT_EQ(tree.size(), 0u);
    EXPECT_FALSE(tree.contains(42));
}

TEST(EdgeCases, InsertManyDuplicates) {
    AVLTree<int> tree;
    for (int i = 0; i < 10; ++i) {
        tree.insert(5);
    }
    EXPECT_EQ(tree.size(), 1u);
}

TEST(EdgeCases, EraseFromEmptyTreeThrows) {
    AVLTree<int> tree;
    EXPECT_THROW(tree.erase(5), InvalidArgumentException);
}

TEST(EdgeCases, ClearEmptyTree) {
    AVLTree<int> tree;
    tree.clear();
    EXPECT_TRUE(tree.is_empty());
}

TEST(EdgeCases, ContainsInEmptyTree) {
    AVLTree<int> tree;
    EXPECT_FALSE(tree.contains(1));
    EXPECT_FALSE(tree.contains(-1));
    EXPECT_FALSE(tree.contains(0));
}