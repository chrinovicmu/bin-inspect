#ifndef UTILS_H
#define UTILS_H


#define BFD_ERROR(msg) \
    do { \
        std::cerr << msg << ": " << bfd_errmsg(bfd_get_error()) << "\n"; \
    } while(0)

#define BFD_FAIL(format, fname) \
    do { \
        BFD_ERROR(format, fname); \
        return NULL; \
    } while(0)

#endif // !UTILS_H
