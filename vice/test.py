
import sys
import vice
import select


def putString(s) :
	for c in s :
		vice.put_key(ord(c))

lastsids = [0] * 32
sids = [0] * 32

def doVblank():
	global lastsids, sids
	try :
		if sys.stdin in select.select([sys.stdin], [], [], 0)[0] :
			line = sys.stdin.readline()
	  		if line :
	  			putString(line)
	  	for i in range(0,0x1d) :
	  		if lastsids[i] != sids[i] :
	  			print "%02x -> %04x" % (sids[i], i + 0xd400)
	  	print "######### VBLANK"
	  	lastsids = list(sids)
	except :
		print "Unexpected error:", sys.exc_info()[1]


def sidWrite(adr, val) :
	#print "%04x <- %02x" % (adr, val)
	sids[adr - 0xd400] = val

def charOutput(a,x,y) :
	try :
		if a == 10 :
			return
		elif a == 13 :
			a = 10
		sys.stdout.write(chr(a))
		sp = vice.get_sp()
		stack = vice.read_memory(0x100 + sp-2, 2)
		ra = ord(stack[0]) | (ord(stack[1])<<8)
		#print "%x [%x %x]" % (sp, ord(stack[0]), ord(stack[1]))
		# Does not work?
		#if a == 65 :
		#	vice.set_sp(sp-2)
		#	vice.set_pc(ra+1)
	except :
		print "Unexpected error:", sys.exc_info()[1]

vice.on_write(0xd400, 0x1d, sidWrite)
vice.break_on(0xffd2, charOutput);
vice.on_frame(doVblank)
vice.warp_frames(150)
#vice.write_memory(0x400, '')
#putString("10 print \"hello\"\n20 goto 10\n")

vice.run_frames(30000)

