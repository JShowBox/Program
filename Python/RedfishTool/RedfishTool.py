import sys
import getopt
from Redfish import cRedfish
from RedfishCmdTable import cRedfishCmdTable
from RedfishCache import cRedfishCache
	
def Usage(argv = sys.argv):
	print 'Usage: %s [options...] <command>' %(argv[0])
	print 'Options:'
	print '\t-i, --ip           - IP Address of Redfish Server'
	print '\t-p, --port         - Port Number of Redfish Server (default : 80)'
	print '\t-u, --uri          - URI'
	print '\t-c, --clear        - Clear all cache'
	#print '\t-f, --file         - Config File'
	print '\t-h, --help         - Help'
	print
	cRedfishCmdTable().Usage();

def getRefishSchema(ipAdr,iPort,iUri):
	Rf = cRedfish();			
	try:
		Rf.connect(ipAdr,iPort);
	except UnboundLocalError:
		print "Unkwon IP address or Port Number";
	
	#Set Request Header
	Rf.setReqHeader("Content-type","application/json");
	Rf.setReqHeader("Accept","*/*");
	Rf.setReqHeader("Connection","keep-alive");
		
	Rf.getMethod(iUri);
	status,reason = Rf.getResStatus();
	header = Rf.getResHeader();
	msg = Rf.getResMsg() ;

	if isinstance(msg,dict):
		Rf.print_dict(msg,0,False);
	else:
		print msg;

	if status != 200:
		if status == 304:
			print '********The Information Is From Cache********';
		else:
			print 'error:';
			print '  Status Code is %d(%s) with %s ' %(status,reason,iUri);

	print;
	Rf.close();
			
def main(argv = None):
	if argv is None:
		argv = sys.argv;
		
	if len(argv) < 2:
		Usage(argv);
		return;
	try:
		opts,args = getopt.getopt(argv[1:],"i:p:u:hc",["ip=","port=","uri=","help","clear"]);				  		     
	except getopt.error:
		print "Parsing arguments failed";
		return -1;
	
	mIpAdr = None;
	mPort = 80;
	mUri = None;
	mConfFile = None;
	bHelp = False;
	bClearCache = False;
		
	for opt,arg in opts:
		if opt in ("-i","--ip"): 
			mIpAdr = arg;
		elif opt in ("-p","--port"):
			mPort = arg;
		elif opt in ("-u","--uri"):
			mUri = arg;
#		elif opt in ("-f","--file"):
#			mConfFile = arg;
		elif opt in ("-c","--clear"):
			bClearCache = True;
		elif opt in ("-h","--help"):
			bHelp = True;
		else:
			print "Unkown parameter %s" %(opt);
			Usage();
			return 0;

	if bClearCache:
		cRedfishCache().clearCacheFile();	
		print 'Clear Cache Success!!!';
		return 0;

	if mIpAdr is None:
		Usage();
		print "===============ERROR===============";
		print "Parameter: -i,--ip is required!!!";
		print "===================================";
		print ;
		return 0;
		
	#Run Command Table
	if args:
		gOpts = {"IP":mIpAdr,"Port":mPort,"URI":mUri,"Help":bHelp};
		Rfct = cRedfishCmdTable(); #Redfish Command Table
		Rfct.redfishCmdHandler(args,gOpts);
		return 0;
		
	# Print Usage if Help is true	
	if bHelp:
		Usage();
		return 0;
			
#	try:
#		Rf.loadConfigFile(mConfFile)				
#	except UnboundLocalError:
#		pass;

	#Check the ip address and uri
	if mUri is None:
		Usage();
		print "===============ERROR===============";
		print "parameter: -u / <command> is required !!!";
		print "===================================";
		return 0;

	getRefishSchema(mIpAdr,mPort,mUri);
	 	
if __name__ == '__main__':
	sys.exit(main());

