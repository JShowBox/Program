import os
import shutil

cache_dir=".cache";

class cRedfishCache:
	def __init__(self):
		self.cacheRoot = cache_dir;
		if not os.path.exists(self.cacheRoot):
			os.makedirs(self.cacheRoot);		
		
	def checkAndCreateDir(self,path):
		if not os.path.exists(self.cacheRoot+path):
			os.makedirs(self.cacheRoot+path);		
			
	def openCacheFile(self,path,filename='cache.json'):
		self.checkAndCreateDir(path);
		self.mfilename = self.cacheRoot+path+'/'+filename;

				
	def getEtagAndMsg(self):
		fp = open(self.mfilename,'ab+');
		mEtag = fp.readline();
		mMsg = fp.read();
#		if mEtag is None:
#			mEtag = '';
#		if mMsg is None:
#			mMsg = '';
		fp.close();
		return (mEtag,mMsg);
	
	def putEtagAndMsg(self,eTag,mMsg):
		fp = open(self.mfilename,'wb+');
		fp.write(eTag+'\n');
		fp.write(str(mMsg));
		fp.close();

	def getEtag(self):
		fp = open(self.mfilename,'ab+');
		mEtag = fp.readline();
		if mEtag is None:
			mEtag = '';
		fp.close();
		return mEtag;
		
	def getEtag(self):
		fp = open(self.mfilename,'ab+');
		mEtag = fp.readline();
		mMsg = fp.read();
		if mMsg is None:
			mMsg = '';
		fp.close();
		return mMsg;

	def clearCacheFile(self):
		#os.removedirs('.cache');
		shutil.rmtree('.cache');

def main():
	cache = cRedfishCache();
	cache.openCacheFile('/rest/v1/systems/1/','1.json');
	mMsg={"abc":"1234","abc1":"1234","abc2":"1234","abc3":"1234"};
	cache.putEtagAndMsg('ABCDEFGHIJK'+'\n',mMsg);
	eTag,bMsg = cache.getEtagAndMsg();
	print eTag;
	print bMsg;
	
 	return 0;
 	
if __name__ == '__main__':
	main();
