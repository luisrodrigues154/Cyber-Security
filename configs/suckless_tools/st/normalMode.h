void normalMode();
void historyPreDraw();
void historyOverlay(int x, int y, Glyph* g);
void historyModeToggle(int start);
/// Handles keys in normal mode.
typedef enum {failed=0, succ=1, exitMotion=2, exitOp=3, finished=4} ExitState;
ExitState kpressHist(char const *txt, int len, int ctrl, KeySym const *kSym);
