/*
 * LICENSE : This library is license under the GNU Public License Version 3 (29 june 2007).
 * ORIGINAL AUTHOR: kickhead13
 *
 * */
#ifndef __BGO_H__
#define __BGO_H__

#include <stdio.h>  /* printf */ 
#include <string.h> /* strcmp, strcpy, strcat */
#include <unistd.h> /* write */
#include <stdlib.h> /* exit */

#define BASE_CHAR_LEN 50
#define EWRITEL(message) write(2, (message), sizeof((message)))
#define QUIT_ON_NULL(ptr, message, exit_val)\
  if((ptr) == NULL) {\
    EWRITEL((message));\
    exit((exit_val));\
  }
#define PRINTF_IF_NOT_NULL(str)\
  if((str) != NULL) printf("%s", (str));

enum bgo_type {
  BGO_INT = 0,
  BGO_STR = 1,
  BGO_BOOL = 2
};

typedef struct opt_t {
  void         *  value;
  char         **    fv;
  struct opt_t *   next;
  struct opt_t *   prev;
  enum bgo_type   vtype;
} bgo_opt_t;

typedef struct info_t {
  char  * usage;
  char  *  desc;
  char  ** opts;
  char  *  exmp;
} bgo_info_t;

typedef struct opts_t {
  bgo_opt_t  * head;
  bgo_opt_t  * tail;
  bgo_info_t * info;
  size_t        len;
} bgo_opts_t;

void bgo_init(bgo_opts_t *);
void bgo_add_desc(bgo_opts_t *, const char*);
void bgo_add_name(bgo_opts_t *, const char*);
void bgo_add_flag(bgo_opts_t *, const char*, const char*, int *);
void bgo_add_int_flag(bgo_opts_t *, const char*, const char*, int *);
void bgo_add_str_flag(bgo_opts_t *, const char*, const char*, char **);
void bgo(bgo_opts_t *, int, char **);
void __bgo_disp_help(bgo_opts_t *);
void __bgo_add_opt(bgo_opts_t *, bgo_opt_t *);
void __bgo_set_bool(bgo_opt_t *);
void __bgo_set_int(bgo_opt_t *, int);
void __bgo_set_str(bgo_opt_t *, char*); 
void __bgo_free(bgo_opts_t *);
bgo_opt_t * __bgo_find_opt(bgo_opts_t *, char*);


void bgo_init(bgo_opts_t * opts) {
  opts -> head = NULL;
  opts -> info = (bgo_info_t *)malloc(sizeof(bgo_info_t));
  opts -> info -> usage = NULL;
  opts -> info -> opts = (char**)malloc(sizeof(char*) * 100);
  opts -> len = 0;
}

void bgo_add_name(bgo_opts_t * opts, const char * name) {
  if(opts -> info -> usage != NULL) free(opts -> info -> usage);

  opts -> info -> usage = (char*)malloc(sizeof(char) * (strlen(name) + 17));
  QUIT_ON_NULL(opts -> info -> usage, "(bgo.h) Couldn't allocate HEAP memory for command usage prompt.\n", -1);
  
  strcpy(opts -> info -> usage, "Usage: ");
  strcat(opts -> info -> usage, name);
  strcat(opts -> info -> usage, " [OPTIONS]\n");
}

void bgo_add_desc(bgo_opts_t * opts, const char * desc) {
  if(opts -> info -> desc != NULL) free(opts -> info -> desc);
  
  opts -> info -> desc = (char*)malloc(sizeof(char) * (strlen(desc) + 17));
  QUIT_ON_NULL(opts -> info -> desc, "(bgo.h) Couldn't allocate HEAP memory for command description prompt.\n", -1);
  
  strcpy(opts -> info -> desc, desc);
}

void __bgo_disp_help(bgo_opts_t * opts) {
  QUIT_ON_NULL(opts -> info -> usage, "(bgo.h) Command name must be set.\n", -1);

  printf("%s", opts -> info -> usage);
  PRINTF_IF_NOT_NULL(opts -> info -> desc);

  printf("\nOPTIONS:\n");
  size_t iter = 0;
  for(;iter < opts->len; iter++) {
    printf("\t%s\n", (opts -> info -> opts)[iter]);
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

void bgo_add_flag(bgo_opts_t *opts, const char *sf, const char *lf, int *value) {
  bgo_opt_t * opt = (bgo_opt_t*)malloc(sizeof(bgo_opt_t));
  QUIT_ON_NULL(opt, "(bgo.h) Couldn't allocate HEAP memory for boolean flag option.\n", -1);

  opt -> fv = (char**)malloc(sizeof(char*) * 2);
  QUIT_ON_NULL(opt -> fv, "(bgo.h) Couldn't allocate HEAP memory for boolean flag variants list.\n", -1);

  (opt -> fv)[0] = (char*)malloc(sizeof(char) * (strlen(sf) + 1));
  (opt -> fv)[1] = (char*)malloc(sizeof(char) * (strlen(lf) + 1));
  QUIT_ON_NULL((opt -> fv)[0], "(bgo.h) Couldn't allocate HEAP memory for boolean flag variants list.\n", -1);
  QUIT_ON_NULL((opt -> fv)[1], "(bgo.h) Couldn't allocate HEAP memory for boolean flag variants list.\n", -1);
  strcpy((opt -> fv)[0], sf);
  strcpy((opt -> fv)[1], lf);

  opt -> value = (void*)value;
  opt -> vtype = BGO_BOOL;
  opt -> next = NULL;
  opt -> prev = NULL;
  __bgo_add_opt(opts, opt);

  (opts -> info -> opts)[opts->len] = (char*)malloc(sizeof(char) + (strlen(sf) + strlen(lf) + 4));
  strcpy((opts -> info -> opts)[opts -> len], sf);
  strcat((opts -> info -> opts)[opts -> len], ", ");
  strcat((opts -> info -> opts)[opts -> len], lf);
  opts -> len = (opts->len) + 1;
}

void bgo_add_int_flag(bgo_opts_t *opts, const char *sf, const char *lf, int *value) {
  bgo_opt_t * opt = (bgo_opt_t*)malloc(sizeof(bgo_opt_t));
  QUIT_ON_NULL(opt, "(bgo.h) Couldn't allocate HEAP memory for boolean flag option.\n", -1);

  opt -> fv = (char**)malloc(sizeof(char*) * 2);
  QUIT_ON_NULL(opt -> fv, "(bgo.h) Couldn't allocate HEAP memory for boolean flag variants list.\n", -1);

  (opt -> fv)[0] = (char*)malloc(sizeof(char) * (strlen(sf) + 1));
  (opt -> fv)[1] = (char*)malloc(sizeof(char) * (strlen(lf) + 1));
  QUIT_ON_NULL((opt -> fv)[0], "(bgo.h) Couldn't allocate HEAP memory for boolean flag variants list.\n", -1);
  QUIT_ON_NULL((opt -> fv)[1], "(bgo.h) Couldn't allocate HEAP memory for boolean flag variants list.\n", -1);
  strcpy((opt -> fv)[0], sf);
  strcpy((opt -> fv)[1], lf);

  opt -> value = (void*)value;
  opt -> vtype = BGO_INT;
  opt -> next = NULL;
  opt -> prev = NULL;

  __bgo_add_opt(opts, opt);

  (opts -> info -> opts)[opts->len] = (char*)malloc(sizeof(char) + (strlen(sf) + strlen(lf) + 12));
  strcpy((opts -> info -> opts)[opts -> len], sf);
  strcat((opts -> info -> opts)[opts -> len], ", ");
  strcat((opts -> info -> opts)[opts -> len], lf);
  strcat((opts -> info -> opts)[opts -> len], " <INT> ");
  opts -> len = (opts->len) + 1;
}

void bgo_add_str_flag(bgo_opts_t *opts, const char *sf, const char *lf, char **value) {
  bgo_opt_t * opt = (bgo_opt_t*)malloc(sizeof(bgo_opt_t));
  QUIT_ON_NULL(opt, "(bgo.h) Couldn't allocate HEAP memory for boolean flag option.\n", -1);

  opt -> fv = (char**)malloc(sizeof(char*) * 2);
  QUIT_ON_NULL(opt -> fv, "(bgo.h) Couldn't allocate HEAP memory for boolean flag variants list.\n", -1);

  (opt -> fv)[0] = (char*)malloc(sizeof(char) * (strlen(sf) + 1));
  (opt -> fv)[1] = (char*)malloc(sizeof(char) * (strlen(lf) + 1));
  QUIT_ON_NULL((opt -> fv)[0], "(bgo.h) Couldn't allocate HEAP memory for boolean flag variants list.\n", -1);
  QUIT_ON_NULL((opt -> fv)[1], "(bgo.h) Couldn't allocate HEAP memory for boolean flag variants list.\n", -1);
  strcpy((opt -> fv)[0], sf);
  strcpy((opt -> fv)[1], lf);

  opt -> value = (void*)value;
  opt -> vtype = BGO_STR;
  opt -> next = NULL;
  opt -> prev = NULL;

  __bgo_add_opt(opts, opt);

  (opts -> info -> opts)[opts->len] = (char*)malloc(sizeof(char) + (strlen(sf) + strlen(lf) + 12));
  strcpy((opts -> info -> opts)[opts -> len], sf);
  strcat((opts -> info -> opts)[opts -> len], ", ");
  strcat((opts -> info -> opts)[opts -> len], lf);
  strcat((opts -> info -> opts)[opts -> len], " <STR> ");
  opts -> len = (opts->len) + 1;
}


void __bgo_set_bool(bgo_opt_t * opt) {

  if(opt -> vtype != BGO_BOOL)  {
    EWRITEL("(bgo.h) Wrong type for ");
    write(2, (opt -> fv)[0], strlen((opt->fv)[0]));
    EWRITEL(", ");
    write(2, (opt -> fv)[1], strlen((opt->fv)[1]));
    EWRITEL(" bool flags.\n");
    exit(-1);
  }
  *((int*)(opt -> value)) = 1;

}

void __bgo_set_int(bgo_opt_t * opt, int value) {

  if(opt -> vtype != BGO_INT)  {
    EWRITEL("(bgo.h) Wrong type for ");
    write(2, (opt -> fv)[0], strlen((opt->fv)[0]));
    EWRITEL(", ");
    write(2, (opt -> fv)[1], strlen((opt->fv)[1]));
    EWRITEL(" int flags.\n");
    exit(-1);
  }
  *((int*)(opt -> value)) = value;

}

void __bgo_set_str(bgo_opt_t * opt, char * value) {

  if(opt -> vtype != BGO_STR)  {
    EWRITEL("(bgo.h) Wrong type for ");
    write(2, (opt -> fv)[0], strlen((opt->fv)[0]));
    EWRITEL(", ");
    write(2, (opt -> fv)[1], strlen((opt->fv)[1]));
    EWRITEL(" string flags.\n");
    exit(-1);
  }


  // TODO: FREE STUFF HERE ??

  *((char**)(opt -> value)) = (char*)malloc(sizeof(char) * (strlen(value) + 1)); 
  strcpy(*((char**)(opt -> value)), value);

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
      default:
        break;
    }
  }
  
  __bgo_free(opts);

}

#endif
