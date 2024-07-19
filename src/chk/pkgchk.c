/*
 ============================================================================
 Name        : pkgchk.c
 ============================================================================
 */

#include "chk/pkgchk.h"
#include "tree/merkletree.h"
// PART 1

/**
 * Loads the package for when a value path is given.
 * @param path The path to the package file.
 * @return struct bpkg_obj* A pointer to the dynamically allocated package object.
 */
struct bpkg_obj* bpkg_load(const char* path) {
    // Open the file for reading
    FILE* file = fopen(path, "r");
    if (!file) {
        // Print error if the file cannot be opened
        perror("Unable to open the file");
        return NULL;
    }
    
    // Allocate memory for the bpkg_obj structure
    struct bpkg_obj* obj = malloc(sizeof(struct bpkg_obj));
    if (!obj) {
        perror("Unable to complete memory allocation");
        fclose(file);
        return NULL;
    }

    char buffer[2048];
    // Read the first line and store the ident value in obj->ident
    if (fgets(buffer, sizeof(buffer), file) == NULL) goto error;
    sscanf(buffer, "ident: %1024s", obj->ident);

    // Read the second line and store the filename value in obj->filename
    if (fgets(buffer, sizeof(buffer), file) == NULL) goto error;
    sscanf(buffer, "filename: %256s", obj->filename);
    
    // Read the third line and store the size value in obj->size
    if (fgets(buffer, sizeof(buffer), file) == NULL) goto error;
    sscanf(buffer, "size: %u", &obj->size);

    // Read the fourth line and store the nhashes value in obj->nhashes
    if (fgets(buffer, sizeof(buffer), file) == NULL) goto error;
    sscanf(buffer, "nhashes: %u", &obj->nhashes);
    
    // Allocate memory for the array of hash pointers
    obj->hashes = calloc(obj->nhashes, sizeof(char *));  
    if (!obj->hashes) goto error;

    // Read and store each hash value
    if (fgets(buffer, sizeof(buffer), file) == NULL) goto error;
    for (uint32_t i = 0; i < obj->nhashes; i++) {
        if (fgets(buffer, sizeof(buffer), file) == NULL) goto error;

        // Allocate memory for each hash string
        obj->hashes[i] = malloc((MAX_HASH_LEN + 1) * sizeof(char));  
        if (!obj->hashes[i]) goto error;

        // Read the hash into the allocated memory
        sscanf(buffer, "%64s", obj->hashes[i]);  
    }

    // Read the nchunks value
    if (fgets(buffer, sizeof(buffer), file) == NULL) goto error;
    sscanf(buffer, "nchunks: %u", &obj->nchunks);

    if (fgets(buffer, sizeof(buffer), file) == NULL) goto error;
    // Allocate memory for the chunks array
    obj->chunks = malloc(obj->nchunks * sizeof(struct chunk));
    if (!obj->chunks) goto error;
    // Read and store each chunk's information
    for (uint32_t i = 0; i < obj->nchunks; i++) {
        if (fgets(buffer, sizeof(buffer), file) == NULL) goto error;
        sscanf(buffer, "%64s,%u,%u", obj->chunks[i].hash, &obj->chunks[i].offset, 
            &obj->chunks[i].size);
    }

    // Close the file and return the populated object
    fclose(file);
    return obj;

error:
    // Handle errors by cleaning up allocated resources
    fclose(file);
    if (obj) {
        if (obj->hashes) {
            for (uint32_t i = 0; i < obj->nhashes; i++) {
                free(obj->hashes[i]);
            }
            free(obj->hashes);
        }
        if (obj->chunks) {
            free(obj->chunks);
        }
        free(obj);
    }
    return NULL;
}



/**
 * Checks to see if the referenced filename in the bpkg file
 * exists or not.
 * @param bpkg, constructed bpkg object
 * @return query_result, a single string should be
 *      printable in hashes with len sized to 1.
 * 		If the file exists, hashes[0] should contain "File Exists"
 *		If the file does not exist, hashes[0] should contain "File Created"
 */
struct bpkg_query bpkg_file_check(struct bpkg_obj* bpkg){
    // Initialize a bpkg_query structure with zero values
    struct bpkg_query qry = { 0 };
    qry.len = 1;
    // Allocate memory for one hash pointer
    qry.hashes = malloc(sizeof(char*));
    qry.hashes[0] = malloc(MAX_HASH_LEN + 1);
    
    if (qry.hashes == NULL) {
        // If memory allocation fails, print error and exit
        perror("Failed to allocate memory for hashes");
        exit(EXIT_FAILURE);
    }
    
    // Allocate memory for the hash entry
    if (qry.hashes[0] == NULL) {
        // If memory allocation fails, print error and exit
        perror("Failed to allocate memory for hash entry");
        exit(EXIT_FAILURE);
    }
    
    // Attempt to open the file specified by bpkg->filename for reading
    FILE *file = fopen(bpkg->filename, "r");
    if (file) {
        // If the file can be opened, it exists
        // Set qry.hashes[0] to "File Exists" and close the file
        strcpy(qry.hashes[0], "File Exists");
        fclose(file);
    } else {
        file = fopen(bpkg->filename, "w");
        if (file) {
            // If the file can be created, set qry.hashes[0] to "File Created"
            strcpy(qry.hashes[0], "File Created");
            fclose(file);
        } else {
            // If the file cannot be created, set qry.hashes[0] to "File Error"
            strcpy(qry.hashes[0], "File Error");
        }
    }
    // Return the query structure with the result
    return qry;
}


/**
 * Retrieves a list of all hashes within the package/tree
 * @param bpkg, constructed bpkg object
 * @return query_result, This structure will contain a list of hashes
 * 		and the number of hashes that have been retrieved
 */
struct bpkg_query bpkg_get_all_hashes(struct bpkg_obj* bpkg) {
    struct bpkg_query qry = {0};
    // Setting the qry.len value and allocating memory for the hashes
    qry.len = bpkg->nhashes + bpkg->nchunks;
    // Allocate memory for the array of hash pointers
    qry.hashes = malloc(qry.len * sizeof(char*));

    if (qry.hashes == NULL) {
        // If memory allocation fails, print error and exit
        perror("Failed to allocate memory for hashes");
        exit(EXIT_FAILURE);
    }

    // Looping and setting the hashes and chunks to associated qry.hashes values
    for (int i = 0; i < qry.len; i++) {
        // Allocate memory for each hash entry
        qry.hashes[i] = malloc((MAX_HASH_LEN + 1) * sizeof(char));
        if (qry.hashes[i] == NULL) {
            // If memory allocation fails, print error and exit
            perror("Failed to allocate memory for hash entry");
            exit(EXIT_FAILURE);
        }
        if (i < bpkg->nhashes) {
            // Copy hash from bpkg->hashes if index is less than number of hashes
            strcpy(qry.hashes[i], bpkg->hashes[i]);
        } else {
            // Copy hash from bpkg->chunks if index is greater or equal to number 
            // of hashes
            strcpy(qry.hashes[i], bpkg->chunks[i - bpkg->nhashes].hash);
        }
    }
    // Return the qry structure with the combined hashes
    return qry;
}

/**
 * Retrieves all completed chunks of a package object
 * @param bpkg, constructed bpkg object
 * @return query_result, This structure will contain a list of hashes
 * 		and the number of hashes that have been retrieved
 */
struct bpkg_query bpkg_get_completed_chunks(struct bpkg_obj* bpkg) { 
    struct bpkg_query qry = {0};

    // First, determine the number of completed chunks
    for (uint32_t i = 0; i < bpkg->nchunks; i++) {
        if (bpkg->chunks[i].size > 0) {
            qry.len++;
        }
    }

    // Allocate memory for the hashes array
    qry.hashes = malloc(qry.len * sizeof(char*));
    if (qry.hashes == NULL) {
        perror("Failed to allocate memory for hashes");
        exit(EXIT_FAILURE);
    }

    // Populate the hashes array with the hashes of completed chunks
    int hash_index = 0;
    for (uint32_t i = 0; i < bpkg->nchunks; i++) {
        if (bpkg->chunks[i].size > 0) {
            qry.hashes[hash_index] = malloc((MAX_HASH_LEN + 1) * sizeof(char));
            if (qry.hashes[hash_index] == NULL) {
                perror("Failed to allocate memory for hash entry");
                exit(EXIT_FAILURE);
            }
            strcpy(qry.hashes[hash_index], bpkg->chunks[i].hash);
            hash_index++;
        }
    }
    return qry;
}


/**
 * Gets the mininum of hashes to represented the current completion state
 * Example: If chunks representing start to mid have been completed but
 * 	mid to end have not been, then we will have (N_CHUNKS/2) + 1 hashes
 * 	outputted
 *
 * @param bpkg, constructed bpkg object
 * @return query_result, This structure will contain a list of hashes
 * 		and the number of hashes that have been retrieved
 */
struct bpkg_query bpkg_get_min_completed_hashes(struct bpkg_obj* bpkg) {
    struct bpkg_query qry = { 0 };
    qry.len = 1;
    qry.hashes = malloc(qry.len * sizeof(char*));


    if (qry.hashes == NULL) {
        // If memory allocation fails, print error and exit
        perror("Failed to allocate memory for hashes");
        exit(EXIT_FAILURE);
    }

    // Looping and setting the hashes and chunks to associated qry.hashes values
    for (int i = 0; i < qry.len; i++) {
        // Allocate memory for each hash entry
        qry.hashes[i] = malloc((MAX_HASH_LEN + 1) * sizeof(char));
        if (qry.hashes[i] == NULL) {
            // If memory allocation fails, print error and exit
            perror("Failed to allocate memory for hash entry");
            exit(EXIT_FAILURE);
        }
        if (i < bpkg->nhashes) {
            // Copy hash from bpkg->hashes if index is less than number of hashes
            strcpy(qry.hashes[i], bpkg->hashes[i]);
        }
    }
    return qry;
}


/**
 * Retrieves all chunk hashes given a certain an ancestor hash (or itself)
 * Example: If the root hash was given, all chunk hashes will be outputted
 * 	If the root's left child hash was given, all chunks corresponding to
 * 	the first half of the file will be outputted
 * 	If the root's right child hash was given, all chunks corresponding to
 * 	the second half of the file will be outputted
 * @param bpkg, constructed bpkg object
 * @return query_result, This structure will contain a list of hashes
 * 		and the number of hashes that have been retrieved
 */
struct bpkg_query bpkg_get_all_chunk_hashes_from_hash(struct bpkg_obj* bpkg, 
    char* hash) {
        struct bpkg_query qry = { 0 };
        qry.len = 0;

        qry.hashes = (char**)malloc(sizeof(char*));
        if (qry.hashes == NULL) {
            puts("Unable to allocate query hash");
            exit(1);
        }

        struct merkle_tree* newtree = create_merkle_tree(bpkg);
        if (newtree == NULL) {
            puts("Unable to load tree");
            exit(1);
        }

        // traverse tree until the node containing the hash is found
        struct merkle_tree_node* found_node = NULL;
        inordersearch(newtree->root, hash, &found_node);
        if (found_node != NULL) {
            // printf("Found node:%s\n", found_node->expected_hash);
        } else {
            printf("Node not found\n");
            free_merkle_tree(newtree);
            free(qry.hashes);
            exit(1);
        }

        // Allocate sufficient memory to hold all potential hashes (if known, else estimate or grow dynamically)
        // Resize to hold all hashes
        qry.hashes = realloc(qry.hashes, newtree->n_nodes * sizeof(char*)); 
        if (qry.hashes == NULL) {
            puts("Unable to reallocate query hashes");
            free_merkle_tree(newtree);
            exit(1);
        }

        // traverse the tree from this node - if it is a leaf node then add to hash array and increase count
        inorder(found_node, &qry);
        free_merkle_tree(newtree);
        return qry;
    }




/**
 * Deallocates the query result after it has been constructed from
 * the relevant queries above.
 */
void bpkg_query_destroy(struct bpkg_query* qry) {
    //TODO: Deallocate here!
    if (qry && qry->hashes) {
        for (int i = 0; i < qry->len; i++) {
            free(qry->hashes[i]);
        }
        free(qry->hashes);
        // Ensure pointer is reset after freeing
        qry->hashes = NULL; 
    }
}

/**
 * Deallocates memory at the end of the program,
 * make sure it has been completely deallocated
 */
void bpkg_obj_destroy(struct bpkg_obj* obj) {
    //TODO: Deallocate here!
    if (obj) {
        for (uint32_t i = 0; i < obj->nhashes; i++) {
            free(obj->hashes[i]);
        }
        free(obj->hashes);
        free(obj->chunks);
        free(obj);
    }

}



