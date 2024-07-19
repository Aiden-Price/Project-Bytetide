/*
 ============================================================================
 Name        : merkletree.c
 ============================================================================
 */


#include "tree/merkletree.h"

/**
 * Calculate the depth of the Merkle tree based on the number of hashes.
 * @param nhashes The number of hashes in the Merkle tree.
 * @return The depth of the Merkle tree.
 */
int calculate_depth(int nhashes) {
    return (int)ceil(log2(nhashes)) + 1;
}

/**
 * Calculate the width at a given depth of the Merkle tree.
 * @param depth The depth of the Merkle tree.
 * @return The width of the Merkle tree at the given depth.
 */
int calculate_width(int depth) {
    // calculate width at a given depth
    return pow(2, depth - 1);  
}

/**
 * The create_merkle_tree function constructs a Merkle tree from a given bpkg_obj structure.
 * It calculates the required depth and width of the tree, allocates memory for the tree nodes,
 * initializes the tree with hashes and chunk hashes, links the nodes appropriately, and returns
 * the constructed Merkle tree.
 * @param obj A pointer to the bpkg_obj structure containing the hashes and chunks.
 * @return struct merkle_tree* A pointer to the dynamically allocated struct merkle_tree that
 *         represents the root of the constructed Merkle tree.
 */
struct merkle_tree* create_merkle_tree(struct bpkg_obj* obj) {
    // Calculate the depth and width of the merkle tree based on the number of hashes
    int DEPTH = calculate_depth(obj->nhashes);
    int WIDTH = calculate_width(DEPTH);
    // Allocate memory for the array of pointers to tree nodes
    struct merkle_tree_node **treearray = (struct merkle_tree_node **)malloc(DEPTH * sizeof(struct merkle_tree_node *));

    
    
    // Allocate memory for each level of the tree
    for (int i = 0; i < DEPTH; ++i) {
        treearray[i] = (struct merkle_tree_node *)malloc(WIDTH * sizeof(struct merkle_tree_node));
        if (!treearray[i]) {  // Check if allocation was successful
            perror("Failed to allocate memory for tree layer");
            // Properly free any previously allocated memory before exiting
            for (int j = 0; j < i; j++) {
                free(treearray[j]);
            }
            free(treearray);
            exit(EXIT_FAILURE);
        }
    }
    // Number of nodes at the current level
    int nodecount = 1;
    // Current level in the tree
    int treecount = 0;

     // Initialize all tree nodes to have is_leaf set to -1 (uninitialized)
    for (int i = 0; i < DEPTH; i++) {
        for (int j = 0; j < WIDTH; j++) {
            treearray[i][j].is_leaf = -1;
        }
    }

    // Load the hashes into the tree nodes
    int hashcount = 0;
    while (hashcount < obj->nhashes) {
        for (int x = 0; x < nodecount && hashcount < obj->nhashes; x++) {
            treearray[treecount][x].is_leaf = 0;
            strcpy(treearray[treecount][x].expected_hash, obj->hashes[hashcount]);
            hashcount++;
        }
        treecount++;
        // Each level has twice as many nodes as the previous one
        nodecount *= 2;
    }

    // Add the chunks to the last row of the tree
    for (int chunkcount = 0; chunkcount < obj->nchunks; chunkcount++) {
        treearray[treecount][chunkcount].is_leaf = 1;
        treearray[treecount][chunkcount].left = NULL;
        treearray[treecount][chunkcount].right = NULL;
        strcpy(treearray[treecount][chunkcount].expected_hash, obj->chunks[chunkcount].hash);
    }

    // Link the tree nodes to their left and right children
    int count;
    for (int i = 0; i < treecount; i++) {
        count = 0;
        for (int j = 0; j < WIDTH; j++) {
            if (treearray[i][j].is_leaf != -1) {
                if (treearray[i][j].is_leaf == 0) {
                    treearray[i][j].left = &treearray[i + 1][count];
                    count++;
                    treearray[i][j].right = &treearray[i + 1][count];
                    count++;
                } else {
                    treearray[i][j].left = NULL;
                    treearray[i][j].right = NULL;
                }
            }
        }
    }
       
    // Allocate memory for the merkle tree structure
    struct merkle_tree *tree = (struct merkle_tree *)malloc(sizeof(struct merkle_tree));
    
    if (!tree) {
        perror("Failed to allocate memory for merkle_tree struct");
        // Free the tree array if allocation of the tree structure fails
        for (int i = 0; i < DEPTH; i++) {
            free(treearray[i]);
        }
        free(treearray);
        exit(EXIT_FAILURE);
    }

    // Set the root of the tree to the first node in the treearray
    tree->root = &treearray[0][0];
    tree->n_nodes = count;
    tree->depth = DEPTH;
    tree->treearray = treearray;
    
    return tree;
}



/**
 * The free_merkle_tree function is responsible for deallocating all memory 
   associated with a given Merkle tree. It ensures that all nodes and the tree 
   structure itself are properly freed.
 * @param tree A pointer to the dynamically allocated struct merkle_tree that 
                represents the Merkle tree to be freed
 */
void free_merkle_tree(struct merkle_tree* tree) {
    if (tree == NULL) {
        printf("Merkle tree pointer is NULL, nothing to free.\n");
        return;
    }

    for (int i = 0; i < tree->depth; i++) {
        free(tree->treearray[i]);
    }
    free(tree->treearray);

    free(tree);
}

/**
 * The inordersearch function searches for a node with a given hash in a Merkle tree.
 * It traverses the tree in an inorder manner.
 * @param node A pointer to the current node in the Merkle tree.
 * @param h The hash to search for.
 * @param found A pointer to a pointer that will be set to the found node if it exists.
 */
void inordersearch(struct merkle_tree_node* node, char* h, struct merkle_tree_node** found) {
	if (node == NULL) {
         // Base case: reached the end of a branch 
        return; 
    }

    // Search in the left subtree
    inordersearch(node->left, h, found);  

    // Check the current node before going deeper which might give us an early success
    if (strcmp(node->expected_hash, h) == 0) {
         // Set the found node
        *found = node; 
        // Early return to prevent further unnecessary traversal
        return;  
    }

    // Search in the right subtree
    inordersearch(node->right, h, found);
    return;
}

/**
 * The inorder function traverses the Merkle tree in an inorder manner and adds
 * the hashes of leaf nodes to the given query structure.
 * @param node A pointer to the current node in the Merkle tree.
 * @param qry A pointer to the bpkg_query structure to store the hashes.
 */
void inorder(struct merkle_tree_node* node, struct bpkg_query* qry) {
    if (node == NULL) return;

    // Recursively process the left subtree
    if (node->left && (uintptr_t)node->left != 0xbebebebebebebebe) {
        inorder(node->left, qry);
    }

    // Process the current node if it is a leaf
    if (node->is_leaf) {
        if (qry->hashes == NULL) {
            // Allocate the first element if not yet allocated
            qry->hashes = malloc(sizeof(char*)); 
            if (qry->hashes == NULL) {
                perror("Unable to allocate query hash array");
                return; 
            }
        } else {
            char** new_hashes = realloc(qry->hashes, (qry->len + 1) * sizeof(char*));
            if (new_hashes == NULL) {
                perror("Unable to allocate or expand query hash array");
                return; 
            }
            qry->hashes = new_hashes;
        }

        qry->hashes[qry->len] = malloc((MAX_HASH_LEN + 1) * sizeof(char));
        if (qry->hashes[qry->len] == NULL) {
            perror("Unable to allocate query hash string");
            return; 
        }
        strcpy(qry->hashes[qry->len], node->expected_hash);
        qry->len++;
    }

    // Recursively process the right subtree
    if (node->right && (uintptr_t)node->right != 0xbebebebebebebebe) {
        inorder(node->right, qry);
    }
}





