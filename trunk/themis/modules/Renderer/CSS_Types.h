#ifndef _CSS_TYPES_H_
#define _CSS_TYPES_H_

union IntOrFloat {
	signed float real;
	signed int32 integer;
};

class CSSType {
	public:
							CSSType(float value);
							CSSType(int value);
							~CSSType();
		
				bool 		typer;  //true when the union contains an int 
									//and false when a float
		
		inline int32 		Value() { return value.integer };
		inline float 		Value() { return value.real };

	private:
				IntOrFloat 	value; 
};

class CSSNumber : public CSSType
{
	public:
							CSSNumber(int value){};
		virtual				~CSSNumber(){};
};

class CSSLength : public CSSType
{
	public:
							CSSLength(float value);
		virtual				~CSSLength();
	
};
#endif



