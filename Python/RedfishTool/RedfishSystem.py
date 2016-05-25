import sys
import httplib
import ConfigParser
import json
from Redfish import cRedfish
from RedfishBase import cRedfishBase

class cRedfishSystem(cRedfishBase):

#	def __init__(self):
#		cRedfishBase().__init__();
	def Usage(self):
		print 'Options:';
		print '    enum                           - Enumerate Host System Under Redfish Server'
		print '    system <idx>                   - Get System Information';
		print '    cpu    <idx>                   - Get Cpu Information';
		print '    memory <idx>                   - Get Memory Information';
		print '    power <idx> state              - Get Power State'
		print '    power <idx> on                 - Power On'
		print '    power <idx> off                - Power Off'
		print '    power <idx> nmi                - Power To NMI'
		print '    power <idx> reset              - Power Reset'
		print '    power <idx> coldreset          - Power Coldreset'
		print '    power <idx> GracefulShutdown   - Power GracefulShutdown'
		print '    power <idx> GracefulReset      - Power GracefulReset'
		print ;

	def enum(self):
		mUri = '/rest/v1/Systems';
		self.Rf.getMethod(mUri);
		status,reason = self.Rf.getResStatus();
		header = self.Rf.getResHeader();
		msg = self.Rf.getResMsg();
		if status != 200 and status != 304:
			print 'error:';
			print '  Status Code is %d(%s) with %s ' %(status,reason,mUri);
			return;
		for i in range(0,len(msg['links']['Member']),1):
			print i+1,':',msg['links']['Member'][i]['href'];
		
	def getSystem(self,idx):
		mUri = '/rest/v1/Systems/'+ str(idx);
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
				print '********The Information Is From Cache********';
			else:
				print 'error:';
				print '  Status Code is %d(%s) with %s ' %(status,reason,mUri);

	def getCpuInfo(self,idx):
		mUri = '/rest/v1/Systems/'+ str(idx);
		self.Rf.getMethod(mUri);
		status,reason = self.Rf.getResStatus();
		header = self.Rf.getResHeader();
		msg = self.Rf.getResMsg();

		if status == 200 or status == 304:
			print 'CPU Count:',msg['Processor']['Count'];
			print 'ProcessorFamily',msg['Processor']['ProcessorFamily'];
			print ;
			for cpu in msg['Processor']['Oem']:
				print '******',cpu['Socket'],'******'
				print 'Manufacturer:',cpu['Manufacturer'];
				print 'NumberOfCores:',cpu['NumberOfCores'];
				print 'NumberOfThreads:',cpu['NumberOfThreads'];
				print 'MaxClkSpdMHz:',cpu['MaxClockSpeedMHz'];
				print 'CurClkSpdMHz:',cpu['CurrentClockSpeedMHz'];
				print 'ExtClock:',cpu['ExternalClock'];
				print 'L1Cache:',cpu['L1Cache'];
				print 'L2Cache:',cpu['L2Cache'];
				print 'L3Cache:',cpu['L3Cache'];
				print ;
		if status != 200:
			if status == 304:
				print '********The Information Is From Cache********';
			else:
				print 'error:';
				print '  Status Code is %d(%s) with %s ' %(status,reason,mUri);

	def getMemInfo(self,idx):
		mUri = '/rest/v1/Systems/'+ str(idx);
		self.Rf.getMethod(mUri);
		status,reason = self.Rf.getResStatus();
		header = self.Rf.getResHeader();
		msg = self.Rf.getResMsg();

		if status == 200 or status == 304:
			print 'Total Size:',msg['Memory']['TotalSystemMemoryGB'],'GBytes';
			print ;
			for mem in msg['Memory']['Oem']:
				print '******',mem['Description'],'******'
				print 'Type:',mem['Type'];
				print 'Size:',mem['Size'];
				print 'Speed:',mem['Speed'];
				print 'SerialNumber:',mem['SerialNumber'];
				print 'PartNumber:',mem['PartNumber'];
				print 'Manufacturer:',mem['Manufacturer'];
				print 'Manufacture Date:',mem['Manufacture Date'];
				print ;

		if status != 200:
			if status == 304:
				print '********The Information Is From Cache********';
			else:
				print 'error:';
				print '  Status Code is %d(%s) with %s ' %(status,reason,mUri);

	def getPower(self,idx):
		mUri = '/rest/v1/Systems/' + str(idx);
		self.Rf.getMethod(mUri);
		status,reason = self.Rf.getResStatus();
		header = self.Rf.getResHeader();
		msg = self.Rf.getResMsg();

		if status == 200 or status == 304 :
			print 'power :',msg['Power']
			print ;
		else:
			print 'error:';
			print '  Status Code is %d(%s) with %s ' %(status,reason,mUri);
	def power(self,idx,action):
		mUri = '/rest/v1/Systems/' + str(idx);
		Body = json.dumps({"Action":"Reset","ResetType":action})
		self.Rf.postMethod(mUri,body=Body)
		status,reason = self.Rf.getResStatus();
		header = self.Rf.getResHeader();
		msg = self.Rf.getResMsg();
		if status == 200:
			print 'power ' + action + ' success';
		else:
			print 'error:';
			print '  Status Code is %d(%s) with %s ' %(status,reason,mUri);
	def run(self,args,opts):
		if len(args) < 2:
			self.Usage();
			return 0;

		self.Connect(opts['IP'],opts['Port']);

		if args[1] == 'system':
			if len(args) == 3:
				self.getSystem(args[2]);
			else:
				self.Usage();
		elif args[1] == 'enum':
			if len(args) == 2:
				self.enum();
			else:
				self.Usage();
		elif args[1] == 'cpu':
			if len(args) == 3:
				self.getCpuInfo(args[2]);
			else:
				self.Usage();
		elif args[1] == 'memory':
			if len(args) == 3:
				self.getMemInfo(args[2]);
			else:
				self.Usage();
		elif args[1] == 'power':
			if len(args) == 4:
				if args[3] == 'state':
					self.getPower(args[2]);
				else: 
					self.power(args[2],args[3]);
			else:
				self.Usage();
		else:
			print 'Unkown Parameters !!!!'
def main():
	pass;

if __name__ == '__main__':
	main();
