#pragma once

#include <string>

namespace Reporter
{
	class Fraction
	{
	public:
		Fraction(const std::string& val);
		Fraction(const std::string& mantisa_, int exponent_) :mantisa(mantisa_), exponent(exponent_), sign('+') {}
		Fraction(char sign_, const std::string& mantisa_, int exponent_) :mantisa(mantisa_), exponent(exponent_), sign(sign_) {}

		Fraction&& operator - (const Fraction& arg);
		Fraction&& operator + (const Fraction& arg);
		std::string operator ()();

	private:
		char sign;
		std::string mantisa;
		int exponent;
	};
}
