#ifndef MAIN_H
#define MAIN_H

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // Provides standard integer types
#include <assert.h>
#include <errno.h>

/* Tag definitions */
#define TagRoot 1
#define TagNode 2
#define TagLeaf 3

/**
 * Type definitions for clarity and portability
 * Using standard types from <stdint.h>
 */
typedef unsigned char Tag;

// Forward declarations for structs, necessary because they might refer to each other
// or to the Tree union.
struct s_node;
struct s_leaf;
union u_tree; // Forward declare the union as well

/**
 * Define the Leaf structure first.
 * It uses 'Tree *west', so 'Tree' needs to be forward-declared or fully defined.
 * Since Tree is a union of Node and Leaf, we'll forward declare the union.
 */
struct s_leaf {
    union u_tree *west;  // Pointer to a Tree (could be a Node or Leaf)
    struct s_leaf *east; // Pointer to another Leaf (sibling)
    int8_t key[128];     // Key data
    int8_t *value;       // Pointer to value data
    int16_t size;        // Size of the value
    Tag tag;             // Tag for this leaf
};
typedef struct s_leaf Leaf;

/**
 * Define the Node structure.
 * It uses pointers to s_node and s_leaf, which are now either fully defined (s_leaf)
 * or forward-declared (s_node for recursive pointers).
 */
struct s_node {
    struct s_node *north; // Pointer to parent Node
    struct s_node *west;  // Pointer to child Node
    struct s_leaf *east;  // Pointer to child Leaf
    uint8_t path[256];     // Path data
    Tag tag;              // Tag for this node
};
typedef struct s_node Node;

/**
 * Now define the union u_tree, as Node and Leaf are now fully defined.
 * The members 'node' and 'leaf' are direct struct members, not pointers.
 */
union u_tree {
    Node node; // Node is now a complete type
    Leaf leaf; // Leaf is now a complete type
};
typedef union u_tree Tree;

/**
 * Function declarations/prototype
 */
Node *create_node(Node *parent, int8_t *path);
int main(int argc, const char *argv[]);

#endif /* MAIN_H */
