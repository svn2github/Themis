/*
Copyright (c) 2000 Z3R0 One. All Rights Reserved.

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

Original Author & Project Manager: Z3R0 One (z3r0_one@yahoo.com)
Project Start Date: October 18, 2000
*/

/*
Include *both* plugclass.h *and* plugclass.cpp in your plugin!
*/

#include "plugclass.h"
#include <stdio.h>
/*
 The strtoval function takes the first four characters of a string, and converts them to
a 32-bit integer value. This value, when viewed properly, will represent an integer version
of those four characters; similar to what BeOS originated BMessages have as what values.
*/
int32 strtoval(char *proto) {
	int32 value=0;
	if (strlen(proto)>=4) {
	 value=(int(proto[0])<<24);
	 value+=(int(proto[1])<<16);
	 value+=(int(proto[2])<<8);
	 value+=int(proto[3]);
	}
	else
	 {
	  if (strlen(proto)==3) {
		 value=(int(proto[0])<<24);
		 value+=(int(proto[1])<<16);
		 value+=(int(proto[2])<<8);
		 value+=(int('_'));
	  }
	  else
	  	value=-1;
	 }
	return value;
}

/*
The PlugClass constructor can take a BMessage that contains various tidbits of information
that can be used by the plug-in. The down side of this is that currently, this information
is lost because most plug-ins are loaded, passed some information, then destroyed when
the app is started. When the plug-in is brought back into memory depends on what's calling
it, and what the plug-in actually does. For instance, the protocol plug-ins are handed
URL information, and a number of pointers to objects which they might need.
*/
PlugClass::PlugClass(BMessage *info) {
	InitInfo=info;
	thread=0;
	Window=NULL;
	PlugMan=NULL;
	uses_heartbeat=false;
}

PlugClass::~PlugClass() {
}

uint32 PlugClass::PlugID() {
	return 'none';
}

uint32 PlugClass::SecondaryID(){
	return 'none';
}

char *PlugClass::PlugName(){
	return "Not A Plug-in";
}

float PlugClass::PlugVersion(){
	return 0.0;
}

bool PlugClass::NeedsThread(){
	return false;
}

int32 PlugClass::SpawnThread(BMessage *info){
	//See Be Book Documentation on thread creation.
	return 0;
}

int32 PlugClass::StartThread(){
	return (resume_thread(thread));
}

thread_id PlugClass::Thread(){
	return thread;
}

void PlugClass::Stop(){
	atomic_add(&Cancel,1);
}

int32 PlugClass::TypePrimary(){
	return -1;
}

int32 PlugClass::TypeSecondary(){
	return -1;
}

bool PlugClass::IsHandler(){
	return false;
}

BHandler *PlugClass::Handler(){
	return NULL;
}

bool PlugClass::IsPersistant(){
	return false;
}

bool PlugClass::IsLooper(){
	return false;
}

BLooper *PlugClass::Looper(){
	return NULL;
}

void PlugClass::Run(){
}

bool PlugClass::IsView(){
	return false;
}

BView *PlugClass::View(){
	return NULL;
}

BView *PlugClass::Parent(){
	return NULL;
}

entry_ref *PlugClass::SetRef(entry_ref *nuref){
	ref=nuref;
	return ref;
}

entry_ref *PlugClass::Ref(){
	return ref;
}
/*
These functions signal the plug-in that it's either safe to add or remove menu items from
the menu passed as a parameter.
*/
void PlugClass::AddMenuItems(BMenu *menu) {
}
void PlugClass::RemoveMenuItems() {
}

void PlugClass::Heartbeat() {
//This does something whenever called.
	printf("\t[%s] Default Heartbeat action.\n",PlugName());
}

bool PlugClass::RequiresHeartbeat() {
	return uses_heartbeat;
}
