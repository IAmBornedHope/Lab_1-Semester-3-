#include <gtest/gtest.h>
#include "AVLTree.hpp"

TEST(SubtreeTests, GetSubtreeExists) {
    AVLTree<int> tree;
    for (int i : {5, 3, 7, 2, 4}) tree.insert(i);

    AVLTree<int> sub = tree.get_subtree(3);
    EXPECT_EQ(sub.size(), 3u);
    EXPECT_TRUE(sub.contains(3));
    EXPECT_TRUE(sub.contains(2));
    EXPECT_TRUE(sub.contains(4));
}

TEST(SubtreeTests, GetSubtreeNotExists) {
    AVLTree<int> tree;
    tree.insert(5);

    AVLTree<int> sub = tree.get_subtree(99);
    EXPECT_TRUE(sub.is_empty());
}

TEST(SubtreeTests, OperatorEquals) {
    AVLTree<int> tree1;
    AVLTree<int> tree2;

    for (int i : {1, 2, 3}) tree1.insert(i);
    for (int i : {3, 2, 1}) tree2.insert(i);

    EXPECT_TRUE(tree1 == tree2);
}

TEST(SubtreeTests, OperatorNotEquals) {
    AVLTree<int> tree1;
    AVLTree<int> tree2;

    tree1.insert(1);
    tree2.insert(2);

    EXPECT_TRUE(tree1 != tree2);
}

TEST(SubtreeTests, OperatorPlusEqual) {
    AVLTree<int> tree;
    tree += 5;
    tree += 3;

    EXPECT_EQ(tree.size(), 2u);
    EXPECT_TRUE(tree.contains(5));
}

TEST(SubtreeTests, OperatorMinusEqual) {
    AVLTree<int> tree;
    tree += 5;
    tree += 3;
    tree -= 3;

    EXPECT_EQ(tree.size(), 1u);
    EXPECT_FALSE(tree.contains(3));
}

TEST(SubtreeEdgeCases, GetSubtreeFromEmptyTree) {
    AVLTree<int> tree;
    AVLTree<int> sub = tree.get_subtree(5);
    EXPECT_TRUE(sub.is_empty());
}

TEST(SubtreeEdgeCases, GetSubtreeOfRoot) {
    AVLTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);

    AVLTree<int> sub = tree.get_subtree(5);
    EXPECT_EQ(sub.size(), 3u);
    EXPECT_TRUE(sub.contains(5));
    EXPECT_TRUE(sub.contains(3));
    EXPECT_TRUE(sub.contains(7));
}

TEST(SubtreeEdgeCases, GetSubtreeOfLeaf) {
    AVLTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);

    AVLTree<int> sub = tree.get_subtree(7);
    EXPECT_EQ(sub.size(), 1u);
    EXPECT_TRUE(sub.contains(7));
    EXPECT_FALSE(sub.contains(5));
}

TEST(SubtreeEdgeCases, EqualsEmptyTrees) {
    AVLTree<int> tree1;
    AVLTree<int> tree2;
    EXPECT_TRUE(tree1 == tree2);
}

TEST(SubtreeEdgeCases, EqualsOneEmptyOneNot) {
    AVLTree<int> tree1;
    AVLTree<int> tree2;
    tree2.insert(5);
    EXPECT_FALSE(tree1 == tree2);
    EXPECT_TRUE(tree1 != tree2);
}

TEST(SubtreeEdgeCases, PlusEqualAndMinusEqual) {
    AVLTree<int> tree;
    tree += 10;
    tree += 20;
    tree += 30;

    EXPECT_EQ(tree.size(), 3u);

    tree -= 20;
    EXPECT_EQ(tree.size(), 2u);
    EXPECT_FALSE(tree.contains(20));

    tree -= 100;
    EXPECT_EQ(tree.size(), 2u);
}