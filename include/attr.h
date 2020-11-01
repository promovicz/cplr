#ifndef CPLR_ATTR_H
#define CPLR_ATTR_H

#define ATTR_FUN_ARTIFICIAL                     \
  __attribute__((artificial))
#define ATTR_FUN_COLD                           \
  __attribute__((cold))
#define ATTR_FUN_CONST                          \
  __attribute__((const))
#define ATTR_FUN_PURE                           \
  __attribute__((pure))
#define ATTR_FUN_MALLOC                         \
  __attribute__((malloc))
#define ATTR_FUN_NORETURN                       \
  __attribute__((noreturn))
#define ATTR_FUN_RETURNS_NONNULL                \
  __attribute__((returns_nonnull))
#define ATTR_FUN_FORMAT(arch, fmtindex, argindex)       \
  __attribute__((format(arch, fmtindex, argindex)))
#define ATTR_ARG_ALLOC_SIZE(...)                \
  __attribute__((alloc_size(__VA_ARGS__)))
#define ATTR_ARG_FORMAT(...)                    \
  __attribute__((format_arg(__VA_ARGS__)))
#define ATTR_ARG_NONNULL(...)                   \
  __attribute__((nonnull(__VA_ARGS__)))

#endif /* !CPLR_ATTR_H */
