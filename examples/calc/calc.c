#include "bgo.h"

int main(int argc, char **argv) {

  bgo_opts_t flags;
  int value1 = 0;
  int value2 = 0;
  
  bgo_init(&flags);
  bgo_add_desc(&flags, "Simple calculator (adds two values).");
  bgo_add_name(&flags, "calc");
  bgo_add_int_flag(&flags, "-1", "--value1", &value1);
  bgo_add_int_flag(&flags, "-2", "--value2", &value2);
  bgo(&flags, argc, argv);

  printf("%d", value1 + value2);

}
