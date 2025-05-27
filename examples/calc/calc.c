#include "bgo.h"

int main(int argc, char **argv) {

  bgo_opts_t flags;
  int value1 = 0;
  int value2 = 0;
  char ** mul = NULL;
  
  bgo_init(&flags);
  bgo_add_desc(&flags, "Simple calculator (adds two values).");
  bgo_add_name(&flags, "calc");
  bgo_add_flag(&flags, "-b", "--boolean", NULL, NULL);
  bgo_add_int_flag(&flags, "-1", "--value1", &value1, "this is the first number");
  bgo_add_int_flag(&flags, "-2", "--value2", &value2, "this is the second number");
  bgo_add_mul_flag(&flags, "-m", "--mul", &mul, "multiple parameter flag :)");
  bgo(&flags, argc, argv);

  printf("%d", value1 + value2);
  size_t iter = 0;

  while(mul != NULL && mul[iter] != NULL) {
    printf("%s\n", mul[iter]);
    iter++;
  } 
}
