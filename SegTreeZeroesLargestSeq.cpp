#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>
#include <string>

class SegmentTree {
private :
    //{[0] ... [len/2 - 1] ___ [len/2]     ...    [len - 1]}
    // UB  ... max[len-2, len-1] _ data[0] ... data.length() | neutral
    struct Node {
        int32_t value;
        int32_t left_value;
        int32_t right_value;
        size_t left;
        size_t right;

        Node() : value(-1), left_value(0), right_value(0), left(0), right(0) {}

        Node(int32_t val, int32_t l_v, int32_t r_v, size_t left, size_t right) :
                value(val), left_value(l_v), right_value(r_v), left(left), right(right) {}
    };

    Node *values;
    size_t len;

    [[nodiscard]] Node merge(Node l_son, Node r_son, size_t i_ancestor) const {
        Node ancestor;
        if (i_ancestor * 2 >= len / 2) {
            if (!l_son.value && !r_son.value) {
                ancestor = Node(2, 2, 2, l_son.left, r_son.right);
            } else if (!l_son.value) {
                ancestor = Node(1, 1, 0, l_son.left, r_son.right);
            } else if (!r_son.value) {
                ancestor = Node(1, 0, 1, l_son.left, r_son.right);
            } else {
                ancestor = Node(0, 0, 0, l_son.left, r_son.right);
            }
        } else {
            ancestor = Node(std::max({l_son.value, r_son.value, l_son.right_value + r_son.left_value}),
                            l_son.left_value, r_son.right_value, l_son.left, r_son.right);
            if (l_son.left_value == (l_son.right - l_son.left + 1)) {
                ancestor.left_value += r_son.left_value;
            }
            if (r_son.right_value == (r_son.right - r_son.left + 1)) {
                ancestor.right_value += l_son.right_value;
            }
        }
        return ancestor;
    }

    Node giveZeroesLen(size_t l, size_t r, size_t s_i, size_t e_i, size_t i) {
        if (r < s_i || l > e_i) {
            return {};
        }
        if (s_i >= l && e_i <= r) {
            return values[i];
        }
        return merge(giveZeroesLen(l, r, s_i, s_i + (e_i - s_i) / 2, i * 2),
                     giveZeroesLen(l, r, s_i + (e_i - s_i) / 2 + 1, e_i, i * 2 + 1), i);
    }

    void addNeutral(size_t index) {
        values[index] = Node(-1, 0, 0, index, index);
    }

    void fillForCountZeroes() {
        for (size_t i = len / 2 - 1; i > 0; i--) {
            values[i] = merge(values[i * 2], values[i * 2 + 1], i);
        }
    }

public:

    explicit SegmentTree(std::vector<int32_t> &data) {
        size_t s = 1;
        while (s < data.size()) {
            s <<= 1;
        }
        values = new Node[s * 2];
        for (size_t i = 0; i < s; i++) {
            if (i < data.size()) {
                values[s + i] = Node(data[i], data[i] == 0, data[i] == 0, i, i);
            } else {
                addNeutral(s + i);
            }
        }
        len = s * 2;
        fillForCountZeroes();
    }

    std::int32_t giveZeroesLen(size_t left, size_t right) {
        return giveZeroesLen(left, right, 1, len / 2, 1).value;
    }

    void change(int32_t index, int32_t value) {
        if (values[index + len / 2].value == value) {
            return;
        }
        if (!values[index + len / 2].value) {
            values[index + len / 2] = Node(value, 0, 0, index, index);
        } else if (!value) {
            values[index + len / 2] = Node(value, 1, 1, index, index);
        } else {
            values[index + len / 2].value = value;
            return;
        }
        for (size_t i = (index + len / 2) / 2; i > 0; i /= 2) {
            values[i] = merge(values[i * 2], values[i * 2 + 1], i);
        }
    }

    void printTree() {
        std::cout << "0_el ";
        for (size_t i = 1; i < len / 2; i++) {
            std::cout << values[i].value << " ";
        }
        std::cout << std::endl << "  ";
        for (size_t i = len / 2; i < len; i++) {
            std::cout << values[i].value << " ";
            std::cout << std::endl;
        }
    }

    //3/5 Rule broken MAYBE ERROR
    ~SegmentTree() {
        delete[] values;
    }
};

int main() {
    uint32_t n;
    std::cin >> n;
    std::vector<int> input_data;
    for (size_t i = 0; i < n; i++) {
        int32_t tmp;
        std::cin >> tmp;
        input_data.push_back(tmp);
    }
    SegmentTree tree = SegmentTree(input_data);
    //tree.printTree();
    std::cin >> n;
    for (size_t i = 0; i < n; i++) {
        std::string tmp;
        int32_t l, r;
        std::cin >> tmp >> l >> r;
        if (tmp == "QUERY") {
            std::cout << tree.giveZeroesLen(l, r) << std::endl;
        } else if (tmp == "UPDATE") {
            tree.change(l - 1, r);
        } else {
            std::cout << "Uncorrected input data" << std::endl;
            break;
        }
    }
    return 0;
}
