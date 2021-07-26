/* See LICENSE file for copyright and license details. */
/* Default settings; can be overriden by command line. */

static int instant = 0;
static int topbar = 1;                      /* -b  option; if 0, dmenu appears at bottom     */
/* -fn option overrides fonts[0]; default X11 font or font set */
static const char *fonts[] = {
	"Victor Mono Nerd Font:size=12"
};
static const char *prompt      = NULL;      /* -p  option; prompt to the left of input field */
static const char *colors[SchemeLast][2] = {
	/*     fg         bg       */
	[SchemeNorm] = { "#f8f8f2", "#282a36" },
	[SchemeSel] = { "#f8f8f2", "#6272a4" },
        [SchemeNormHighlight] = { "#f1fa8c", "#282a36" },
	[SchemeSelHighlight] = { "#44475a", "#8be9fd" },
	[SchemeOut] = { "#f8f8f2", "#282a36" },
};
/* -l option; if nonzero, dmenu uses vertical list with given number of lines */
static unsigned int lines      = 0;
static unsigned int lineheight = 0;         /* -h option; minimum height of a menu line     */

/*
 * Characters not considered part of a word while deleting words
 * for example: " /?\"&[]"
 */
static const char worddelimiters[] = " ";
