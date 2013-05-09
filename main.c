/** main.c ******************************************************************
 * 27/01/2013               CWG - Crossword generator                       *
 * Content of this file:                                                    *
 *   Main program. Parsing command line arguments, reading input file,      *
 *   generating crossword, printing crossword.                              *
 * Author: Rodolphe Lepigre <rlepigre@gmail.com>                            *
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include "const.h"
#include "word.h"
#include "cw.h"
#include "output.h"

/*
 * Output formats
 */
#define TEXT_FORMAT 0
#define LATEX_FORMAT 1
#define HTML_FORMAT 2

/*
 * Verbose mode.
 */
#define VERBOSE_OFF 0
#define VERBOSE_ON 1

/*
 * Display help.
 * pname : program name (basically argv[0]).
 */
void display_help(char* pname);

/*
 * Parse command line arguments.
 * argc : argc.
 * argv : argv.
 * i    : result for input file.
 * w    : result for width.
 * h    : result for height.
 * v    : if set to 1 verbose.
 *        otherwise don't.
 * f    : printing format.
 *        0 default
 *        1 latex
 */
void get_args(int argc, char** argv,
              FILE** i, int* w, int* h, int* v, int* f);

/*
 * Main program.
 */
int main(int argc, char** argv){
  // Default parameters (modified by command line parsing).
  int width = DEFAULT_WIDTH;
  int height = DEFAULT_HEIGHT;
  int verbose = VERBOSE_OFF;
  int format = TEXT_FORMAT;
  FILE* input = NULL;
  int tries = 1000; // TODO add in getargs

  // Variables for the program.
  word_list words = NULL;
  crossword cw = NULL;
  int best_nr = -1;
  crossword best_cw = NULL;
  int i;

  // Random initialization
  srand(time(NULL));

  // Parse arguments.
  get_args(argc, argv, &input, &width, &height, &verbose, &format);
  if(verbose) fprintf(stderr, "Parsing command line done.\n\n");

  // Reading input file.
  if(verbose) fprintf(stderr, "Reading words from the words file...\n");
  fscan_words(input, &words, verbose);
  if(0 != fclose(input)){
    fprintf(stderr, "Error while closing the input file !\n");
    free_words(words);
    exit(EXIT_FAILURE);
  }
  if(verbose) fprintf(stderr, "Done.\n\n");

  for(i = 0; i < tries; i++){
    // Initialisation of the crossword.
    if(0 != cw_init(&cw, width, height)){
      fprintf(stderr, "Error while initializing the crossword !\n");
      free_words(words);
      exit(EXIT_FAILURE);
    }

    // Computing the crosword.
    if(0 != cw_compute(cw, words)){
      fprintf(stderr, "Error while computing the crossword !\n");
      free_words(words);
      cw_free(cw);
      if(NULL != best_cw) cw_free(best_cw);
      exit(-1);
    }

    // Check if first try.
    if(NULL == best_cw){
      best_cw = cw;
      best_nr = best_cw->nb_words;

      if(verbose)
        fprintf(stderr, "First try, %d/%d words placed.\n",
                        cw->nb_words, words->next_free);
    } else if(cw->nb_words > best_nr){ // Check if better
      cw_free(best_cw);
      best_cw = cw;
      best_nr = best_cw->nb_words;

      if(verbose)
        fprintf(stderr, "Did better: %d/%d words placed.\n",
                        cw->nb_words, words->next_free);
    } else { // Not better...
      cw_free(cw);
      all_words_unused(words);
    }

    // Optimization
    if(best_nr == words->next_free){
      if(verbose) fprintf(stderr, "Optimal reached.\n");
      break;
    }
  }

  if(verbose) fprintf(stderr, "\nNumber of word placed: %d/%d\n\n",
                              best_nr, words->next_free);

  // Can now free the word list.
  if(verbose) fprintf(stderr, "Liberating words memory...\n");
  free_words(words);
  if(verbose) fprintf(stderr, "Done.\n\n");

  // Printing the crossword.
  switch(format){
  case TEXT_FORMAT :
    if(verbose) fprintf(stderr, "Printing crossword in text mode...\n");
    cw_print(stdout, best_cw);
    break;
  case LATEX_FORMAT :
    if(verbose) fprintf(stderr, "Printing crossword in LaTeX mode...\n");
    if(verbose) cw_print(stderr, best_cw);
    if(0 != cw_print_latex(best_cw)){
      fprintf(stderr, "Error while printing the crossword... (LaTeX mode)\n");
      cw_free(best_cw);
      exit(-1);
    }
    break;
  case HTML_FORMAT :
    if(verbose) fprintf(stderr, "Printing crossword in HTML mode...\n");
    if(verbose) cw_print(stderr, best_cw);
    if(0 != cw_print_html(best_cw)){
      fprintf(stderr, "Error while printing the crossword... (HTML mode)\n");
      cw_free(best_cw);
      exit(-1);
    }
    break;
  default :
    break;
  }
  if(verbose) fprintf(stderr, "Done.\n\n");

  // Memory free.
  if(verbose) fprintf(stderr, "Liberating crossword memory...\n");
  cw_free(best_cw);
  if(verbose) fprintf(stderr, "Done.\n\n");

  if(verbose) fprintf(stderr, "End of the program.\n");
  return 0;
}

/*
 * Display help.
 * pname : program name (basically argv[0]).
 */
void display_help(char* pname){
  printf("\n");
  printf("Usage: %s [options]\n", pname);
  printf("  -i file   : set input file.\n");
  printf("  -W int    : set the width of the crosswords.\n");
  printf("  -H int    : set the height of the crosswords.\n");
  printf("  -v        : verbose mode.\n");
  printf("  -l        : latex format.\n");
  printf("  -w        : web format.\n");
  printf("  -h        : display this help message.");
  printf("\n");
  printf("Examples:\n");
  printf("  %s -i words.txt > crossword.txt\n", pname);
  printf("  %s -li words.txt | rubber-pipe --pdf > crossword.pdf\n", pname);
  printf("  %s -wi words.txt > crossword.html\n", pname);
  printf("\n");
}

/*
 * Parse command line arguments.
 * argc : argc.
 * argv : argv.
 * i    : result for input file.
 * w    : result for width.
 * h    : result for height.
 * v    : if set to 1 verbose.
 *        otherwise don't.
 * f    : printing format.
 *        0 default
 *        1 latex
 */
void get_args(int argc, char** argv,
              FILE** i, int* w, int* h, int* v, int* f){
  char opt;
  int n;
  char* fn;

  // Default options.
  *i = NULL;
  *w = DEFAULT_WIDTH;
  *h = DEFAULT_HEIGHT;
  *v = VERBOSE_OFF;
  *f = TEXT_FORMAT;

  // Check for arguments.
  if(1 == argc){
    printf("This program needs arguments...\n");
    display_help(argv[0]);
    exit(EXIT_FAILURE);
  }

  // Read arguments.
  while(-1 != (opt = getopt(argc, argv, "i:W:H:vlwh"))){
    switch(opt){
    case 'i' :
      fn = optarg;
      if(NULL == (*i = fopen(fn, "r"))){
        fprintf(stderr, "Cannot open the input file (may not exist)...");
        exit(EXIT_FAILURE);
      }
      break;
    case 'W' :
      n = atoi(optarg);
      if((n < MIN_SIZE) || (n > MAX_SIZE)){
        fprintf(stderr,
                "The width of the grid must be between %i and %i...\n",
                MIN_SIZE, MAX_SIZE);
        exit(EXIT_FAILURE);
      }
      *w = n;
      break;
    case 'H' :
      n = atoi(optarg);
      if((n < MIN_SIZE) || (n > MAX_SIZE)){
        fprintf(stderr,
                "The height of the grid must be between %i and %i...\n",
                MIN_SIZE, MAX_SIZE);
        exit(EXIT_FAILURE);
      }
      *h = n;
      break;
    case 'v' :
      *v = VERBOSE_ON;
      break;
    case 'l' :
      *f = LATEX_FORMAT;
      break;
    case 'w' :
      *f = HTML_FORMAT;
      break;
    case 'h' :
      display_help(argv[0]);
      exit(EXIT_SUCCESS);
    default  :
      fprintf(stderr, "Unrecognized argument...\n");
      exit(EXIT_FAILURE);
    }
  }

  // Check for compulsary argument (file).
  if(NULL == *i){
    fprintf(stderr, "An input file should be specified...\n");
    exit(EXIT_FAILURE);
  }
}
