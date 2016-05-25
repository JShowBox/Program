import sys
import httplib
import ConfigParser
import json
import os
from RedfishCache import cRedfishCache

class cRedfish:
	def __init__(self):
		self.setDefaultHeader(); #set default Request Headers
		self.mReqBody = "";
			
	def loadConfigFile(self,filename=None):
		if filename is not None:
			config = ConfigParser.RawConfigParser();
			config.read(filename);
			self.ip = config.get('Redfish','server');
			self.port = config.get('Redfish','port');
			self.bConfig = True;
			return 0;
		else:
			print 'Config filename is NULL';
			return -1;
			     
	def connect(self,ip=None,port=80):
		if port is None:	
			self.mConn = httplib.HTTPConnection(ip,80);
		else:
			self.mConn = httplib.HTTPConnection(ip,port);
	   	
	def doMethod(self,action,uri,hdr={},body=""):
		if hdr == {}:
			if self.mReqHdr != {}:
				hdr = self.mReqHdr;
		
		if body == "":
			if self.mReqBody != "":
				body = self.mReqBody;
		
		if action == 'GET':
			#handle it with cache
			#check if the cache file
			mCache = cRedfishCache();
			mCache.openCacheFile(uri);
			self.eTag,self.cacheMsg = mCache.getEtagAndMsg();
			#Add Etag Header
			if self.eTag is not None:
				self.setReqHeader("if-None-Match",self.eTag[:-1]); #eTag

		self.mConn.request(action,uri,body,hdr);
		try:			
			mRes = self.mConn.getresponse();
		except:
			print 'Could Not Get Any Response';
			exit(0);
			
		self.mResHeader = mRes.msg;
		mResMsg = mRes.read();
		self.mStatus = mRes.status;
		self.mReason = mRes.reason;

		if mRes.status == 304:
		#Read Body From Cache
			#print '======================Cache======================';
			try:
				self.mResMsg = json.loads(self.cacheMsg); #this will rearrage the key's location
			except:
				self.mResMsg = self.cacheMsg;
		else:
			mEtag = mRes.getheader('ETag',None);
			#print self.mResHeader; 
			if mEtag is not None:
				#Save Cache Infomation
				mCache.putEtagAndMsg(mEtag,mResMsg);	
			try:
				self.mResMsg = json.loads(mResMsg); #this will rearrage the key's location
			except:
				self.mResMsg = mResMsg;
	    
	def getMethod(self,uri,hdr={},body=""):
	    self.doMethod('GET',uri,hdr,body);
	        
	def patchMethod(self,uri,hdr={},body=""):
	    self.doMethod('PATCH',uri,hdr,body);
	    
	def pushMethod(self,uri,hdr={},body=""):
	    self.doMethod('PUSH',uri,hdr,body);
	    
	def headMethod(self,uri,hdr={},body=""):
	    self.doMethod('HEAD',uri,hdr,body);
	    
	def deleteMethod(self,uri,hdr={},body=""):
	    self.doMethod('DELETE',uri,hdr,body);
	
	def postMethod(self,uri,hdr={},body=""):
	    self.doMethod('POST',uri,hdr,body);
	    
# Request Header Functions -------------------	
	def setReqHeader(self,hdr,value):
		self.mReqHdr[hdr] = value;
		
	def getReqHeader(self):
		return self.mReqHdr;
	
	def clrReqHeader(self):
		self.mReqHdr.clear();

	def setDefaultHeader(self):
		self.mReqHdr = {"USER-Agent":"Redfish Client of Python"}; #set default Request Headers
		
# Get Response Infomations Functions ----------			
	def getResHeader(self): 
	    return self.mResHeader;
	    
	def getResMsg(self):
	    return self.mResMsg;

	def getResStatus(self):
		return (self.mStatus,self.mReason);	
			
	def close(self):
	    self.mConn.close(); 
	
	def print_dict(self,mDict,idx,inArray):
		if not inArray:
			spc = '   ' * idx;
		else:
			spc = '   ' * idx + '  '; 
		if isinstance(mDict,dict):
			for Ks,Vs in mDict.items():
				if idx == 0:
					print '=================================================';
				if isinstance(Vs,dict):
					print spc,Ks,":";
					self.print_dict(Vs,idx+1,inArray);
				elif isinstance(Vs,list):
					print spc,Ks,":",;
					self.print_list(Vs,idx+1);
				else:
					print spc,Ks,":",Vs;
		if idx == 0:
			print '=================================================';
		
	def print_list(self,mList,idx):
		spc = '   ' * idx;
		if isinstance(mList,list):	
			if not mList:
				print ;
			for member in mList:
				if isinstance(member,dict):
					print ;
					print spc,'[';
					self.print_dict(member,idx,True);
					print spc,']';
					#return;
				else:
					print member,;
					if member == mList[-1]:
						print ;
					else:
						print ',', ;		
				
def main():
	Rf=cRedfish();
	Rf.connect('10.162.224.100');
	Rf.getMethod("/rest/v1/Systems");
	header = Rf.getResHeader();
	msg = Rf.getResMsg();
	Rf.loadConfigFile('RedFishConfigFile.cfg');
	print header;
	print msg;
	Rf.close();
 
if __name__ == '__main__':
	main();
