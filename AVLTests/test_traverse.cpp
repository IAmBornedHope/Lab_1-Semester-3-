#include <gtest/gtest.h>
#include "AVLTree.hpp"

int current_index = 0;
int expected_values[10];

void check_value(int val) {
    EXPECT_EQ(val, expected_values[current_index]);
    current_index++;
}

TEST(TraverseTests, InOrderIsSorted) {
    AVLTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);

    int expected[] = {2, 3, 4, 5, 6, 7, 8};
    for (int i = 0; i < 7; ++i) expected_values[i] = expected[i];
    current_index = 0;

    tree.traverse(AVLTree<int>::TraverseOrder::LKP, check_value);
    EXPECT_EQ(current_index, 7);
}

TEST(TraverseTests, PreOrder) {
    AVLTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);

    int expected[] = {5, 3, 2, 4, 7};
    for (int i = 0; i < 5; ++i) expected_values[i] = expected[i];
    current_index = 0;

    tree.traverse(AVLTree<int>::TraverseOrder::KLP, check_value);
    EXPECT_EQ(current_index, 5);
}

TEST(TraverseTests, PostOrder) {
    AVLTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);

    int expected[] = {2, 4, 3, 7, 5};
    for (int i = 0; i < 5; ++i) expected_values[i] = expected[i];
    current_index = 0;

    tree.traverse(AVLTree<int>::TraverseOrder::LPK, check_value);
    EXPECT_EQ(current_index, 5);
}

TEST(TraverseTests, ReverseInOrderIsDescending) {
    AVLTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);

    int expected[] = {8, 7, 6, 5, 4, 3, 2};
    for (int i = 0; i < 7; ++i) expected_values[i] = expected[i];
    current_index = 0;

    tree.traverse(AVLTree<int>::TraverseOrder::PKL, check_value);
    EXPECT_EQ(current_index, 7);
}