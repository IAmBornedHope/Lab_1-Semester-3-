#pragma once

#include <functional>
#include <cstddef>
#include <memory>
#include <utility>

template<typename T, class Comparator = std::less<T>>
class AVLTree {
    struct Node {
        T value;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        int height;
        
        explicit Node(const T& v) : value(v), height(1), left(nullptr), right(nullptr) {}
    };

    std::unique_ptr<Node> root;
    size_t node_count = 0;
    Comparator comp;


    int get_height(const Node* node) const {
        if (node) {
            return node->height;
        }
        return 0;
    }

    void update_height(Node* node) {
        int left_height = get_height(node->left.get());
        int right_height = get_height(node->right.get());

        if (left_height > right_height) {
            node->height = left_height + 1;
        }
        else {
            node->height = right_height + 1;
        }
    }

    int get_balance(const Node* node) const {
        return get_height(node->left.get()) - get_height(node->right.get());
    }
};