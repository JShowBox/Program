import sys
import httplib
import ConfigParser
from Redfish import cRedfish

class cRedfishBase():
#	def __init__(self):
#		pass;
		
	def Connect(self,mIpAdr,mPort=80):
		self.mIpAdr = mIpAdr;
		self.mPort = mPort;
		self.Rf=cRedfish();
		try:
			self.Rf.connect(mIpAdr,mPort);
		except UnboundLocalError:
			print "Redfish:Unkwon IP address or Port Number";
			exit(1);
		
		#Set Request Header
		self.Rf.setReqHeader("Content-type","application/json");
		self.Rf.setReqHeader("Accept","*/*");
		self.Rf.setReqHeader("Connection","keep-alive");
