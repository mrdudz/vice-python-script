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

viccolors = [ curses.COLOR_BLACK, curses.COLOR_WHITE, curses.COLOR_RED, curses.COLOR_CYAN,
	  curses.COLOR_MAGENTA, curses.COLOR_GREEN, curses.COLOR_BLUE, curses.COLOR_YELLOW,
	  curses.COLOR_YELLOW, curses.COLOR_RED, curses.COLOR_RED, curses.COLOR_WHITE,
	  curses.COLOR_WHITE, curses.COLOR_GREEN, curses.COLOR_BLUE, curses.COLOR_WHITE ]

old_bg = -1

def setupColors(bg) :
	global old_bg
	old_bg = bg
	for i in range(0,16) :
		curses.init_pair(i+1, viccolors[i], bg);

def doVblank():
	try :
		uc = vice.get_mem(0xd018)
		bg = vice.get_mem(0xd021) & 0xf
		if bg != old_bg :
			setupColors(viccolors[bg])

		x = stdscr.getch()
		if x != curses.ERR :
			if x >= curses.KEY_F1 and x <= curses.KEY_F8 :
				x = x - curses.KEY_F1
			elif x == curses.KEY_RIGHT :
				x = 0x10
			elif x == curses.KEY_LEFT :
				x = 0x11
			elif x == curses.KEY_DOWN :
				x = 0x12
			elif x == curses.KEY_UP :
				x = 0x13
			elif x == curses.KEY_HOME :
				x = 0xc
			elif x == curses.KEY_F9 :
				x = 0xf
			elif x == curses.KEY_BACKSPACE :
				x = 8
			vice.put_key(x)
		mem = vice.read_memory(0x400, 25*40)
		colors = vice.read_memory(0xd800, 25*40)

		for i in range(0,len(mem)) :
			x = i % 40
			y = i / 40
			code = ord(mem[i])
			c = ord(colors[i]) & 0xf
			a = curses.color_pair(c + 1)
			if c in (1,10,13,14,7,2) :
				a |= curses.A_BOLD

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
	setupColors(curses.COLOR_BLUE)
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
