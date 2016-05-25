import sys
import httplib
import ConfigParser
import json
from Redfish import cRedfish
from RedfishBase import cRedfishBase

class cRedfishAccountService(cRedfishBase):	
#	def __init__(self):
	def Usage(self):
		print 'Options:';
		print '    create <idx> <UserName> <Password>  - Create User Account';
		print '    get                                 - Get Account';
		print '    modify <idx> <username> <password>  - Modify the Account Username and Password';
		print '    modify <idx> name <username>        - Modify the Account Username';
		print '    modify <idx> password <password>    - Modify the Account Passowrd';
		print '    delete <idx>                        - Delete User Account';
		print ;

	def getAccount(self,idx):
		mUri = '/rest/v1/AccountService/Accounts';
		self.Rf.getMethod(mUri);
		status,reason = self.Rf.getResStatus();
		header = self.Rf.getResHeader();
		msg = self.Rf.getResMsg(); 
		if status != 200:
			print 'error:';
			print '  Status Code is %d(%s) with %s ' %(status,reason,mUri);
		else:
			for usr in msg['Items']:
				print usr['UserName'],':',usr['links']['self']['href'];
			print;
			#self.Rf.print_dict(msg,0,False);

	def delAccount(self,idx):
		if idx.isdigit() is False:
			print 'error: idx is not a digital';
			return;

		if int(idx) > 10:
			print '<idx> is out of range (1-10)';
			return;

		mUri = '/rest/v1/AccountService/Accounts/' + idx;
		self.Rf.deleteMethod(mUri);
		status,reason = self.Rf.getResStatus();
		if status != 200:
			print 'error';
			print '  Status Code is %d(%s) with %s ' %(status,reason,mUri);
		else:
			print 'delete account success!!';

	def modifyAccount(self,idx,usr,pw):
		if idx.isdigit() is False:
			print 'error: idx is not a digital';
			return;
		if int(idx) > 10:
			print '<idx> is out of range (1-10)';
			return;

		mUri = '/rest/v1/AccountService/Accounts/' + idx;
		dictBody={};
		if usr is not None:
			dictBody['UserName'] = usr;
		if pw is not None:
			dictBody['Password'] = pw;
		mbody = json.dumps(dictBody);
		self.Rf.patchMethod(mUri,body=mbody);
		status,reason = self.Rf.getResStatus();
		header = self.Rf.getResHeader();
		msg = self.Rf.getResMsg();
		if status != 200:
			print 'error:';
			print '  Status Code is %d(%s) with %s ' %(status,reason,mUri);
		else:
			print 'Modify Account Success';

	def createAccount(self,idx,usr,password):
		if idx.isdigit() is False:
			print 'error: idx is not a digital';
			return;
		if int(idx) > 10:
			print '<idx> is out of range (1-10)';
			return;

		mUri = '/rest/v1/AccountService/Accounts';
		mbody = json.dumps({"UserName":usr,"Password":password,"Oem":{"UserId":int(idx)}});
		self.Rf.postMethod(mUri,body=mbody);
		status,reason = self.Rf.getResStatus();
		header = self.Rf.getResHeader();
		msg = self.Rf.getResMsg();
		if status != 200:
			print 'error:';
			print '  Status Code is %d(%s) with %s ' %(status,reason,mUri);
		else:
			print 'Create Account Success';

	def run(self,args,opts):
		if len(args) < 2:
			self.Usage();
			return 0;
		
		self.Connect(opts['IP'],opts['Port']);
		
		if args[1] == 'modify':
			if len(args) == 5:
				if args[3] == 'name':
					self.modifyAccount(args[2],args[4],None);
				elif args[3] == 'password':
					self.modifyAccount(args[2],None,args[4]);
				else:
					self.modifyAccount(args[2],args[3],args[4]);
			else:
				print 'Unkown Parameters !!!!'

		elif args[1] == 'get':
			self.getAccount(0);
		elif args[1] == 'delete':
			if len(args) == 3:
				self.delAccount(args[2]);
			else:
				print 'Parameter error';
		elif args[1] == 'create':
			if len(args) == 5:
				self.createAccount(args[2],args[3],args[4]);
			else:
				print 'Parameter error';
		else:
			print 'Unkown Parameters !!!!'
			
def main():
	pass;

if __name__ == '__main__':
	main();
#	print cmdList;