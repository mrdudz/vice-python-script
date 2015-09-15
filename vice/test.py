
import sys
import vice
import select


def putString(s) :
	for c in s :
		vice.put_key(ord(c))

def doVblank():
	if sys.stdin in select.select([sys.stdin], [], [], 0)[0] :
		line = sys.stdin.readline()
  		if line :
  			putString(line)

def charOutput(a,x,y) :
	if a == 10 :
		return
	elif a == 13 :
		a = 10
	sys.stdout.write(chr(a))

vice.break_on(0xffd2, charOutput);
vice.on_frame(doVblank)

vice.run_frames(150)
vice.write_memory(0x400, 'ABCDEF')
putString("10 print \"hello\"\n20 goto 10\n")

vice.run_frames(30000)

