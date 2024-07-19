/*
 ============================================================================
 Name        : merkletree.h
 ============================================================================
 */


#ifndef MERKLE_TREE_H
#define MERKLE_TREE_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "crypt/sha256.h"
#include "chk/pkgchk.h"
#include <math.h>

#define SHA256_HEXLEN (64)

/**
 * Structure representing a node in the Merkle tree.
 * Each node contains:
 * - key: A pointer to the key associated with the node (optional).
 * - value: A pointer to the value associated with the node (optional).
 * - left: A pointer to the left child node.
 * - right: A pointer to the right child node.
 * - is_leaf: An integer indicating if the node is a leaf (1 if true, 0 if false).
 * - expected_hash: The expected hash value for the node.
 * - computed_hash: The computed hash value for the node.
 */
struct merkle_tree_node {
    void* key;
    void* value;
    struct merkle_tree_node* left;
    struct merkle_tree_node* right;
    int is_leaf;
    char expected_hash[SHA256_HEXLEN];
    char computed_hash[SHA256_HEXLEN];
};

/**
 * Structure representing a Merkle tree.
 * The Merkle tree contains:
 * - root: A pointer to the root node of the tree.
 * - treearray: A pointer to the array of tree nodes.
 * - n_nodes: The total number of nodes in the tree.
 * - depth: The depth of the tree.
 */
struct merkle_tree {
    struct merkle_tree_node* root;
    struct merkle_tree_node** treearray;
    size_t n_nodes;
    int depth;
};


/**
 * The create_merkle_tree function constructs a Merkle tree from a 
 * given bpkg_obj structure. It calculates the required depth and 
 * width of the tree, allocates memory for the tree nodes, initializes 
 * the tree with hashes and chunk hashes, links the nodes appropriately, 
 * and returns the constructed Merkle tree.
 * @param obj A pointer to the bpkg_obj structure containing the data for the Merkle tree.
 * @return struct merkle_tree* A pointer to a dynamically allocated struct 
 *         merkle_tree that represents the root of the constructed Merkle tree.
 *         - Root Node: tree->root points to the root node of the Merkle tree, 
 *           which is the first node in the treearray.
 *         - Number of Nodes: tree->n_nodes holds the total number of nodes in 
 *           the tree.
 *         - Depth: tree->depth represents the depth of the tree.
 */
struct merkle_tree* create_merkle_tree(struct bpkg_obj* obj);

/**
 * The free_merkle_tree function is responsible for deallocating all memory 
 * associated with a given Merkle tree. It ensures that all nodes and the tree 
 * structure itself are properly freed.
 * @param tree A pointer to the dynamically allocated struct merkle_tree that 
 *             represents the Merkle tree to be freed.
 */
void free_merkle_tree(struct merkle_tree* tree);

/**
 * The inordersearch function performs an inorder search on the Merkle tree 
 * to find a node with a specific hash.
 * @param node A pointer to the current node in the Merkle tree.
 * @param h The hash value to search for.
 * @param found A pointer to the found node, or NULL if not found.
 */
void inordersearch(struct merkle_tree_node* node, char* h, struct merkle_tree_node** found);

/**
 * The inorder function performs an inorder traversal of the Merkle tree, 
 * collecting hashes of leaf nodes into a bpkg_query structure.
 * @param node A pointer to the current node in the Merkle tree.
 * @param qry A pointer to the bpkg_query structure where the hashes will be stored.
 */
void inorder(struct merkle_tree_node* node, struct bpkg_query* qry);
#endif