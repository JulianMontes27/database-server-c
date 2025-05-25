/* main.c */
#include "main.h"

// Helper function to zero out a block of memory.
// It uses memset, which is the standard and most efficient way to fill a memory block with a constant byte.
// Parameters:
//   ptr: A pointer to the block of memory to fill.
//   size: The number of bytes to be set to zero.
void zero(uint8_t *ptr, uint16_t size)
{
    // Use assert to ensure the pointer is not NULL. This helps catch programming errors early.
    assert(ptr != NULL);
    // memset sets the first 'size' bytes of the block of memory pointed to by 'ptr' to zero.
    memset(ptr, 0, size);
}

// Function to create a new Node in the tree structure.
// Parameters:
//   parent: A pointer to the parent Node under which the new node will be created.
//   path: A pointer to a character array (string) representing the path for the new node.
// Returns:
//   A pointer to the newly created Node, or NULL if memory allocation fails.
Node *create_node(Node *parent, int8_t *path)
{
    Node *node;         // Pointer to the new Node
    uint16_t node_size; // Size of the Node structure

    // Assert that the parent pointer is not NULL. A new node must have a parent.
    assert(parent);

    // Calculate the size of the Node structure for memory allocation.
    node_size = sizeof(struct s_node);
    // Allocate memory for the new Node.
    node = (Node *)malloc(node_size);
    // Check if malloc failed (returned NULL).
    if (node == NULL)
    {
        // Print an error message to stderr if memory allocation fails.
        perror("Failed to allocate memory for node");
        return NULL; // Return NULL to indicate failure.
    }

    // Zero out the newly allocated memory for the Node structure.
    // We cast 'node' (which is Node*) to 'uint8_t*' because 'zero' expects a byte pointer.
    zero((uint8_t *)node, node_size);

    // Print information about the newly created node.
    // Using %u for unsigned integer (node_size) and %p for pointer addresses.
    printf("Node structure size: %u bytes\n", node_size);
    printf("Address of new Node in memory: %p\n", (void *)node); // Cast to void* for %p

    // Link the new node to its parent.
    parent->west = node;  // Set the parent's 'west' child pointer to the new node.
    node->tag = TagNode;  // Set the tag of the new node to TagNode.
    node->north = parent; // Set the new node's 'north' (parent) pointer.
    node->east = NULL;    // Initialize 'east' (leaf child) to NULL.

    // Safely copy the provided path into the new node's path field.
    // snprintf ensures that the string is null-terminated and prevents buffer overflows.
    // Cast to (char *) for string functions as path is int8_t[].
    snprintf((char *)node->path, sizeof(node->path), "%s", (char *)path);

    return node; // Return the pointer to the newly created node.
}

// Declare the root of the tree globally.
// This variable will hold the top-level structure of your in-memory database.
Tree root;

// Main function, the entry point of the program.
int main(int argc, const char *argv[])
{
    // Suppress unused parameter warnings for 'argc' and 'argv'.
    // This is a common practice when these parameters are not used in the function body.
    (void)argc;
    (void)argv;

    Node *node;                  // Pointer to hold a newly created node.
    Node *rootAddress;           // Pointer to the Node part of the root Tree union.
    rootAddress = (Node *)&root; // Get the address of the 'node' member within the 'root' union.

    // Initialize the root Tree.
    // The root is treated as a Node type for structural purposes.
    root.node.north = NULL;  // The root has no parent.
    root.node.west = NULL;   // Initially, no child nodes.
    root.node.east = NULL;   // Initially, no child leaves.
    root.node.tag = TagRoot; // Set the tag to indicate this is the root.

    // Initialize the path for the root node.
    // For the root, the path is typically empty.
    root.node.path[0] = '\0'; // Null-terminate the path string.

    // Print initial information about the root.
    printf("Root tag: %d\n", root.node.tag);
    printf("Root tree address: %p\n", (void *)&root); // Cast to void* for %p

    // Create a new node under the root.
    // We explicitly cast 'root.node.path' to 'int8_t *' to match the function signature
    // and resolve any potential signedness warnings from the compiler.
    node = create_node(rootAddress, (int8_t *)root.node.path);

    // Check if the node creation was successful.
    if (node == NULL)
    {
        fprintf(stderr, "Error: Failed to create node.\n");
        return 1; // Exit with an error code.
    }

    // Print information about the newly created node.
    printf("Successfully created a new node under root.\n");
    printf("New node tag: %d\n", node->tag);
    printf("New node north (parent) address: %p\n", (void *)node->north);
    printf("New node path: '%s'\n", (char *)node->path); // Print the path as a string

    // IMPORTANT: Free the memory allocated for 'node'.
    // In a full database server, memory management would be more sophisticated,
    // but for this example, freeing it here prevents a simple memory leak.
    free(node);
    node = NULL; // Set the pointer to NULL after freeing to avoid dangling pointers.

    return 0; // Exit successfully.
}