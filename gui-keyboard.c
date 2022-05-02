#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>
#include <keyboard.h>

void
eresized(int new) {
	if (new && getwindow(display, Refnone) < 0) {
		sysfatal("can't reattach to window");
	}
}

void
main(int argc,char *argv[]) {
	USED(argc);
	USED(argv);
	
	Event ev;

	int e;
	initdraw(0,0,"Example: Keyboard");
	eresized(0);
	einit(Ekeyboard);

	for (;;) {
		e = event(&ev);
		if (e == Ekeyboard) {
			print("key: %d\n",ev.kbdc);
			if (ev.kbdc == 27) {
				print("Escaped\n");
				break;
			}
		}
	}
}