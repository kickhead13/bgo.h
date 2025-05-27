/*
 * LICENSE : This library is licensed under the GNU Public License Version 3 (29 june 2007).
 * ORIGINAL AUTHOR: kickhead13
 * ORIGINAL REPO: https://github.com/kickhead13/bgo.h
 *
 * */
#ifndef __BGO_H__
#define __BGO_H__

#include <stdio.h>  /* printf */ 
#include <string.h> /* strcmp, strcpy, strcat */
#include <unistd.h> /* write */
#include <stdlib.h> /* exit */

#define __BGO_EWRITEL(message) write(2, (message), (sizeof((message)) - 1))
#define __BGO_QUIT_ON_NULL(ptr, message, exit_val)\
  if((ptr) == NULL) {\
    __BGO_EWRITEL((message));\
    exit((exit_val));\
  }
#define __BGO_PRINTF_NOT_NULL(str)\
  if((str) != NULL) printf("%s", (str));

enum bgo_type {
  BGO_INT = 0,
  BGO_STR = 1,
  BGO_BOOL = 2,
  BGO_MUL = 3
};

typedef struct opt_t {
  void         *  value;
  char         **    fv;
  struct opt_t *   next;
  struct opt_t *   prev;
  enum bgo_type   vtype;
} bgo_opt_t;

typedef struct opts_t {
  bgo_opt_t  * head;
  bgo_opt_t  * tail;
  char       * desc;
  char       *usage;
  size_t        len;
} bgo_opts_t;

void bgo_init(bgo_opts_t *);
void bgo_add_desc(bgo_opts_t *, const char*);
void bgo_add_name(bgo_opts_t *, const char*);
void bgo_add_flag(bgo_opts_t *, const char*, const char*, int *);
void bgo_add_int_flag(bgo_opts_t *, const char*, const char*, int *);
void bgo_add_str_flag(bgo_opts_t *, const char*, const char*, char **);
void bgo_add_mul_flag(bgo_opts_t *, const char*, const char*, char ***);
void bgo(bgo_opts_t *, int, char **);
void __bgo_disp_help(bgo_opts_t *);
void __bgo_add_opt(bgo_opts_t *, bgo_opt_t *);
void __bgo_set_bool(bgo_opt_t *);
void __bgo_set_int(bgo_opt_t *, int);
void __bgo_set_str(bgo_opt_t *, char*); 
void __bgo_free(bgo_opts_t *);
bgo_opt_t * __bgo_find_opt(bgo_opts_t *, char*);
bgo_opt_t * __new_opt_t(const char*, const char*, enum bgo_type);

void bgo_init(bgo_opts_t * opts) {
  opts -> head = NULL;
  opts -> usage = NULL;
  opts -> desc = NULL;
  opts -> len = 0;
}

void bgo_add_name(bgo_opts_t * opts, const char * name) {
  if(opts -> usage != NULL) free(opts -> usage);
  
  opts -> usage = (char*)malloc( (strlen(name) + 17));
  __BGO_QUIT_ON_NULL(opts -> usage, "(bgo.h) Couldn't allocate HEAP memory for command usage prompt.\n", -1);
  
  strcpy(opts -> usage, "Usage: ");
  strcat(opts -> usage, name);
  strcat(opts -> usage, " [OPTIONS]\n");
}

void bgo_add_desc(bgo_opts_t * opts, const char * desc) {
  if(opts -> desc != NULL) free(opts -> desc);
  
  opts -> desc = (char*)malloc( (strlen(desc) + 17));
  __BGO_QUIT_ON_NULL(opts -> desc, "(bgo.h) Couldn't allocate HEAP memory for command description prompt.\n", -1);
  
  strcpy(opts -> desc, desc);
}

void __bgo_disp_help(bgo_opts_t * opts) {
  __BGO_QUIT_ON_NULL(opts -> usage, "(bgo.h) Command name must be set.\n", -1);

  printf("%s", opts -> usage);
  __BGO_PRINTF_NOT_NULL(opts -> desc);

  printf("\nOPTIONS:\n");
  bgo_opt_t * opt = opts -> head;
  for(;opt; opt = opt->next) {
    printf("\t%s, %s ", (opt -> fv)[0], (opt->fv)[1]);
    switch(opt->vtype) {
      case BGO_INT:
        printf("<INT>\n");
        break;
      case BGO_STR:
        printf("<STR>\n");
        break;
      case BGO_MUL:
        printf("<STR STR ...>\n");
        break;
      case BGO_BOOL:
      default:
        printf("\n");
        break;
    }
  }
}
  
void __bgo_add_opt(bgo_opts_t * opts, bgo_opt_t * opt) {
  if(opts -> head == NULL) {
    opts -> head = opt;
    opts -> tail = opt;
    return;
  }
  opt -> prev = opts -> tail;
  opts -> tail -> next = opt;
  opts -> tail = opt;
}

bgo_opt_t * __bgo_new_opt_t(const char *sf, const char *lf, void * value, enum bgo_type vtype) {
  bgo_opt_t * opt = (bgo_opt_t*)malloc(sizeof(bgo_opt_t));
  __BGO_QUIT_ON_NULL(opt, "(bgo.h) Couldn't allocate HEAP memory for boolean flag option.\n", -1);

  opt -> fv = (char**)malloc(sizeof(char*) * 2);
  __BGO_QUIT_ON_NULL(opt -> fv, "(bgo.h) Couldn't allocate HEAP memory for boolean flag variants list.\n", -1);

  (opt -> fv)[0] = (char*)malloc( (strlen(sf) + 1));
  (opt -> fv)[1] = (char*)malloc( (strlen(lf) + 1));
  __BGO_QUIT_ON_NULL((opt -> fv)[0], "(bgo.h) Couldn't allocate HEAP memory for boolean flag variants list.\n", -1);
  __BGO_QUIT_ON_NULL((opt -> fv)[1], "(bgo.h) Couldn't allocate HEAP memory for boolean flag variants list.\n", -1);
  strcpy((opt -> fv)[0], sf);
  strcpy((opt -> fv)[1], lf);

  opt -> value = value;
  opt -> vtype = vtype;
  opt -> next = NULL;
  opt -> prev = NULL;

  return opt;
}

void __bgo_add(bgo_opts_t *opts, const char *sf, const char *lf, void *value, enum bgo_type vtype) {
  bgo_opt_t * opt = __bgo_new_opt_t(sf, lf, value, vtype);

  __bgo_add_opt(opts, opt);
}

void bgo_add_flag(bgo_opts_t *opts, const char *sf, const char *lf, int *value) {
  __bgo_add(opts, sf, lf, (void*)value, BGO_BOOL);
}

void bgo_add_int_flag(bgo_opts_t *opts, const char *sf, const char *lf, int *value) {
  __bgo_add(opts, sf, lf, (void*)value, BGO_INT);
}

void bgo_add_str_flag(bgo_opts_t *opts, const char *sf, const char *lf, char **value) {
  __bgo_add(opts, sf, lf, (void*)value, BGO_STR);
}

void bgo_add_mul_flag(bgo_opts_t *opts, const char *sf, const char *lf, char ***value) {
  __bgo_add(opts, sf, lf, (void*)value, BGO_MUL);
}

void __bgo_set_bool(bgo_opt_t * opt) {

  if(opt -> vtype != BGO_BOOL)  {
    __BGO_EWRITEL("(bgo.h) Wrong type for ");
    write(2, (opt -> fv)[0], strlen((opt->fv)[0]));
    __BGO_EWRITEL(", ");
    write(2, (opt -> fv)[1], strlen((opt->fv)[1]));
    __BGO_EWRITEL(" bool flags.\n");
    exit(-1);
  }
  *((int*)(opt -> value)) = 1;

}

void __bgo_set_int(bgo_opt_t * opt, int value) {

  if(opt -> vtype != BGO_INT)  {
    __BGO_EWRITEL("(bgo.h) Wrong type for ");
    write(2, (opt -> fv)[0], strlen((opt->fv)[0]));
    __BGO_EWRITEL(", ");
    write(2, (opt -> fv)[1], strlen((opt->fv)[1]));
    __BGO_EWRITEL(" int flags.\n");
    exit(-1);
  }
  *((int*)(opt -> value)) = value;

}

void __bgo_set_str(bgo_opt_t * opt, char * value) {

  if(opt -> vtype != BGO_STR)  {
    __BGO_EWRITEL("(bgo.h) Wrong type for ");
    write(2, (opt -> fv)[0], strlen((opt->fv)[0]));
    __BGO_EWRITEL(", ");
    write(2, (opt -> fv)[1], strlen((opt->fv)[1]));
    __BGO_EWRITEL(" string flags.\n");
    exit(-1);
  }


  // TODO: FREE STUFF HERE ??

  *((char**)(opt -> value)) = (char*)malloc( (strlen(value) + 1)); 
  strcpy(*((char**)(opt -> value)), value);

}

size_t __bgo_set_mul(bgo_opts_t * opts, bgo_opt_t * opt, int argc, char ** argv, size_t offset) {
  size_t o_off = offset;
  size_t count = 0;
  while(offset < argc && __bgo_find_opt(opts, argv[offset]) == NULL) {
    count++;
    offset++;
  }

  if(count == 0) return o_off;

  *((char***)(opt -> value)) = malloc(sizeof(char**) * (count + 1));

  size_t iter = 0;
  for(;o_off < offset; o_off++) {
    (*((char***)(opt -> value)))[iter] = (char*)malloc(strlen(argv[o_off]) + 1);
    strcpy((*((char***)(opt -> value)))[iter], argv[o_off]);
    iter++;
  }

  (*((char***)(opt -> value)))[iter] = NULL;
  return o_off - 1;
}


bgo_opt_t * __bgo_find_opt(bgo_opts_t * opts, char * fv) {
  bgo_opt_t *opt = opts -> head;
  while(opt) {
    if(strcmp((opt->fv)[0], fv) == 0 || strcmp((opt->fv)[1], fv) == 0) 
      return opt;
    opt = opt -> next;
  }
  return NULL;
}

void __bgo_free(bgo_opts_t * opts) {
  // TODO
}

void bgo(bgo_opts_t * opts, int argc, char ** argv) {
  size_t iter = 1;
  bgo_opt_t * opt;
  for(; iter < argc; iter++) {
    if(strcmp(argv[iter], "-h") == 0 || strcmp(argv[iter], "--help") == 0) {
      __bgo_disp_help(opts);
      exit(0);
    }
    opt = __bgo_find_opt(opts, argv[iter]);
    if(opt == NULL) {
      __bgo_disp_help(opts);
      exit(-1);
    }
    switch(opt -> vtype) {
      case BGO_BOOL:
        __bgo_set_bool(opt);
        break;
      case BGO_INT: 
        __bgo_set_int(opt, atoi(argv[iter+1]));
        iter++;
        break;
      case BGO_STR:
        __bgo_set_str(opt, argv[iter+1]);
        iter++;
        break;
      case BGO_MUL:
        iter = __bgo_set_mul(opts, opt, argc, argv, iter + 1);
        break;
      default:
        break;
    }
  }
  
  __bgo_free(opts);

}

#endif
