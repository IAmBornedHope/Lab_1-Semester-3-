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

    void do_insert(std::unique_ptr<Node>& node, const T& value) {
        if (!node) {
            node = std::make_unique<Node>(value);
            ++node_count;
            return;
        }

        if (comp(value, node->value)) {
            do_insert(node->left, value);
        }
        else if (comp(node->value, value)) {
            do_insert(node->right, value);
        }
        else {
            return;
        }

        node = balance_tree(std::move(node));
    }

    std::unique_ptr<Node> extract_min(std::unique_ptr<Node>& node) {
        if (!node->left) {
            auto min_node = std::move(node);
            node = std::move(min_node->right);
            return min_node;
        }

        auto min_node = extract_min(node->left);
        node = balance_tree(std::move(node));
        return min_node;
    }

    void do_erase(std::unique_ptr<Node>& node, const T& value) {
        if (!node) {
            return;
        }
        if (comp(value, node->value)) {
            do_erase(node->left, value);
        }
        else if (comp(node->value, value)) {
            do_erase(node->right, value);
        }
        else {
            --node_count;
            if (!node->left) {
                node = std::move(node->right);
            }
            else if (!node->right) {
                node = std::move(node->left);
            }
            else {
                auto successor = extract_min(node->right);
                successor->left = std::move(node->left);
                successor->right = std::move(node->right);
                node = std::move(successor);
            }
        }
        node = balance_tree(std::move(node));
    }

public:
    bool insert(const T& value) {
        size_t before = node_count;
        do_insert(root, value);
        return node_count > before;
    }

    bool erase(const T& value) {
        size_t before = node_count;
        do_erase(root, value);
        return node_count < before;
    }

    bool contains(const T& value) const {
        const Node* current = root.get();
        while (current) {
            if (comp(value, current->value)) {
                current = current->left.get();
            }
            else if (comp(current->value, value)) {
                current = current->right.get();
            }
            else {
                return true;
            }
        }
        return false;
    }

    size_t size() const {
        return node_count;
    }

    bool is_empty() const {
        return node_count == 0;
    }
};


