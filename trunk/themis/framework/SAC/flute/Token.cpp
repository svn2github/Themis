#include "Token.h"

using namespace flute;
using namespace std;

Token::Token()
{
}

Token::~Token()
{
}

Token* Token::newToken(int ofKind)
{
	switch(ofKind)
	{
		default : return new Token();
	}
}