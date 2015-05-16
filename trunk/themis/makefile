
#locations for various files
#To change the basic build location for the app and plugins, just change BASE_INSTALL_DIR.
#Remember to include the '/' at the end.

#platform
PLATFORM=$(HOSTTYPE)

#dependency switch
DEPSWITCH=

#includes switch
INCSWITCH=

#shared library link command
SHAREDCMD=

#link command tells compiler to pass options to linker and switch
LINKCMD=
LINKSWITCH=
#

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
COMMON_DEPENDS_DIR=$(BASE_DEPENDS_DIR)common/
DOMLIBRARY_DEPENDS_DIR=$(BASE_LIBRARY_DEPENDS_DIR)DOM/
DOMSTYLELIBRARY_DEPENDS_DIR=$(BASE_LIBRARY_DEPENDS_DIR)DOMStyle/
TESTPLUG_DEPENDS_DIR=$(BASE_DEPENDS_DIR)testplug/
HTMLPARSER_DEPENDS_DIR=$(BASE_DEPENDS_DIR)HTMLParser/
IMAGEH_DEPENDS_DIR=$(BASE_ADDONS_DEPENDS_DIR)imageh/
CSSPARSER_DEPENDS_DIR=$(BASE_ADDONS_DEPENDS_DIR)CSSParser/
CSSRENDERER_DEPENDS_DIR=$(BASE_ADDONS_DEPENDS_DIR)CSSRenderer/

#object directories
BASE_OBJECT_DIR=objects/
BASE_LIBRARY_OBJECT_DIR=$(BASE_OBJECT_DIR)lib/
FRAMEWORK_OBJECT_DIR=$(BASE_OBJECT_DIR)framework/
COMMON_OBJECT_DIR=$(BASE_OBJECT_DIR)common/
BASE_ADDONS_OBJECT_DIR=$(BASE_OBJECT_DIR)add-ons/
DOMLIBRARY_OBJECT_DIR=$(BASE_LIBRARY_OBJECT_DIR)DOM/
DOMSTYLELIBRARY_OBJECT_DIR=$(BASE_LIBRARY_OBJECT_DIR)DOMStyle/
TESTPLUG_OBJECT_DIR=$(BASE_OBJECT_DIR)testplug/
HTMLPARSER_OBJECT_DIR=$(BASE_OBJECT_DIR)HTMLParser/
IMAGEH_OBJECT_DIR=$(BASE_ADDONS_OBJECT_DIR)imageh/
CSSPARSER_OBJECT_DIR=$(BASE_ADDONS_OBJECT_DIR)CSSParser/
CSSRENDERER_OBJECT_DIR=$(BASE_ADDONS_OBJECT_DIR)CSSRenderer/

#source files
FRAMEWORKSOURCES= \
	framework/main.cpp \
	framework/app.cpp \
	framework/win.cpp \
	framework/aboutview.cpp \
	framework/aboutwin.cpp \
	framework/PrefsWin.cpp \
	framework/PrefsViews.cpp \
	framework/appaboutview.cpp \
	framework/ThemisNavView.cpp \
	framework/ThemisStatusView.cpp \
	framework/ThemisTab.cpp \
	framework/GlobalHistory.cpp \
	framework/TabHistory.cpp \
	framework/TPictureButton.cpp \
	framework/ThemisTabView.cpp \
	framework/ThemisUrlPopUpView.cpp \
	framework/ThemisUrlPopUpWindow.cpp \
	framework/ThemisUrlView.cpp \
	framework/msgdaemon.cpp \
	framework/networkableobject.cpp \
	framework/connection.cpp \
	framework/netbuffer.cpp \
	framework/tcpmanager.cpp \
	framework/base64.cpp \
	framework/protocol_plugin.cpp \
	framework/SiteHandler.cpp \
	framework/SiteEntry.cpp \
	framework/UrlEntry.cpp \
	framework/date.cpp \
	framework/UrlItem.cpp \
	framework/AboutItem.cpp \
	framework/BufferPool.cpp
#	framework/UrlHandler.cpp
COMMONSOURCES= \
	common/plugclass.cpp \
	common/msgsystem.cpp \
	common/plugman.cpp \
	common/cacheplug.cpp \
	common/cacheuser.cpp \
	common/cacheobject.cpp \
	common/stripwhite.cpp \
	common/TTextView.cpp \
	common/prefsman.cpp \
	common/BasePrefsView.cpp \
	common/BaseEntry.cpp \
	common/DOMEntry.cpp \
	common/CSSDOMEntry.cpp
#add similar lines for new plugins, libraries, etc.
HTTPADDONSOURCES= \
	http/optionshandler.cpp \
	http/authvw.cpp \
	http/cookieman.cpp \
	http/httpv4main.cpp \
	http/httpv4.cpp \
	http/authmanager.cpp \
	http/authtype.cpp \
	http/authtypedigest.cpp \
	http/authtypebasic.cpp \
	http/uriprocessor.cpp
CACHEADDONSOURCES= \
	cache/cacheman.cpp \
	cache/cache_main.cpp \
	cache/diskcacheobject.cpp \
	cache/ramcacheobject.cpp \
	cache/dfcacheobject.cpp \
	cache/cachesorts.cpp
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
DOMSTYLELIBRARYSOURCES= \
	framework/DOMStyle/Counter.cpp \
	framework/DOMStyle/CSSCharsetRule.cpp \
	framework/DOMStyle/CSSFontFaceRule.cpp \
	framework/DOMStyle/CSSImportRule.cpp \
	framework/DOMStyle/CSSMediaRule.cpp \
	framework/DOMStyle/CSSPageRule.cpp \
	framework/DOMStyle/CSSPrimitiveValue.cpp \
	framework/DOMStyle/CSSRule.cpp \
	framework/DOMStyle/CSSRuleList.cpp \
	framework/DOMStyle/CSSStyleDeclaration.cpp \
	framework/DOMStyle/CSSStyleRule.cpp \
	framework/DOMStyle/CSSStyleSheet.cpp \
	framework/DOMStyle/CSSUnknownRule.cpp \
	framework/DOMStyle/CSSValue.cpp \
	framework/DOMStyle/CSSValueList.cpp \
	framework/DOMStyle/MediaList.cpp \
	framework/DOMStyle/Property.cpp \
	framework/DOMStyle/Rect.cpp \
	framework/DOMStyle/RGBColor.cpp \
	framework/DOMStyle/StyleSheet.cpp \
	framework/DOMStyle/StyleSheetList.cpp
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
	HTMLParser/TSchema.cpp \
	HTMLParser/TSchemaRule.cpp \
	HTMLParser/TElementDeclaration.cpp \
	HTMLParser/HTMLParser.cpp \
	HTMLParser/AttrListDeclParser.cpp \
	HTMLParser/BaseParser.cpp \
	HTMLParser/SGMLText.cpp \
	HTMLParser/State.cpp \
	HTMLParser/CommentDeclParser.cpp \
	HTMLParser/DTDParser.cpp \
	HTMLParser/DeclarationParser.cpp \
	HTMLParser/DocTypeDeclParser.cpp \
	HTMLParser/ElementDeclException.cpp \
	HTMLParser/ElementDeclParser.cpp  \
	HTMLParser/ElementParser.cpp  \
	HTMLParser/ElementToken.cpp \
	HTMLParser/EntityDeclParser.cpp  \
	HTMLParser/MarkedSecDeclParser.cpp  \
	HTMLParser/Position.cpp  \
	HTMLParser/ReadException.cpp  \
	HTMLParser/SGMLParser.cpp \
	HTMLParser/SGMLScanner.cpp \
	HTMLParser/HTMLParserPrefsView.cpp
CSSPARSERSOURCES= \
	CSSParser/AtRuleParser.cpp \
	CSSParser/BaseParser.cpp \
	CSSParser/CSSParser.cpp \
	CSSParser/CSSParserPlugin.cpp \
	CSSParser/CSSScanner.cpp \
	CSSParser/ImportAtRuleParser.cpp \
	CSSParser/MediaAtRuleParser.cpp \
	CSSParser/Position.cpp \
	CSSParser/ReadException.cpp \
	CSSParser/RuleSetParser.cpp \
	CSSParser/CSSParserPrefsView.cpp
	
DOMVIEWSOURCES= \
	DOMView/DOMView.cpp \
	DOMView/DOMViewer.cpp
IMAGEHSOURCES= \
	imageh/imageman.cpp \
	imageh/image.cpp
FILEPROTOCOLSOURCES= \
	FileProtocol/FileProtocol.cpp
MESSAGEVIEWERSOURCES= \
	MsgViewer/MsgView.cpp \
	MsgViewer/MsgViewer.cpp \
	MsgViewer/MsgTextView.cpp
CSSVIEWERSOURCES= \
	CSSViewer/CSSView.cpp \
	CSSViewer/CSSViewer.cpp \
	CSSViewer/ColumnListView.cpp \
	CSSViewer/ColumnTypes.cpp \
	CSSViewer/ColorTools.cpp
CSSRENDERERSOURCES= \
	CSSRenderer/CSSRenderer.cpp \
	CSSRenderer/CSSRendererView.cpp \
	CSSRenderer/CSSScrolledRendererView.cpp \
	CSSRenderer/CSSView.cpp \
	CSSRenderer/TextBox.cpp \
	CSSRenderer/CSSStyleContainer.cpp \
	CSSRenderer/InlineDisplayView.cpp \
	CSSRenderer/BlockDisplayView.cpp \
	CSSRenderer/TableDisplayView.cpp \
	CSSRenderer/TableRowDisplayView.cpp \
	CSSRenderer/TableCellDisplayView.cpp \
	CSSRenderer/TableGroupDisplayView.cpp \
	CSSRenderer/TableFooterGroupDisplayView.cpp \
	CSSRenderer/TableHeaderGroupDisplayView.cpp \
	CSSRenderer/TableRowGroupDisplayView.cpp \
	CSSRenderer/NoneDisplayView.cpp \
	CSSRenderer/FormDisplayView.cpp \
	CSSRenderer/InputDisplayView.cpp \
	CSSRenderer/ButtonInputDisplayView.cpp \
	CSSRenderer/TextInputDisplayView.cpp \
	CSSRenderer/HiddenInputDisplayView.cpp

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
#css parser add-on object directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)CSSParser ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)CSSParser ; echo 0 ; fi)" "0"
endif
#file protocol add-on object directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)FileProtocol/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)FileProtocol/ ; echo 0 ; fi)" "0"
endif
#message viewer add-on object directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)MsgViewer/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)MsgViewer/ ; echo 0 ; fi)" "0"
endif
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
#DOMStyle Library object directory
ifeq "$(shell if [ -d $(DOMSTYLELIBRARY_OBJECT_DIR) ] ; then echo 1 ; else mkdir -p $(DOMSTYLELIBRARY_OBJECT_DIR) ; echo 0 ; fi)" "0"
endif
#HTML Parser object directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)HTMLParser/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)HTMLParser/ ; echo 0 ; fi)" "0"
endif
#DOM View object directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)DOMView/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)DOMView/ ; echo 0 ; fi)" "0"
endif
#CSS Viewer object directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)CSSViewer/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)CSSViewer/ ; echo 0 ; fi)" "0"
endif
#testplug add-on object directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)testplug/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)testplug/ ; echo 0 ; fi)" "0"
endif
#imageh add-on object directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)imageh/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)imageh/ ; echo 0 ; fi)" "0"
endif
#CSS Renderer add-on object directory
ifeq "$(shell if [ -d $(BASE_ADDONS_OBJECT_DIR)CSSRenderer/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_OBJECT_DIR)CSSRenderer/ ; echo 0 ; fi)" "0"
endif


#Make depends directories as necessary
ifeq "$(shell if [ -d $(FRAMEWORK_DEPENDS_DIR) ] ; then echo 1 ; else mkdir -p $(FRAMEWORK_DEPENDS_DIR) ; echo 0 ; fi)" "0"
endif
ifeq "$(shell if [ -d $(COMMON_DEPENDS_DIR) ] ; then echo 1 ; else mkdir -p $(COMMON_DEPENDS_DIR) ; echo 0 ; fi)" "0"
endif
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR) ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR) ; echo 0 ; fi)" "0"
endif
#add a new line similar to those below for each plug-in/add-on
#css parser add-on depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR)CSSParser ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR)CSSParser ; echo 0 ; fi)" "0"
endif
#file protocol add-on depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR)FileProtocol/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR)FileProtocol/ ; echo 0 ; fi)" "0"
endif
#message viewer add-on depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR)MsgViewer/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR)MsgViewer/ ; echo 0 ; fi)" "0"
endif
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
#DOMStyle Library depends directory
ifeq "$(shell if [ -d $(DOMSTYLELIBRARY_DEPENDS_DIR) ] ; then echo 1 ; else mkdir -p $(DOMSTYLELIBRARY_DEPENDS_DIR) ; echo 0 ; fi)" "0"
endif
#HTML Parser depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR)HTMLParser/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR)HTMLParser/ ; echo 0 ; fi)" "0"
endif
#DOM View depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR)DOMView/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR)DOMView/ ; echo 0 ; fi)" "0"
endif
#CSS Viewer depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR)CSSViewer/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR)CSSViewer/ ; echo 0 ; fi)" "0"
endif
#testplug add-on depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR)testplug/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR)testplug/ ; echo 0 ; fi)" "0"
endif
#imageh add-on depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR)imageh/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR)imageh/ ; echo 0 ; fi)" "0"
endif
#CSS Renderer add-on depends directory
ifeq "$(shell if [ -d $(BASE_ADDONS_DEPENDS_DIR)CSSRenderer/ ] ; then echo 1 ; else mkdir -p $(BASE_ADDONS_DEPENDS_DIR)CSSRenderer/ ; echo 0 ; fi)" "0"
endif

#Detect whether we are running Zeta or R5/Dano
STDLIBS=-lbe -ltextencoding -lroot
ifeq "$(shell if [ -f /boot/beos/system/lib/libzeta.so ] ; then echo 1 ; else echo 0 ; fi)" "1"
	STDLIBS+= -lzeta
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
NETWORKLIBS=-lsocket -lbind  #-lopenssl
NETWORKING=NONE
# by default, debugging information will be included. to turn it off, remove or comment out (#) the -g
# on the next line 
COMPILE_FLAGS=-g -D_Themis_ -DDEBUG -Wall -Wno-multichar 
ifeq "$(shell if [ -f $(BASE_APP_LIB_INSTALL_DIR)libcl.so ] ; then echo 0; else unzip -d $(BASE_APP_LIB_INSTALL_DIR) 3rd_party/cryptlib_netserver.zip;  ln -s libcl.so.3.1.0 $(BASE_APP_LIB_INSTALL_DIR)libcl.so ; echo 1; fi)" "1"
endif
ifeq "$(shell if [ -f $(BASE_APP_LIB_INSTALL_DIR)libz.so ] ; then echo 0; else unzip -d $(BASE_APP_LIB_INSTALL_DIR) 3rd_party/libz.zip; echo 1; fi)" "1"
endif
ifeq "$(shell if [ -f /boot/develop/headers/be/bone/sys/socket.h ] ; then echo 1 ; else echo 0 ; fi)" "1"
	NETWORKLIBS=-lsocket -lbind  -I/boot/develop/headers/be/bone/ 
	NETWORKING=BONE
	COMPILE_FLAGS+= -DUSEBONE -D_ZETA_USING_DEPRECATED_API_
else
ifeq "$(shell uname)" "Haiku"
	NETWORKLIBS=-lnetwork
	NETWORKING=HAIKU
	COMPILE_FLAGS+= -DHAIKU
else
	NETWORKLIBS=-lnet 
	NETWORKING=net_server
ifeq "$(PLATFORM)" "i586"
	COMPILE_FLAGS+= -DUSENETSERVER -D _Themis_
else
	COMPILE_FLAGS+= -d USENETSERVER -d _Themis_
endif
endif
endif
ifeq "$(PLATFORM)" "ppc"
	DEPSWITCH=-make
	COMPILE_FLAGS += -export pragma -d _PowerPC_ -cwd source -d _Themis_ -d MSIPL_USING_NAMESPACE 
	INCSWITCH=-i
	SHAREDCMD=-shared
	LINKSWITCH=-l
else
	DEPSWITCH=-M
	INCSWITCH=-I
	SHAREDCMD=-nostart
	LINKCMD=-Xlinker
	LINKSWITCH=-L
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
#			core
#			domvaddon
#			libs
#			domtest
#			testplug
#           cssraddon
all: framework libs addons
	@echo "Themis application and add-ons built successfully."
addons: httpaddon cacheaddon htmlpaddon cssraddon fileaddon csspaddon
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
core: framework cacheaddon httpaddon htmlpaddon csspaddon
	@echo Done building the core application files.
core2: core testplug domvaddon cssvaddon
	@echo Done building the core and special selection files.
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
	@echo "	domvaddon	DOMView add-on"
	@echo " cssvaddon   CSSViewer add-on"
	@echo "	core		Framework, HTTP, HTMLParser and cache add-ons"
	@echo "	core2		Framework, HTTP, cache, testplug, HTMLParser, DOMView and CSSViewer add-ons"
	@echo "	testplug	A sample/test plug-in"
	@echo "	imagehaddon	The official image handling add-on"
	@echo " cssraddon   CSSRenderer add-on"
	@echo "	zip		zips up the files in the Themis directory to Themis.zip"
	@echo "*** NOTE ***"
	@echo "You will get errors if you use any of the \"clean\" targets in combination with any of the build targets."
libs: domlib domstylelib
dist: all zip
	@echo "Done."
zip:
	-@rm Themis.zip 
	@zip -ry9D Themis.zip Themis/
	@echo "Themis.zip has been created."
	
COMMONOBJ= $(foreach file,$(COMMONSOURCES),$(COMMON_OBJECT_DIR)$(notdir $(file:.cpp=.o)))
#framework application
framework: $(BASE_INSTALL_DIR)Themis
$(BASE_INSTALL_DIR)Themis: framework/themis.rsrc frameworkdepends commondepends frameworkobjects commonobjects #$(foreach file,$(notdir $(FRAMEWORKSOURCES:.cpp=.o)),$(FRAMEWORK_OBJECT_DIR)$(file)) $(COMMONOBJ)
#$(BASE_INSTALL_DIR)Themis: frameworkdepends commondepends frameworkobjects commonobjects
	@echo "Linking Themis."
	@$(CC) -o$(BASE_INSTALL_DIR)Themis $(foreach file,$(notdir $(FRAMEWORKSOURCES:.cpp=.o)),$(FRAMEWORK_OBJECT_DIR)$(file)) $(foreach file,$(notdir $(COMMONSOURCES:.cpp=.o)),$(COMMON_OBJECT_DIR)$(file)) -Xlinker $(LDFLAGS) -L ./ \
		-L $(BASE_APP_LIB_INSTALL_DIR) $(NETWORKLIBS) $(STDLIBS) -Xlinker -soname=_APP_  -O0 -lcl -lstdc++.r4 -ltranslation  # -ldom /boot/develop/lib/x86/i386-mcount.o
	@echo "Done linking, adding resource file."
	@xres -o $(BASE_INSTALL_DIR)Themis framework/themis.rsrc
	@mimeset -F $(BASE_INSTALL_DIR)Themis
	@echo "Done adding resources. Themis framework app is ready to run."
	@$(shell if [ ! -f _APP_ ] ; then ln -s Themis/Themis _APP_ ; fi)
frameworkdepends: $(FRAMEWORKSOURCES) $(foreach file,$(FRAMEWORKSOURCES:.cpp=.d),$(FRAMEWORK_DEPENDS_DIR)$(notdir $(file)))
	@echo "Framework dependencies are done."

ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(FRAMEWORKSOURCES:.cpp=.d),$(FRAMEWORK_DEPENDS_DIR)$(notdir $(file)))
endif

frameworkobjects: $(foreach file,$(FRAMEWORKSOURCES),$(FRAMEWORK_OBJECT_DIR)$(notdir $(file:.cpp=.o)))
	@echo "Framework object files are done."
	

$(FRAMEWORK_DEPENDS_DIR)%.d: framework/%.cpp
	@echo "Framework: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(FRAMEWORK_DEPENDS_DIR)$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(FRAMEWORK_OBJECT_DIR)"|sed 's/\//\\\//g'); $(CC) -Icommon -Iframework/DOM -I./ -M $(CPPFLAGS) $(COMPILE_FLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/framework\/\1.o depends\/framework\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@

$(FRAMEWORK_OBJECT_DIR)%.o: framework/%.cpp $(FRAMEWORK_DEPENDS_DIR)%.d
	@echo "Framework: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -Iframework/DOM -I./ -c $< -o $@


#files located in the common directory
commondepends: $(COMMONSOURCES) $(foreach file,$(COMMONSOURCES:.cpp=.d),$(COMMON_DEPENDS_DIR)$(notdir $(file)))
	@echo "Common depends are done."

ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(COMMONSOURCES:.cpp=.d),$(COMMON_DEPENDS_DIR)$(notdir $(file)))
endif

commonobjects: $(foreach file,$(COMMONSOURCES),$(COMMON_OBJECT_DIR)$(notdir $(file:.cpp=.o)))
	@echo "Common object files are done."

$(COMMON_DEPENDS_DIR)%.d: common/%.cpp
	@echo "Common: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(COMMON_DEPENDS_DIR)$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(COMMON_OBJECT_DIR)"|sed 's/\//\\\//g'); $(CC) -Icommon -I./ -Iframework/DOM -Iframework/DOMStyle -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/common\/\1.o depends\/common\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@

$(COMMON_OBJECT_DIR)%.o: common/%.cpp $(COMMON_DEPENDS_DIR)%.d
	@echo "Common: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -I./ -Iframework/DOM -Iframework/DOMStyle -c $< -o $@
	

#file protocol addon
fileaddon: framework $(BASE_ADDON_INSTALL_DIR)file_protocol.so
$(BASE_ADDON_INSTALL_DIR)file_protocol.so: $(BASE_INSTALL_DIR)Themis $(foreach file,$(FILEPROTOCOLSOURCES),$(BASE_ADDONS_OBJECT_DIR)FileProtocol/$(notdir $(file:.cpp=.o)))
	@echo "Linking file protocol..."
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)file_protocol.so $(foreach file,$(notdir $(FILEPROTOCOLSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)FileProtocol/$(file)) -nostart -Xlinker $(LDFLAGS) -soname=file_protocol.so -L ./ $(STDLIBS) -lstdc++.r4 _APP_
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)file_protocol.so
	@echo "File Protocol plug-in compiled and linked successfully."
$(BASE_ADDONS_DEPENDS_DIR)FileProtocol/%.d: $(BASE_ADDON_SOURCE_DIR)FileProtocol/%.cpp 
	@echo "File Protocol: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(FILEPROTOCOL_DEPENDS_DIR)$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/add-ons\/FileProtocol\/\1.o depends\/add-ons\/FileProtocol\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@
$(BASE_ADDONS_OBJECT_DIR)FileProtocol/%.o: $(BASE_ADDON_SOURCE_DIR)FileProtocol/%.cpp $(BASE_ADDONS_DEPENDS_DIR)FileProtocol/%.d
	@echo "File Protocol: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@
ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(BASE_ADDON_SOURCE_DIR)$(FILEPROTOCOLSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)FileProtocol/$(notdir $(file)))
endif
#css parser addon
csspaddon: framework $(BASE_ADDON_INSTALL_DIR)css_parser.so domlib domstylelib
$(BASE_ADDON_INSTALL_DIR)css_parser.so: $(BASE_INSTALL_DIR)Themis $(foreach file,$(CSSPARSERSOURCES),$(BASE_ADDONS_OBJECT_DIR)CSSParser/$(notdir $(file:.cpp=.o)))
	@echo "Linking css parser..."
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)css_parser.so $(foreach file,$(notdir $(CSSPARSERSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)CSSParser/$(file)) -nostart -Xlinker $(LDFLAGS) -soname=css_parser.so -L ./ -L $(BASE_APP_LIB_INSTALL_DIR)  $(STDLIBS) -lstdc++.r4 -ldomstyle -ldom _APP_
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)css_parser.so
	@echo "CSS Parser plug-in compiled and linked successfully."
	@mkdir -p /boot/home/config/settings/Themis/css
	@cp modules/CSSParser/html4.css /boot/home/config/settings/Themis/css/
	@echo "CSS files installed."
$(BASE_ADDONS_DEPENDS_DIR)CSSParser/%.d: $(BASE_ADDON_SOURCE_DIR)CSSParser/%.cpp 
	@echo "CSS Parser: Updating dependency file $(notdir $@)"
	@set -e;$(CC) -Icommon -Iframework/DOM -Iframework/DOMStyle -I./ -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/add-ons\/CSSParser\/\1.o depends\/add-ons\/CSSParser\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@
$(BASE_ADDONS_OBJECT_DIR)CSSParser/%.o: $(BASE_ADDON_SOURCE_DIR)CSSParser/%.cpp $(BASE_ADDONS_DEPENDS_DIR)CSSParser/%.d
	@echo "CSS Parser: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -Iframework/DOM -Iframework/DOMStyle -I./ -c $< -o $@
ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(BASE_ADDON_SOURCE_DIR)$(CSSPARSERSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)CSSParser/$(notdir $(file)))
endif
#message viewer addon
msgvaddon: msgvaddon $(BASE_ADDON_INSTALL_DIR)message_viewer.so
$(BASE_ADDON_INSTALL_DIR)message_viewer.so: $(BASE_INSTALL_DIR)Themis $(foreach file,$(MESSAGEVIEWERSOURCES),$(BASE_ADDONS_OBJECT_DIR)MsgViewer/$(notdir $(file:.cpp=.o)))
	@echo "Linking Message Viewer..."
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)message_viewer.so $(foreach file,$(notdir $(MESSAGEVIEWERSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)MsgViewer/$(file)) -nostart -Xlinker $(LDFLAGS) -soname=file_protocol.so -L ./  $(STDLIBS) -lstdc++.r4 _APP_
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)message_viewer.so
	@echo "Message Viewer plug-in compiled and linked successfully."
$(BASE_ADDONS_DEPENDS_DIR)MsgViewer/%.d: $(BASE_ADDON_SOURCE_DIR)MsgViewer/%.cpp 
	@echo "Message Viewer: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(FILEPROTOCOL_DEPENDS_DIR)$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/add-ons\/MsgViewer\/\1.o depends\/add-ons\/MsgViewer\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@
$(BASE_ADDONS_OBJECT_DIR)MsgViewer/%.o: $(BASE_ADDON_SOURCE_DIR)MsgViewer/%.cpp $(BASE_ADDONS_DEPENDS_DIR)MsgViewer/%.d
	@echo "Message Viewer: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@
ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(BASE_ADDON_SOURCE_DIR)$(MESSAGEVIEWERSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)MsgViewer/$(notdir $(file)))
endif

#cache add-on
cacheaddon: framework $(BASE_ADDON_INSTALL_DIR)cache.so
$(BASE_ADDON_INSTALL_DIR)cache.so: $(BASE_INSTALL_DIR)Themis $(COMMONOBJ) $(foreach file,$(notdir $(CACHEADDONSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)cache/$(file)) #cacheaddondepends cacheaddonobjects
	@echo "Linking cache add-on" 
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)cache.so $(foreach file,$(notdir $(CACHEADDONSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)cache/$(file)) -nostart -Xlinker $(LDFLAGS) -soname=cache.so -L ./ $(STDLIBS) _APP_
	@echo "Cache add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)cache.so
	@echo "Cache plug-in compiled and linked successfully."
	
cacheaddondepends: $(foreach file,$(CACHEADDONSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp common/cacheplug.cpp common/cacheobject.cpp common/cacheuser.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(CACHEADDONSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)cache/$(notdir $(file)))
	@echo "Cache add-on depends are done."
ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(BASE_ADDON_SOURCE_DIR)$(CACHEADDONSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)cache/$(notdir $(file)))
endif
	
cacheaddonobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(CACHEADDONSOURCES),$(BASE_ADDONS_OBJECT_DIR)cache/$(notdir $(file:.cpp=.o)))
	@echo "Cache add-on objects are done."
	
$(BASE_ADDONS_DEPENDS_DIR)cache/%.d:  $(BASE_ADDON_SOURCE_DIR)cache/%.cpp 
	@echo "Cache: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)cache/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)cache/"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/add-ons\/cache\/\1.o depends\/add-ons\/cache\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@
$(BASE_ADDONS_OBJECT_DIR)cache/%.o: $(BASE_ADDON_SOURCE_DIR)cache/%.cpp $(BASE_ADDONS_DEPENDS_DIR)cache/%.d
	@echo "Cache: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@


#testplug add-on
testplug: framework $(BASE_ADDON_INSTALL_DIR)testplug.so
$(BASE_ADDON_INSTALL_DIR)testplug.so: $(BASE_INSTALL_DIR)Themis $(COMMONOBJ) $(foreach file,$(notdir $(TESTPLUGSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)testplug/$(file)) #testplugdepends testplugobjects
	@echo "Linking testplug" 
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)testplug.so $(foreach file,$(notdir $(TESTPLUGSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)testplug/$(file))  -nostart -Xlinker $(LDFLAGS) -soname=testplug.so -L ./ -lbe -ltextencoding -lroot -ltranslation _APP_
	@echo "testplug add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)cache.so
	@echo "testplug plug-in compiled and linked successfully."
	
testplugdepends: $(foreach file,$(TESTPLUGSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(TESTPLUGSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)testplug/$(notdir $(file)))
	@echo "testplug depends are done."
ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(BASE_ADDON_SOURCE_DIR)$(TESTPLUGSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)testplug/$(notdir $(file)))
endif
	
testplugobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(TESTPLUGSOURCES),$(BASE_ADDONS_OBJECT_DIR)testplug/$(notdir $(file:.cpp=.o)))
	@echo "testplug objects are done."
	
$(BASE_ADDONS_DEPENDS_DIR)testplug/%.d:  $(BASE_ADDON_SOURCE_DIR)testplug/%.cpp
	@echo "testplug: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)testplug/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)testplug/"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/add-ons\/testplug\/\1.o depends\/add-ons\/testplug\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@

$(BASE_ADDONS_OBJECT_DIR)testplug/%.o: $(BASE_ADDON_SOURCE_DIR)testplug/%.cpp $(BASE_ADDONS_DEPENDS_DIR)testplug/%.d
	@echo "testplug: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@

#imageh add-on
imagehaddon: framework $(BASE_ADDON_INSTALL_DIR)image_handler.so
$(BASE_ADDON_INSTALL_DIR)image_handler.so: $(BASE_INSTALL_DIR)Themis $(COMMONOBJ) $(foreach file,$(notdir $(IMAGEHSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)imageh/$(file)) #commondepends imagehdepends commonobjects imagehobjects
	@echo "Linking image handler" 
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)image_handler.so $(foreach file,$(notdir $(IMAGEHSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)imageh/$(file)) -nostart -Xlinker $(LDFLAGS) -soname=image_handler.so -L ./ -lbe -lroot -ltranslation _APP_
	@echo "image handler add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)cache.so
	@echo "Image Handler plug-in compiled and linked successfully."
	
imagehdepends: $(foreach file,$(IMAGEHSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(IMAGEHSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)imageh/$(notdir $(file)))
	@echo "image handler depends are done."
ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(BASE_ADDON_SOURCE_DIR)$(IMAGEHSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)imageh/$(notdir $(file)))
endif
	
imagehobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(IMAGEHSOURCES),$(BASE_ADDONS_OBJECT_DIR)imageh/$(notdir $(file:.cpp=.o)))
	@echo "image handler objects are done."
	
$(BASE_ADDONS_DEPENDS_DIR)imageh/%.d:  $(BASE_ADDON_SOURCE_DIR)imageh/%.cpp 
	@echo "ImageH: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)imageh/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)imageh/"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/add-ons\/imageh\/\1.o depends\/add-ons\/imageh\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@

$(BASE_ADDONS_OBJECT_DIR)imageh/%.o: $(BASE_ADDON_SOURCE_DIR)imageh/%.cpp $(BASE_ADDONS_DEPENDS_DIR)imageh/%.d
	@echo "ImageH: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@


#html parser add-on
htmlpaddon: framework $(BASE_ADDON_INSTALL_DIR)html_parser.so domlib
$(BASE_ADDON_INSTALL_DIR)html_parser.so:  $(BASE_INSTALL_DIR)Themis $(foreach file,$(HTMLPARSERSOURCES),$(BASE_ADDONS_OBJECT_DIR)HTMLParser/$(notdir $(file:.cpp=.o))) #domlib htmlparserdepends htmlparserobjects
	@echo "Linking HTML Parser add-on $@" 
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)html_parser.so $(foreach file,$(notdir $(HTMLPARSERSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)HTMLParser/$(file)) -nostart -Xlinker $(LDFLAGS) -soname=html_parser.so -L ./ -L $(BASE_APP_LIB_INSTALL_DIR)  -lbe -lroot -ldom -lstdc++.r4 _APP_
	@echo "HTML Parser add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)html_parser.so
	@echo "HTML Parser plug-in compiled and linked successfully."
	@mkdir -p /boot/home/config/settings/Themis/dtd
	@cp modules/HTMLParser/HTML.3.2.Final.DTD /boot/home/config/settings/Themis/dtd/
	@cp modules/HTMLParser/HTML.4.0.Transitional.DTD /boot/home/config/settings/Themis/dtd/
	@echo "HTML Parser DTD's installed."
	
htmlparserdepends: $(foreach file,$(HTMLPARSERSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(HTMLPARSERSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)HTMLParser/$(notdir $(file)))
	@echo "HTML Parser depends are done."
	
htmlparserobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(HTMLPARSERSOURCES),$(BASE_ADDONS_OBJECT_DIR)HTMLParser/$(notdir $(file:.cpp=.o)))
	@echo "HTML Parser objects are done."
	
$(BASE_ADDONS_DEPENDS_DIR)HTMLParser/%.d:  $(BASE_ADDON_SOURCE_DIR)HTMLParser/%.cpp
	@echo "HTML Parser: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)HTMLParser/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)HTMLParser/"|sed 's/\//\\\//g'); $(CC) -Icommon -Iframework/DOM -I./ -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/add-ons\/HTMLParser\/\1.o depends\/add-ons\/HTMLParser\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@
ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(BASE_ADDON_SOURCE_DIR)$(HTMLPARSERSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)HTMLParser/$(notdir $(file)))
endif

$(BASE_ADDONS_OBJECT_DIR)HTMLParser/%.o: $(BASE_ADDON_SOURCE_DIR)HTMLParser/%.cpp $(BASE_ADDONS_DEPENDS_DIR)HTMLParser/%.d
	@echo "HTML Parser: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -Iframework/DOM -I./ -c $< -o $@

#domview add-on
domvaddon: framework $(BASE_ADDON_INSTALL_DIR)DOMView.so domlib
$(BASE_ADDON_INSTALL_DIR)DOMView.so: $(BASE_INSTALL_DIR)Themis $(foreach file,$(DOMVIEWSOURCES),$(BASE_ADDONS_OBJECT_DIR)DOMView/$(notdir $(file:.cpp=.o))) #commondepends domvdepends commonobjects domvobjects
	@echo "Linking DOM View add-on" 
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)DOMView.so $(foreach file,$(notdir $(DOMVIEWSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)DOMView/$(file)) -nostart -Xlinker $(LDFLAGS) -soname=DOMView.so -L ./ -L$(BASE_APP_LIB_INSTALL_DIR)  -lbe -lroot -ldom -lstdc++.r4 _APP_
	@echo "DOM View add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)DOMView.so
	@echo "DOM View plug-in compiled and linked successfully."
	
domvdepends: $(foreach file,$(DOMVIEWSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d\
				$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(DOMVIEWSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)DOMView/$(notdir $(file)))
	@echo "DOMView depends are done."
ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(BASE_ADDON_SOURCE_DIR)$(DOMVIEWSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)DOMView/$(notdir $(file)))
endif
	
domvobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(DOMVIEWSOURCES),$(BASE_ADDONS_OBJECT_DIR)DOMView/$(notdir $(file:.cpp=.o)))
	@echo "DOM View objects are done."
	
$(BASE_ADDONS_DEPENDS_DIR)DOMView/%.d:  $(BASE_ADDON_SOURCE_DIR)DOMView/%.cpp 
	@echo "DOM View: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)DOMView/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)DOMView/"|sed 's/\//\\\//g'); $(CC) -Icommon -Iframework/DOM -I./ -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/add-ons\/DOMView\/\1.o depends\/add-ons\/DOMView\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@

$(BASE_ADDONS_OBJECT_DIR)DOMView/%.o: $(BASE_ADDON_SOURCE_DIR)DOMView/%.cpp $(BASE_ADDONS_DEPENDS_DIR)DOMView/%.d
	@echo "DOM View: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -Iframework/DOM -I./ -c $< -o $@

#cssviewer add-on
cssvaddon: framework $(BASE_ADDON_INSTALL_DIR)css_viewer.so domlib
$(BASE_ADDON_INSTALL_DIR)css_viewer.so: $(BASE_INSTALL_DIR)Themis $(foreach file,$(CSSVIEWERSOURCES),$(BASE_ADDONS_OBJECT_DIR)CSSViewer/$(notdir $(file:.cpp=.o)))
	@echo "Linking CSS Viewer add-on" 
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)css_viewer.so $(foreach file,$(notdir $(CSSVIEWERSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)CSSViewer/$(file)) -nostart -Xlinker $(LDFLAGS) -soname=css_viewer.so -L ./ -L$(BASE_APP_LIB_INSTALL_DIR)  -lbe -lroot -ldomstyle -ldom -lstdc++.r4 _APP_
	@echo "CSS Viewer add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)css_viewer.so
	@echo "CSS Viewer plug-in compiled and linked successfully."
	
cssvdepends: $(foreach file,$(CSSVIEWERSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d\
				$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(CSSVIEWERSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)CSSViewer/$(notdir $(file)))
	@echo "CSSViewer depends are done."
ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(BASE_ADDON_SOURCE_DIR)$(CSSVIEWERSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)CSSViewer/$(notdir $(file)))
endif
	
cssvobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(CSSVIEWERSOURCES),$(BASE_ADDONS_OBJECT_DIR)CSSViewer/$(notdir $(file:.cpp=.o)))
	@echo "CSS Viewer objects are done."
	
$(BASE_ADDONS_DEPENDS_DIR)CSSViewer/%.d:  $(BASE_ADDON_SOURCE_DIR)CSSViewer/%.cpp 
	@echo "CSS Viewer: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)CSSViewer/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)CSSViewer/"|sed 's/\//\\\//g'); $(CC) -Icommon -Iframework/DOM -Iframework/DOMStyle -I./ -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/add-ons\/CSSViewer\/\1.o depends\/add-ons\/CSSViewer\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@

$(BASE_ADDONS_OBJECT_DIR)CSSViewer/%.o: $(BASE_ADDON_SOURCE_DIR)CSSViewer/%.cpp $(BASE_ADDONS_DEPENDS_DIR)CSSViewer/%.d
	@echo "CSS Viewer: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -Iframework/DOM -Iframework/DOMStyle -I./ -c $< -o $@

#cssrenderer add-on
cssraddon: framework $(BASE_ADDON_INSTALL_DIR)css_renderer.so domlib domstylelib
$(BASE_ADDON_INSTALL_DIR)css_renderer.so: $(BASE_INSTALL_DIR)Themis $(foreach file,$(CSSRENDERERSOURCES),$(BASE_ADDONS_OBJECT_DIR)CSSRenderer/$(notdir $(file:.cpp=.o)))
	@echo "Linking CSS Renderer add-on" 
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)css_renderer.so $(foreach file,$(notdir $(CSSRENDERERSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)CSSRenderer/$(file)) -nostart -Xlinker $(LDFLAGS) -soname=css_renderer.so -L ./ -L$(BASE_APP_LIB_INSTALL_DIR)  -lbe -lroot -ltranslation -ldomstyle -ldom -lstdc++.r4 _APP_
	@echo "CSS Renderer add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)css_renderer.so
	@echo "CSS Renderer plug-in compiled and linked successfully."
	
cssrdepends: $(foreach file,$(CSSRENDERERSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d\
				$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(CSSRENDERERSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)CSSRenderer/$(notdir $(file)))
	@echo "CSSRenderer depends are done."
ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(BASE_ADDON_SOURCE_DIR)$(CSSRENDERERSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)CSSRenderer/$(notdir $(file)))
endif
	
cssrobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o\
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(CSSRENDERERSOURCES),$(BASE_ADDONS_OBJECT_DIR)CSSRenderer/$(notdir $(file:.cpp=.o)))
	@echo "CSS Renderer objects are done."
	
$(BASE_ADDONS_DEPENDS_DIR)CSSRenderer/%.d:  $(BASE_ADDON_SOURCE_DIR)CSSRenderer/%.cpp 
	@echo "CSS Renderer: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)CSSRenderer/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)CSSRenderer/"|sed 's/\//\\\//g'); $(CC) -Icommon -Iframework/DOM -Iframework/DOMStyle -I./ -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/add-ons\/CSSRenderer\/\1.o depends\/add-ons\/CSSRenderer\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@

$(BASE_ADDONS_OBJECT_DIR)CSSRenderer/%.o: $(BASE_ADDON_SOURCE_DIR)CSSRenderer/%.cpp $(BASE_ADDONS_DEPENDS_DIR)CSSRenderer/%.d
	@echo "CSS Renderer: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -Iframework/DOM -Iframework/DOMStyle -I./ -c $< -o $@

#http add-on
httpaddon: framework $(BASE_ADDON_INSTALL_DIR)http_plugin.so
$(BASE_ADDON_INSTALL_DIR)http_plugin.so: $(BASE_INSTALL_DIR)Themis $(foreach file,$(notdir $(HTTPADDONSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)http/$(file)) #commondepends httpaddondepends commonobjects httpaddonobjects
	@echo "Linking http add-on" 
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)http_plugin.so  $(foreach file,$(notdir $(HTTPADDONSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)http/$(file)) -nostart -Xlinker $(LDFLAGS) -soname=http_plugin.so -L ./ -L$(BASE_APP_LIB_INSTALL_DIR) -lz -lcl $(NETWORKLIBS) $(STDLIBS) -ltranslation  _APP_
	@echo "HTTP add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)http_plugin.so
	@echo "HTTP plug-in compiled and linked successfully."

# the next line will replace the current one when the new networking stack is added to the repository. at that time
# be sure to remove the extra backslash at the end of the line
httpaddondepends: $(foreach file,$(HTTPADDONSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d \
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(HTTPADDONSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)http/$(notdir $(file)))
	@echo "HTTP add-on depends are done."
ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(BASE_ADDON_SOURCE_DIR)$(HTTPADDONSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)http/$(notdir $(file)))
endif
	
httpaddonobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o \
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(HTTPADDONSOURCES),$(BASE_ADDONS_OBJECT_DIR)http/$(notdir $(file:.cpp=.o)))
	@echo "HTTP add-on objects are done."
	
$(BASE_ADDONS_DEPENDS_DIR)http/%.d:  $(BASE_ADDON_SOURCE_DIR)http/%.cpp 
	@echo "HTTP: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)http/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)http/"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $(COMPILE_FLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/add-ons\/http\/\1.o depends\/add-ons\/http\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@

$(BASE_ADDONS_OBJECT_DIR)http/%.o: $(BASE_ADDON_SOURCE_DIR)http/%.cpp $(BASE_ADDONS_DEPENDS_DIR)http/%.d
	@echo "HTTP: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@

#cookie add-on
cookieaddon: framework $(BASE_ADDON_INSTALL_DIR)cookie_plugin.so
$(BASE_ADDON_INSTALL_DIR)cookie_plugin.so: $(BASE_INSTALL_DIR)Themis $(COMMONOBJ) $(foreach file,$(notdir $(COOKIEADDONSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)cookie/$(file)) #commondepends cookieaddondepends commonobjects cookieaddonobjects
	@echo "Linking cookie add-on"
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)cookie_plugin.so  $(foreach file,$(notdir $(COOKIEADDONSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)cookie/$(file)) -nostart -Xlinker $(LDFLAGS) -soname=cookie_plugin.so -L ./ -lbe -lroot -ltextencoding _APP_
	@echo "Cookie add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)cookie_plugin.so
	@echo "Cookie plug-in compiled and linked successfully."
cookieaddondepends: $(foreach file,$(COOKIEADDONSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d \
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(COOKIEADDONSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)cookie/$(notdir $(file)))
	@echo "Cookie add-on depends are done."
ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(BASE_ADDON_SOURCE_DIR)$(COOKIEADDONSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)cookie/$(notdir $(file)))
endif
cookieaddonobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o  \
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(COOKIEADDONSOURCES),$(BASE_ADDONS_OBJECT_DIR)cookie/$(notdir $(file:.cpp=.o)))
	@echo "Cookie add-on objects are done."
$(BASE_ADDONS_DEPENDS_DIR)cookie/%.d:  $(BASE_ADDON_SOURCE_DIR)cookie/%.cpp 
	@echo "Cookie: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)cookie/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)cookie/"|sed 's/\//\\\//g'); $(CC) -Icommon -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/add-ons\/cookie\/\1.o depends\/add-ons\/cookie\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@

$(BASE_ADDONS_OBJECT_DIR)cookie/%.o: $(BASE_ADDON_SOURCE_DIR)cookie/%.cpp $(BASE_ADDONS_DEPENDS_DIR)cookie/%.d
	@echo "Cookie: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -c $< -o $@

#Javascript add-on
jsaddon: domlib $(BASE_ADDON_INSTALL_DIR)js_plugin.so
$(BASE_ADDON_INSTALL_DIR)js_plugin.so: $(BASE_INSTALL_DIR)Themis $(COMMONOBJ) $(foreach file,$(notdir $(JSADDONSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)js/$(file)) #domlib commondepends jsaddondepends commonobjects jsaddonobjects
	@echo "Copying Javscript library and supporting libraries to install directory."
	@$(shell if [ "$(NETWORKING)" == "BONE" ] ; then /bin/unzip -uqd Themis/lib/ javascript/bone_jslibs.zip ; else /bin/unzip -uqd Themis/lib/ javascript/netserver_jslibs.zip ; fi )
	@echo "Linking Javascript add-on"
	@$(CC) -o$(BASE_ADDON_INSTALL_DIR)js_plugin.so $(foreach file,$(notdir $(JSADDONSOURCES:.cpp=.o)),$(BASE_ADDONS_OBJECT_DIR)js/$(file)) -nostart -Xlinker $(LDFLAGS) -soname=js_plugin.so -L ./ -L javascript/ -L $(BASE_APP_LIB_INSTALL_DIR) -lbe -lroot -ltextencoding -ljs -ldom _APP_
	@echo "Javascript add-on is now linked. Doing mimeset."
	@mimeset -F $(BASE_ADDON_INSTALL_DIR)js_plugin.so
	@echo "Javascript plug-in compiled and linked successfully."
#	@$(shell for file in $(JAVASCRIPT_SUPPORT_LIBRARIES); do if [ ! -f $(BASE_APP_LIB_INSTALL_DIR)`basename $$file` ] ; then cp $$file $(BASE_APP_LIB_INSTALL_DIR); fi ;  done)
	@echo "Done with Javascript add-on."
jsaddondepends: $(foreach file,$(JSADDONSOURCES),$(BASE_ADDON_SOURCE_DIR)$(file)) common/plugman.cpp common/plugclass.cpp $(COMMON_DEPENDS_DIR)plugman.d $(COMMON_DEPENDS_DIR)plugclass.d \
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(JSADDONSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)js/$(notdir $(file)))
	@echo "Javascript add-on depends are done."
ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(BASE_ADDON_SOURCE_DIR)$(JSADDONSOURCES:.cpp=.d),$(BASE_ADDONS_DEPENDS_DIR)js/$(notdir $(file)))
endif
jsaddonobjects: $(COMMON_OBJECT_DIR)plugman.o $(COMMON_OBJECT_DIR)plugclass.o  \
					$(foreach file,$(BASE_ADDON_SOURCE_DIR)$(JSADDONSOURCES),$(BASE_ADDONS_OBJECT_DIR)js/$(notdir $(file:.cpp=.o)))
	@echo "Javascript add-on objects are done."
$(BASE_ADDONS_DEPENDS_DIR)js/%.d:  $(BASE_ADDON_SOURCE_DIR)js/%.cpp 
	@echo "Javascript: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(BASE_ADDONS_DEPENDS_DIR)js/$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(BASE_ADDONS_OBJECT_DIR)js/"|sed 's/\//\\\//g'); $(CC) -DXP_UNIX -Icommon -Ijavascript -Iframework/DOM -I./ -M $(CPPFLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/add-ons\/js\/\1.o depends\/add-ons\/js\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@

$(BASE_ADDONS_OBJECT_DIR)js/%.o: $(BASE_ADDON_SOURCE_DIR)js/%.cpp $(BASE_ADDONS_DEPENDS_DIR)js/%.d
	@echo "Javascript: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -DXP_UNIX -D_BEOS_ -Icommon -Ijavascript  -Iframework/DOM -I./ -c $< -o $@

#DOM Library
domlib: framework $(BASE_APP_LIB_INSTALL_DIR)libdom.so
$(BASE_APP_LIB_INSTALL_DIR)libdom.so: $(foreach file,$(notdir $(DOMLIBRARYSOURCES:.cpp=.o)),$(DOMLIBRARY_OBJECT_DIR)$(file)) #domlibdepends domlibobjects
	@echo "Linking DOM Library"
	@$(CC) -o $(BASE_APP_LIB_INSTALL_DIR)libdom.so $(foreach file,$(notdir $(DOMLIBRARYSOURCES:.cpp=.o)),$(DOMLIBRARY_OBJECT_DIR)$(file)) -nostart -Xlinker $(LDFLAGS) -shared -Xlinker -soname=libdom.so -L ./ -L $(DOMLIBRARY_OBJECT_DIR) -lstdc++.r4
	@echo "DOM Library is now linked. Doing mimeset."
	@mimeset -F $(BASE_APP_LIB_INSTALL_DIR)libdom.so
	@echo "DOM Library compiled and linked successfully."
	
domlibdepends: $(DOMLIBRARYSOURCES) $(foreach file,$(DOMLIBRARYSOURCES:.cpp=.d),$(DOMLIBRARY_DEPENDS_DIR)$(notdir $(file)))
	@echo "DOM Library depends are done. $*"
ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(DOMLIBRARYSOURCES:.cpp=.d),$(DOMLIBRARY_DEPENDS_DIR)$(notdir $(file)))
endif
domlibobjects: $(foreach file,$(DOMLIBRARYSOURCES),$(DOMLIBRARY_OBJECT_DIR)$(notdir $(file:.cpp=.o)))
	@echo "DOM Library objects are done."
	
$(DOMLIBRARY_DEPENDS_DIR)%.d: framework/DOM/$(notdir %).cpp framework/DOM/DOMSupport.h framework/DOM/TComment.h
	@echo "DOM: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(DOMLIBRARY_DEPENDS_DIR)$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(DOMLIBRARY_OBJECT_DIR)"|sed 's/\//\\\//g'); $(CC) -Icommon -I./ -M $(CPPFLAGS) $(COMPILE_FLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/lib\/DOM\/\1.o depends\/lib\/DOM\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@

$(DOMLIBRARY_OBJECT_DIR)%.o: framework/DOM/%.cpp $(DOMLIBRARY_DEPENDS_DIR)%.d
	@echo "DOM: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -I./ -c $< -o $@ -Wall -Wno-multichar -Wno-ctor-dtor-privacy -Woverloaded-virtual -O3

#DOM Test application
domtest: framework domlib domtestdepends domtestobjects
	@$(CC) -o $(BASE_INSTALL_DIR)DOMTest $(foreach file,$(notdir $(DOMTESTSOURCES:.cpp=.o)),$(DOMLIBRARY_OBJECT_DIR)$(file)) -Xlinker $(LDFLAGS) -L ./ -L $(BASE_APP_LIB_INSTALL_DIR) -lbe -ldom -lstdc++.r4
	@echo "DOM Test application is now linked. Doing mimeset."
	@mimeset -F $(BASE_INSTALL_DIR)DOMTest
	@echo "DOM Test application compiled and linked successfully."
domtestdepends:  $(DOMTESTSOURCES) $(foreach file,$(DOMTESTSOURCES:.cpp=.d),$(DOMLIBRARY_DEPENDS_DIR)$(notdir $(file)))
	@echo "DOM Test application depends are done."
ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(DOMTESTSOURCES:.cpp=.d),$(DOMLIBRARY_DEPENDS_DIR)$(notdir $(file)))
endif
	
domtestobjects: $(foreach file,$(DOMTESTSOURCES),$(DOMLIBRARY_OBJECT_DIR)$(notdir $(file:.cpp=.o)))
	@echo "DOM Test application objects are done."

#DOMStyle Library
domstylelib: framework $(BASE_APP_LIB_INSTALL_DIR)libdomstyle.so domlib
$(BASE_APP_LIB_INSTALL_DIR)libdomstyle.so: $(foreach file,$(notdir $(DOMSTYLELIBRARYSOURCES:.cpp=.o)),$(DOMSTYLELIBRARY_OBJECT_DIR)$(file))
	@echo "Linking DOMStyle Library"
	@$(CC) -o $(BASE_APP_LIB_INSTALL_DIR)libdomstyle.so $(foreach file,$(notdir $(DOMSTYLELIBRARYSOURCES:.cpp=.o)),$(DOMSTYLELIBRARY_OBJECT_DIR)$(file)) -nostart -Xlinker $(LDFLAGS) -shared -Xlinker -soname=libdomstyle.so -L ./ -L$(BASE_APP_LIB_INSTALL_DIR) -L $(DOMSTYLELIBRARY_OBJECT_DIR) -lstdc++.r4 -ldom
	@echo "DOMStyle Library is now linked. Doing mimeset."
	@mimeset -F $(BASE_APP_LIB_INSTALL_DIR)libdomstyle.so
	@echo "DOMStyle Library compiled and linked successfully."

$(DOMSTYLELIBRARY_DEPENDS_DIR)%.d: framework/DOMStyle/$(notdir %).cpp
	@echo "DOMStyle: Updating dependency file $(notdir $@)"
	@set -e;VAR1=$(shell echo "$(DOMSTYLELIBRARY_DEPENDS_DIR)$(notdir $@)"|sed 's/\//\\\//g');VAR2=$(shell echo "$(DOMSTYLELIBRARY_OBJECT_DIR)"|sed 's/\//\\\//g'); $(CC) -Icommon -Iframework/DOM -I./ -M $(CPPFLAGS) $(COMPILE_FLAGS) $< | sed 's/\($*\)\.o[ :]*/objects\/lib\/DOMStyle\/\1.o depends\/lib\/DOMStyle\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@
	
$(DOMSTYLELIBRARY_OBJECT_DIR)%.o: framework/DOMStyle/%.cpp $(DOMSTYLELIBRARY_DEPENDS_DIR)%.d
	@echo "DOMStyle: Updating object file $(notdir $@)"
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -Icommon -Iframework/DOM -I./ -c $< -o $@ -Wall -Wno-multichar -Wno-ctor-dtor-privacy -Woverloaded-virtual -O3

ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(DOMSTYLELIBRARYSOURCES:.cpp=.d),$(DOMSTYLELIBRARY_DEPENDS_DIR)$(notdir $(file)))
endif
