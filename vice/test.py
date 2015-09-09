
import sys
import vice

def charOutput(a,x,y) :
	if a == 0xd :
		a = 10;

	#sys.stdout.write(chr(a))
	vice.write(chr(a))
	#a = 'X'  #vice.get_a()
	#print("%d %d %d\n", (a,x,y))
	#vice.rts()
	#print(chr(a), end='')

vice.output('PIRATES')
vice.attach_disk("EdgeOfDisgrace_0.d64")

vice.break_on(0xffd2, charOutput);

vice.run_frames(3000)
