#include <fsl_debug_console.h>

void failed(char *file, int line, char *expr) {
  PRINTF("! FAIL '%s' @ %s:%d\r\n", expr, file, line);
  while(1);
}
