#include "huffman.h"
#include <memory>
#include <iostream>

void HuffmanTree::Build(const std::vector<uint8_t> &code_lengths,
                        const std::vector<uint8_t> &values) {
    (void)code_lengths;
    (void)values;
    try {
        int i = 2;
        if (code_lengths.size() > 16) {
            throw std::invalid_argument("3");
        }
        int summ = 0;
        for (auto elem : code_lengths) {
            summ += elem;
            if (elem > i) {
                throw std::invalid_argument("4");
            }
            i *= 2;
        }
        if (summ != values.size()) {
            throw std::invalid_argument("5");
        }
    } catch (...) {
        throw std::invalid_argument("6");
    }
    head_ = std::make_shared<Node>();
    curr_node_ = head_;
    nodes_.push_back(head_);
    int counter = 1;
    int count = 1;
    int summ = 0;
    for (size_t i = 0; i != code_lengths.size(); ++i) {
        int x = nodes_.size();
        int val = code_lengths[i];
        counter = count;
        count = 0;
        for (int j = x - counter; j != x; ++j) {
            if (val > 0) {
                nodes_[j]->left = std::make_shared<Node>(values[summ]);
                ++summ;
                --val;
                nodes_.push_back(nodes_[j]->left);
                if (val > 0) {
                    nodes_[j]->right = std::make_shared<Node>(values[summ]);
                    ++summ;
                    --val;
                    nodes_.push_back(nodes_[j]->right);
                } else {
                    nodes_[j]->right = std::make_shared<Node>();
                    nodes_.push_back(nodes_[j]->right);
                    ++count;
                }
            } else {
                nodes_[j]->left = std::make_shared<Node>();
                nodes_[j]->right = std::make_shared<Node>();
                count += 2;
                nodes_.push_back(nodes_[j]->left);
                nodes_.push_back(nodes_[j]->right);
            }
        }
    }
    for (int i = 0; i != nodes_.size(); ++i) {
        std::cout << nodes_[i] << " ";
    }
    std::cout << nodes_.size();
}

bool HuffmanTree::Move(bool bit, int &value) {
    (void)bit;
    (void)value;
    if (!bit) {
        if (!curr_node_ || !curr_node_->left) {
            throw std::invalid_argument("1");
        }
        if (curr_node_->left->val != -1) {
            value = curr_node_->left->val;
            curr_node_ = head_;
            return true;
        } else {
            curr_node_ = curr_node_->left;
            return false;
        }
    } else {
        if (!curr_node_ || !curr_node_->right) {
            throw std::invalid_argument("7");
        }
        if (curr_node_->right->val != -1) {
            value = curr_node_->right->val;
            curr_node_ = head_;
            return true;
        } else {
            curr_node_ = curr_node_->right;
            return false;
        }
    }
}
