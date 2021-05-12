// Calculator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <sstream> 
#include <stdexcept>

#define NUMBER_PRECEDENCE 0
#define ADD_PRECEDENCE 1
#define SUB_PRECEDENCE 1
#define MULT_PRECEDENCE 2
#define DIV_PRECEDENCE 2
#define POW_PRECEDENCE 3

class Token {
public:
	enum Type {
		Number,
		Operator,
		OpeningBracket,
		ClosingBracket,
	};

	const Type type;
	const int value; // acts as the number if type is Number or precedence if type is Operator
	const char operatorChar;

	Token(const Type t, const int v, const int c = '?') : type(t), value(v), operatorChar(c) {
	}
};

void print_help() {
	std::cout << "##### HELP #####" << std::endl;
}

std::queue<Token> tokenize(const std::string equation) {
	std::cout << "evaluating equation " << equation << std::endl;
	std::queue<Token> output;

	for (size_t i = 0; i < equation.length(); ++i)
	{
		const char c = equation[i];
		if (isdigit(c)) {
			std::cout << "type:digit\n";
			/*
			int len = 1;
			while (isdigit(equation[i + len])) {
				++len;
			}
			output.push(Token(Token::Type::Number, equation.substr(i, len), 0));
			i += len - 1; // skip the "read" characters
			*/
			int number = std::stoi(equation.substr(i));
			output.push(Token(Token::Type::Number, number));
			i += number / 10; // skip the "read" characters
		} else {
			switch (c)
			{
			case '+':
				output.push(Token(Token::Type::Operator, ADD_PRECEDENCE, c));
				break;
			case '-':
				output.push(Token(Token::Type::Operator, SUB_PRECEDENCE, c));
				break;
			case '*':
				output.push(Token(Token::Type::Operator, MULT_PRECEDENCE, c));
				break;
			case '/':
				output.push(Token(Token::Type::Operator, DIV_PRECEDENCE, c));
				break;
			case '(':
				output.push(Token(Token::Type::OpeningBracket, 0));
				break;
			case ')':
				output.push(Token(Token::Type::ClosingBracket, 0));
				break;
			default:
				throw std::invalid_argument("invalid character detected in the equation!");
				break;
			}
		}
	}
	std::cout << "tokenization complete" << std::endl;
	return output;
	// std::cout << equation << " + 1 = " << std::stof(equation) + 1 << std::endl;
}

void print_tokens(std::queue<Token> tokens) {
	std::cout << "#############" << std::endl;
	while (!tokens.empty()) {
		if (tokens.front().type == Token::Type::Number) {
			std::cout << tokens.front().value << std::endl;
		} else {
			std::cout << tokens.front().operatorChar << std::endl;
		}
		tokens.pop();
	}
}

std::queue<Token> shunting_yard(std::queue<Token> tokens) {
	// uses the shunting-yard algorithm
	std::queue<Token> outQ;    // output queue
	std::stack<Token> opStack; // operator stack
	while (!tokens.empty()) {
		Token token = tokens.front();
		tokens.pop();

		if (token.type == Token::Type::Number) {
			outQ.push(token);
		} else if (token.type == Token::Type::Operator) {
			while (!opStack.empty() && opStack.top().value > token.value && opStack.top().type != Token::Type::OpeningBracket) {
				outQ.push(opStack.top());
				opStack.pop();
			}
			opStack.push(token);
		} else if (token.type == Token::Type::OpeningBracket) {
			opStack.push(token);
		} else if (token.type == Token::Type::ClosingBracket) {
			while (!opStack.empty() && opStack.top().type != Token::Type::ClosingBracket) {
				outQ.push(opStack.top());
				opStack.pop();
			}
			if (opStack.empty()) {
				throw std::invalid_argument("Mismatching brackets!");
			} else if (opStack.top().type == Token::Type::OpeningBracket) {
				opStack.pop();
			} else {
				throw std::invalid_argument("Unknown error while attempting to find bracket pairs!");
			}
			opStack.push(token);
		}
	}

	// pop leftover operator stack 
	while (!opStack.empty()) {
		if (opStack.top().type == Token::Type::OpeningBracket || opStack.top().type == Token::Type::ClosingBracket) {
			throw std::invalid_argument("Mismatching brackets!");
		}
		outQ.push(opStack.top());
		opStack.pop();
	}


	return outQ;
}

float evaluate(std::queue<Token> tokens) {
	std::stack<float> numberStack;
	while (!tokens.empty()) {
		Token token = tokens.front();
		tokens.pop();

		if (token.type == Token::Type::Number) {
			numberStack.push(token.value);
		} else if (token.type == Token::Type::Operator) {
			// get operands
			float num1;
			if (numberStack.empty()) {
				throw std::invalid_argument("Mismatched operators/numbers!");
			} else {
				num1 = numberStack.top();
				numberStack.pop();
			}
			float num2;
			if (numberStack.empty()) {
				throw std::invalid_argument("Mismatched operators/numbers!");
			} else {
				num2 = numberStack.top();
				numberStack.pop();
			}

			// evaluate
			switch (token.operatorChar)
			{
			case '+': numberStack.push(num2 + num1); break;
			case '-': numberStack.push(num2 - num1); break;
			case '*': numberStack.push(num2 * num1); break;
			case '/':
				if (num1 == 0) throw std::invalid_argument("Division by zero error!");
				numberStack.push(num2 / num1);
				break;
			default: throw std::invalid_argument("This message should not appear. I'll be scared if it does..."); break;
			}

		} else if (token.type == Token::Type::OpeningBracket || token.type == Token::Type::ClosingBracket) {
			throw std::invalid_argument("This message should not appear. I'll be scared if it does...");
		}
	}

	if (numberStack.size() != 1) {
		throw std::invalid_argument("Mismatched operators/numbers!");
	} else {
		return numberStack.top();
	}
}
int main()
{
	while (true) {
		std::cout << "Enter your equation or type 'help' for help.\n";
		std::cout << ">> ";

		std::string input;
		std::getline(std::cin, input); // ignores whitespace

		if (input == "help") {
			print_help();
		} else {

			try {
				std::queue<Token> tokens = tokenize(input);
				print_tokens(tokens);
				tokens = shunting_yard(tokens);
				print_tokens(tokens);
				std::cout << evaluate(tokens) << std::endl;
			}
			catch (std::invalid_argument& e) {
				std::cerr << e.what() << std::endl;
			}

		}
	}
}

