#include <gtest/gtest.h>
#include "AVLTree.hpp"

TEST(IteratorTests, IEnumeratorInterface) {
    AVLTree<int> tree;
    for (int i : {5, 3, 7}) tree.insert(i);

    auto it = tree.get_enumerator(AVLTree<int>::LKP());
    int expected[] = {3, 5, 7};
    int index = 0;

    while (!it->is_end()) {
        EXPECT_EQ(it->get_elem(), expected[index]);
        index++;
        it->next_elem();
    }
    EXPECT_EQ(index, 3);
}

TEST(IteratorEdgeCases, EmptyTreeIterator) {
    AVLTree<int> tree;
    auto it = tree.get_enumerator();
    EXPECT_TRUE(it->is_end());
}

TEST(IteratorEdgeCases, SingleElementIterator) {
    AVLTree<int> tree;
    tree.insert(42);

    auto it = tree.get_enumerator();
    EXPECT_FALSE(it->is_end());
    EXPECT_EQ(it->get_elem(), 42);
    it->next_elem();
    EXPECT_TRUE(it->is_end());
}

TEST(IteratorEdgeCases, NegativeNumbersIterator) {
    AVLTree<int> tree;
    tree.insert(-5);
    tree.insert(-10);
    tree.insert(0);

    auto it = tree.get_enumerator(AVLTree<int>::LKP());
    EXPECT_EQ(it->get_elem(), -10);
    it->next_elem();
    EXPECT_EQ(it->get_elem(), -5);
    it->next_elem();
    EXPECT_EQ(it->get_elem(), 0);
    it->next_elem();
    EXPECT_TRUE(it->is_end());
}