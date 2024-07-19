/*
 ============================================================================
 Name        : package.c
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chk/pkgchk.h"
#include "package.h"

/**
 * Adds a package to the package list by reading package details from a file.
 *
 * @param pkgList Pointer to the list of packages.
 * @param directory The directory where the package files are located.
 * @param filename The filename of the package to be added.
 */
void addPackage(PackageList *pkgList, char *directory, char* filename) {
    // Construct the full path to the package file
    char fullpath[1025];
    strcpy(fullpath, directory);
    strcat(fullpath, "/");
    strcat(fullpath, filename);

    // Ensure the fullpath is not NULL
    if (fullpath == NULL) {
        puts("Missing file argument.");
        return;
    }

    // Open the file to read package details
    FILE* file = fopen(fullpath, "r");
    if (!file) {
        printf("Cannot open file %s.\n", fullpath);
        return;
    }   
    
    char buffer[2048];
    char identifier[1025]; 
    char nameoffile[257];
    // Read the first line and store the ident value in obj->ident
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        printf("Unable to read filename from file %s.\n", filename);
        fclose(file);
        return;
    } else {
        sscanf(buffer, "ident: %1024s", identifier);
    }

    // Read the second line and store the filename value in obj->filename
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        printf("Unable to read identifier from file %s.\n", filename);
        fclose(file);
        return;
    } else {
        sscanf(buffer, "filename: %256s", nameoffile);
    }

    fclose(file);

    // Buffer to hold up to 32 characters + null terminator
    char trimmedIdentifier[33];  
    strncpy(trimmedIdentifier, identifier, 32);
    // Ensure null termination
    trimmedIdentifier[32] = '\0';  

    pkgList->packages = realloc(pkgList->packages, (pkgList->count + 1) * sizeof(Package));
    if (!pkgList->packages) {
        puts("Failed to allocate memory for packages");
        return;
    }

    char fullname[1025];
    strcpy(fullname, directory);
    strcat(fullname, "/");
    strcat(fullname, nameoffile);

    strcpy(pkgList->packages[pkgList->count].identifier, trimmedIdentifier);
    strcpy(pkgList->packages[pkgList->count].filename, fullname);
    pkgList->count++;
}

/**
 * Removes a package from the list based on its identifier.
 *
 * @param pkgList Pointer to the list of packages.
 * @param ident The identifier of the package to remove.
 */
void removePackage(PackageList *pkgList, char* ident) {
    if (!ident) {
        puts("Missing identifier argument, please specify whole 1024 character or at least 20 characters.");
        return;
    }

    // Loop through all packages in the list to find the matching identifier
    for (int i = 0; i < pkgList->count; i++) {
        if (strncmp(pkgList->packages[i].identifier, ident, 20) == 0) {
            // printf("Removing package with identifier: %.*s\n", 20, pkgList->packages[i].identifier);

            // Reduce the package count first
            pkgList->count--;

            // If not the last item, shift the rest of the elements left
            if (i < pkgList->count) {
                memmove(&pkgList->packages[i], &pkgList->packages[i + 1], (pkgList->count - i) * sizeof(Package));
            }

            // Attempt to reduce the array size by reallocating
            if (pkgList->count > 0) {
                Package* temp = realloc(pkgList->packages, pkgList->count * sizeof(Package));
                if (temp) {
                    pkgList->packages = temp; 
                } else {
                    puts("Memory reallocation failed, potential memory waste.");
                }
            } else {
                // If no packages are left, free the array
                free(pkgList->packages);
                pkgList->packages = NULL;
            }

            puts("Package has been removed");
            return;
        }
    }

    printf("Identifier provided does not match managed packages");
}

/**
 * Lists all packages managed in the package list.
 *
 * @param pkgList Pointer to the list of packages.
 */
void listPackages(PackageList *pkgList) {
    if (pkgList->count == 0) {
        puts("No packages managed");
        return;
    }

    for (int i = 0; i < pkgList->count; i++) {
        printf("%d. %s, %s : %s\n", i + 1, pkgList->packages[i].identifier, pkgList->packages[i].filename, "INCOMPLETE");  
    }
}

/**
 * Cleans up the memory used by the package list.
 *
 * @param pkgList Pointer to the list of packages.
 */
void cleanupPackages(PackageList *pkgList) {
    free(pkgList->packages);
    pkgList->packages = NULL;
    pkgList->count = 0;
}
