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

#include "cache_main.h"
#include "cacheman.h"
cacheman *CacheMan;
status_t Initialize(void *info)
 {
  CacheMan=new cacheman;
  if (CacheMan)
   return B_OK;
  return B_ERROR;
 }
status_t Shutdown(bool now=false)
 {
//  if ((BHandler*)CacheMan->Looper()!=NULL)
//   (BHandler*)(CacheMan->Looper())->RemoveHandler(CacheMan);
  if (now)
  	if (CacheMan!=NULL)
	   delete CacheMan;
  return B_OK;
 }

BHandler* GetHandler()
 {
  return CacheMan;
 }
PlugClass *GetObject(void)
 {
  return CacheMan;
 }
char *GetPluginName(void)
 {
  return CacheMan->PlugName();
 }
int32 GetPluginID(void)
 {
  return CacheMan->PlugID();
 }
float GetPluginVers(void)
 {
  return CacheMan->PlugVersion();
 }
bool IsPersistant(void)
 {
  return true;
 }
