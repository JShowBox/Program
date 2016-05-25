from RedfishLog import cRedfishLog
from RedfishAccountService import cRedfishAccountService
from RedfishSystem import cRedfishSystem

class cRedfishCmdTable:
	cmdTable={};
	def __init__(self):
		cRedfishCmdTable.cmdTable['Logs'] = cRedfishLog();
		cRedfishCmdTable.cmdTable['AccountService'] = cRedfishAccountService();
		cRedfishCmdTable.cmdTable['Systems'] = cRedfishSystem();
		
	def addRedfishCmd(self,cmdName,className):
		cRedfishCmdTable.cmdTable[cmdName] = className;
	
	def getRedfishCmd(self,CmdName):
		try:
			return cRedfishCmdTable.cmdTable[CmdName];
		except:
			return None;
		
	def redfishCmdHandler(self,args,opts):
		cmd = self.getRedfishCmd(args[0]);
		if cmd is not None:
			cmd.run(args,opts);
		else:
			print 'Unknow command:',args[0];
			return 0;
			
	def	Usage(self):
		print 'Commands:'
		for key in cRedfishCmdTable.cmdTable.keys():
			print '    ',key;
		print ;
		
def main():
	pass;
 
if __name__ == '__main__':
	main();
