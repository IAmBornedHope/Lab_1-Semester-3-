#include <gtest/gtest.h>
#include "AVLTree.hpp"

TEST(SerializeTests, EmptyTree) {
    AVLTree<int> tree;
    EXPECT_EQ(tree.serialize(), "");
}

TEST(SerializeTests, SingleNode) {
    AVLTree<int> tree;
    tree.insert(5);
    EXPECT_EQ(tree.serialize(AVLTree<int>::KLP()), "{5}");
}

TEST(SerializeTests, SimpleTree) {
    AVLTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);

    std::string str = tree.serialize(AVLTree<int>::KLP());
    EXPECT_FALSE(str.empty());
}

TEST(SerializeTests, DeserializeBasic) {
    AVLTree<int> tree;
    tree.deserialize("{5}({3}()[])[{7}()[]]");

    EXPECT_EQ(tree.size(), 3u);
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_TRUE(tree.contains(7));
}

TEST(SerializeTests, DeserializeErrorNoClosingBrace) {
    AVLTree<int> tree;
    EXPECT_THROW(tree.deserialize("{5"), InvalidArgumentException);
}

TEST(SerializeTests, DeserializeErrorEmptyBraces) {
    AVLTree<int> tree;
    EXPECT_THROW(tree.deserialize("{5}({})[]"), InvalidArgumentException);
}

TEST(SerializeTests, RoundTrip) {
    AVLTree<int> original;
    for (int i : {10, 5, 15, 3, 7}) {
        original.insert(i);
    }

    std::string serialized = original.serialize();
    AVLTree<int> restored;
    restored.deserialize(serialized);

    EXPECT_EQ(original, restored);
}

TEST(SerializeEdgeCases, SingleNegativeNode) {
    AVLTree<int> tree;
    tree.insert(-5);
    EXPECT_EQ(tree.serialize(), "{-5}");
}

TEST(SerializeEdgeCases, SingleStringNode) {
    AVLTree<std::string> tree;
    tree.insert("hello");
    EXPECT_EQ(tree.serialize(), "{hello}");
}

TEST(SerializeEdgeCases, StringWithSpaces) {
    AVLTree<std::string> tree;
    tree.insert("hello world");
    tree.deserialize("{hello world}()[]");
    EXPECT_TRUE(tree.contains("hello world"));
}

TEST(SerializeEdgeCases, DeserializeEmptyString) {
    AVLTree<int> tree;
    tree.insert(5);
    tree.deserialize("");
    EXPECT_TRUE(tree.is_empty());
}

TEST(SerializeEdgeCases, DeserializeWithGarbageAround) {
    AVLTree<int> tree;
    tree.deserialize("garbage {5}()[] more garbage");
    EXPECT_EQ(tree.size(), 1u);
    EXPECT_TRUE(tree.contains(5));
}

TEST(SerializeEdgeCases, DeserializeNoBraces) {
    AVLTree<int> tree;
    EXPECT_THROW(tree.deserialize("hello"), InvalidArgumentException);
}