#include "ParseException.h"

using namespace sac;
using namespace flute;

ParseException::ParseException(const Token& currentTokenVal,
	vector< vector<int> > expectedTokenSequencesVal,
	vector<string> tokenImageVal)
:CSSException("")
{
	specialConstructor = true;
	currentToken = const_cast<Token*>(&(currentTokenVal));
	expectedTokenSequences = expectedTokenSequencesVal;
    tokenImage = tokenImageVal;
}

ParseException::ParseException()
:CSSException()
{
	specialConstructor = false;
}

ParseException::ParseException(const string& message)
:CSSException(message)
{
	specialConstructor = false;	
}

string ParseException::getMessage()
{
	if (!specialConstructor) 
	{
      return CSSException::getMessage();
    }
    string expected = "";
    size_t maxSize = 0;
    for(unsigned int i=0; i<expectedTokenSequences.size(); i++)
    {
    	if(maxSize < expectedTokenSequences[i].size())
    		maxSize = expectedTokenSequences[i].size();
    	
    	for(unsigned int j=0; j<expectedTokenSequences[i].size(); j++)
    	{
    		expected += tokenImage[expectedTokenSequences[i][j]] + " ";
    	}
    	
    	if(expectedTokenSequences[i][expectedTokenSequences[i].size() - 1] != 0)
    		expected += "...";
    	
    	expected += "\n    ";
    }
   	string retval = "Encountered \"";
   	Token* tok = currentToken->next;
   	for(unsigned int i=0; i<maxSize; i++)
   	{
   		if(i != 0) 
   			retval += " ";
   		if(tok->kind == 0)
   		{
   			retval += tokenImage[0];
   			break;
   		}
   		retval += add_excapes( tok->image );
   		tok = tok->next;
   	}
   	retval += "\" at line ";
   	retval += currentToken->next->beginLine;
   	retval += ", column ";
   	retval += currentToken->next->beginColumn;
   	retval += ".\n";
   	if (expectedTokenSequences.size() == 1) {
      retval += "Was expecting:\n    ";
    } else {
      retval += "Was expecting one of:\n    ";
    }
    retval += expected;
    return retval;
}

string ParseException::add_excapes(const string& str)
{
	string retval = "";
	for(size_t i=0; i<str.size(); i++)
	{
		switch((int)str[i])
		{
			case 0:
				break;
			case '\b':
				retval += "\\b";
				break;
			case '\t':
				retval += "\\t";
				break;
			case '\n':
				retval += "\\n";
				break;
			case '\f':
				retval += "\\f";
				break;
			case '\r':
				retval += "\\\r";
				break;
			case '\"':
				retval += "\\\"";
				break;
			case '\'':
				retval += "\\\'";
				break;
			case '\\':
				retval += "\\\\";
				break;
			default:
				retval += str[i];
				break;
		}
	}
	return retval;
}