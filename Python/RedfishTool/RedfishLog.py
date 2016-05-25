import sys
import httplib
import ConfigParser
import json
from Redfish import cRedfish
from RedfishBase import cRedfishBase

class cRedfishLog(cRedfishBase):	
#	def __init__(self):
#		cRedfishBase().__init__();
		
	def Usage(self):
		print 'Options:';
		print '    getLog           - Get Logs';
		print '    clearLog         - Clear Logs';
		print ;

	def clearLog(self,vUri):
		self.mUri = vUri;
		mUri = self.mUri+"/Logs/log1"
		mbody = json.dumps({"Action":"ClearLog"});
		self.Rf.postMethod(mUri,body=mbody);
		status,reason = self.Rf.getResStatus();
		header = self.Rf.getResHeader();
		msg = self.Rf.getResMsg();
		if status != 200:
			print 'error:';
			print '  Status Code is %d(%s) with %s ' %(status,reason,mUri);
		else:
			print 'Clear Log Success';

#		if isinstance(msg,dict):
#			self.Rf.print_dict(msg,0,False);
#		else:
#			print msg;	
	
	def getLog(self,vUri,idx):
		self.mUri = vUri;
		mUri = self.mUri+"/Logs/log1/Entries";
		self.Rf.getMethod(mUri);
		status,reason = self.Rf.getResStatus();
		header = self.Rf.getResHeader();
		msg = self.Rf.getResMsg(); 

		if isinstance(msg,dict):
			self.Rf.print_dict(msg,0,False);
		else:
			print msg;

		if status != 200:
			if status == 304:
				print '********The Information is From Cache********';
			else:
				print 'error:';
				print '  Status Code is %d(%s) with %s ' %(status,reason,mUri);

	def run(self,args,opts):
		if len(args) < 2:
			self.Usage();
			return 0;
		
		self.Connect(opts['IP'],opts['Port']);
		
		if args[1] == 'clearLog':
			self.clearLog('/rest/v1/Systems/1');
		elif args[1] == 'getLog':
			self.getLog('/rest/v1/Systems/1',0);
		else:
			print 'Unkown Parameters !!!!'
def main():
	pass;
 
if __name__ == '__main__':
	main();
