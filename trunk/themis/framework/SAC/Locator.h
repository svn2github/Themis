#ifndef __LOCATOR_H__
#define __LOCATOR_H__

#include <string>

namespace sac
{

class Locator
{
	public:
		/**
	     * Return the URI for the current document event.
	     *
	     * <p>The parser must resolve the URI fully before passing it to the
	     * application.</p>
	     *
	     * @return A string containing the URI, or null
	     *         if none is available.
	     */
		virtual std::string getURI() const = 0;

		/**
	     * Return the line number where the current document event ends.
	     * Note that this is the line position of the first character
	     * after the text associated with the document event.
	     * @return The line number, or -1 if none is available.
	     * @see #getColumnNumber
	     */
		virtual int getLineNumber() const = 0;
		
	    /**
	     * Return the column number where the current document event ends.
	     * Note that this is the column number of the first
	     * character after the text associated with the document
	     * event.  The first column in a line is position 1.
	     * @return The column number, or -1 if none is available.
	     * @see #getLineNumber
	     */
		virtual int getColumnNumber() const = 0;
};

}

#endif // __LOCATOR_H__