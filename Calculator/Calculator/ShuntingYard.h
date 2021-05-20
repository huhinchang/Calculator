#pragma once
class Token {
public:
	enum Type {
		Number,
		Operator,
		OpeningBracket,
		ClosingBracket,
	};

	const Type type;
	const float value; // acts as the number if type is Number or precedence if type is Operator
	const char operatorChar;

	Token(const Type t, const float v, const int c = '?') : type(t), value(v), operatorChar(c) {
	}
};

float parse_and_evaluate(const std::string equation, const float vars[]);