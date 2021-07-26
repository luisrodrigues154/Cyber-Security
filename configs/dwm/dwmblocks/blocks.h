//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
	/*Icon*/ /*Command*/ /*Update Interval*/ /*Update Signal*/
	{"  ", "pulsemixer --get-volume | sed \"s/\\ .*/%/g\"", 0, 10},
	{"  ", "date \"+%a %b %d, %Y  %H:%M\"", 5, 0},
};

//sets delimeter between status commands. NULL character ('\0') means no delimeter.
static char delim = ' ';
