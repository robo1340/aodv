import subprocess
from subprocess import Popen
import os
import signal
import time


if __name__	== "__main__":
	
	print('spawning nodes')
	redirect_primary = ''
	redirect = '> /dev/null'
	
	node1 = Popen("./node -a 0000000000000001 -l 0000000000000002 0.95 \
												0000000000000005 0.99 \
												%s" % (redirect_primary,), shell = True)

	node2 = Popen("./node -a 0000000000000002 -l 0000000000000001 0.95 \
												0000000000000005 0.99 \
												%s" % (redirect,), shell = True)
	
	node3 = Popen("./node -a 0000000000000003 -l 0000000000000002	0.95 \
												0000000000000004	0.95 \
								   				0000000000000006	0.99 \
												%s" % (redirect,), shell = True)
	
	node4 = Popen("./node -a 0000000000000004 -l 0000000000000003 0.95 \
												%s" % (redirect,), shell = True)
	
	node5 = Popen("./node -a 0000000000000005 -l 0000000000000001 0.95	 \
								   				0000000000000002	0.99 \
												%s" % (redirect,), shell = True)
	
	node6 = Popen("./node -a 0000000000000006 -l 0000000000000003 0.95 \
												%s" % (redirect,), shell = True)
	
	nodes = [node1,node2,node3,node4,node5,node6]
	
	
	time.sleep(5)
	
	for node in nodes:
		node.terminate()
		
		
	print('shutting down')
	
								





								

								
