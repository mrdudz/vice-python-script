# -*- coding: utf-8 -*-
import sys
import vice
import curses

stdscr = None
mainwin = None
start = 0x400

screencodes  = u"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]↑← !\"#$%&'()*+,-./0123456789:;<=>?─♠│───_││╮╰╯?╲╱??●_♥?╭╳○♣?♦┼?│π◥ ▌▄▔▁▎▒??◤?├▗└┐▂┌┴┬┤▎▍???▃✓▖▝┘▘▚"
screencodes2 = u"@abcdefghijklmnopqrstuvwxyz[\\]↑← !\"#$%&'()*+,-./0123456789:;<=>?─ABCDEFGHIJKLMNOPQRSTUVWXYZ   ▒  ▌▄▔▁▎▒??◤?├▗└┐▂┌┴┬┤▎▍???▃✓▖▝┘▘▚"

import locale
locale.setlocale(locale.LC_ALL, '')

def putString(s) :
	for c in s :
		vice.put_key(ord(c))

def doVblank():
	global start
	try :
		uc = ord(vice.get_memory(0xd018, 1)[0])

		x = stdscr.getch()
		if x != curses.ERR :
			if x == curses.KEY_F1 :
				start = (start + 0x400) % 0x10000
			else :
				vice.put_key(x)
		mem = vice.get_memory(start, 25*40)

		for i in range(0,len(mem)) :
			x = i % 40
			y = i / 40
			code = ord(mem[i])
			a = 0
			if code >= 0x80 :
				code %= 0x80
				a |= curses.A_REVERSE
			if code < len(screencodes) :
				if uc & 2 == 0 :
					stdscr.addstr(y, x, screencodes[code].encode('utf-8'), a)
				else :
					stdscr.addstr(y, x, screencodes2[code].encode('utf-8'), a)
		#stdscr.refresh()
	except :
		print "Unexpected error:", sys.exc_info()[1]


def runThis() :
	global stdscr, mainwin
	stdscr = curses.initscr()
	curses.start_color()
	curses.noecho()
	#stdscr.cbreak()
	curses.curs_set(0)
	stdscr.nodelay(1)
	stdscr.keypad(1)

	vice.on_frame(doVblank)
	vice.run_frames(30000)

#################################################################
try :
	runThis()
except :
	print "Unexpected error:", sys.exc_info()[1]
