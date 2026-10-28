#pragma once

#include <string>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <functional>
#include <cstddef>
#include <memory>
#include <utility>

#include "ICollection.hpp"
#include "base_structures/array_sequence/ArraySequence.hpp"
#include "base_structures/array_sequence/MutableArraySequence.hpp"
#include "base_structures/exceptions/Exceptions.hpp"
#include "base_structures/sequence/IEnumerator.hpp"

template<typename T, class Comparator = std::less<T>>
class AVLTree : public ICollection<T> {
public:
    enum class TraverseOrder {
        KLP,
        KPL,
        LPK,
        LKP,
        PLK,
        PKL
    };

private:
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

    void preorder(MutableArraySequence<const Node*>& buffer, const Node* node) const {
        if (!node) {
            return;
        }
        buffer.append(node);
        preorder(buffer, node->left.get());
        preorder(buffer, node->right.get());
    }

    void inorder(MutableArraySequence<const Node*>& buffer, const Node* node) const {
        if (!node) {
            return;
        }
        inorder(buffer, node->left.get());
        buffer.append(node);
        inorder(buffer, node->right.get());
    }

    void postorder(MutableArraySequence<const Node*>& buffer, const Node* node) const {
        if (!node) {
            return;
        }
        postorder(buffer, node->left.get());
        postorder(buffer, node->right.get());
        buffer.append(node);
    }

    void build_serialized(const Node* node, TraverseOrder order, std::string& result) const {
        if (!node) {
            return;
        }

        std::ostringstream oss;
        oss << node->value;
        std::string middle_part = "{" + oss.str() + "}";
        
        std::string left_part;
        build_serialized(node->left.get(), order, left_part);
        left_part = "(" + left_part + ")";

        std::string right_part;
        build_serialized(node->right.get(), order, right_part);
        right_part = "[" + right_part + "]";

        switch (order) {
            case TraverseOrder::KLP:
                result = middle_part + left_part + right_part;
                break;
            case TraverseOrder::KPL:
                result = middle_part + right_part + left_part;
                break;
            case TraverseOrder::LKP:
                result = left_part + middle_part + right_part;
                break;
            case TraverseOrder::LPK:
                result = left_part + right_part + middle_part;
                break;
            case TraverseOrder::PKL:
                result = right_part + middle_part + left_part;
                break;
            case TraverseOrder::PLK:
                result = right_part + left_part + middle_part;
                break;
        }
    }

    bool compare_recursive(const Node* node, const AVLTree& other) const {
        if (!node) {
            return true;
        }
        
        if (!other.contains(node->value)) {
            return false;
        }
        
        return compare_recursive(node->left.get(), other) && compare_recursive(node->right.get(), other);
    }

public:

    AVLTree() = default;
    explicit AVLTree(const Comparator& comparator) : comp(comparator) {}

    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    AVLTree(AVLTree&& other) : root(std::move(other.root)), node_count(other.node_count), comp(std::move(other.comp)) {
        other.node_count = 0;
    }

    AVLTree& operator=(AVLTree&& other) {
        if (this != &other) {
            root = std::move(other.root);
            node_count = other.node_count;
            comp = std::move(other.comp);
            other.node_count = 0;
        }
        return *this;
    }

    bool insert(const T& value) override {
        size_t before = node_count;
        do_insert(root, value);
        return node_count > before;
    }

    bool erase(const T& value) override {
        size_t before = node_count;
        do_erase(root, value);
        if (node_count == before) {
            throw InvalidArgumentException("erase", "Элемент не найден в дереве");
        }
        return true;
    }

    bool contains(const T& value) const override {
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

    size_t size() const override {
        return node_count;
    }

    bool is_empty() const override {
        return node_count == 0;
    }

    void clear() override {
        root.reset();
        node_count = 0;
    }

    AVLTree& operator+=(const T& value) {
        insert(value);
        return *this;
    }

    AVLTree& operator-=(const T& value) {
        if (contains(value)) {
            erase(value);
        }
        return *this;
    }

    bool operator==(const AVLTree& other) const {
        if (size() != other.size()) {
            return false;
        }
        
        return compare_recursive(root.get(), other);
    }

    bool operator!=(const AVLTree& other) const {
        return !(*this == other);
    }

    template<class Func>
    void traverse(TraverseOrder order, Func&& action) const {
        if (!root) {
            return;
        }
        MutableArraySequence<const Node*> buffer;
        bool is_reverse = (order == TraverseOrder::KPL || order == TraverseOrder::PKL || order == TraverseOrder::PLK);

        switch (order) {
            case TraverseOrder::KLP:
            case TraverseOrder::KPL:
                preorder(buffer, root.get());
                break;
            case TraverseOrder::LKP:
            case TraverseOrder::PKL:
                inorder(buffer, root.get());
                break;
            case TraverseOrder::LPK:
            case TraverseOrder::PLK:
                postorder(buffer, root.get());
                break;
        }
        
        size_t length = buffer.get_length();
        if (!is_reverse) {
            for (size_t index = 0; index < length; ++index) {
                action(buffer.get(index)->value);
            }
        }
        else {
            for (size_t index = length; index > 0; --index) {
                action(buffer.get(index - 1)->value);
            }
        }

    }

    AVLTree<T, Comparator> map(T (*func)(T)) const {
        if (func == nullptr) {
            throw NullPointerException("func (в map)");
        }
        AVLTree<T, Comparator> mapped(comp);
        traverse(TraverseOrder::LKP, [&mapped, func](const T& value) { mapped.insert(func(value)); });
        return mapped;
    }

    AVLTree<T, Comparator> where(bool (*predicate)(T)) const {
        if (predicate == nullptr) {
            throw NullPointerException("predicate (в where)");
        }
        AVLTree<T, Comparator> filtered(comp);
        traverse(TraverseOrder::LKP, [&filtered, predicate](const T& value) { if (predicate(value)) filtered.insert(value); });
        return filtered;
    }

    T reduce(T (*func)(T, T), T starter) const {
        if (func == nullptr) {
            throw NullPointerException("func (в reduce)");
        }
        T result = starter;
        traverse(TraverseOrder::LKP, [&result, func](const T& value) {
            result = func(result, value);
        });
        return result;
    }

    std::string serialize(TraverseOrder order = TraverseOrder::KLP) const {
        std::string result;
        build_serialized(root.get(), order, result);
        return result;
    }

    void deserialize(const std::string& str) {
        clear();
        if (str.empty()) {
            return;
        }

        size_t pos = 0;
        bool is_found = false;

        while ((pos = str.find('{', pos)) != std::string::npos) {
            is_found = true;
            size_t end = str.find('}', pos);
            
            if (end == std::string::npos) {
                throw InvalidArgumentException("Deserialize", "Нет закрывающей '}'");
            }

            std::string value_str = str.substr(pos + 1, end - pos - 1);
            
            if (value_str.empty()) {
                throw InvalidArgumentException("Deserialize", "Пустые скобки {}");
            }

            T value;
            
            if constexpr (std::is_same_v<T, std::string>) {
                value = value_str;
            } else {
                std::istringstream iss(value_str);
                if (!(iss >> value)) {
                    throw InvalidArgumentException("Deserialize", "Неверный формат значения '" + value_str + "'");
                }
            }

            insert(value);
            pos = end + 1;
        }

        if (!is_found) {
            throw InvalidArgumentException("Deserialize", "Строка не содержит данных в формате '{...}'");
        }
    }
    
    class AVLTreeIterator : public IEnumerator<T> {
    private:
        MutableArraySequence<const Node*> buffer_;
        const Node* current_;

        void go_left(const Node* node) {
            while (node) {
                buffer_.append(node);
                node = node->left.get();
            }
        }
    public:
        AVLTreeIterator(const Node* root) : current_(nullptr) {
            go_left(root);
            if (buffer_.get_length() > 0) {
                current_ = buffer_.get(buffer_.get_length() - 1);
            }
        }

        const T& get_elem() const override {
            if (!current_) {
                throw EmptyListException("AVLTreeIterator (get_elem)");
            }
            return current_->value;
        }

        void next_elem() override {
            if (buffer_.get_length() == 0) {
                current_ = nullptr;
                return;
            }
            const Node* node = buffer_.get(buffer_.get_length() - 1);
            buffer_.pop_back();
            go_left(node->right.get());

            if (buffer_.get_length() > 0) {
                current_ = buffer_.get(buffer_.get_length() - 1);
            }
            else {
                current_ = nullptr;
            }
        }

        bool is_end() const override {
            return current_ == nullptr;

        }

        AVLTreeIterator& operator++() {
            next_elem();
            return *this;
        }
        
        const T& operator*() const {
            return get_elem();
        }
        
        bool operator!=(const AVLTreeIterator& other) const {
            return current_ != other.current_;
        }

    };

    AVLTreeIterator begin() const {
        return AVLTreeIterator(root.get());
    }

    AVLTreeIterator end() const {
        return AVLTreeIterator(nullptr);
    }

    IEnumerator<T>* get_enumerator() const {
        return new AVLTreeIterator(root.get());
    }

};

template<typename T, class Comparator>
std::ostream& operator<<(std::ostream& os, const AVLTree<T, Comparator>& tree) {
    os << tree.serialize();
    return os;
}


