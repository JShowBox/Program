#include <stdio.h>
#include <stdlib.h>

#define DBG(fmt,arg...) printf(fmt,##arg)

/*convert str to string("")*/
#define STR(str)	  #str
/*connect two words(not string) CON(xxx) = hello_xxx*/
#define CON(a)        hello_##a
#define CON1(a,b)     a##b
/*Connect two string*/
#define CON2(a,b)     a b

int CON(world) (int arg)
{
	DBG("declare hello_world function\n");
	return 0;	
}

int CON1(hello,world) (int arg)
{
	DBG("declare helloworld function\n");
	return 0;	
}

int main(int argc,char argv[])
{
	
	DBG("This is DBG %s\n","testing");
	
	DBG("STR(str) = %s\n",STR(helloworld));
	
	hello_world(0);
	
	helloworld(0);
	
	DBG("CON2(hello,world) = %s\n",CON2("hello","world"));
	return 0;
}
