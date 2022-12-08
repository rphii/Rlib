#ifndef RGBIO_H

#if 0
#include <stdarg.h>

#include "rstr.h"
#include "try.h"

#define RGBIO_RESET         "#[reset]"
#define RGBIO_BOLD          "#[bold]"
#define RGBIO_UNDERLINE     "#[underline]"
#define RGBIO_FOREGROUND    "#[foreground]"
#define RGBIO_BACKGROUND    "#[background]"

#define RGBIO_BLACK     "#[blue]"
#define RGBIO_RED       "#[red]"
#define RGBIO_GREEN     "#[green]"
#define RGBIO_YELLOW    "#[yellow]"
#define RGBIO_BLUE      "#[blue]"
#define RGBIO_MAGENTA   "#[magenta]"
#define RGBIO_CYAN      "#[cyan]"
#define RGBIO_WHITE     "#[white]"


#define RGBIO_RS    "#[rs]"
#define RGBIO_BO    "#[bo]"
#define RGBIO_UL    "#[ul]"
#define RGBIO_FG    "#[fg]"
#define RGBIO_BG    "#[bg]"

#define RGBIO_BK    "#[bl]"
#define RGBIO_RD    "#[rd]"
#define RGBIO_GN    "#[gn]"
#define RGBIO_YL    "#[yl]"
#define RGBIO_BL    "#[bl]"
#define RGBIO_MG    "#[mg]"
#define RGBIO_CY    "#[cy]"
#define RGBIO_WT    "#[wt]"

#define RGBIO_RGB(rd,gn,bl) "#[r"rd"g"gn"b"bl"]"

void rgbio_printf(char *fmt, ...);
void rgbio_fprintf();
void rgbio_snprintf();
#endif

#define F(msg,fmt) "\033["fmt"m"msg"\033[0m"

#define BOLD    ";1"     /* bold */
#define IT      ";3"     /* italic */
#define UL      ";4"     /* underline */

/* === dimmed foreground colors === */
#define BK_DF ";30"    /* black */
#define RD_DF ";31"    /* red */
#define GN_DF ";32"    /* green */
#define YL_DF ";33"    /* yellow */
#define BL_DF ";34"    /* blue */
#define MG_DF ";35"    /* magenta */
#define CY_DF ";36"    /* cyan */
#define WT_DF ";37"    /* white */

/* === dimmed background colors === */
#define BK_DB ";40"    /* black */
#define RD_DB ";41"    /* red */
#define GN_DB ";42"    /* green */
#define YL_DB ";43"    /* yellow */
#define BL_DB ";44"    /* blue */
#define MG_DB ";45"    /* magenta */
#define CY_DB ";46"    /* cyan */
#define WT_DB ";47"    /* white */

/* === bright foreground colors === */
#define BK_BF ";90"    /* black */
#define RD_BF ";91"    /* red */
#define GN_BF ";92"    /* green */
#define YL_BF ";93"    /* yellow */
#define BL_BF ";94"    /* blue */
#define MG_BF ";95"    /* magenta */
#define CY_BF ";96"    /* cyan */
#define WT_BF ";97"    /* white */

/* === bright background colors === */
#define BK_BB ";100"    /* black */
#define RD_BB ";101"    /* red */
#define GN_BB ";102"    /* green */
#define YL_BB ";103"    /* yellow */
#define BL_BB ";104"    /* blue */
#define MG_BB ";105"    /* magenta */
#define CY_BB ";106"    /* cyan */
#define WT_BB ";107"    /* white */

/* === red-green-blue === */
#define RGBF(rgb)   ";38;2;"rgb   /* string like "255;10;45" */
#define RGBB(rgb)   ";48;2;"rgb   /* string like "255;10;45" */

#define RGBIO_H
#endif
