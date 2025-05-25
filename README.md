# bgo.h (Better get_opt.h)

## Description

```bgo.h``` is a C (single header file) library for setting up a CL Utility in a better, faster and more scalable way than with standard C tools like get_opt.h.

## Examples

After including the the header in your code (as you would usually do with headers) you can use the utilities of the library to set up your CL Tool easily. Here is a simple example (for more examples check the examples directory of this repo):

```C 
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
```

Compiling this makes a small calculator utility program that adds two numbers given by the user through the '-1' and '-2' flags.
