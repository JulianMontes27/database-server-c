/* main.c */
#include "main.h"

void zero(uint8_t *ptr, uint16_t size)
{
    // Pre-condition check: Ensure the pointer is valid before attempting to dereference.
    // `assert` is used for debugging; it will terminate the program if the condition is false.
    assert(ptr != NULL && "Error: Attempted to zero NULL memory block.");

    // Use `memset` for efficient memory zeroing. `memset` is a standard library function
    // optimized for setting a block of memory to a specified byte value.
    memset(ptr, 0, size);
}

Node *create_node(Node *parent, int8_t *path)
{
    Node *node;         // Pointer to hold the address of the newly allocated Node.
    uint16_t node_size; // Stores the calculated size of the Node structure.

    // Pre-condition check: A parent node is required to create a new child node.
    assert(parent != NULL && "Error: Parent node cannot be NULL when creating a new node.");

    // Determine the exact size required for a `Node` structure.
    node_size = sizeof(struct s_node);
    // Allocate memory from the heap for the new Node.
    node = (Node *)malloc(node_size);

    // Error handling: Check if memory allocation was successful.
    if (node == NULL)
    {
        // Print a system-specific error message to standard error stream.
        perror("ERROR: Failed to allocate memory for a new Node");
        return NULL; // Return NULL to indicate allocation failure.
    }

    // Initialize the allocated memory to all zeros. This is crucial to prevent
    // undefined behavior from uninitialized pointers or data.
    zero((uint8_t *)node, node_size);

    // --- Debugging Output ---
    printf("--- Node Creation Details ---\n");
    printf("  Node structure size: %u bytes\n", node_size);
    printf("  Address of new Node in memory: %p\n", (void *)node);

    // Link the newly created node into the tree structure.
    parent->west = node;  // Link the parent's 'west' child pointer to this new node.
    node->tag = TagNode;  // Assign the appropriate tag to identify this as a Node.
    node->north = parent; // Set the new node's 'north' (parent) pointer.
    node->east = NULL;    // Initialize 'east' (pointer to first Leaf) to NULL.

    // Safely copy the provided path segment into the new node's `path` field.
    // `snprintf` is used to prevent buffer overflows by limiting the number of characters copied.
    snprintf((char *)node->path, sizeof(node->path), "%s", (char *)path);

    printf("  New Node path initialized to: '%s'\n", (char *)node->path);
    printf("-----------------------------\n");

    return node; // Return the pointer to the newly created node.
}

Leaf *find_last_linear(Node *parent)
{
    Leaf *current_leaf; // Iterator for traversing the leaf list.

    // Pre-condition check: Ensure the parent node is valid.
    assert(parent != NULL && "Error: Parent node cannot be NULL for find_last_linear.");

    // If the parent has no 'east' leaf, the list is empty.
    if (parent->east == NULL)
    {
        // Set errno to NoError (0) and return NULL, indicating no leaf was found but no error occurred.
        // Using the reterr macro to handle error setting and return.
        reterr(NoError);
    }

    // Start traversal from the first leaf linked to the parent.
    current_leaf = parent->east;

    // Iterate through the linked list of leaves until the last one is found.
    // The loop continues as long as `current_leaf->east` is not NULL, meaning there's a next leaf.
    while (current_leaf->east != NULL)
    {
        // Assert that the current_leaf pointer is valid before moving to the next.
        assert(current_leaf != NULL && "Error: NULL leaf pointer encountered during linear search.");
        current_leaf = current_leaf->east; // Move to the next leaf.
    }

    return current_leaf; // Return the last leaf found.
}

Leaf *create_leaf(Node *parent, uint8_t *key, uint16_t count, uint8_t value)
{
    Leaf *leaf, *new_leaf;
    uint16_t size;

    // Pre-condition check: The 'west' link (parent/sibling) must be valid.
    assert(parent != NULL && "Error: 'parent' link cannot be NULL when creating a new leaf.");

    leaf = find_last(parent);
    size = sizeof(struct s_leaf);
    new_leaf = (Leaf *)malloc(size);
    assert(new_leaf);
    if (!leaf)
    {
        // directly connected to the node
        (*parent).east = new_leaf;
    }
    else
    {
        // leaf is a leaf
        (*leaf).east = new_leaf;
    }

    zero((uint8_t *)new_leaf, size);
    new_leaf->tag = TagLeaf;

    // Set the west pointer - this needs to handle the union type properly
    if (!leaf)
    {
        // First leaf points back to parent node
        new_leaf->west = (Tree *)parent;
    }
    else
    {
        // Subsequent leaves point to previous leaf
        // Need to cast leaf to Tree* to match the expected type
        new_leaf->west = (Tree *)leaf;
    }

    strncpy((char *)new_leaf->key, (char *)key, 127);

    new_leaf->value = (uint8_t *)malloc(count);
    zero(new_leaf->value, count);
    assert(new_leaf->value);

    strncpy((char *)new_leaf->value, (char *)value, count);
    new_leaf->size = count;

    return leaf; // Return the pointer to the newly created leaf.
}

/**
 * @brief The global root of the in-memory database tree.
 *
 * This `Tree` union variable serves as the entry point to the entire database
 * structure. It is initialized as a `Node` and tagged as `TagRoot`.
 */
Tree root;

int main(int argc, const char *argv[])
{
    // Suppress unused parameter warnings for `argc` and `argv`.
    // This is a common practice when these parameters are not used in the function body.
    (void)argc;
    (void)argv;

    Node *newNode = NULL;  // Pointer to hold a newly created node.
    Leaf *newLeaf = NULL;  // Pointer to hold a newly created leaf.
    Node *rootNodeAddress; // Pointer to the Node part of the global `root` Tree union.

    // Get the address of the `node` member within the `root` union.
    rootNodeAddress = (Node *)&root;

    // --- Initialize the Database Root ---
    // The `root` is treated as a `Node` type for structural purposes,
    // serving as the top-level container for the database.
    printf("\n--- Initializing Database Root ---\n");
    root.node.north = NULL;   // The root has no parent.
    root.node.west = NULL;    // Initially, no child nodes or sub-paths.
    root.node.east = NULL;    // Initially, no child leaves directly under root.
    root.node.tag = TagRoot;  // Set the tag to explicitly identify this as the root.
    root.node.path[0] = '\0'; // Initialize the root's path to an empty string.

    printf("  Root tag: %d\n", root.node.tag);
    printf("  Root tree address: %p\n", (void *)&root);
    printf("----------------------------------\n\n");

    // --- Demonstrate Node Creation ---
    printf("Attempting to create a new Node under the root...\n");
    // Create a new node under the root.
    // The path for the new node is currently an empty string (copied from root.node.path).
    newNode = create_node(rootNodeAddress, (int8_t *)root.node.path);

    // Check if the node creation was successful.
    if (newNode == NULL)
    {
        fprintf(stderr, "FATAL ERROR: Failed to create initial node. Exiting.\n");
        return 1; // Exit with an error code if critical allocation fails.
    }

    printf("Successfully created a new Node under root.\n\n");

    // --- Demonstrate Leaf Creation ---
    printf("Attempting to create a new Leaf linked to the new Node...\n");
    // Create a new leaf and link it to the newly created node.
    // The 'west' parameter for create_leaf expects a Tree*, so we pass the newNode cast to Tree*.
    // Provide a sample key and size for the leaf.
    newLeaf = create_leaf((Node *)newNode, (uint8_t *)"sample_key", 10, 200 ); // Example key and size

    // Check if the leaf creation was successful.
    if (newLeaf == NULL)
    {
        fprintf(stderr, "FATAL ERROR: Failed to create initial leaf. Exiting.\n");
        // Remember to free previously allocated memory if an error occurs.
        free(newNode);
        newNode = NULL;
        return 1;
    }

    printf("Successfully created a new Leaf.\n\n");

    // --- Cleanup: Free Allocated Memory ---
    // In a full database server, memory management would be more sophisticated
    // (e.g., a garbage collector or a dedicated memory pool).
    // For this simple example, we explicitly free the allocated nodes/leaves.
    printf("--- Cleaning up allocated memory ---\n");
    if (newLeaf != NULL)
    {
        printf("  Freeing newLeaf at %p\n", (void *)newLeaf);
        free(newLeaf);
        newLeaf = NULL;
    }
    if (newNode != NULL)
    {
        printf("  Freeing newNode at %p\n", (void *)newNode);
        free(newNode);
        newNode = NULL;
    }
    printf("------------------------------------\n");

    return 0; // Program executed successfully.
}