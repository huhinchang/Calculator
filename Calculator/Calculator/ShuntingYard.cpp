#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include "ShuntingYard.h"
#include "Conversion.h"

#define NUMBER_PRECEDENCE 0
#define ADD_PRECEDENCE 1
#define SUB_PRECEDENCE 1
#define MULT_PRECEDENCE 2
#define DIV_PRECEDENCE 2
#define POW_PRECEDENCE 3

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

std::queue<Token> tokenize(const std::string equation, const float vars[]) {
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
			case '^':
				output.push(Token(Token::Type::Operator, POW_PRECEDENCE, c));
				break;
			case '(':
				output.push(Token(Token::Type::OpeningBracket, 0, '('));
				break;
			case ')':
				output.push(Token(Token::Type::ClosingBracket, 0, ')'));
				break;
			default:
				if (c >= 'a' && c <= 'z') {
					output.push(Token(Token::Type::Number, vars[alphabet_to_int(c)], c));
				} else {
					throw std::invalid_argument("invalid character detected in the equation!");
				}
				break;
			}
		}
	}
	return output;
	// std::cout << equation << " + 1 = " << std::stof(equation) + 1 << std::endl;
}

std::queue<Token> shunting_yard(std::queue<Token> tokens) {
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
			float num_right;
			if (numberStack.empty()) {
				throw std::invalid_argument("Mismatched operators/numbers!");
			} else {
				num_right = numberStack.top();
				numberStack.pop();
			}
			float num_left;
			if (numberStack.empty()) {
				throw std::invalid_argument("Mismatched operators/numbers!");
			} else {
				num_left = numberStack.top();
				numberStack.pop();
			}

			// evaluate
			switch (token.operatorChar)
			{
			case '+': numberStack.push(num_left + num_right); break;
			case '-': numberStack.push(num_left - num_right); break;
			case '*': numberStack.push(num_left * num_right); break;
			case '/':
				if (num_right == 0) throw std::invalid_argument("Division by zero error!");
				numberStack.push(num_left / num_right);
				break;
			case '^':
				if (num_left == 0 && num_right == 0) throw std::invalid_argument("0^0 is undefined!");
				if (num_left < 0 && ceilf(num_right) != num_right) throw std::invalid_argument("Can't raise negative number to non-integer power!");
				if (num_left == 0 && num_right < 0) throw std::invalid_argument("Can't raise 0 to negative power!");
				numberStack.push(pow(num_left, num_right));
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

float parse_and_evaluate(const std::string equation, const float vars[]) {
	std::queue<Token> tokens = tokenize(equation, vars);
	tokens = shunting_yard(tokens);
	return evaluate(tokens);
}