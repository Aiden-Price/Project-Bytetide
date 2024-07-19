/*
 ============================================================================
 Name        : package.h
 Author      : 530502821
 ============================================================================
 */
#ifndef PACKAGE_H
#define PACKAGE_H

typedef struct {
    char identifier[33];  
    char filename[256];   
} Package;

typedef struct {
    Package* packages;  // Dynamic array of packages
    int count;          // Number of packages
} PackageList;

/**
 * Adds a package to the package list by reading package details from a file.
 *
 * @param pkgList Pointer to the list of packages.
 * @param directory The directory where the package files are located.
 * @param filename The filename of the package to be added.
 */
void addPackage(PackageList *pkgList, char *directory, char* filename);

/**
 * Removes a package from the list based on its identifier.
 *
 * @param pkgList Pointer to the list of packages.
 * @param ident The identifier of the package to remove.
 */
void removePackage(PackageList *pkgList, char* ident);

/**
 * Lists all packages managed in the package list.
 *
 * @param pkgList Pointer to the list of packages.
 */
void listPackages(PackageList *pkgList);

/**
 * Cleans up the memory used by the package list.
 *
 * @param pkgList Pointer to the list of packages.
 */
void cleanupPackages(PackageList *pkgList);

#endif // PACKAGE_H