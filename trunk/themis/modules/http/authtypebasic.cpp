/*
Copyright (c) 2004 Raymond "Z3R0 One" Rodgers. All Rights Reserved. 

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation 
files (the "Software"), to deal in the Software without 
restriction, including without limitation the rights to use, 
copy, modify, merge, publish, distribute, sublicense, and/or 
sell copies of the Software, and to permit persons to whom 
the Software is furnished to do so, subject to the following 
conditions: 

   The above copyright notice and this permission notice 
   shall be included in all copies or substantial portions 
   of the Software. 

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY 
KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Original Author & Project Manager: Raymond "Z3R0 One" Rodgers (z3r0_one@users.sourceforge.net)
Project Start Date: October 18, 2000
*/
#include "authtypebasic.h"
#include "authtypedigest.h"
#include "base64.h"
#include <stdio.h>
#include <string.h>
AuthTypeBasic::AuthTypeBasic()
	:AuthType()
{
}
AuthTypeBasic::AuthTypeBasic(const char *host,uint16 port, const char *uri,const char *user,const char *password,const char *realm)
	:AuthType(host,port,uri,user,password,realm)
{
}
AuthTypeBasic::~AuthTypeBasic()
{
}
const char *AuthTypeBasic::Credentials(void)
{
	uint32 unencrypt_length=0,encrypt_length=0;
	unencrypt_length=strlen(User())+strlen(Password())+1;
	const char *unencrypted_credentials=NULL,*encrypted_credentials=NULL;
	if ((Password()!=NULL) && (User()!=NULL))
	{
		const char *password=NULL;
		unencrypted_credentials=new char[unencrypt_length+1];
		memset((char*)unencrypted_credentials,0,unencrypt_length+1);
		strcpy((char*)unencrypted_credentials,User());
		strcat((char*)unencrypted_credentials,":");
		password=Password();
		strcat((char*)unencrypted_credentials,password);
		memset((char*)password,0,strlen(password)+1);
//		printf("Unencrypted Credentials: %s\n",unencrypted_credentials);
		delete password;
		password=NULL;
		encrypt_length=base64::expectedencodedsize(unencrypt_length);
		encrypted_credentials=new char[encrypt_length+1];
		memset((char*)encrypted_credentials,0,unencrypt_length+1);
		base64::encode((void*)unencrypted_credentials,unencrypt_length,(void*)encrypted_credentials,&encrypt_length);
//		printf("auth credentials: %s\n",encrypted_credentials);
		memset((char*)unencrypted_credentials,0,unencrypt_length+1);
		delete unencrypted_credentials;
		unencrypted_credentials=NULL;
	}
	return encrypted_credentials;
}
