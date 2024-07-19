/*
 ============================================================================
 Name        : config.h
 ============================================================================
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

typedef struct {
    char directory[256];
    int max_peers;
    uint16_t port;
} Config;

/**
 * Ensures that a directory exists at the specified path.
 * If the directory does not exist, it attempts to create it.
 * 
 * @param dir_path Path of the directory to check or create.
 * @return int Returns 0 on success, or 3 if an error occurs.
 */
int ensure_directory(const char *dir_path);

/**
 * Parses a configuration file to extract configuration settings.
 * The configuration settings include directory path, maximum peers, and port number.
 * 
 * @param filename Path to the configuration file.
 * @param config Pointer to the Config structure to store the parsed data.
 * @return int Returns 0 on successful parsing, various error codes on failure.
 */
int parse_config(const char *filename, Config *config);

#endif // CONFIG_H