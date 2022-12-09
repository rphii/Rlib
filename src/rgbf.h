#ifndef RGBF_H

#define F(msg,fmt) "\033["fmt"m"msg"\033[0m"

#define BOLD    ";1"     /* bold */
#define IT      ";3"     /* italic */
#define UL      ";4"     /* underline */

/* === dimmed foreground colors === */
#define F_KD ";30"    /* black dimmed foreground */
#define F_RD ";31"    /* red dimmed foreground */
#define F_GD ";32"    /* green dimmed foreground */
#define F_YD ";33"    /* yellow dimmed foreground */
#define F_BD ";34"    /* blue dimmed foreground */
#define F_MD ";35"    /* magenta dimmed foreground */
#define F_CD ";36"    /* cyan dimmed foreground */
#define F_WD ";37"    /* white dimmed foreground */

/* === dimmed background colors === */
#define F_Kd ";40"    /* black dimmed background */
#define F_Rd ";41"    /* red dimmed background */
#define F_Gd ";42"    /* green dimmed background */
#define F_Yd ";43"    /* yellow dimmed background */
#define F_Bd ";44"    /* blue dimmed background */
#define F_Md ";45"    /* magenta dimmed background */
#define F_Cd ";46"    /* cyan dimmed background */
#define F_Wd ";47"    /* white dimmed background */

/* === bright foreground colors === */
#define F_KB ";90"    /* black bright foreground */
#define F_RB ";91"    /* red bright foreground */
#define F_GB ";92"    /* green bright foreground */
#define F_YB ";93"    /* yellow bright foreground */
#define F_BB ";94"    /* blue bright foreground */
#define F_MB ";95"    /* magenta bright foreground */
#define F_CB ";96"    /* cyan bright foreground */
#define F_WB ";97"    /* white bright foreground */

/* === bright background colors === */
#define F_Kb ";100"    /* black bright background */
#define F_Rb ";101"    /* red bright background */
#define F_Gb ";102"    /* green bright background */
#define F_Yb ";103"    /* yellow bright background */
#define F_Bb ";104"    /* blue bright background */
#define F_Mb ";105"    /* magenta bright background */
#define F_Cb ";106"    /* cyan bright background */
#define F_Wb ";107"    /* white bright background */

/* === red-green-blue === */
#define RGBF(rgb)   ";38;2;"rgb   /* string like "255;10;45" */
#define RGBB(rgb)   ";48;2;"rgb   /* string like "255;10;45" */

#define RGBF_H
#endif
