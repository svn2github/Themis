
#locations for various files
#To change the basic build location for the app and plugins, just change BASE_INSTALL_DIR.
#Remember to include the '/' at the end.

#target directories
BASE_INSTALL_DIR=Themis/
BASE_ADDON_INSTALL_DIR=$(BASE_INSTALL_DIR)add-ons/
BASE_APP_LIB_INSTALL_DIR=$(BASE_INSTALL_DIR)lib/

#source directories (if needed)
BASE_ADDON_SOURCE_DIR=modules/

#depends directories
BASE_DEPENDS_DIR=depends/
BASE_LIBRARY_DEPENDS_DIR=$(BASE_DEPENDS_DIR)lib/
FRAMEWORK_DEPENDS_DIR=$(BASE_DEPENDS_DIR)framework/
BASE_ADDONS_DEPENDS_DIR=$(BASE_DEPENDS_DIR)add-ons/
COMMON_DEPENDS_DIR=$(BASE_DEPENDS_DIR)comon/
DOMLIBRARY_DEPENDS_DIR=$(BASE_LIBRARY_DEPENDS_DIR)DOM/
TESTPLUG_DEPENDS_DIR=$(BASE_DEPENDS_DIR)testplug/
HTMLPARSER_DEPENDS_DIR=$(BASE_DEPENDS_DIR)HTMLParser/

#object directories
BASE_OBJECT_DIR=objects/
BASE_LIBRARY_OBJECT_DIR=$(BASE_OBJECT_DIR)lib/
FRAMEWORK_OBJECT_DIR=$(BASE_OBJECT_DIR)framework/
COMMON_OBJECT_DIR=$(BASE_OBJECT_DIR)common/
BASE_ADDONS_OBJECT_DIR=$(BASE_OBJECT_DIR)add-ons/
DOMLIBRARY_OBJECT_DIR=$(BASE_LIBRARY_OBJECT_DIR)DOM/
TESTPLUG_OBJECT_DIR=$(BASE_OBJECT_DIR)testplug/
HTMLPARSER_OBJECT_DIR=$(BASE_OBJECT_DIR)HTMLParser/

#source files
FRAMEWORKSOURCES= \
	framework/main.cpp \
	framework/app.cpp \
	framework/win.cpp \
	framework/winview.cpp \
	framework/infowin.cpp \
	framework/aboutview.cpp \
	framework/aboutwin.cpp \
	framework/appaboutview.cpp
#	framework/htmlparser.cpp
COMMONSOURCES= \
	common/plugclass.cpp \
	common/tcplayer.cpp \
	common/plugman.cpp 
#	common/lockres.cpp
#add similar lines for new plugins, libraries, etc.
HTTPADDONSOURCES= \
	http/http_main.cpp \
	http/http_proto.cpp \
	http/httplayer.cpp \
	http/optionshandler.cpp \
	http/authvw.cpp
#the above lines are commented out in part or whole because the new sources are not yet ready to go into the repository
CACHEADDONSOURCES= \
	cache/cache_main.cpp \
	cache/cacheman.cpp
COOKIEADDONSOURCES= \
	cookie/cookiemain.cpp \
	cookie/cookieman.cpp
DOMLIBRARYSOURCES= \
	framework/DOM/TNode.cpp \
	framework/DOM/TDOMException.cpp \
	framework/DOM/TNamedNodeMap.cpp \
	framework/DOM/TNodeListContainer.cpp \
	framework/DOM/TNodeList.cpp \
	framework/DOM/TElement.cpp \
	framework/DOM/TAttr.cpp \
	framework/DOM/TCharacterData.cpp \
	framework/DOM/TText.cpp \
	framework/DOM/TDocument.cpp
DOMTESTSOURCES= \
	framework/DOM/DOMTest.cpp
DOMLOADTESTSOURCES= \
	framework/DOM/LoadTest.cpp
JAVASCRIPT_LIBRARY= \
	js
JAVASCRIPT_SUPPORT_LIBRARIES= \
	javascript/libjs.so \
	javascript/libnspr4.so \
	javascript/libplc4.so \
	javascript/libplds4.so
JSADDONSOURCES= \
	js/jsmain.cpp \
	js/jsman.cpp
TESTPLUGSOURCES= \
	testplug/testplug.cpp
HTMLPARSERSOURCES= \
	HTMLParser/HTMLParser.cpp
DOMVIEWSOURCES= \
	DOMView/DOMView.cpp
	
#make install directories as necessary
ifeq "$(shell if [ -d $(BASE_ADDON_INSTALL_DIR) ] ; then echo 1 ; else mkdir -p $(BASE_ADDON_INSTALL_DIR) ; echo 0 ; fi)" "0"
endif
#library directory creation is commented out until Themis *has* a library, such as DOM
ifeq "$(shell if [ -d $(BASE_APP_LIB_INSTALL_DIR) ] ; then echo 1 ; else mkdir -p $(BASE_APP_LIB_INSTALL_DIR) ; echo 0 ; fi)" "0"
endif

#Make object directories as necessary
ifeq "$(shell if [ -d $(FRAMEWORK_OBJECT_DIR) ] ; then echo 1 ; else mkdir -p $(FRAMEWORK_OBJECT_DIR) ; echo 0 ; fi)" "0"
endif
ifeq "$(shell if [ -d $(COMMON_OBJECT_DIR) ] ; then echo 1 ; else mkdir -p $(COMMON_OBJECT_DIR) ; echo 0 ; fi)" "0"
endif
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR) ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR) ; echo 0 ; fi)" "0"
endif
#add a new line similar to those below for each plug-in/add-on
#http add-on object directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)http/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)http/ ; echo 0 ; fi)" "0"
endif
#cache add-on object directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)cache/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)cache/ ; echo 0 ; fi)" "0"
endif
#cookie add-on object directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)cookie/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)cookie/ ; echo 0 ; fi)" "0"
endif
#js add-on object directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)js/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)js/ ; echo 0 ; fi)" "0"
endif
#DOM Library object directory
ifeq "$(shell if [ -d $(DOMLIBRARY_OBJECT_DIR) ] ; then echo 1 ; else mkdir -p $(DOMLIBRARY_OBJECT_DIR) ; echo 0 ; fi)" "0"
endif
#HTML Parser object directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)HTMLParser/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)HTMLParser/ ; echo 0 ; fi)" "0"
endif
#DOM View object directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)DOMView/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)DOMView/ ; echo 0 ; fi)" "0"
endif

#testplug add-on object directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)testplug/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)testplug/ ; echo 0 ; fi)" "0"
endif

#Make depends directories as necessary
ifeq "$(shell if [ -d $(FRAMEWORK_DEPENDS_DIR) ] ; then echo 1 ; else mkdir -p $(FRAMEWORK_DEPENDS_DIR) ; echo 0 ; fi)" "0"
endif
ifeq "$(shell if [ -d $(COMMON_DEPENDS_DIR) ] ; then echo 1 ; else mkdir -p $(COMMON_DEPENDS_DIR) ; echo 0 ; fi)" "0"
endif
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR) ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR) ; echo 0 ; fi)" "0"
endif
#add a new line similar to those below for each plug-in/add-on
#http add-on depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR)http/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR)http/ ; echo 0 ; fi)" "0"
endif
#cache add-on depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR)cache/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR)cache/ ; echo 0 ; fi)" "0"
endif
#cookie add-on depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR)cookie/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR)cookie/ ; echo 0 ; fi)" "0"
endif
#javascript add-on depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR)js/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR)js/ ; echo 0 ; fi)" "0"
endif
#DOM Library depends directory
ifeq "$(shell if [ -d $(DOMLIBRARY_DEPENDS_DIR) ] ; then echo 1 ; else mkdir -p $(DOMLIBRARY_DEPENDS_DIR) ; echo 0 ; fi)" "0"
endif
#HTML Parser depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR)HTMLParser/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR)HTMLParser/ ; echo 0 ; fi)" "0"
endif
#DOM View depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR)DOMView/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR)DOMView/ ; echo 0 ; fi)" "0"
endif
#testplug add-on depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR)testplug/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR)testplug/ ; echo 0 ; fi)" "0"
endif

#Detect whether net_server or BONE is installed, and set the appropriate flags
#to compile in/out appropriate sections. Also do the same for OpenSSL.
#These can be detected with #ifdef statements. For instance:
#	#ifdef USEBONE
#	printf("BONE is used on this system\n");
#	#else
#	printf("NET_SERVER is used on this system\n");
#	#endif
#While this doesn't have a major effect on current code, the new
#networking support will utilize it.
NETWORKLIBS=-lsocket -lbind -lbnetapi -lopenssl
NETWORKING=NONE
# by default, debugging information will be included. to turn it off, remove or comment out (#) the -g
# on the next line 
COMPILE_FLAGS=-g -D_Themis_
ifeq "$(shell if [ -f /system/lib/libsocket.so ] ; then echo 1 ; else echo 0 ; fi)" "1"
	NETWORKLIBS=-lsocket -lbind -lbnetapi
	NETWORKING=BONE
	COMPILE_FLAGS+= -DUSEBONE
else
	NETWORKLIBS=-lnet -lnetapi
	NETWORKING=net_server
	COMPILE_FLAGS+= -DUSENETSERVER
endif
ifeq "$(shell if [ -f /boot/develop/lib/x86/libopenssl.so ] ; then echo 1 ; else echo 0 ; fi)" "1"
	NETWORKLIBS+= -lopenssl
	COMPILE_FLAGS+= -DUSEOPENSSL
else
	ifeq "$(shell if [ -f /boot/develop/lib/x86/libcrypto.so ] ; then echo 1 ; else echo 0 ; fi)" "1"
		NETWORKLIBS+= -lcrypto -lssl
		COMPILE_FLAGS+= -DUSEOPENSSL
	endif
endif

#These are the make targets. The only two that should really need to be called directly are "all" and "clean".
#However, there might be times when you want to compile just the framework, add-ons, or an individual component
#such as a specific add-on. To that end, here is a list of valid targets:
#			all
#			addons
#			clean
#			framework
#			cacheaddon
#			cookieaddon
#			jsaddon
#			htmlpaddon
#			httpaddon
#			help
#			dist
#			domlib
#			libs
#			domtest
#			domloadtest
#			testplug
all: framework addons domtest domloadtest
	@echo "Themis application and add-ons built successfully."
addons: httpaddon cacheaddon htmlpaddon cookieaddon jsaddon
	@echo "All add-ons built successfully."
clean: 
	@rm -rf objects depends Themis
	@echo Done.
cleanframe:
	@rm -rf objects/framework objects/common depends/framework depends/common Themis/Themis
	@echo Done.
cleanlibs:
	@rm -rf objects/lib depends/lib Themis/lib
	@echo Done.
cleanaddon:
	@rm -rf objects/add-ons objects/common depends/add-ons depends/common Themis/add-ons
	@echo Done.
help:
	@echo "To compile, type \"make <target>\", where \"<target>\" is one or more of the following:"
	@echo "	all		All of Themis"
	@echo "	addons		Just the add-ons"
	@echo "	clean		Remove the dependency, object, and binary files"
	@echo "	cleanframe	Remove the dependency, object, and binary files for the framework"
	@echo "	cleanaddon	Remove the dependency, object, and binary files for the add-ons"
	@echo "	cleanlibs	Remove the dependency, object, and binary files for the libraries"
	@echo "	framework	Just the Themis framework"
	@echo "	cacheaddon	Just the cache addon"
	@echo "	cookieaddon	Just the cookie addon"
	@echo "	jsaddon		Javascript addon"
	@echo "	htmlpaddon	HTML Parser addon"
	@echo "	httpaddon	Just the HTTP addon"
	@echo "	help		This message"
	@echo "	dist		Distribution ready zip file"
	@echo "	domlib		DOM library"
	@echo "	libs		Just the ibraries"
	@echo "	domtest		DOM test application"
	@echo "	domloadtest	DOM LoadTest application"
	@echo "	testplug	A sample/test plug-in"
	@echo "*** NOTE ***"
	@echo "You will get errors if you use any of the \"clean\" targets in combination with any of the build targets."
libs: domlib
dist: all
	@zip -ry9D Themis.zip Themis/
	@echo "Themis.zip has been created."

#framework application
framework: domlib frameworkdepends commondepends frameworkobjects commonobjects
	@echo "Linking Themis."
	@$(CC) -o$(BASE_INSTALL_DIR)Themis $(foreach file,$(notdir $(FRAMEWORKSOURCES:.cpp=.o)),$(FRAMEWORK_OBJECT_DIR)$(file)) $(foreach file,$(notdir $(COMMONSOURCES:.cpp=.o)),$(COMMON_OBJECT_DIR)$(file)) -Xlinker $(LDFLAGS) -L ./ \
		-L $(BELIBRARIES) -L $(BASE_APP_LIB_INSTALL_DIR) $(NETWORKLIBS) -ldom -lbe -ltextencoding -lroot
	@echo "Done linking, adding resource file."
	@xres -o $(BASE_INSTALL_DIR)Themis framework/themis.rsrc
	@echo "Done adding resources. Themis framework app is ready to run."
frameworkdepends: $(FRAMEWORKSOURCES) $(foreach file,$(FRAMEWORKSOURCES:.cpp=.d),$(FRAMEWORK_DEPENDS_DIR)$(notdir $(file)))
	@echo "Framework dependencies are done."

frameworkobjects: $(foreach file,$(FRAMEWORKSOURCES),$(FRAMEWORK_OBJECT_DIR)$(notdir $(file:.cpp=.o)))
	@echo "Framework object files are done."

$(FRAMEWORK_DEPENDS_DIR)%.d: framework/%.cpp
	@echo "Framework: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(FRAMEWORK_DEPENDS_DIR)$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(FRAMEWORK_OBJECT_DIR)"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/$(VAR2)\1.o $(VAR1) : /g' > $@;[ -s $@ ] || rm -f $@

$(FRAMEWORK_OBJECT_DIR)%.o: framework/%.cpp $(FRAMEWORK_DEPENDS_DIR)%.d
	@echo "Framework: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@


#files located in the common directory
commondepends: $(COMMONSOURCES) $(foreach file,$(COMMONSOURCES:.cpp=.d),$(COMMON_DEPENDS_DIR)$(notdir $(file)))
	@echo "Common depends are done."

commonobjects: $(foreach file,$(COMMONSOURCES),$(COMMON_OBJECT_DIR)$(notdir $(file:.cpp=.o)))
	@echo "Common object files are done."

$(COMMON_DEPENDS_DIR)%.d: common/%.cpp common/%.h
	@echo "Common: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(COMMON_DEPENDS_DIR)$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(COMMON_OBJECT_DIR)"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/$(VAR2)\1.o $(VAR1) : /g' > $@;[ -s $@ ] || rm -f $@

$(COMMON_OBJECT_DIR)%.o: common/%.cpp $(COMMON_DEPENDS_DIR)%.d
	@echo "Common: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@

#cache add-on
cacheaddon: cacheaddondepends cacheaddonobjects
	@echo "Linking cache add-on" 
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)cache.so $(foreach file,$(notdir $(CACHEADDONSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)cache/$(file)) $(COMMON_OBJECT_DIR)plugclass.o $(COMMON_OBJECT_DIR)plugman.o -nostart -Xlinker $(LDFLAGS) -soname=cache.so -L ./ -L $(BELIBRARIES)  -lbe -ltextencoding -lroot
	@echo "Cache add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)cache.so
	@echo "Cache plug-in compiled and linked successfully."
	
cacheaddondepends: $(foreach file,$(CACHEADDONSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(CACHEADDONSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)cache/$(notdir $(file)))
	@echo "Cache add-on depends are done."
	
cacheaddonobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(CACHEADDONSOURCES),$(BASE_ADDONS_OBJECT_DIR)cache/$(notdir $(file:.cpp=.o)))
	@echo "Cache add-on objects are done."
	
$(BASE_ADDONS_DEPENDS_DIR)cache/%.d:  $(BASE_ADDON_SOURCE_DIR)cache/%.cpp $(BASE_ADDON_SOURCE_DIR)cache/%.h
	@echo "Cache: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)cache/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)cache/"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/$(VAR2)\1.o $(VAR1) : /g' > $@;[ -s $@ ] || rm -f $@
$(BASE_ADDONS_OBJECT_DIR)cache/%.o: $(BASE_ADDON_SOURCE_DIR)cache/%.cpp $(BASE_ADDONS_DEPENDS_DIR)cache/%.d
	@echo "Cache: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@


#testplug add-on
testplug: testplugdepends testplugobjects
	@echo "Linking testplug" 
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)testplug.so $(foreach file,$(notdir $(TESTPLUGSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)testplug/$(file)) $(COMMON_OBJECT_DIR)plugclass.o $(COMMON_OBJECT_DIR)plugman.o -nostart -Xlinker $(LDFLAGS) -soname=testplug.so -L ./ -L $(BELIBRARIES)  -lbe -ltextencoding -lroot -ltranslation
	@echo "testplug add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)cache.so
	@echo "Cache plug-in compiled and linked successfully."
	
testplugdepends: $(foreach file,$(TESTPLUGSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(TESTPLUGSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)testplug/$(notdir $(file)))
	@echo "testplug depends are done."
	
testplugobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(TESTPLUGSOURCES),$(BASE_ADDONS_OBJECT_DIR)testplug/$(notdir $(file:.cpp=.o)))
	@echo "testplug objects are done."
	
$(BASE_ADDONS_DEPENDS_DIR)testplug/%.d:  $(BASE_ADDON_SOURCE_DIR)testplug/%.cpp $(BASE_ADDON_SOURCE_DIR)testplug/%.h
	@echo "testplug: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)testplug/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)testplug/"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/$(VAR2)\1.o $(VAR1) : /g' > $@;[ -s $@ ] || rm -f $@

$(BASE_ADDONS_OBJECT_DIR)testplug/%.o: $(BASE_ADDON_SOURCE_DIR)testplug/%.cpp $(BASE_ADDONS_DEPENDS_DIR)testplug/%.d
	@echo "testplug: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@

#html parser add-on
htmlpaddon: domlib htmlparserdepends htmlparserobjects
	@echo "Linking HTML Parser add-on" 
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)html_parser.so $(foreach file,$(notdir $(HTMLPARSERSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)HTMLParser/$(file)) $(COMMON_OBJECT_DIR)plugclass.o $(COMMON_OBJECT_DIR)plugman.o -nostart -Xlinker $(LDFLAGS) -soname=html_parser.so -L ./ -L $(BELIBRARIES) -L$(BASE_APP_LIB_INSTALL_DIR)  -lbe -lroot -ldom -lstdc++.r4
	@echo "HTML Parser add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)html_parser.so
	@echo "HTML Parser plug-in compiled and linked successfully."
	
htmlparserdepends: $(foreach file,$(HTMLPARSERSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(HTMLPARSERSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)HTMLParser/$(notdir $(file)))
	@echo "HTML Parser depends are done."
	
htmlparserobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(HTMLPARSERSOURCES),$(BASE_ADDONS_OBJECT_DIR)HTMLParser/$(notdir $(file:.cpp=.o)))
	@echo "HTML Parser objects are done."
	
$(BASE_ADDONS_DEPENDS_DIR)HTMLParser/%.d:  $(BASE_ADDON_SOURCE_DIR)HTMLParser/%.cpp $(BASE_ADDON_SOURCE_DIR)HTMLParser/%.h
	@echo "HTML Parser: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)HTMLParser/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)HTMLParser/"|sed 's/\//\\\//g'); $(CC) -Icommon -Iframework/DOM -I./ -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/$(VAR2)\1.o $(VAR1) : /g' > $@;[ -s $@ ] || rm -f $@

$(BASE_ADDONS_OBJECT_DIR)HTMLParser/%.o: $(BASE_ADDON_SOURCE_DIR)HTMLParser/%.cpp $(BASE_ADDONS_DEPENDS_DIR)HTMLParser/%.d
	@echo "HTML Parser: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -Iframework/DOM -I./ -c $< -o $@

#domview add-on
domvaddon: domvdepends domvobjects
	@echo "Linking DOM View add-on" 
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)DOMView.so $(foreach file,$(notdir $(DOMVIEWSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)DOMView/$(file)) $(COMMON_OBJECT_DIR)plugclass.o $(COMMON_OBJECT_DIR)plugman.o -nostart -Xlinker $(LDFLAGS) -soname=DOMView.so -L ./ -L $(BELIBRARIES) -L$(BASE_APP_LIB_INSTALL_DIR)  -lbe -lroot -ldom -lstdc++.r4
	@echo "DOM View add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)DOMView.so
	@echo "DOM View plug-in compiled and linked successfully."
	
domvdepends: $(foreach file,$(DOMVIEWSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(DOMVIEWSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)DOMView/$(notdir $(file)))
	@echo "DOMView depends are done."
	
domvobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(DOMVIEWSOURCES),$(BASE_ADDONS_OBJECT_DIR)DOMView/$(notdir $(file:.cpp=.o)))
	@echo "DOM View objects are done."
	
$(BASE_ADDONS_DEPENDS_DIR)DOMView/%.d:  $(BASE_ADDON_SOURCE_DIR)DOMView/%.cpp $(BASE_ADDON_SOURCE_DIR)DOMView/%.h
	@echo "DOM View: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)DOMView/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)DOMView/"|sed 's/\//\\\//g'); $(CC) -Icommon -Iframework/DOM -I./ -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/$(VAR2)\1.o $(VAR1) : /g' > $@;[ -s $@ ] || rm -f $@

$(BASE_ADDONS_OBJECT_DIR)DOMView/%.o: $(BASE_ADDON_SOURCE_DIR)DOMView/%.cpp $(BASE_ADDONS_DEPENDS_DIR)DOMView/%.d
	@echo "DOM View: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -Iframework/DOM -I./ -c $< -o $@

#http add-on
httpaddon: httpaddondepends httpaddonobjects
	@echo "Linking http add-on" 
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)http_plugin.so  $(foreach file,$(notdir $(HTTPADDONSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)http/$(file)) $(foreach file,$(notdir $(COMMONSOURCES:.cpp=.o)),$(COMMON_OBJECT_DIR)$(file)) -nostart -Xlinker $(LDFLAGS) -soname=http_plugin.so -L ./ -L $(BELIBRARIES)  $(NETWORKLIBS) -lbe -ltextencoding -lroot
	@echo "HTTP add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)http_plugin.so
	@echo "HTTP plug-in compiled and linked successfully."

# the next line will replace the current one when the new networking stack is added to the repository. at that time
# be sure to remove the extra backslash at the end of the line
httpaddondepends: $(foreach file,$(HTTPADDONSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d common/tcplayer.cpp $(COMMON_DEPENDS_DIR)tcplayer.d \
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(HTTPADDONSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)http/$(notdir $(file)))
	@echo "HTTP add-on depends are done."
	
httpaddonobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o $(COMMON_OBJECT_DIR)tcplayer.o \
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(HTTPADDONSOURCES),$(BASE_ADDONS_OBJECT_DIR)http/$(notdir $(file:.cpp=.o)))
	@echo "HTTP add-on objects are done."
	
$(BASE_ADDONS_DEPENDS_DIR)http/%.d:  $(BASE_ADDON_SOURCE_DIR)http/%.cpp  $(BASE_ADDON_SOURCE_DIR)http/%.h
	@echo "HTTP: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)http/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)http/"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/$(VAR2)\1.o $(VAR1) : /g' > $@;[ -s $@ ] || rm -f $@

$(BASE_ADDONS_OBJECT_DIR)http/%.o: $(BASE_ADDON_SOURCE_DIR)http/%.cpp $(BASE_ADDONS_DEPENDS_DIR)http/%.d
	@echo "HTTP: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@

#cookie add-on
cookieaddon: cookieaddondepends cookieaddonobjects
	@echo "Linking cookie add-on"
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)cookie_plugin.so $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o $(foreach file,$(notdir $(COOKIEADDONSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)cookie/$(file)) -nostart -Xlinker $(LDFLAGS) -soname=cookie_plugin.so -L ./ -L $(BELIBRARIES) -lbe -lroot -ltextencoding
	@echo "Cookie add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)cookie_plugin.so
	@echo "Cookie plug-in compiled and linked successfully."
cookieaddondepends: $(foreach file,$(COOKIEADDONSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d \
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(COOKIEADDONSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)cookie/$(notdir $(file)))
	@echo "Cookie add-on depends are done."
cookieaddonobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o  \
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(COOKIEADDONSOURCES),$(BASE_ADDONS_OBJECT_DIR)cookie/$(notdir $(file:.cpp=.o)))
	@echo "Cookie add-on objects are done."
$(BASE_ADDONS_DEPENDS_DIR)cookie/%.d:  $(BASE_ADDON_SOURCE_DIR)cookie/%.cpp  $(BASE_ADDON_SOURCE_DIR)cookie/%.h
	@echo "Cookie: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)cookie/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)cookie/"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/$(VAR2)\1.o $(VAR1) : /g' > $@;[ -s $@ ] || rm -f $@

$(BASE_ADDONS_OBJECT_DIR)cookie/%.o: $(BASE_ADDON_SOURCE_DIR)cookie/%.cpp $(BASE_ADDONS_DEPENDS_DIR)cookie/%.d
	@echo "Cookie: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@

#Javascript add-on
jsaddon: domlib jsaddondepends jsaddonobjects
	@echo "Copying Javscript library and supporting libraries to install directory."
	@$(shell if [ "$(NETWORKING)" == "BONE" ] ; then /bin/unzip -uqd Themis/lib/ javascript/bone_jslibs.zip ; else /bin/unzip -uqd Themis/lib/ javascript/netserver_jslibs.zip ; fi )
	@echo "Linking Javascript add-on"
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)js_plugin.so $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o $(foreach file,$(notdir $(JSADDONSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)js/$(file)) -nostart -Xlinker $(LDFLAGS) -soname=js_plugin.so -L ./ -L javascript/ -L $(BELIBRARIES) -L $(BASE_APP_LIB_INSTALL_DIR) -lbe -lroot -ltextencoding -ljs -ldom
	@echo "Javascript add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)js_plugin.so
	@echo "Javascript plug-in compiled and linked successfully."
#	@$(shell for file in $(JAVASCRIPT_SUPPORT_LIBRARIES); do if [ ! -f $(BASE_APP_LIB_INSTALL_DIR)`basename $$file` ] ; then cp $$file $(BASE_APP_LIB_INSTALL_DIR); fi ;  done)
	@echo "Done with Javascript add-on."
jsaddondepends: $(foreach file,$(JSADDONSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d \
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(JSADDONSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)js/$(notdir $(file)))
	@echo "Javascript add-on depends are done."
jsaddonobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o  \
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(JSADDONSOURCES),$(BASE_ADDONS_OBJECT_DIR)js/$(notdir $(file:.cpp=.o)))
	@echo "Javascript add-on objects are done."
$(BASE_ADDONS_DEPENDS_DIR)js/%.d:  $(BASE_ADDON_SOURCE_DIR)js/%.cpp $(BASE_ADDON_SOURCE_DIR)js/%.h
	@echo "Javascript: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)js/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)js/"|sed 's/\//\\\//g'); $(CC) -DXP_UNIX -Icommon -Ijavascript -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/$(VAR2)\1.o $(VAR1) : /g' > $@;[ -s $@ ] || rm -f $@

$(BASE_ADDONS_OBJECT_DIR)js/%.o: $(BASE_ADDON_SOURCE_DIR)js/%.cpp $(BASE_ADDONS_DEPENDS_DIR)js/%.d
	@echo "Javascript: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -DXP_UNIX -D_BEOS_ -Icommon -Ijavascript -c $< -o $@

#DOM Library
domlib: domlibdepends domlibobjects
	@echo "Linking DOM Library"
	@$(CC) -o $(BASE_APP_LIB_INSTALL_DIR)libdom.so $(foreach file,$(notdir $(DOMLIBRARYSOURCES:.cpp=.o)),$(DOMLIBRARY_OBJECT_DIR)$(file)) -nostart -Xlinker $(LDFLAGS) -shared -Xlinker -soname=libdom.so -L ./ -L $(BELIBRARIES) -L $(DOMLIBRARY_OBJECT_DIR) -lstdc++.r4
	@echo "DOM Library is now linked. Doing mimeset."
	@mimeset -F $(BASE_APP_LIB_INSTALL_DIR)libdom.so
	@echo "DOM Library compiled and linked successfully."
	
domlibdepends: $(DOMLIBRARYSOURCES) $(foreach file,$(DOMLIBRARYSOURCES:.cpp=.d),$(DOMLIBRARY_DEPENDS_DIR)$(notdir $(file)))
	@echo "DOM Library depends are done."
	
domlibobjects: $(foreach file,$(DOMLIBRARYSOURCES),$(DOMLIBRARY_OBJECT_DIR)$(notdir $(file:.cpp=.o)))
	@echo "DOM Library objects are done."
	
$(DOMLIBRARY_DEPENDS_DIR)%.d: framework/DOM/$(notdir %).cpp framework/DOM/$(notdir %).h framework/DOM/DOMSupport.h framework/DOM/TComment.h
	@echo "DOM: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(DOMLIBRARY_DEPENDS_DIR)$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(DOMLIBRARY_OBJECT_DIR)"|sed 's/\//\\\//g'); $(CC) -Icommon -I./ -M $(CPPFLAGS) $(COMPILE_FLAGS) $< | sed 's/\($*\)\.o[ :]*/$(VAR2)\1.o $(VAR1) : /g' > $@;[ -s $@ ] || rm -f $@

$(DOMLIBRARY_OBJECT_DIR)%.o: framework/DOM/%.cpp $(DOMLIBRARY_DEPENDS_DIR)%.d
	@echo "DOM: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -I./ -c $< -o $@ -Wall -Wno-multichar -Wno-ctor-dtor-privacy -Woverloaded-virtual -O3

#DOM Test application
domtest: domlib domtestdepends domtestobjects
	@$(CC) -o $(BASE_INSTALL_DIR)DOMTest $(foreach file,$(notdir $(DOMTESTSOURCES:.cpp=.o)),$(DOMLIBRARY_OBJECT_DIR)$(file)) -Xlinker $(LDFLAGS) -L ./ -L $(BELIBRARIES) -L $(BASE_APP_LIB_INSTALL_DIR) -lbe -ldom -lstdc++.r4
	@echo "DOM Test application is now linked. Doing mimeset."
	@mimeset -F $(BASE_INSTALL_DIR)DOMTest
	@echo "DOM Test application compiled and linked successfully."
domtestdepends:  $(DOMTESTSOURCES) $(foreach file,$(DOMTESTSOURCES:.cpp=.d),$(DOMLIBRARY_DEPENDS_DIR)$(notdir $(file)))
	@echo "DOM Test application depends are done."
	
domtestobjects: $(foreach file,$(DOMTESTSOURCES),$(DOMLIBRARY_OBJECT_DIR)$(notdir $(file:.cpp=.o)))
	@echo "DOM Test application objects are done."

#DOM LoadTest application
domloadtest: domlib domltestdepends domltestobjects
	@$(CC) -o $(BASE_INSTALL_DIR)DOMLoadTest $(foreach file,$(notdir $(DOMLOADTESTSOURCES:.cpp=.o)),$(DOMLIBRARY_OBJECT_DIR)$(file)) -Xlinker $(LDFLAGS) -L ./ -L $(BELIBRARIES) -L $(BASE_APP_LIB_INSTALL_DIR) -lbe -ldom -lstdc++.r4
	@echo "DOM LoadTest application (DOMLoadTest) is now linked. Doing mimeset."
	@mimeset -F $(BASE_INSTALL_DIR)DOMLoadTest
	@cp framework/DOM/LoadTest.html $(BASE_INSTALL_DIR)
	@echo "DOM LoadTest application (DOMLoadTest) compiled and linked successfully."
domltestdepends:  $(DOMTESTSOURCES) $(foreach file,$(DOMLOADTESTSOURCES:.cpp=.d),$(DOMLIBRARY_DEPENDS_DIR)$(notdir $(file)))
	@echo "DOM Test application depends are done."
	
domltestobjects: $(foreach file,$(DOMLOADTESTSOURCES),$(DOMLIBRARY_OBJECT_DIR)$(notdir $(file:.cpp=.o)))
	@echo "DOM Test application objects are done."
