#include <stdexcept>
#include "Conversion.h"

int alphabet_to_int(const char c) {
	if (c < 'a' || c >'z') throw std::invalid_argument("Alphabet must be lowercase a-z");
	return c - 'a';
}
char int_to_alphabet(const int n) {
	if (n < 0 || n > 25) throw std::invalid_argument("Number must be between 0 and 25 (inclusive)");
	return n + 'a';
}