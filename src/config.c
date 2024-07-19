/*
 ============================================================================
 Name        : config.c
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <errno.h>

#define MAX_PATH_LENGTH 256
#include "config.h"

/**
 * Ensures that a directory exists at the specified path.
 * If the directory does not exist, it attempts to create it.
 * 
 * @param dir_path Path of the directory to check or create.
 * @return int Returns 0 on success, or 3 if an error occurs.
 */
int ensure_directory(const char *dir_path) {
    struct stat st = {0};
    // Check if the directory exists
    if (stat(dir_path, &st) == -1) {
        // Directory does not exist, try to create it
        if (mkdir(dir_path, 0700) != 0) {
            perror("Failed to create directory");
            // Unable to create directory
            return 3; 
        }
    } else if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Provided path is not a directory\n");
        // Path exists but is not a directory
        return 3; 
    }
    return 0; 
}

/**
 * Parses a configuration file to extract configuration settings.
 * The configuration settings include directory path, maximum peers, and port number.
 * 
 * @param filename Path to the configuration file.
 * @param config Pointer to the Config structure to store the parsed data.
 * @return int Returns 0 on successful parsing, various error codes on failure.
 */
int parse_config(const char *filename, Config *config) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening configuration file");
        // File opening failed
        return 1; 
    }

    // Buffer to store lines read from the file
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        // Buffers to store key and value pairs
        char key[50], value[256];
        // Parse the line for a key:value pair
        if (sscanf(line, "%49[^:]:%255s", key, value) == 2) {
            if (strcmp(key, "directory") == 0) {
                strncpy(config->directory, value, sizeof(config->directory) - 1);
            } else if (strcmp(key, "max_peers") == 0) {
                // Parse the maximum number of peers and validate
                config->max_peers = atoi(value);
                if (config->max_peers < 1 || config->max_peers > 2048) {
                    fclose(file);
                    // Invalid max_peers value
                    return 4; 
                }
            } else if (strcmp(key, "port") == 0) {
                // Parse the port number and validate
                long port_val = strtol(value, NULL, 10);
                if (port_val < 1024 || port_val > 65535) {
                    fclose(file);
                    // Invalid port value
                    return 5; 
                }
                config->port = (uint16_t)port_val;
            }
        }
    }
    fclose(file);

    return 0; 
}