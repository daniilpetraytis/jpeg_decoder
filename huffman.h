#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>
#include <memory>

struct Node {
    std::shared_ptr<Node> left = nullptr;
    std::shared_ptr<Node> right = nullptr;
    int val;
    Node() {
        left = nullptr;
        right = nullptr;
        val = -1;
    }
    Node(int x) {
        left = nullptr;
        right = nullptr;
        val = x;
    }
};

// HuffmanTree decoder for DHT section.
class HuffmanTree {
private:
    std::vector<std::shared_ptr<Node>> nodes_;
    std::shared_ptr<Node> curr_node_ = nullptr;
    std::shared_ptr<Node> head_ = nullptr;

public:
    // code_lengths is the array of size no more than 16 with number of
    // terminated nodes in the Huffman tree.
    // values are the values of the terminated nodes in the consecutive
    // level order.
    void Build(const std::vector<uint8_t>& code_lengths, const std::vector<uint8_t>& values);

    // Moves the state of the huffman tree by |bit|. If the node is terminated,
    // returns true and overwrites |value|. If it is intermediate, returns false
    // and value is unmodified.
    bool Move(bool bit, int& value);
};
