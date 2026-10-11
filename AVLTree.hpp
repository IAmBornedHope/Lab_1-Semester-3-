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

    std::unique_ptr<Node> rotate_right(std::unique_ptr<Node> root) {
        auto new_root = std::move(root->left);
        root->left = std::move(new_root->right);
        new_root->right = std::move(root);

        update_height(new_root->right.get());
        update_height(new_root.get());

        return new_root;
    }

    std::unique_ptr<Node> rotate_left(std::unique_ptr<Node> root) {
        auto new_root = std::move(root->right);
        root->right = std::move(new_root->left);
        new_root->left = std::move(root);

        update_height(new_root->left.get());
        update_height(new_root.get());

        return new_root;
    }

    std::unique_ptr<Node> balance_tree(std::unique_ptr<Node> root) {
        if (!root) {
            return root;
        }
        update_height(root.get());
        int balance = get_balance(root.get());

        if (balance > 1) {
            if (get_balance(root->left.get()) < 0) {
                root->left = rotate_left(std::move(root->left));
            }
            return rotate_right(std::move(root));
        }

        if (balance < -1) {
            if (get_balance(root->right.get()) > 0) {
                root->right = rotate_right(std::move(root->right));
            }
            return rotate_left(std::move(root));
        }

        return root;
    }

    std::unique_ptr<Node> do_insert(std::unique_ptr<Node> node, const T& value) {
        if (!node) {
            node = std::make_unique<Node>(value);
            ++node_count;
            return node;
        }

        if (comp(value, node->value)) {
            node->left = do_insert(std::move(node->left), value);
        }
        else if (comp(node->value, value)) {
            node->right = do_insert(std::move(node->right), value);
        }
        else {
            return node;
        }

        return balance_tree(std::move(node));
    }


};


