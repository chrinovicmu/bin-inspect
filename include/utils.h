
#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <bfd.h>

#define BFD_ERROR(msg) \
    do { \
        std::cerr << (msg) << ": " << bfd_errmsg(bfd_get_error()) << "\n"; \
    } while (0)

#define BFD_ERROR_FILE(format, fname) \
    do { \
        std::cerr << (format) << " (" << (fname) << "): " \
                  << bfd_errmsg(bfd_get_error()) << "\n"; \
    } while (0)

#define BFD_FAIL(format, fname) \
    do { \
        BFD_ERROR_FILE(format, fname); \
        return NULL; \
    } while (0)

#endif // !UTILS_H
