// clang-format off
#define datam_VERSION_MAJOR 0
#define datam_VERSION_MINOR 0
#define datam_VERSION_PATCH 1
// clang-format on

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)
#define VERSION                  \
  STRINGIFY(datam_VERSION_MAJOR) \
  "." STRINGIFY(datam_VERSION_MINOR) "." STRINGIFY(datam_VERSION_PATCH)

#define datam_PROG_NAME "datam"
#define datam_PROG_NAME_EXEC "datam"
#define TARGET_FPS 30

#define VERTEX_BUFFER_MAX_SIZE 1000000

// Trigger debut printing
#define DEBUG 0
