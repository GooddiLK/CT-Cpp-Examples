#ifndef TREE_HPP
#define TREE_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <random>
#include <tuple>
#include <vector>

class Treap {
private:
    struct Node {
        int value;
        int64_t priority;
        Node* left{nullptr};
        Node* right{nullptr};

        Node(int value) : value(value) {
            static thread_local std::mt19937 gen(std::random_device{}());
            std::uniform_int_distribution<int64_t> dist(std::numeric_limits<int64_t>::min(),
                                                        std::numeric_limits<int64_t>::max());
            priority = dist(gen);
        }

        ~Node() {
            delete left;
            delete right;
        }

        static Node* merge(Node* a, Node* b);
        static std::tuple<Treap::Node*, Treap::Node*> split(Node* node, int x);
    };

    Node* root = nullptr;
    size_t s   = 0;

    Node* insert(Node* ancestor, Node* node);
    Node* remove(Node* ancestor, int x);
    void fill_values(const Node* node, std::vector<int>& result) const;

public:
    [[nodiscard]] bool contains(int value) const noexcept;
    bool insert(int value);
    bool remove(int value);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] std::vector<int> values() const noexcept;

    ~Treap();
};

#endif
