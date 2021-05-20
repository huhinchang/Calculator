// Calculator.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <string>
#include <assert.h>
#include "ShuntingYard.h"
#include "Conversion.h"

void print_help() {
	std::cout << std::endl << "##### HELP #####" << std::endl;
	std::cout << "* This is an implementation of the shunting-yard algorithm of parsing infix-notation mathematical equations." << std::endl;
	std::cout << "* It takes into account nested brackets and operator precedence." << std::endl;
	std::cout << "* This implementation also checks for mismatched brackets, mismatched operators and decimal points, division by zero, and other undefined operations." << std::endl;
	std::cout << std::endl << "##### FORMATTING #####" << std::endl;
	std::cout << "* Parser does not support unary operators, so to input negative numbers you must format it as (0-x)" << std::endl;
	std::cout << "* Supported operators: +, -, /, *, ^, (, )" << std::endl;
	std::cout << "* Supported numbers  : 0-9" << std::endl;
	std::cout << "* Supported variables: a-z" << std::endl;
	std::cout << "* Sample equation: 9.1*(2+3) + e" << std::endl;
	std::cout << "  Output of above: 48.21" << std::endl;
}

void print_vars(const float vars[]) {
	assert(vars != NULL);
	std::cout << std::endl << "##### VARIABLES #####" << std::endl;
	for (size_t i = 0; i < 26; i++)
	{
		std::cout << int_to_alphabet(i) << ": " << vars[i] << std::endl;
	}
}

int main()
{
	float vars[26] = { 0 };
	vars[alphabet_to_int('c')] = 299792458;
	vars[alphabet_to_int('e')] = 2.71828;
	vars[alphabet_to_int('g')] = 9.81;
	vars[alphabet_to_int('p')] = 3.14159;

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
				float result = parse_and_evaluate(equation, vars);
				std::cout << "= " << result << std::endl;
			}
			catch (std::invalid_argument& e) {
				std::cerr << e.what() << std::endl;
			}
		} else if (input == "assign") {
			try {
				// get variable  part of the string
				char var;
				std::cin >> var;
				const int var_index = alphabet_to_int(var);

				// get equation part of the string
				std::string equation;
				std::cin >> equation;

				float result = parse_and_evaluate(equation, vars);

				// assign
				std::cout << var << " changed from " << vars[var_index] << " to " << result << std::endl;
				vars[var_index] = result;
			}
			catch (std::invalid_argument& e) {
				std::cerr << e.what() << std::endl;
			}
		} else {
			std::cerr << "Unknown command '" << input << "'" << std::endl;
		}

		std::getline(std::cin, input);
		if (input != "") {
			std::cout << "ignored extraneous input" << std::endl;
		}
	}
}

