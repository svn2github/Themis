#ifndef __FLUTE_PARSER_CHARSTREAM_H__
#define __FLUTE_PARSER_CHARSTREAM_H__

#include <string>
#include <vector>

namespace flute
{

class IOException
{
};

class CharStream
{
	public:
		/**
		 * Returns the next character from the selected input.  The method
		 * of selecting the input is the responsibility of the class
		 * implementing this interface.  Can throw any java.io.IOException.
		 */
		virtual char readChar() throw(IOException) = 0;
		
		/**
		 * Returns the column position of the character last read.
		 * @deprecated 
		 * @see #getEndColumn
		 */
		virtual int getColumn() const = 0;
	
		/**
		 * Returns the line number of the character last read.
		 * @deprecated 
		 * @see #getEndLine
		 */	
		virtual int getLine() const = 0;
		
		/**
		 * Returns the column number of the last character for current token (being
		 * matched after the last call to BeginTOken).
		 */
		virtual int getEndColumn() const = 0;
			
		/**
		 * Returns the line number of the last character for current token (being
		 * matched after the last call to BeginTOken).
		 */
		 virtual int getEndLine() const = 0;
		 
		/**
		 * Returns the column number of the first character for current token (being
		 * matched after the last call to BeginTOken).
		 */
		 virtual int getBeginColumn() const = 0;
		 
		/**
		 * Returns the line number of the first character for current token (being
		 * matched after the last call to BeginTOken).
		 */
		 virtual int getBeginLine() const = 0;
		 
		/**
		 * Backs up the input stream by amount steps. Lexer calls this method if it
		 * had already read some characters, but could not use them to match a
		 * (longer) token. So, they will be used again as the prefix of the next
		 * token and it is the implemetation's responsibility to do this right.
		 */
		virtual void backup(int amount) = 0;
		
		/**
		 * Returns the next character that marks the beginning of the next token.
		 * All characters must remain in the buffer between two successive calls
		 * to this method to implement backup correctly.
		 */
		virtual char BeginToken() throw(IOException) = 0;
		
		/**
		 * Returns a string made up of characters from the marked token beginning 
		 * to the current buffer position. Implementations have the choice of returning
		 * anything that they want to. For example, for efficiency, one might decide
		 * to just return null, which is a valid implementation.
		 */
		virtual std::string GetImage() = 0;
		
		/**
		 * Returns an array of characters that make up the suffix of length 'len' for
		 * the currently matched token. This is used to build up the matched string
		 * for use in actions in the case of MORE. A simple and inefficient
		 * implementation of this is as follows :
		 *
		 *   {
		 *      String t = GetImage();
		 *      return t.substring(t.length() - len, t.length()).toCharArray();
		 *   }
		 */
		virtual std::vector<char> GetSuffix(int len) = 0;
		
		/**
		 * The lexer calls this function to indicate that it is done with the stream
		 * and hence implementations can free any resources held by this class.
		 * Again, the body of this function can be just empty and it will not
		 * affect the lexer's operation.
		 */
		virtual void Done() = 0;
};

}

#endif // __FLUTE_PARSER_CHARSTREAM_H__