#ifndef RGBF_H

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

#define RGBF_H
#endif
