
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

#object directories
BASE_OBJECT_DIR=objects/
BASE_LIBRARY_OBJECT_DIR=$(BASE_OBJECT_DIR)lib/
FRAMEWORK_OBJECT_DIR=$(BASE_OBJECT_DIR)framework/
COMMON_OBJECT_DIR=$(BASE_OBJECT_DIR)common/
BASE_ADDONS_OBJECT_DIR=$(BASE_OBJECT_DIR)add-ons/
DOMLIBRARY_OBJECT_DIR=$(BASE_LIBRARY_OBJECT_DIR)DOM/

#source files
FRAMEWORKSOURCES= \
	framework/main.cpp \
	framework/app.cpp \
	framework/win.cpp \
	framework/winview.cpp \
	framework/plugman.cpp \
	framework/infowin.cpp
COMMONSOURCES= \
	common/plugclass.cpp \
	common/tcplayer.cpp
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
DOMLIBRARYSOURCES= \
	framework/DOM/TNode.cpp \
	framework/DOM/TDOMException.cpp \
	framework/DOM/TNamedNodeMap.cpp \
	framework/DOM/TNodeListContainer.cpp \
	framework/DOM/TNodeList.cpp \
	framework/DOM/TElement.cpp \
	framework/DOM/TAttr.cpp \
	framework/DOM/TCharacterData.cpp 
DOMTESTSOURCES= \
	framework/DOM/DOMTest.cpp
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
#http add-on depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)http/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)http/ ; echo 0 ; fi)" "0"
endif
#cache add-on depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)cache/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)cache/ ; echo 0 ; fi)" "0"
endif
#DOM Library depends directory
ifeq "$(shell if [ -d $(DOMLIBRARY_OBJECT_DIR) ] ; then echo 1 ; else mkdir -p $(DOMLIBRARY_OBJECT_DIR) ; echo 0 ; fi)" "0"
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
#DOM Library depends directory
ifeq "$(shell if [ -d $(DOMLIBRARY_DEPENDS_DIR) ] ; then echo 1 ; else mkdir -p $(DOMLIBRARY_DEPENDS_DIR) ; echo 0 ; fi)" "0"
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
# by default, debugging information will be included. to turn it off, remove or comment out (#) the -g
# on the next line 
COMPILE_FLAGS=-g
ifeq "$(shell if [ -f /system/lib/libsocket.so ] ; then echo 1 ; else echo 0 ; fi)" "1"
	NETWORKLIBS=-lsocket -lbind -lbnetapi
	COMPILE_FLAGS+= -DUSEBONE
else
	NETWORKLIBS=-lnet -lnetapi
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
#			httpaddon
#			help
#			dist
#			domlib
#			libs
#			domtest
all: framework httpaddon cacheaddon domtest
	@echo "Themis application and add-ons built successfully."
addons: httpaddon cacheaddon
	@echo "All add-ons built successfully."
clean: 
	rm -rf objects depends Themis
help:
	@echo "To compile, type \"make <target>\", where \"<target>\" is one of the following:"
	@echo "	all		All of Themis"
	@echo "	addons		Just the add-ons"
	@echo "	clean		Remove the dependency, object, and binary files"
	@echo "	framework	Just the Themis framework"
	@echo "	cacheaddon	Just the cache addon"
	@echo "	httpaddon	Just the HTTP addon"
	@echo "	help		This message"
	@echo "	dist		Distribution ready zip file"
	@echo "	domlib		DOM library"
	@echo "	libs		Just the ibraries"
	@echo "	domtest		DOM test application"
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

$(FRAMEWORK_DEPENDS_DIR)%.d: $(FRAMEWORKSOURCES)
	@echo "Framework: Updating dependency file $@"
	@set -e;VAR1=$(shell echo "$(FRAMEWORK_DEPENDS_DIR)$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(FRAMEWORK_OBJECT_DIR)"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/$(VAR2)\1.o $(VAR1) : /g' > $@;[ -s $@ ] || rm -f $@

$(FRAMEWORK_OBJECT_DIR)%.o: framework/%.cpp $(FRAMEWORK_DEPENDS_DIR)$(notdir %.d)
	@echo "Updating object file $@"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@


#files located in the common directory
commondepends: $(COMMONSOURCES) $(foreach file,$(COMMONSOURCES:.cpp=.d),$(COMMON_DEPENDS_DIR)$(notdir $(file)))
	@echo "Common depends are done."

commonobjects: $(foreach file,$(COMMONSOURCES),$(COMMON_OBJECT_DIR)$(notdir $(file:.cpp=.o)))
	@echo "Common object files are done."

$(COMMON_DEPENDS_DIR)%.d: $(COMMONSOURCES)
	@echo "Common: Updating dependency file $@"
	@set -e;VAR1=$(shell echo "$(COMMON_DEPENDS_DIR)$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(COMMON_OBJECT_DIR)"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/$(VAR2)\1.o $(VAR1) : /g' > $@;[ -s $@ ] || rm -f $@

$(COMMON_OBJECT_DIR)%.o: common/%.cpp $(COMMON_DEPENDS_DIR)%.d
	@echo "Updating object file $@"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@

#cache add-on
cacheaddon: cacheaddondepends cacheaddonobjects
	@echo "Linking cache add-on" 
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)cache.so $(foreach file,$(notdir $(CACHEADDONSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)cache/$(file)) $(COMMON_OBJECT_DIR)plugclass.o $(FRAMEWORK_OBJECT_DIR)plugman.o -nostart -Xlinker $(LDFLAGS) -soname=cache.so -L ./ -L $(BELIBRARIES)  -lbe -ltextencoding -lroot
	@echo "Cache add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)cache.so
	@echo "Cache plug-in compiled and linked successfully."
	
cacheaddondepends: $(foreach file,$(CACHEADDONSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) framework/plugman.cpp common/plugclass.cpp $(FRAMEWORK_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(CACHEADDONSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)cache/$(notdir $(file)))
	@echo "Cache add-on depends are done."
	
cacheaddonobjects: $(FRAMEWORK_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(CACHEADDONSOURCES),$(BASE_ADDONS_OBJECT_DIR)cache/$(notdir $(file:.cpp=.o)))
	@echo "Cache add-on objects are done."
	
$(BASE_ADDONS_DEPENDS_DIR)cache/%.d: $(foreach file,$(CACHEADDONSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file))
	@echo "Cache: Updating dependency file $@"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)cache/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)cache/"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/$(VAR2)\1.o $(VAR1) : /g' > $@;[ -s $@ ] || rm -f $@

$(BASE_ADDONS_OBJECT_DIR)cache/%.o: $(BASE_ADDON_SOURCE_DIR)cache/%.cpp $(BASE_ADDONS_DEPENDS_DIR)cache/%.d
	@echo "Updating object file $@"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@

#http add-on
httpaddon: httpaddondepends httpaddonobjects
	@echo "Linking http add-on" 
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)http_plugin.so $(FRAMEWORK_OBJECT_DIR)plugman.o $(foreach file,$(notdir $(HTTPADDONSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)http/$(file)) $(foreach file,$(notdir $(COMMONSOURCES:.cpp=.o)),$(COMMON_OBJECT_DIR)$(file)) -nostart -Xlinker $(LDFLAGS) -soname=http_plugin.so -L ./ -L $(BELIBRARIES)  $(NETWORKLIBS) -lbe -ltextencoding -lroot
	@echo "HTTP add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)http_plugin.so
	@echo "HTTP plug-in compiled and linked successfully."

# the next line will replace the current one when the new networking stack is added to the repository. at that time
# be sure to remove the extra backslash at the end of the line
httpaddondepends: $(foreach file,$(HTTPADDONSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) framework/plugman.cpp common/plugclass.cpp $(FRAMEWORK_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d common/tcplayer.cpp $(COMMON_DEPENDS_DIR)tcplayer.d \
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(HTTPADDONSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)http/$(notdir $(file)))
	@echo "HTTP add-on depends are done."
	
httpaddonobjects: $(FRAMEWORK_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o $(COMMON_OBJECT_DIR)tcplayer.o \
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(HTTPADDONSOURCES),$(BASE_ADDONS_OBJECT_DIR)http/$(notdir $(file:.cpp=.o)))
	@echo "HTTP add-on objects are done."
	
$(BASE_ADDONS_DEPENDS_DIR)http/%.d:  $(BASE_ADDON_SOURCE_DIR)http/%.cpp  #$(foreach file,$(HTTPADDONSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file))
	@echo "HTTP: Updating dependency file $@"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)http/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)http/"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/$(VAR2)\1.o $(VAR1) : /g' > $@;[ -s $@ ] || rm -f $@

$(BASE_ADDONS_OBJECT_DIR)http/%.o: $(BASE_ADDON_SOURCE_DIR)http/%.cpp $(BASE_ADDONS_DEPENDS_DIR)http/%.d
	@echo "Updating object file $@"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@

#DOM Library
domlib: domlibdepends domlibobjects
	@echo "Linking DOM Library"
	@$(CC) -o $(BASE_APP_LIB_INSTALL_DIR)libdom.so $(foreach file,$(notdir $(DOMLIBRARYSOURCES:.cpp=.o)),$(DOMLIBRARY_OBJECT_DIR)$(file)) -nostart -Xlinker $(LDFLAGS) -shared -Xlinker -soname=libdom.so -L ./ -L $(BELIBRARIES) -L $(DOMLIBRARY_OBJECT_DIR) -lbe
	@echo "DOM Library is now linked. Doing mimeset."
	@mimeset -F $(BASE_APP_LIB_INSTALL_DIR)libdom.so
	@echo "DOM Library compiled and linked successfully."
	
domlibdepends: $(DOMLIBRARYSOURCES) $(foreach file,$(DOMLIBRARYSOURCES:.cpp=.d),$(DOMLIBRARY_DEPENDS_DIR)$(notdir $(file)))
	@echo "DOM Library depends are done."
	
domlibobjects: $(foreach file,$(DOMLIBRARYSOURCES),$(DOMLIBRARY_OBJECT_DIR)$(notdir $(file:.cpp=.o)))
	@echo "DOM Library objects are done."
	
$(DOMLIBRARY_DEPENDS_DIR)%.d: $(DOMLIBRARYSOURCES)
	@echo "DOM: Updating dependency file $@"
	@set -e;VAR1=$(shell echo "$(DOMLIBRARY_DEPENDS_DIR)$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(DOMLIBRARY_OBJECT_DIR)"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/$(VAR2)\1.o $(VAR1) : /g' > $@;[ -s $@ ] || rm -f $@

$(DOMLIBRARY_OBJECT_DIR)%.o: framework/DOM/%.cpp $(DOMLIBRARY_DEPENDS_DIR)%.d
	@echo "Updating object file $@"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@ -Wall -Wno-multichar -Wno-ctor-dtor-privacy -Woverloaded-virtual -O3

#DOM Test application
domtest: domlib domtestdepends domtestobjects
	@$(CC) -o $(BASE_INSTALL_DIR)DOMTest $(foreach file,$(notdir $(DOMTESTSOURCES:.cpp=.o)),$(DOMLIBRARY_OBJECT_DIR)$(file)) -Xlinker $(LDFLAGS) -L ./ -L $(BELIBRARIES) -L $(BASE_APP_LIB_INSTALL_DIR) -lbe -ldom
	@echo "DOM Test application is now linked. Doing mimeset."
	@mimeset -F $(BASE_INSTALL_DIR)DOMTest
	@echo "DOM Test application compiled and linked successfully."
domtestdepends:  $(DOMTESTSOURCES) $(foreach file,$(DOMTESTSOURCES:.cpp=.d),$(DOMLIBRARY_DEPENDS_DIR)$(notdir $(file)))
	@echo "DOM Test application depends are done."
	
domtestobjects: $(foreach file,$(DOMTESTSOURCES),$(DOMLIBRARY_OBJECT_DIR)$(notdir $(file:.cpp=.o)))
	@echo "DOM Test application objects are done."