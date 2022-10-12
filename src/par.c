/*********************/
/* par.c             */
/* for Par 3.20      */
/* Copyright 1993 by */
/* Adam M. Costello  */
/*********************/

/* This is ANSI C code. */


#include "errmsg.h"
#include "buffer.h"    /* Also includes <stddef.h>. */
#include "reformat.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>

#undef NULL
#define NULL ((void *) 0)


// const char * const progname = "par";
// const char * const version = "3.20";
const char * const progname_version = "par 3.20\n";
const char * const wps_err = "Error: <width> <= <prefix> + <suffix>\n";

static int digtoint(char c)

/* Returns the value represented by the digit c,   */
/* or -1 if c is not a digit. Does not use errmsg. */
{
  return c == '0' ? 0 :
         c == '1' ? 1 :
         c == '2' ? 2 :
         c == '3' ? 3 :
         c == '4' ? 4 :
         c == '5' ? 5 :
         c == '6' ? 6 :
         c == '7' ? 7 :
         c == '8' ? 8 :
         c == '9' ? 9 :
         -1;

  /* We can't simply return c - '0' because this is ANSI  */
  /* C code, so it has to work for any character set, not */
  /* just ones which put the digits together in order.    */
}


static int strtoudec(const char *s, int *pn)

/* Puts the decimal value of the string s into *pn, returning */
/* 1 on success. If s is empty, or contains non-digits,       */
/* or represents an integer greater than 9999, then *pn       */
/* is not changed and 0 is returned. Does not use errmsg.     */
{
  int n = 0;

  if (!*s) return 0;

  /* chengzhi think 1000 may be an error*/
  do {
    if (n >= 10000 || !isdigit(*s)) return 0;
    n = 10 * n + digtoint(*s);
  } while (*++s);

  *pn = n;

  return 1;
}

static void parseopt(
  int argc, const char *const *argv, int *pwidth, int *pprefix,
  int *psuffix, int *phang, int *plast, int *pmin, int *pversion
)
/* Parses the single option in opt, setting *pwidth, *pprefix,     */
/* *psuffix, *phang, *plast, or *pmin as appropriate. Uses errmsg. */
{
  /*use for error*/
  char *bp;
  size_t bs;
  FILE *stream;
  /*use for error*/

  int opt, temp;

  optind = 1;

  while(1){

    int option_index = 0;

    static struct option long_options[] =
    {
      {"version", no_argument,       NULL, 'v'},
      {"width",   required_argument, NULL, 'w'},
      {"prefix",  required_argument, NULL, 'p'},
      {"suffix",  required_argument, NULL, 's'},
      {"hang",    required_argument, NULL, 'h'},
      {"last",    no_argument ,      NULL, 't'},
      {"no-last", no_argument ,      NULL, 'T'},
      {"min",     no_argument ,      NULL, 'n'},
      {"no-min",  no_argument ,      NULL, 'N'},
      {NULL,      0,                 NULL, 0},
    };

    opt = getopt_long(argc, (char **)argv, "-:w:p:s:h:l:m:", long_options, &option_index);

    if(opt == -1){
      break;
    }

    temp = -1;
    switch(opt){

      case 'v': if(!is_error()){
                  set_error((char *)progname_version);
                  *pversion = 1;
                  return;
                }
                break;

      case 'w': if(strtoudec(optarg, &temp)){
                  *pwidth = temp;
                }else{
                  goto badopt;
                }
                break;

      case 'p': if(strtoudec(optarg, &temp)){
                  *pprefix = temp;
                }else{
                  goto badopt;
                }
                break;

      case 's': if(strtoudec(optarg, &temp)){
                  *psuffix = temp;
                }else{
                  goto badopt;
                }
                break;

      case 'h': if(strtoudec(optarg, &temp)){
                  *phang = temp;
                }else{
                  goto badopt;
                }
                break;

      case 'l': if(strtoudec(optarg, &temp)){
                  if(temp == 1 || temp == 0){
                    *plast = temp;
                  }else{
                    goto badopt;
                  }
                }else{
                  goto badopt;
                }
                break;

      case 'm': if(strtoudec(optarg, &temp)){
                  if(temp == 1 || temp == 0){
                    *pmin = temp;
                  }else{
                    goto badopt;
                  }
                }else{
                  goto badopt;
                }
                break;

      case 't': *plast = 1;
                break;

      case 'T': *plast = 0;
                break;

      case 'n': *pmin = 1;
                break;

      case 'N': *pmin = 0;
                break;

      case '?': goto badopt;
                break;

      case ':': goto badopt;
                break;

      case 0: goto badopt;
              break; //only --

      case 1: if(strtoudec(optarg, &temp)){
                if(temp <= 8){
                  *pprefix = temp;
                }
                else{
                  *pwidth = temp;
                }
              }
              else{
                goto badopt;
              }
              break;

      default:  goto badopt;
                break;
    }
  }

  clear_error();
  return;

  badopt:
  //sprintf(errmsg, "Bad option: %.149s\n", saveopt);
  stream = open_memstream(&bp, &bs);
  fprintf(stream, "Bad option: %s\n", *(argv+optind-1));
  fclose(stream);
  if(!is_error()){
    set_error(bp);
  }
  free(bp);
  return;
}
// static void parseopt(
//   const char *opt, int *pwidth, int *pprefix,
//   int *psuffix, int *phang, int *plast, int *pmin
// )
// /* Parses the single option in opt, setting *pwidth, *pprefix,     */
// /* *psuffix, *phang, *plast, or *pmin as appropriate. Uses errmsg. */
// {
//   const char *saveopt = opt;
//   char oc;
//   int n, r;

//   if (*opt == '-') ++opt;

//   if (!strcmp(opt, "version")) {
//     //sprintf(errmsg, "%s %s\n", progname, version);
//     if(!is_error()){
//       set_error(progname);
//       set_error(" ");
//       set_error(version);
//       set_error("\n");
//     }
//     return;
//   }

//   oc = *opt;

//   if (isdigit(oc)) {
//     if (!strtoudec(opt, &n)) goto badopt;
//     if (n <= 8) *pprefix = n;
//     else *pwidth = n;
//   }
//   else {
//     if (!oc) goto badopt;
//     n = 1;
//     r = strtoudec(opt + 1, &n);
//     if (opt[1] && !r) goto badopt;

//     if (oc == 'w' || oc == 'p' || oc == 's') {
//       if (!r) goto badopt;
//       if      (oc == 'w') *pwidth  = n;
//       else if (oc == 'p') *pprefix = n;
//       else                *psuffix = n;
//     }
//     else if (oc == 'h') *phang = n;
//     else if (n <= 1) {
//       if      (oc == 'l') *plast = n;
//       else if (oc == 'm') *pmin = n;
//     }
//     else goto badopt;
//   }

//   //*errmsg = '\0';
//   clear_error();
//   return;

//   /*chengzhi badopt should have a return*/
//   badopt:
//   //sprintf(errmsg, "Bad option: %.149s\n", saveopt);
//   if(!is_error()){
//     set_error("Bad option: ");
//     set_error(saveopt);
//     set_error("\n");
//   }
//   return;
// }


static char **readlines(void)

/* Reads lines from stdin until EOF, or until a blank line is encountered, */
/* in which case the newline is pushed back onto the input stream. Returns */
/* a NULL-terminated array of pointers to individual lines, stripped of    */
/* their newline characters. Uses errmsg, and returns NULL on failure.     */
{
  struct buffer *cbuf = NULL, *pbuf = NULL;
  int c, blank;
  char ch, *ln, *nullline = NULL, nullchar = '\0', **lines = NULL;

  cbuf = newbuffer(sizeof (char));
  //if (*errmsg) goto rlcleanup;
  if(is_error()) goto rlcleanup;
  pbuf = newbuffer(sizeof (char *));
  //if (*errmsg) goto rlcleanup;
  if(is_error()) goto rlcleanup;

  for (blank = 1;  ; ) {
    c = getchar();
    if (c == EOF) break;
    if (c == '\n') {
      if (blank) {
        ungetc(c,stdin);
        break;
      }
      additem(cbuf, &nullchar);
      //if (*errmsg) goto rlcleanup;
      if(is_error()) goto rlcleanup;
      ln = copyitems(cbuf);
      //if (*errmsg) goto rlcleanup;
      if(is_error()) goto rlcleanup;
      additem(pbuf, &ln);
      //if (*errmsg) goto rlcleanup;
      if(is_error()) goto rlcleanup;
      clearbuffer(cbuf);
      blank = 1;
    }
    else {
      if (!isspace(c)) blank = 0;
      ch = c;
      additem(cbuf, &ch);
      //if (*errmsg) goto rlcleanup;
      if(is_error()) goto rlcleanup;
    }
  }

  /* chengzhi- means if EOF add an null char*/
  if (!blank) {
    additem(cbuf, &nullchar);
    //if (*errmsg) goto rlcleanup;
    if(is_error()) goto rlcleanup;
    ln = copyitems(cbuf);
    //if (*errmsg) goto rlcleanup;
    if(is_error()) goto rlcleanup;
    additem(pbuf, &ln);
    //if (*errmsg) goto rlcleanup;
    if(is_error()) goto rlcleanup;
  }

  /* chengzhi- means add a NULL at the end of line array*/ 
  additem(pbuf, &nullline);
  //if (*errmsg) goto rlcleanup;
  if(is_error()) goto rlcleanup;
  lines = copyitems(pbuf);

rlcleanup:

  if (cbuf) freebuffer(cbuf);
  if (pbuf) {
    if (!lines){
      for (;;) {
        lines = nextitem(pbuf);
        if (!lines) break;
        free(*lines);
      }
    }
    /* chengzhi- added freebuffer(pbuf)*/
    freebuffer(pbuf);
  }

  return lines;
}


static void setdefaults(
  const char * const *inlines, int *pwidth, int *pprefix,
  int *psuffix, int *phang, int *plast, int *pmin
)
/* If any of *pwidth, *pprefix, *psuffix, *phang, *plast, *pmin are     */
/* less than 0, sets them to default values based on inlines, according */
/* to "par.doc". Does not use errmsg because it always succeeds.        */
{
  int numlines;
  const char *start, *end, * const *line, *p1, *p2;

  if (*pwidth < 0) *pwidth = 72;
  if (*phang < 0) *phang = 0;
  if (*plast < 0) *plast = 0;
  if (*pmin < 0) *pmin = *plast;

  for (line = inlines;  *line;  ++line);
  numlines = line - inlines;

  /* chengzhi added {} after if*/
  if (*pprefix < 0){
    if (numlines <= *phang + 1)
      *pprefix = 0;
    else {
      start = inlines[*phang];
      for (end = start;  *end;  ++end);
      for (line = inlines + *phang + 1;  *line;  ++line) {
        for (p1 = start, p2 = *line;  p1 < end && *p1 == *p2;  ++p1, ++p2);
        end = p1;
      }
      *pprefix = end - start;
    }
  }

  /* chengzhi added {} after if*/
  if (*psuffix < 0){
    if (numlines <= 1)
      *psuffix = 0;
    else {
      start = *inlines;
      for (end = start;  *end;  ++end);
      for (line = inlines + 1;  *line;  ++line) {
        /* chengzhi added ; after for loop*/
        for (p2 = *line;  *p2;  ++p2);
        for (p1 = end;
             p1 > start && p2 > *line && p1[-1] == p2[-1];
             --p1, --p2);
        start = p1;
      }
      while (end - start >= 2 && isspace(*start) && isspace(start[1])) ++start;
      *psuffix = end - start;
    }
  }
}


static void freelines(char **lines)
/* Frees the strings pointed to in the NULL-terminated array lines, then */
/* frees the array. Does not use errmsg because it always succeeds.      */
{
  /* chengzhi - changed *line to **line */
  char **line;

  for (line = lines;  *line;  ++line)
    free(*line);

  free(lines);
}

/* chengzhi (1)added int before main (2) changed main to original main*/
int original_main(int argc, const char * const *argv)
{
  int width, widthbak = -1, prefix, prefixbak = -1, suffix, suffixbak = -1,
      hang, hangbak = -1, last, lastbak = -1, min, minbak = -1, c, pisize, vflag=0;
  char *parinit, *picopy = NULL, *opt, **inlines = NULL, **outlines = NULL,
       **line, **piarray=NULL;
  const char * const whitechars = " \f\n\r\t\v";

  parinit = getenv("PARINIT");
  if (parinit) {
    picopy = malloc((strlen(parinit) + 1) * sizeof (char));
    if (!picopy) {
      //strcpy(errmsg,outofmem);
      if(!is_error()){
        set_error((char *)outofmem);
      }
      goto parcleanup;
    }
    strcpy(picopy,parinit);

    //
    pisize = 1;
    opt = strtok(picopy,whitechars);
    while(opt){
      pisize++;
      opt = strtok(NULL,whitechars);
    }

    piarray = malloc((pisize+1)*sizeof(char *));
    *(piarray) = NULL;
    strcpy(picopy,parinit);
    opt = strtok(picopy,whitechars);
    for(int k=1; k<pisize; k++){
      //fprintf(stdout, "%s\n" ,opt);
      *(piarray+k) = opt;
      opt = strtok(NULL,whitechars);
    }
    *(piarray+pisize) = NULL;
    parseopt(pisize, (const char * const *)piarray, &widthbak, &prefixbak, &suffixbak, &hangbak, &lastbak, &minbak, &vflag);
    if(is_error()) goto parcleanup;
    free(piarray);
    piarray=NULL;
    free(picopy);
    picopy = NULL;


    // opt = strtok(picopy,whitechars);
    // while (opt) {
    //   parseopt(opt, &widthbak, &prefixbak, &suffixbak, &hangbak, &lastbak, &minbak);
    //   //if (*errmsg) goto parcleanup;
    //   if(is_error()) goto parcleanup;
    //   opt = strtok(NULL,whitechars);
    // }
    // free(picopy);
    // picopy = NULL;
  }

  // while (*++argv) {
  //   parseopt(*argv, &widthbak, &prefixbak, &suffixbak, &hangbak, &lastbak, &minbak);

  //   //if (*errmsg) goto parcleanup;
  //   if(is_error()) goto parcleanup;
  // }

  parseopt(argc, argv, &widthbak, &prefixbak, &suffixbak, &hangbak, &lastbak, &minbak, &vflag);
  if(is_error()) goto parcleanup;

  for (;;) {
    for (;;) {
      c = getchar();
      if (c != '\n') break;
      putchar(c);
    }
    ungetc(c,stdin);

    inlines = readlines();
    //if (*errmsg) goto parcleanup;
    if(is_error()) goto parcleanup;
    if (!*inlines) {
      free(inlines);
      inlines = NULL;
      /*chengzhi- instead of continue, do goto parcleanup*/
      /*continue;*/
      goto parcleanup;
    }

    width = widthbak;  prefix = prefixbak;  suffix = suffixbak;
    hang = hangbak;  last = lastbak;  min = minbak;
    setdefaults((const char * const *) inlines,
                &width, &prefix, &suffix, &hang, &last, &min);

    // It is an error if <width> <= <prefix> + <suffix>.
    if(width <= (prefix+suffix) ){
      if(!is_error()){
        set_error((char *)wps_err);
      }
      goto parcleanup;
    }

    outlines = reformat((const char * const *) inlines,
                        width, prefix, suffix, hang, last, min);
    //if (*errmsg) goto parcleanup;
    if(is_error()) goto parcleanup;

    freelines(inlines);
    inlines = NULL;

    for (line = outlines;  *line;  ++line)
      puts(*line);

    freelines(outlines);
    outlines = NULL;
  }

parcleanup:

  if (picopy) free(picopy);
  if (inlines) freelines(inlines);
  if (outlines) freelines(outlines);

  if (is_error()) {
    //fprintf(stderr, "%.163s", errmsg);
    if(report_error(stderr)){
      fprintf(stderr, "Error: EOF");
    }
    clear_error();

    if(vflag == 1){
      exit(EXIT_SUCCESS);
    }
    exit(EXIT_FAILURE);
  }

  clear_error();
  exit(EXIT_SUCCESS);
}
