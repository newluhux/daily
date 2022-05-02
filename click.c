#include <u.h>
#include <libc.h>
#include <draw.h>
#include <thread.h>
#include <event.h>


/*
from: https://pdos.csail.mit.edu/~rsc/click.c
*/

void
eresized(int new)
{
	Rectangle r;
	r = screen->r;
	if(new && getwindow(display, Refnone) < 0)
		fprint(2,"can't reattach to window");
	draw(screen, screen->r, display->white, nil, ZP);
}

void
main(int argc, char **argv)
{
	Mouse m;
	Image *red, *blue;

	initdraw(0,0,0);
	eresized(0);
	einit(Emouse);
	red = allocimagemix(display, DRed, DRed);
	blue = allocimagemix(display, DBlue, DBlue);
	for(;;){
		m = emouse();
		if(m.buttons & 4)
			break;
		if(m.buttons & 1){
			fillellipse(screen, m.xy, 5, 5, red, ZP);
			do m=emouse(); while(m.buttons & 1);
			fillellipse(screen, m.xy, 5, 5, blue, ZP);
			continue;
		}
		if(m.buttons & 2){
			do m=emouse(); while(m.buttons & 2);
			draw(screen, screen->r, display->white, nil, ZP);
			continue;
		}
		draw(screen, rectaddpt(Rect(0,0,2,2), m.xy), display->black, nil, ZP);
	}
}
