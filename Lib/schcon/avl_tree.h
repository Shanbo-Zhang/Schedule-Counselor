//
// Created by Escap on 10/13/2023.
//

#ifndef SCLIB_AVL_TREE_H
#define SCLIB_AVL_TREE_H

#include "base.h"

template<typename K, typename V>
class AVLTree {
public:
    AVLTree() : root_(nullptr) {}

    AVLTree(const AVLTree<K, V> &other) noexcept {

    }

    ~AVLTree() {

    }

    SizeType Count() const noexcept {
        return count_;
    }

    bool IsEmpty() const noexcept {
        return count_ && root_;
    }

    AVLTree<K, V> &Clear() noexcept {
        Clear(root_);
        return *this;
    }

    AVLTree<K, V> &Insert(const K &key, const V &value) {
    }

private:
    static_assert(std::is_trivially_copy_constructible<K>::value,
                  "The given Key type must be copy-constructive.");
    static_assert(std::is_trivially_copy_constructible<V>::value,
                  "The given Value type must be copy-constructive.");

    struct TreeNode {
        K key_;
        V value_;
        TreeNode *left_;
        TreeNode *right_;
        int height_;

        TreeNode(const K &key, const V &value) noexcept
                : key_(key), value_(value), left_(nullptr), right_(nullptr), height_(0) {}
    };

    TreeNode *root_;
    SizeType count_;

    int Height(const TreeNode *&node) noexcept {
        return node ? node->height_ : -1;
    }

    void UpdateHeight(TreeNode *&node) {
        if (node) {
            UpdateHeight(node->left_);
            UpdateHeight(node->right_);
            node->height_ = 1 + max(Height(node->left_), Height(node->right_));
        }
    }

    void RotateLeft(TreeNode *&node) {
        TreeNode *temp = node;
        node = node->right_;
        temp->right_ = node->left_;
        node->left_ = temp;
        temp->height_ = 1 + max(Height(temp->left_, temp->right_));
    }

    void RotateRight(TreeNode *&node) {
        TreeNode *temp = node;
        node = node->left_;
        temp->left_ = node->right_;
        node->right_ = temp;
        temp->height_ = 1 + max(Height(temp->left_, Height(temp->right_)));
        node->height_ = 1 + max(Height(node->left_), Height(node->right_));
    }

    void Rebalance(TreeNode *&node) {
        int balance = Height(node->right_) - Height(node->left_);
        if (balance > 1) {

        } else if (balance < -1) {

        }
    }

    void Clear(TreeNode *&node) noexcept {
        if (node) {
            Clear(node->left_);
            Clear(node->right_);
            delete node;
            node = nullptr;
        }
    }

    void Insert(TreeNode *&node, const K &key, const V &value) {
        if (node) {
            if (key < node->key_) {
                Insert(node->left_, key, value);
            } else {
                Insert(node->right_, key, value);
            }
        } else {
            node = new TreeNode(key, value);
        }
        Rebalance(node);
    }

    void Remove(TreeNode *&node, const K &key) {
        if (!node) {
            return;
        } else if (node->key_ == key) {

        } else {
            if (node->key < key) {
                Remove(node->left_, key);
            } else {
                Remove(node->right_, key);
            }
        }
        Rebalance(node);
    }

};

#endif //SCLIB_AVL_TREE_H
