#include "tree/Tree.hpp"

std::tuple<Treap::Node*, Treap::Node*> Treap::Node::split(Node* node, int x) {
    if (node == nullptr) {
        return std::make_tuple(nullptr, nullptr);
    }
    if (node->value < x) {
        auto [tmp_left, tmp_right] = split(node->right, x);
        node->right                = tmp_left;
        return std::make_tuple(node, tmp_right);
    } else {
        auto [tmp_left, tmp_right] = split(node->left, x);
        node->left                 = tmp_right;
        return std::make_tuple(tmp_left, node);
    }
}

Treap::Node* Treap::Node::merge(Treap::Node* a, Treap::Node* b) {
    if (a == nullptr) {
        return b;
    }
    if (b == nullptr) {
        return a;
    }
    if (a->priority > b->priority) {
        a->right = merge(a->right, b);
        return a;
    } else {
        b->left = merge(a, b->left);
        return b;
    }
}

bool Treap::insert(int value) {
    if (contains(value)) {
        return false;
    }
    Node* tmp = insert(root, new Node(value));
    if (root == nullptr || tmp->priority > root->priority) {
        root = tmp;
    }
    s++;
    return true;
}

Treap::Node* Treap::insert(Treap::Node* ancestor, Treap::Node* node) {
    if (ancestor == nullptr) {
        return node;
    }
    if (node->priority > ancestor->priority) {
        std::tie(node->left, node->right) = Node::split(ancestor, node->value);
        return node;
    }
    if (node->value < ancestor->value) {
        ancestor->left = insert(ancestor->left, node);
    } else {
        ancestor->right = insert(ancestor->right, node);
    }
    return ancestor;
}

bool Treap::remove(int value) {
    if (!contains(value)) {
        return false;
    }
    Node* tmp = remove(root, value);
    if (root == nullptr) {
        root = tmp;
    }
    s--;
    return true;
}

Treap::Node* Treap::remove(Treap::Node* ancestor, int x) {
    if (ancestor->value == x) {
        Node* tmp = Node::merge(ancestor->left, ancestor->right);
        if (root == ancestor) {
            root = nullptr;
        }
        return tmp;
    }
    if (x < ancestor->value) {
        ancestor->left = remove(ancestor->left, x);
    } else {
        ancestor->right = remove(ancestor->right, x);
    }
    return ancestor;
}

[[nodiscard]] bool Treap::contains(int x) const noexcept {
    Node* tmp = root;
    while (tmp != nullptr) {
        if (tmp->value == x) {
            return true;
        }
        if (x < tmp->value) {
            tmp = tmp->left;
        } else {
            tmp = tmp->right;
        }
    }
    return false;
}

[[nodiscard]] std::size_t Treap::size() const noexcept {
    return s;
}

[[nodiscard]] bool Treap::empty() const noexcept {
    return root == nullptr;
}

void Treap::fill_values(const Node* node, std::vector<int>& ans) const {
    if (node != nullptr) {
        ans.push_back(node->value);
        fill_values(node->left, ans);
        fill_values(node->right, ans);
    }
}

[[nodiscard]] std::vector<int> Treap::values() const noexcept {
    std::vector<int> ans;
    fill_values(root, ans);
    sort(ans.begin(), ans.end());
    return ans;
}

Treap::~Treap() {
    delete root;
}