#include <stdbool.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <gc/gc.h>

/* Function types */

static void *(*next_calloc)(size_t n, size_t s);
static void *(*next_malloc)(size_t s);
static void *(*next_realloc)(void *p, size_t s);
static void  (*next_free)(void *p);

/* Internal variables */

/* Last resort allocator */
static char buffer[4096];
static char *alloc = buffer;

/* True when GC has been initialized */
static bool active = false;
/* True when wrapper have been initialized */
static bool initialized = false;
/* Flag for preventing init recursion */
static bool initializing = false;

/* Internal function declarations */

static void init_next(void);
static void *balloc(size_t n, size_t s);

/* Exported functions */

void cgc_init(void) {
  GC_init();
  active = true;
}

/* Internal functions */

static void init_next(void) {
  initializing = true;
  next_calloc = dlsym(RTLD_NEXT, "calloc");
  next_malloc = dlsym(RTLD_NEXT, "malloc");
  next_realloc = dlsym(RTLD_NEXT, "realloc");
  next_free = dlsym(RTLD_NEXT, "free");
  initialized = true;
  initializing = false;
}

static void *balloc(size_t n, size_t s) {
  char *ao = alloc;
  char *an = alloc + (n * s);
  if(an >= (buffer + sizeof(buffer))) {
    return NULL;
  }
  alloc = an;
  return ao;
}

/* Function wrappers */

void *calloc(size_t n, size_t s) {
  if(!initialized) {
    if(initializing) {
      return balloc(n, s);
    }
    init_next();
  }
  if(active) {
    return GC_malloc(n*s);
  } else {
    return next_calloc(n, s);
  }
}

void *malloc(size_t s) {
  if(!initialized) {
    if(initializing) {
      return balloc(1, s);
    }
    init_next();
  }
  if(active) {
    return GC_malloc(s);
  } else {
    return next_malloc(s);
  }
}

void *realloc(void *p, size_t s) {
  if(!initialized) {
    init_next();
  }
  if(active && GC_is_heap_ptr(p)) {
    return GC_realloc(p,s);
  } else {
    return next_realloc(p, s);
  }
}

void free(void *p) {
  if(!initialized) {
    init_next();
  }
  if(active && GC_is_heap_ptr(p)) {
    return GC_free(p);
  } else {
    next_free(p);
  }
}
