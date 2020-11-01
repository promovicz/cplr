#ifndef CPKG_H
#define CPKG_H

#include <stdbool.h>

extern bool cpkg_exists(const char *name, bool verbose);

extern char *cpkg_retrieve(const char *name, const char *what, bool verbose);

#endif /*!CPKG_H */
