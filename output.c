/** Copyright (c) 2012-2013 Rodolphe Lepigre ********************************
 * This file is part of CWG.                                                *
 *                                                                          *
 * CWG is free software: you can redistribute it and/or modify it under the *
 * terms of the GNU General Public License as published by the Free Softwa- *
 * re Foundation, either version 3 of the License, or (at your option) any  *
 * later version.                                                           *
 *                                                                          *
 * This program is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABI-  *
 * LITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public Li- *
 * cense for more details.                                                  *
 *                                                                          *
 * You should have received a copy of the GNU General Public License along  *
 * with this program. If not, see <http://www.gnu.org/licenses/>.           *
 *                                                                          *
 ** output.c ****************************************************************
 * 31/01/2013               CWG - Crossword generator                       *
 * Content of this file:                                                    *
 *   Implementation of the function defined in "output.c".                  *
 * Author: Rodolphe Lepigre <rlepigre@gmail.com>                            *
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "cw.h"
#include "const.h"

/*
 * Prints a string using a given number of chars.
 *   f : the file where to print the crossword.
 *   s : the string.
 *   n : the number of chars.
 */
void print_n_chars(FILE *f, char *s, int n){
  int len = strlen(s);
  int i;

  fprintf(f, "%s", s);

  for(i = len; i < n; i++)
    fprintf(f, " ");
}

/*
 * Print a crossword.
 * f  : the file where to print the crossword.
 * cw : the crossword. Must be initialized.
 */
void cw_print(FILE *f, crossword cw){
  int i, j;

  // Print the grid
  fprintf(f, "\n");
  fprintf(f, "  #");
  for(i = 0; i < cw->width + 1; i++) fprintf(f, " #");
  fprintf(f, "\n");
  for(i = 0; i < cw->height; i++){
    fprintf(f, "  #");
    for(j = 0; j < cw->width; j++){
      fprintf(f, " %c", cw->grid[j][i]);
    }
    fprintf(f, " #\n");
  }
  fprintf(f, "  #");
  for(i = 0; i < cw->width + 1; i++) fprintf(f, " #");
  fprintf(f, "\n");

  // Print the clues.
  fprintf(f, "\n");
  for(i = 0; i < cw->nb_words; i++){
    if(cw->words[i].orient == HORIZONTAL) fprintf(f, "Horiz");
    else fprintf(f, "Verti");
    fprintf(f, " (%2d,%2d) ", cw->words[i].posx, cw->words[i].posy);
    print_n_chars(f, cw->words[i].w.w, MAX_WORD_SIZE);
    fprintf(f, " : %s\n", cw->words[i].w.d);
  }
  fprintf(f, "\n");
}

/*
 * Print a crossword in LaTeX mode.
 * cw : the crossword. Must be initialized.
 * Returns 0 if all went well. -1 otherwise.
 */
int cw_print_latex(crossword cw){
  int i, j, k;
  int num;
  int xmax, ymax;

  // Look for real size.
  xmax = 0;
  ymax = 0;
  for(i = 0; i < cw->height; i++)
    for(j = 0; j < cw->width; j++)
      if(cw->grid[j][i] != EMPTY_CHAR){
        if(i > ymax) ymax = i;
        if(j > xmax) xmax = j;
      }

  // LaTeX header
  printf("%% Latex document generated by cwg.\n");
  printf("%% This is free software.\n");
  printf("%% Author : Rodolphe Lepigre (rlepigre@gmail.com)\n");
  printf("%% Adapted by Veronica Brandt (veronica@brandt.id.au)\n");
  printf("\\documentclass[12pt]{article}\n");
  printf("\\usepackage{cwpuzzle}\n");
  printf("\\usepackage{libertine}\n");
  printf("\\usepackage[cm]{fullpage}\n");
  printf("\\usepackage{fancyhdr}\n");
  printf("\\begin{document}\n");
  printf("\\pagestyle{fancy}\n");
  printf("\\fancyhf{}\n");
  printf("\\renewcommand{\\headrulewidth}{0pt}\n");
  printf("\\renewcommand{\\footrulewidth}{0pt}\n");
  printf("\\fancyfoot[C]{A New Book of Old Hymns --- \\the\\year{} --- www.brandt.id.au}\n");
  printf("\\libertine\n");
  printf("\\renewcommand\\PuzzleClueFont{\\rm\\normalsize}\n");
  printf("\\begin{center}\n");
  printf("  \\huge{%s}\n", "Crossword Puzzle:"); // TODO allow custom title
  printf("\\end{center}\n");
  printf("\\vspace{1.5cm}\n");

  // Print the empty grid
  printf("\\begin{Puzzle}{%d}{%d}\n", cw->width, cw->height);
  for(i = 0; i <= ymax; i++){
    printf("  |");
    for(j = 0; j <= xmax; j++){
      if(cw->grid[j][i] == EMPTY_CHAR){
        printf("{}  |");
      }else if(cw->grid[j][i] == SPACE_CHAR){
        printf("*   |");
      }else{
        num = -1;
        for(k = 0; k < cw->nb_words; k++)
          if((cw->words[k].posx == j) && (cw->words[k].posy == i)){
            num = cw->words[k].label;
          }
        if(-1 == num){
          printf("%c   |", cw->grid[j][i]);
        }else{
          printf("[%d]%c|", num, cw->grid[j][i]);
        }
      }
    }
    printf(".\n");
  }
  printf("\\end{Puzzle}\n");

  // Print the clues
  printf("\\begin{PuzzleClues}{\\textbf{Across:}}\n");
  for(i = 0; i < cw->nb_words; i++)
    if(cw->words[i].orient == HORIZONTAL){
      printf("  \\Clue{%d}{%s}{%s}\n", cw->words[i].label,
             cw->words[i].w.w, cw->words[i].w.d);
    }
  printf("\\end{PuzzleClues}\n");
  printf("\\begin{PuzzleClues}{\\textbf{Down:}}\n");
  for(i = 0; i < cw->nb_words; i++)
    if(cw->words[i].orient == VERTICAL){
      printf("  \\Clue{%d}{%s}{%s}\n", cw->words[i].label,
             cw->words[i].w.w, cw->words[i].w.d);
    }
  printf("\\end{PuzzleClues}\n");

  // Print the solution on a new page.
  printf("\\newpage\n");
  printf("\\begin{center}\n");
  printf("  \\huge{%s - Solution}\n", "Crossword Puzzle:"); // TODO allow custom title
  printf("\\end{center}\n");
  printf("\\vspace{1.5cm}\n");
  printf("\\PuzzleSolution\n");
  printf("\\begin{Puzzle}{%d}{%d}\n", cw->width, cw->height);
  for(i = 0; i <= ymax; i++){
    printf("  |");
    for(j = 0; j <= xmax; j++){
      if(cw->grid[j][i] == EMPTY_CHAR){
        printf("{}  |");
      }else if(cw->grid[j][i] == SPACE_CHAR){
        printf("*   |");
      }else{
        num = -1;
        for(k = 0; k < cw->nb_words; k++)
          if((cw->words[k].posx == j) && (cw->words[k].posy == i)){
            num = cw->words[k].label;
          }
        if(-1 == num){
          printf("%c   |", cw->grid[j][i]);
        }else{
          printf("[%d]%c|", num, cw->grid[j][i]);
        }
      }
    }
    printf(".\n");
  }
  printf("\\end{Puzzle}\n");

  // Document footer
  printf("\\end{document}\n");
  return 0;
}

/*
 * Print a crossword in HTML mode.
 * cw : the crossword. Must be initialized.
 * Returns 0 if all went well. -1 otherwise.
 */
int cw_print_html(crossword cw){
  int i, j, k, c;

  int xmin = cw->width;
  int xmax = 0;
  int ymin = cw->height;
  int ymax = 0;

  int num;

  for(i = 0; i < cw->height; i++)
    for(j = 0; j < cw->width; j++)
      if(cw->grid[j][i] != EMPTY_CHAR){
        if(i < ymin) ymin = i;
        if(i > ymax) ymax = i;
        if(j < xmin) xmin = j;
        if(j > xmax) xmax = j;
      }

  // Header
  printf("<!DOCTYPE html>\n");
  printf("<html>\n");
  printf("<head>\n");
  printf("\t<title>Crossword</title>\n");
  printf("\t<meta http-equiv=\"Content-Type\"" \
         " content=\"text/html; charset=utf-8\"/>\n");
  // CSS
  printf("\t<style>\n");
  printf("\t\t#layout {\n");
  printf("\t\t\twidth : 90%%;\n");
  printf("\t\t\tborder : 1px solid black;\n");
  printf("\t\t\tborder-collapse : collapse;\n");
  printf("\t\t\tmargin : auto;\n");
  printf("\t\t}\n");
  printf("\n");
  printf("\t\t.crossword {\n");
  /* printf("\t\t\tborder : 1px solid black;\n"); */
  printf("\t\t\tmargin : auto;\n");
  printf("\t\t\tmargin-top : 20px;\n");
  printf("\t\t\tborder-collapse : collapse;\n");
  printf("\t\t}\n");
  printf("\n");
  printf("\t\t.crossword tr td {\n");
  printf("\t\t\twidth : 30px;\n");
  printf("\t\t\theight : 30px;\n");
  printf("\t\t}\n");
  printf("\n");
  printf("\t\t.letter {\n");
  printf("\t\t\tborder : 1px solid black;\n");
  printf("\t\t\tbackground-color : white;\n");
  printf("\t\t\ttext-align : center;\n");
  printf("\t\t}\n");
  printf("\n");
  printf("\t\t.num {\n");
  printf("\t\t\tfont-size : 8px;\n");
  printf("\t\t\twidth : 0px;\n");
  printf("\t\t\theight : 0px;\n");
  printf("\t\t\tposition : relative;\n");
  printf("\t\t\ttop : -6px;\n");
  printf("\t\t}\n");
  printf("\n");
  printf("\t\t.clues {\n");
  printf("\t\t\tborder : 1px solid black;\n");
  printf("\t\t\tborder-collapse : collapse;\n");
  printf("\t\t\twidth : 90%%;\n");
  printf("\t\t\tmargin : auto;\n");
  printf("\t\t}\n");
  printf("\n");
  printf("\t\t.clues td {\n");
  printf("\t\t\tborder : 1px solid black;\n");
  printf("\t\t\tvertical-align: top;\n");
  printf("\t\t\twidth : 45%%;\n");
  printf("\t\t}\n");
  printf("\n");
  printf("\t\t.clues td h2 {\n");
  printf("\t\t\ttext-align: center;\n");
  printf("\t\t}\n");
  printf("\n");
  printf("\t\t.clues td ul {\n");
  printf("\t\t\tlist-style-type : none;\n");
  printf("\t\t\tpadding : 8px 8px 8px 8px;\n");
  printf("\t\t\ttext-align : justify;\n");
  printf("\t\t}\n");
  printf("\n");
  printf("\t\t.empty {\n");
  printf("\t\t\tbackground-color : white;\n");
  /* printf("\t\t\tbackground-color : black;\n"); */
  printf("\t\t}\n");
  printf("\n");
  printf("\t\t.dark {\n");
  printf("\t\t\tbackground-color : black;\n");
  /* printf("\t\t\tbackground-color : white;\n"); */
  printf("\t\t}\n");
  printf("\n");
  printf("\t\t.center {\n");
  printf("\t\t\ttext-align : center;\n");
  printf("\t\t}\n");
  printf("\t</style>\n");
  // Javascript
  printf("\t<script>\n");
  printf("\t\tvar res = [ ");
  c = 0;
  for(i = ymin; i <= ymax; i++){
    for(j = xmin; j <= xmax; j++)
      if(cw->grid[j][i] != EMPTY_CHAR){
        if(0 == c) printf("\"%c\"", cw->grid[j][i]);
	else printf(", \"%c\"", cw->grid[j][i]);
	c++;
	if(c % 12 == 0) printf("\n\t\t          ");
      }
  }
  printf("];\n");
  printf("\n");

  printf("\t\tvar num = [ ");
  c = 0;
  for(i = ymin; i <= ymax; i++){
    for(j = xmin; j <= xmax; j++)
      if(cw->grid[j][i] != EMPTY_CHAR){
        num = -1;
        for(k = 0; k < cw->nb_words; k++)
          if((cw->words[k].posx == j) && (cw->words[k].posy == i))
            num = cw->words[k].label;
	if(0 == c) printf("%i", num);
	else printf(", %i", num);
	c++;
	if(c % 12 == 0) printf("\n\t\t          ");
      }
  }
  printf("];\n");
  printf("\n");

  printf("\t\tvar locked = new Array();\n");
  printf("\t\tfor(i = 0; i < res.length; i++) locked[i] = false;\n");
  printf("\n");
  printf("\t\tvar selected = null;\n");
  printf("\t\tvar selecnum = -1;\n");
  printf("\n");
  printf("\t\tfunction clickfun(o, i){\n");
  printf("\t\t\tif(selected != null) selected.style.backgroundColor = " \
         " \"white\";\n");
  printf("\t\t\tselected = null;\n");
  printf("\t\t\tselecnum = -1;\n");
  printf("\t\t\tif(!locked[i]){\n");
  printf("\t\t\t\tselected = o;\n");
  printf("\t\t\t\tselecnum = i;\n");
  printf("\t\t\t\to.style.backgroundColor = \"#DDDDDD\";\n"),
  printf("\t\t\t}\n");
  printf("\t\t}\n");
  printf("\n");
  printf("\t\tfunction setletter(e){\n");
  printf("\t\t\tif(selected != null){\n");
  printf("\t\t\t\tcode = e.charCode;\n");
  printf("\t\t\t\tif(code >= 97) code = code - 97 + 65;\n");
  printf("\t\t\t\tif(code >= 65 && code <= 90){\n");
  printf("\t\t\t\t\tif(num[selecnum] == -1)\n");
  printf("\t\t\t\t\t\tselected.innerHTML = String.fromCharCode(code);\n");
  printf("\t\t\t\t\telse\n");
  printf("\t\t\t\t\t\tselected.innerHTML = \"<div class=\\\"num\\\">\" + num[selecnum]\n");
  printf("\t\t\t\t\t\t\t+ \"</div>\" + String.fromCharCode(code);\n");
  printf("\t\t\t\t\tselected.style.backgroundColor = \"white\";\n");
  printf("\t\t\t\t\tselected = null;\n");
  printf("\t\t\t\t\tselecnum = -1;\n");
  printf("\t\t\t\t}\n");
  printf("\t\t\t}\n");
  printf("\t\t}\n");
  printf("\n");
  printf("\t\tfunction docheck(){\n");
  printf("\t\t\tfor(i = 0; i < res.length; i++){\n");
  printf("\t\t\t\tstr = document.getElementById('l' + i).innerHTML;\n");
  printf("\t\t\t\tif(str != \"\" && str != \"<div class=\\\"num\\\">\" + num[i] + \"</div>\"){\n");
  printf("\t\t\t\t\tif(str == res[i] || str == \"<div class=\\\"num\\\">\" + num[i] + \"</div>\" + res[i]){\n");
  printf("\t\t\t\t\t\tlocked[i] = true;\n");
  printf("\t\t\t\t\t\tdocument.getElementById('l' + i).style" \
         ".backgroundColor = \"#00DD00\"\n");
  printf("\t\t\t\t\t} else {\n");
  printf("\t\t\t\t\t\tdocument.getElementById('l' + i).style" \
         ".backgroundColor = \"#DD0000\"\n");
  printf("\t\t\t\t\t}\n");
  printf("\t\t\t\t}\n");
  printf("\t\t\t}\n");
  printf("\t\t}\n");
  printf("\n");
  printf("\t\tfunction solve(){\n");
  printf("\t\t\tfor(i = 0; i < res.length; i++){\n");
  printf("\t\t\t\tif(num[i] == -1)\n");
  printf("\t\t\t\t\tdocument.getElementById('l' + i).innerHTML = res[i];\n");
  printf("\t\t\t\telse\n");
  printf("\t\t\t\t\tdocument.getElementById('l' + i).innerHTML = \"<div class=\\\"num\\\">\"\n");
  printf("\t\t\t\t\t\t+ num[i] + \"</div>\" + res[i]\n");
  printf("\t\t\t\tdocument.getElementById('l' + i).style" \
         ".backgroundColor = \"#00DD00\"\n");
  printf("\t\t\t}\n");
  printf("\t\t\tselected = null;\n");
  printf("\t\t\tselecnum = -1;\n");
  printf("\t\t}\n");
  printf("\n");
  printf("\t\tfunction reinit(){\n");
  printf("\t\t\tfor(i = 0; i < res.length; i++){\n");
  printf("\t\t\t\tif(num[i] == -1)\n");
  printf("\t\t\t\t\tdocument.getElementById('l' + i).innerHTML = \"\";\n");
  printf("\t\t\t\telse\n");
  printf("\t\t\t\t\tdocument.getElementById('l' + i).innerHTML = \"<div class=\\\"num\\\">\"\n");
  printf("\t\t\t\t\t\t+ num[i] + \"</div>\"\n");
  printf("\t\t\t\tdocument.getElementById('l' + i).style" \
         ".backgroundColor = \"white\"\n");
  printf("\t\t\t\tlocked[i] = false;\n");
  printf("\t\t\t}\n");
  printf("\t\t\tselected = null;\n");
  printf("\t\t\tselecnum = -1;\n");
  printf("\t\t}\n");
  printf("\t</script>\n");

  // Body, crosswords
  printf("</head>\n");
  printf("<body onkeypress=\"setletter(event);\">\n");
  printf("\t<table id=\"layout\">\n");
  printf("\t\t<tr>\n");
  printf("\t\t\t<td colspan=\"2\">\n");
  printf("\t\t\t\t<table class=\"crossword\">\n");
  c = 0;
  for(i = ymin; i <= ymax; i++){
    printf("\t\t\t\t\t<tr>\n");

    for(j = xmin; j <= xmax; j++){
      printf("\t\t\t\t\t\t<td class=\"");

      if(cw->grid[j][i] == SPACE_CHAR){
        printf("dark\"");
        // TODO
      }else if(cw->grid[j][i] != EMPTY_CHAR){
        // Check if the cell is the first letter of a word...
        num = -1;
        for(k = 0; k < cw->nb_words; k++)
          if((cw->words[k].posx == j) && (cw->words[k].posy == i)){
            num = cw->words[k].label;
          }

        printf("letter\" id=\"l%i\" onclick=\"clickfun(this, %i);\">", c, c);
        if(-1 != num) printf("<div class=\"num\">%i</div>", num);
	      c++;
      } else {
        printf("empty\">");
      }

      printf("</td>\n");
    }

    printf("\t\t\t\t\t</tr>\n");
  }
  printf("\t\t\t\t</table>\n");
  printf("\t\t\t\t<p class=\"center\">\n");
  printf("\t\t\t\t\t<input type=\"button\" value=\"Check\"" \
         "onclick=\"docheck();\"/>\n");
  printf("\t\t\t\t\t<input type=\"button\" value=\"Solve\"" \
         "onclick=\"solve();\"/>\n");
  printf("\t\t\t\t\t<input type=\"button\" value=\"Reinit\"" \
         "onclick=\"reinit();\"/>\n");
  printf("\t\t\t\t</p>\n");
  printf("\t\t\t</td>\n");
  printf("\t\t</tr>\n");

  // Body clues.
  printf("\t\t<tr class=\"clues\">\n");
  printf("\t\t\t<td>\n");
  printf("\t\t\t\t<h2>Across</h2>\n");
  printf("\t\t\t\t<ul>\n");
  for(i = 0; i < cw->nb_words; i++)
    if(cw->words[i].orient == HORIZONTAL)
      printf("\t\t\t\t\t<li>%i - %s</li>\n", cw->words[i].label, cw->words[i].w.d);
  printf("\t\t\t\t</ul>\n");
  printf("\t\t\t</td>\n");
  printf("\t\t\t<td>\n");
  printf("\t\t\t\t<h2>Down</h2>\n");
  printf("\t\t\t\t<ul>\n");
  for(i = 0; i < cw->nb_words; i++)
    if(cw->words[i].orient == VERTICAL)
      printf("\t\t\t\t\t<li>%i - %s</li>\n", cw->words[i].label, cw->words[i].w.d);
  printf("\t\t\t\t</ul>\n");
  printf("\t\t\t</td>\n");
  printf("\t\t</tr>\n");
  printf("\t</table>\n");
  printf("</body>\n");
  printf("</html>\n");

  return 0;
}
