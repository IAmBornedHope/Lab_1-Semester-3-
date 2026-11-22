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
    enum class PartKind { Root, Child };
    struct Part {
        PartKind kind;
        size_t child_index;
        static Part root() 
        { 
            return { PartKind::Root, 0 }; 
        }

        static Part child(size_t i) 
        { 
            return { PartKind::Child, i }; 
        }
    };
    using OrderPolicy = MutableArraySequence<Part>;

    static OrderPolicy KLP() {
        OrderPolicy p;
        p.append(Part::root());
        p.append(Part::child(0));
        p.append(Part::child(1));
        return p;
    }
    static OrderPolicy KPL() {
        OrderPolicy p;
        p.append(Part::root());
        p.append(Part::child(1));
        p.append(Part::child(0));
        return p;
    }
    static OrderPolicy LKP() {
        OrderPolicy p;
        p.append(Part::child(0));
        p.append(Part::root());
        p.append(Part::child(1));
        return p;
    }
    static OrderPolicy LPK() {
        OrderPolicy p;
        p.append(Part::child(0));
        p.append(Part::child(1));
        p.append(Part::root());
        return p;
    }
    static OrderPolicy PKL() {
        OrderPolicy p;
        p.append(Part::child(1));
        p.append(Part::root());
        p.append(Part::child(0));
        return p;
    }
    static OrderPolicy PLK() {
        OrderPolicy p;
        p.append(Part::child(1));
        p.append(Part::child(0));
        p.append(Part::root());
        return p;
    }

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
    std::shared_ptr<const int> lifetime_ = std::make_shared<const int>(0);


    static int get_height(const Node* node) {
        if (node) {
            return node->height;
        }
        return 0;
    }

    static void update_height(Node* node) {
        int left_height = get_height(node->left.get());
        int right_height = get_height(node->right.get());

        if (left_height > right_height) {
            node->height = left_height + 1;
        }
        else {
            node->height = right_height + 1;
        }
    }

    static int get_balance(const Node* node) {
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

    template<class Func>
    static void traverse_node(const Node* node, OrderPolicy policy, Func&& action) {
        if (!node) {
            return;
        }

        for (const Part& p : policy) {
            switch (p.kind) {
                case PartKind::Root:
                    action(node->value);
                    break;

                case PartKind::Child: {
                    const Node* child = nullptr;

                    if (p.child_index == 0) {
                        child = node->left.get();
                    } else {
                        child = node->right.get();
                    }

                    traverse_node(child, policy, action);
                    break;
                }
            }
        }
    }

    static void build_serialized(const Node* node, OrderPolicy& order, std::string& result) {
        if (!node) {
            return;
        }

        std::ostringstream oss;
        oss << node->value;
        std::string middle_part = "{" + oss.str() + "}";

        if (!node->left && !node->right) {
            result += middle_part;
            return;
        }

        std::string left_raw;
        build_serialized(node->left.get(), order, left_raw);

        std::string right_raw;
        build_serialized(node->right.get(), order, right_raw);

        for (const Part& p : order) {
            switch (p.kind) {
                case PartKind::Root:
                    result += middle_part;
                    break;

                case PartKind::Child:
                    if (p.child_index == 0) {
                        if (!node->left) {
                            result += "()";
                        } else {
                            result += "(" + left_raw + ")";
                        }
                    } else {
                        if (!node->right) {
                            result += "[]";
                        } else {
                            result += "[" + right_raw + "]";
                        }
                    }
                    break;
            }
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

    static std::unique_ptr<Node> clone(const Node* node) {
        if (!node) {
            return nullptr;
        }
        auto copy = std::make_unique<Node>(node->value);
        copy->height = node->height;
        copy->left = clone(node->left.get());
        copy->right = clone(node->right.get());
        return copy;
    }

    static void copy_subtree(const Node* node, AVLTree& result) {
        if (!node) {
            return;
        }
        result.insert(node->value);
        copy_subtree(node->left.get(), result);
        copy_subtree(node->right.get(), result);
    }

public:

    AVLTree() = default;
    explicit AVLTree(const Comparator& comparator) : comp(comparator) {}

    AVLTree(const AVLTree& other) : root(clone(other.root.get())), node_count(other.node_count), comp(other.comp) {}

    AVLTree& operator=(const AVLTree& other) {
        if (this != &other) {
            AVLTree temp(other);
            root = std::move(temp.root);
            node_count = temp.node_count;
            comp = temp.comp;
        }
        return *this;
    }

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
    void traverse(const OrderPolicy& policy, Func&& action) const {
        traverse_node(root.get(), policy, action);
    }

        

    AVLTree<T, Comparator> map(T (*func)(T)) const {
        if (func == nullptr) {
            throw NullPointerException("func (в map)");
        }
        AVLTree<T, Comparator> mapped(comp);
        traverse(LKP(), [&mapped, func](const T& value) { mapped.insert(func(value)); });
        return mapped;
    }

    AVLTree<T, Comparator> where(bool (*predicate)(T)) const {
        if (predicate == nullptr) {
            throw NullPointerException("predicate (в where)");
        }
        AVLTree<T, Comparator> filtered(comp);
        traverse(LKP(), [&filtered, predicate](const T& value) { if (predicate(value)) filtered.insert(value); });
        return filtered;
    }

    T reduce(T (*func)(T, T), T starter) const {
        if (func == nullptr) {
            throw NullPointerException("func (в reduce)");
        }
        T result = starter;
        traverse(LKP(), [&result, func](const T& value) {
            result = func(result, value);
        });
        return result;
    }

    std::string serialize(OrderPolicy order = KLP()) const {
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
        struct Frame {
            const Node* node = nullptr;
            size_t next_part = 0;
        };

        OrderPolicy policy_;
        MutableArraySequence<Frame> stack_;
        const Node* current_ = nullptr;

        std::weak_ptr<const int> lifetime_;
        bool has_tree_ = false;

        void check_lifetime() const {
            if (has_tree_ && lifetime_.expired()) {
                throw IteratorPointerException("AVLIterator: Дерево удалено");
            }
        }

    public:
        AVLTreeIterator() = default;

        explicit AVLTreeIterator(const std::unique_ptr<Node>& root, const OrderPolicy& policy, const std::shared_ptr<const int>& lifetime) 
        : policy_(policy), lifetime_(lifetime), has_tree_(true) {
            if (policy_.get_length() != 3) {
                throw InvalidArgumentException("AVLIterator", "Обход должен содержать 3 части");
            }

            bool seen[3] = {false, false, false};

            for (const Part& part : policy_) {
                size_t index = 0;

                if (part.kind == PartKind::Child && part.child_index < 2) {
                    index = part.child_index + 1;
                } else if (part.kind != PartKind::Root) {
                    throw InvalidArgumentException("AVLIterator", "Некорректный обход");
                }

                if (seen[index]) {
                    throw InvalidArgumentException("AVLIterator", "Повторная часть обхода");
                }

                seen[index] = true;
            }

            if (root) {
                stack_.append(Frame{root.get(), 0});
            }

            next_elem();
        }

        const T& get_elem() const override {
            check_lifetime();

            if (is_end()) {
                throw IndexOutOfRangeException("get_elem: Нет такого элемента");
            }

            return current_->value;
        }

        void next_elem() override {
            check_lifetime();
            current_ = nullptr;

            while (stack_.get_length() > 0) {
                Frame& frame = stack_[stack_.get_length() - 1];

                if (frame.next_part == policy_.get_length()) {
                    stack_.pop_back();
                    continue;
                }

                const Node* node = frame.node;
                const Part& part = policy_[frame.next_part++];

                if (part.kind == PartKind::Root) {
                    current_ = node;
                    return;
                }

                const Node* child = nullptr;

                if (part.child_index == 0) {
                    child = node->left.get();
                } else {
                    child = node->right.get();
                }

                if (child) {
                    stack_.append(Frame{child, 0});
                }
            }
        }

        bool is_end() const override {
            check_lifetime();
            return current_ == nullptr;
        }

        AVLTreeIterator& operator++() {
            next_elem();
            return *this;
        }

        const T& operator*() const {
            return get_elem();
        }

        bool operator==(const AVLTreeIterator& other) const {
            check_lifetime();
            other.check_lifetime();

            if (current_ != other.current_) {
                return false;
            }

            if (is_end()) {
                return true;
            }

            for (size_t index = 0; index < policy_.get_length(); ++index) {
                const Part& first = policy_.get_reference(index);
                const Part& second = other.policy_.get_reference(index);

                if (first.kind != second.kind || first.child_index != second.child_index) {
                    return false;
                }
            }

            return true;
        }

        bool operator!=(const AVLTreeIterator& other) const {
            return !(*this == other);
        }
    };

    AVLTreeIterator begin(const OrderPolicy& policy = KLP()) const {
        return AVLTreeIterator(root, policy, lifetime_);
    }

    AVLTreeIterator end() const {
        return AVLTreeIterator();
    }

    std::unique_ptr<IEnumerator<T>> get_enumerator(const OrderPolicy& policy = KLP()) const {
        return std::make_unique<AVLTreeIterator>(begin(policy));
    }


    AVLTree<T, Comparator> get_subtree(const T& value) const {
        const Node* current = root.get();
        while (current) {
            if (comp(value, current->value)) {
                current = current->left.get();
            }
            else if (comp(current->value, value)) {
                current = current->right.get();
            }
            else {
                break;
            }
        }

        AVLTree<T, Comparator> result(comp);

        if (current) {
            copy_subtree(current, result);
        }
        return result;
    }

};

template<typename T, class Comparator>
std::ostream& operator<<(std::ostream& os, const AVLTree<T, Comparator>& tree) {
    os << tree.serialize();
    return os;
}


