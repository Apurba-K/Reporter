#include "fraction.h"

Reporter::Fraction::Fraction(const std::string& val)
{
	if (val.empty())
	{
		mantisa = "0";
		exponent = 0;
	}
	else
	{
		int end = 0;
		if ((end = val.find(".")) != std::string::npos)
		{
			std::string part1, part2, final;
			part1 = std::stoi(std::string(val, 0, end));
			part2 = std::stoi(std::string(val, end + 1));

			mantisa = part1 + part2;
			exponent = part2.size();
		}
		else
		{
			mantisa = val;
			exponent = 0;
		}
	}
}

Reporter::Fraction&& Reporter::Fraction::operator - (const Fraction& arg)
{
	std::string finalMantisa;
	int finalExponent;
	char finalSign;

	if (arg.mantisa == "0")
	{
		finalMantisa = mantisa;
		finalExponent = exponent;
		finalSign = sign;
	}
	else if (mantisa == "0")
	{
		finalMantisa = arg.mantisa;
		finalExponent = arg.exponent;
		finalSign = arg.sign == '+' ? '-' : '+';
	}
	else
	{
		std::string val1 = mantisa, val2 = arg.mantisa;
		if (arg.exponent < exponent)
		{
			val2.append("0", exponent - arg.exponent);
			finalExponent = exponent;
		}
		else if (exponent < arg.exponent)
		{
			val1.append("0", arg.exponent - exponent);
			finalExponent = arg.exponent;
		}

		std::string arg1, arg2;
		val1 = std::string(val1, val1.find_first_not_of("0") + 1);
		val2 = std::string(val2, val2.find_first_not_of("0") + 1);

		if (val1 == val2)
		{
			finalMantisa = "0";
			finalExponent = 0;
			finalSign = '+';
		}
		else
		{
			if (val1.size() < val2.size())
			{
				finalSign = '-';
				arg1 = val2, arg2 = val1;
			}
			else if (val1.size() > val2.size())
			{
				finalSign = '+';
				arg1 = val1, arg2 = val2;
			}
			else if (val1 < val2)
			{
				finalSign = '-';
				arg1 = val2, arg2 = val1;
			}
			else
			{
				finalSign = '+';
				arg1 = val1, arg2 = val2;
			}

			typedef std::string::reverse_iterator strp;
			int x, y, carry = 0;
			for (strp end1 = arg1.rbegin(), end2 = arg2.rbegin(); end1 != arg1.rend() && end2 != arg2.rend(); )
			{
				x = *end1;

				if (end2 == arg2.rend())
					y = 0 + carry;
				else
					y = *end2 + carry;

				if (x < y)
				{
					finalMantisa = std::to_string(10 + x - y) + finalMantisa;
					carry = 1;
				}
				else if (x > y)
				{
					finalMantisa = std::to_string(x - y) + finalMantisa;
					carry = 0;
				}
				else
				{
					finalMantisa = "0" + finalMantisa;
					carry = 0;
				}
			}

		}

	}

	return Fraction(finalSign, finalMantisa, finalExponent);
}


		

	




}