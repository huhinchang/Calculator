// Calculator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <sstream> 
#include <stdexcept>
#include <assert.h>

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
	const float value; // acts as the number if type is Number or precedence if type is Operator
	const char operatorChar;

	Token(const Type t, const float v, const int c = '?') : type(t), value(v), operatorChar(c) {
	}
};

void print_help() {
	std::cout << std::endl << "##### HELP #####" << std::endl;
	std::cout << "This is an implementation of the shunting-yard algorithm of parsing infix-notation mathematical equations." << std::endl;
	std::cout << "It takes into account nested brackets and operator precedence." << std::endl;
	std::cout << "This implementation also checks for mismatched brackets, operators, and decimal form" << std::endl;
	std::cout << "IMPORTANT: it does not support unary operators, so to input negative numbers you must format it as (0-x)." << std::endl;
	std::cout << "Supported operators: +, -, /, *" << std::endl;
}

int alphabet_to_int(const char c) {
	if (c < 'a' || c >'z') throw std::invalid_argument("Alphabet must be lowercase a-z");
	return c - 'a';
}
char int_to_alphabet(const int n) {
	if (n < 0 || n > 25) throw std::invalid_argument("Number must be between 0 and 25 (inclusive)");
	return n + 'a';
}

void print_vars(const float constants[]) {
	assert(constants != NULL);
	std::cout << std::endl << "##### CONSTANTS #####" << std::endl;
	for (size_t i = 0; i < 26; i++)
	{
		std::cout << int_to_alphabet(i) << ": " << constants[i] << std::endl;
	}
}

std::queue<Token> tokenize(const std::string equation) {
	std::queue<Token> output;

	for (size_t i = 0; i < equation.length(); ++i)
	{
		const char c = equation[i];
		if (isdigit(c)) {
			size_t number_len = 0;
			float number = std::stof(equation.substr(i), &number_len);
			output.push(Token(Token::Type::Number, number));
			i += number_len - 1;
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
				output.push(Token(Token::Type::OpeningBracket, 0, '('));
				break;
			case ')':
				output.push(Token(Token::Type::ClosingBracket, 0, ')'));
				break;
			default:
				throw std::invalid_argument("invalid character detected in the equation!");
				break;
			}
		}
	}
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
			while (!opStack.empty() && opStack.top().value >= token.value && opStack.top().type != Token::Type::OpeningBracket) {
				outQ.push(opStack.top());
				opStack.pop();
			}
			opStack.push(token);
		} else if (token.type == Token::Type::OpeningBracket) {
			opStack.push(token);
		} else if (token.type == Token::Type::ClosingBracket) {
			while (!opStack.empty() && opStack.top().type != Token::Type::OpeningBracket) {
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
	float vars[26] = { 0 };
	vars[alphabet_to_int('c')] = 299792458;
	vars[alphabet_to_int('e')] = 2.71;
	vars[alphabet_to_int('g')] = 9.81;
	vars[alphabet_to_int('p')] = 3.14;

	while (true) {
		std::cout << std::endl << "##### COMMANDS #####" << std::endl;
		std::cout << "Type 'help' for help.\n";
		std::cout << "Type 'list' to to view all variables.\n";
		std::cout << "Type 'calc <expression>' to evaluate the expression.\n";
		std::cout << "Type 'assign <a-z> <expression>' to assign the value of the expression to the constant.\n";
		std::cout << ">> ";

		std::string input;
		std::cin >> input;
		// std::getline(std::cin, input); // ignores whitespace

		if (input == "help") {
			print_help();
		} if (input == "list") {
			print_vars(vars);
		} else if (input == "calc") {
			std::string equation;
			std::cin >> equation;
			try {
				std::queue<Token> tokens = tokenize(equation);
				//print_tokens(tokens);
				tokens = shunting_yard(tokens);
				//print_tokens(tokens);
				std::cout << "= " << evaluate(tokens) << std::endl;
			}
			catch (std::invalid_argument& e) {
				std::cerr << e.what() << std::endl;
			}
		} else if (input == "assign") {
			try {
				// get variable
				char letter;
				std::cin >> letter;
				const int var_index = alphabet_to_int(letter);

				// get equation
				std::string equation;
				std::cin >> equation;

				// parse and evaluate
				std::queue<Token> tokens = tokenize(equation);
				tokens = shunting_yard(tokens);
				const float result = evaluate(tokens);
				// assign
				vars[alphabet_to_int(letter)] = result;
				std::cout << "assigned " << result << " to " << letter << std::endl;
			}
			catch (std::invalid_argument& e) {
				std::cerr << e.what() << std::endl;
			}
		} else {
			std::cerr << "Unknown command" << std::endl;
		}

		std::getline(std::cin, input);
		if (input != "") {
			std::cout << "ignored extraneous input" << std::endl;
		}
	}
}

