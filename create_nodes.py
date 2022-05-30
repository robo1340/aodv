import subprocess
from subprocess import Popen
import os
import signal
import time


if __name__	== "__main__":
	
	print('spawning nodes')
	redirect_primary = ''
	redirect = '> /dev/null'
	
	USE_SHELL = False
	
	log1 = open('log/node1.txt','w')
	log2 = open('log/node2.txt','w')
	log3 = open('log/node3.txt','w')
	log4 = open('log/node4.txt','w')
	log5 = open('log/node5.txt','w')
	log6 = open('log/node6.txt','w')
	logs = [log1,log2,log3,log4,log5,log6]
	
	#result = subprocess.run(['qmicli', '-d', self.device, command], stdout=subprocess.PIPE, timeout=5.0)
	node1 = Popen(["./node", "-a", "0000000000000001", "-l",\
				   "0000000000000002", "0.95",\
				   "0000000000000005", "0.99",\
				   "0000000000000003", "0.99",\
				  ], shell = USE_SHELL)#stdout=log1, shell = USE_SHELL)
	
	node2 = Popen(["./node", "-a", "0000000000000002", "-l",\
				   "0000000000000001", "0.95",\
				   "0000000000000005", "0.99",\
				   ], stdout=log2, shell = USE_SHELL)
		
	node3 = Popen(["./node", "-a", "0000000000000003", "-l",\
				   "0000000000000002", "0.95",\
				   "0000000000000004", "0.99",\
				   "0000000000000006", "0.95",\
				   ], stdout=log3, shell = USE_SHELL)
		
	node4 = Popen(["./node", "-a", "0000000000000004", "-l",\
				   "0000000000000003", "0.95",\
				   ], stdout=log4, shell = USE_SHELL)
		
	node5 = Popen(["./node", "-a", "0000000000000005", "-l",\
				   "0000000000000001", "0.95",\
				   "0000000000000002", "0.99",\
				   ], stdout=log5, shell = USE_SHELL)
	
	node6 = Popen(["./node", "-a", "0000000000000006", "-l",\
				   "0000000000000003", "0.95",\
				   ], stdout=log6, shell = USE_SHELL)

	nodes = [node1,node2,node3,node4,node5,node6]
	
	
	time.sleep(5)
	
	for node in nodes:
		node.send_signal(signal.SIGINT)
		#node.wait()
		#node.terminate()
		#node.kill()
	for log in logs:
		log.close()
	
	
	#for node in nodes:
	#	print(node.poll())
		
	print('shutting down')
	
								





								

								
