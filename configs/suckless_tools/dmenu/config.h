/* sEE license FILE FOR COPYRIGHT AND LICENSE DETAILS. */
/* appearance */
static const unsigned int borderpx = 5;        /* border pixel of windows */
static const unsigned int snap     = 32;       /* snap pixel */
static const int swallowfloating   = 0;        /* 1 means swallow floating windows by default */
static const unsigned int systraypinning = 0;  /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;  /* systray spacing */
static const int systraypinningfailfirst = 1;  /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray       = 1;        /* 0 means no systray */
static const unsigned int gappih   = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv   = 10;       /* vert inner gap between windows */
static const unsigned int gappoh   = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov   = 10;       /* vert outer gap between windows and screen edge */
static const int smartgaps         = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar           = 1;        /* 0 means no bar */
static const int topbar            = 1;        /* 0 means bottom bar */
static const int bhpad             = 8;        /* bar height padding (split between top and bottom) */

static const int vertpad           = 5;       /* vertical padding of bar */
static const int sidepad           = 5;       /* horizontal padding of bar */

static const char *fonts[]         = { "Libertation Mono:size=12" };
static const char dmenufont[]      = "Libertation Mono:size=12";

static const char col_gray1[]      = "#222222";
static const char col_gray2[]      = "#444444";
static const char col_gray3[]      = "#bbbbbb";
static const char col_gray4[]      = "#eeeeee";
static const char col_cyan[]       = "#005577";

static const char col_drBackground[] = "#282a36";
static const char col_drSelection[]  = "#44475a";
static const char col_drForeground[] = "#f8f8f2";
static const char col_drComment[]    = "#6272a4";
static const char col_drCyan[]       = "#8be9fd";
static const char col_drGreen[]      = "#50fa7b";
static const char col_drOrange[]     = "#ffb86c";
static const char col_drPink[]       = "#ff79c6";
static const char col_drPurple[]     = "#bd93f9";
static const char col_drRed[]        = "#ff5555";
static const char col_drYellow[]     = "#f1fa8c";

static const char *colors[][3]       = {
	/*                    fg                bg                border   */
	[SchemeNorm]      = { col_drForeground, col_drBackground,  col_drBackground },
	[SchemeSel]       = { col_drForeground, col_drBackground,  col_drGreen      },
	[SchemeStatus]    = { col_drForeground, col_drBackground,  "#000000"        }, // Statusbar right {text,background,not used but cannot be empty}
	[SchemeTagsSel]   = { col_drForeground, col_drPurple,     "#000000"        }, // Tagbar left selected {text,background,not used but cannot be empty}
    [SchemeTagsNorm]  = { col_drForeground, col_drBackground,  "#000000"        }, // Tagbar left unselected {text,background,not used but cannot be empty}
    [SchemeInfoSel]   = { col_drForeground, col_drBackground,  "#000000"        }, // infobar middle  selected {text,background,not used but cannot be empty}
    [SchemeInfoNorm]  = { col_drForeground, col_drBackground,  "#000000"        }, // infobar middle  unselected {text,background,not used but cannot be empty}
};

/* tagging */
static const char *tags[] = { "⚪", "⚪", "⚪", "⚪", "⚪" };
static const char *filledtags[] = { "⚫", "⚫", "⚫", "⚫", "⚫"};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
        /* class, instance, title, tags mask, isfloating, isfreesize, isterminal, noswallow, monitor */
        { "sxiv",    NULL, NULL,           0, 0, 0, 0,  0, -1 },
        { "zathura", NULL, NULL,           0, 0, 0, 0,  0, -1 },
	{ "st",      NULL, NULL,           0, 0, 0, 1,  1, -1 },
	{ NULL,      NULL, "Event Tester", 0, 1, 0, 0,  1, -1 }, /* xev */
};

/* layout(s) */
static const float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int attachbelow = 1;    /* 1 means attach after the currently active window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "Tiling",      tile },    /* first entry is default */
	{ "Spiral",      spiral},
	{ "Floating",      NULL },    /* no layout function means floating behavior */
	{ "Monocle",      monocle },
	{ "Grid" , gaplessgrid},
	{ "Dwindle",      dwindle},
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont,
    "-nb", col_drBackground, "-nf", col_drForeground,
    "-sb", col_drComment, "-sf", col_drForeground, NULL };
static const char *termcmd[]  = { "st", NULL };

/* user defined program bindings */
static const char *chromiumcmd[]    = { "chromium", "--new-window", NULL };
static const char *firefoxcmd[]    = { "firefox", NULL };
static const char *unicodecmd[]  = { "/home/waza/.scripts/unicode", NULL };
static const char *printZonecmd[]     = { "flameshot", "gui", NULL };
static const char *printFullcmd[]     = { "flameshot", "screen", NULL };
static const char *shutdowncmd[]    = { "sutdown", "now", NULL };

// MODKEY windows
// Mod1Mask left-alt
// ShiftMask left-shift
// ControlMask left-control

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ Mod1Mask,            			XK_p,      spawn,          {.v = dmenucmd } },
	{ Mod1Mask,                     XK_Return, spawn,          {.v = termcmd } },
    { Mod1Mask|ShiftMask,           XK_b,      spawn,          {.v = chromiumcmd } },
    { Mod1Mask,		               	XK_b,      spawn,          {.v = firefoxcmd } },
    { Mod1Mask,                    	XK_u,      spawn,          {.v = unicodecmd } },
    { Mod1Mask,				       	XK_Print,  spawn,          {.v = printZonecmd } },
	{ Mod1Mask|ShiftMask,  		   	XK_Print,  spawn,          {.v = printFullcmd } },
	//directional focus
	{ Mod1Mask,           			XK_l,      focusright,     {0} },					// focus right
	{ Mod1Mask,           			XK_h,      focusleft,      {0} },					// focus left
	{ Mod1Mask,           			XK_k,      focusdown,      {0} },					// focus down
	{ Mod1Mask,           			XK_j,      focusup,        {0} },					// focus up
	
	{ Mod1Mask,			           	XK_Up,	   incrgaps,       {.i = +1} },				// increment all gaps
	{ Mod1Mask,			           	XK_Down,   incrgaps,       {.i = -1} },				// increment all gaps
	
	{ Mod1Mask,           			XK_plus,   incnmaster,     {.i = +1} },				// increment no of masters
	{ Mod1Mask,           			XK_minus,  incnmaster,     {.i = -1 } },			// decrement no of masters
	{ Mod1Mask,                     XK_minus,  setmfact,       {.f = -0.05} },			// increase master width
	{ Mod1Mask,                     XK_plus,   setmfact,       {.f = +0.05} },			// decrease master width

	{ Mod1Mask,             		XK_Return, zoom,           {0} },					// make active master
	{ Mod1Mask,                     XK_Tab,    view,           {0} },					// toggle to last tag
	{ Mod1Mask,		                XK_q,      killclient,     {0} },				
	{ Mod1Mask,                     XK_s,      setlayout,      {.v = &layouts[1]} },
	{ Mod1Mask,                     XK_t,      setlayout,      {.v = &layouts[0]} },
	{ Mod1Mask,                     XK_f,      setlayout,      {.v = &layouts[2]} },
	{ Mod1Mask,                     XK_g,      setlayout,      {.v = &layouts[3]} },
	{ Mod1Mask,                     XK_d,      setlayout,      {.v = &layouts[4]} },

	{ Mod1Mask,		                XK_space,  togglefloating, {0} },
	{ Mod1Mask,                     XK_0,      view,           {.ui = ~0 } },
	{ Mod1Mask|ShiftMask,           XK_0,      tag,            {.ui = ~0 } },
//  change monitor focus, not needed for now
//	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
//	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
//	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
//	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	{ Mod1Mask|ShiftMask,             XK_BackSpace,      spawn,          {.v = shutdowncmd } },
    { Mod1Mask|ShiftMask,             XK_r,           quit,           {1} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = shutdowncmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

