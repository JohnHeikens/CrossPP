#include "mattnxn.h"
#include "application.h"
#include "endlessPrecision.h"
#include "TextBox.h"
#pragma once

extern application* currentApplication;

constexpr byte segregate(cbyte x, vectn<int, 8> shifts);

template<typename t = uint32_t>
struct shuffleRng
{
	static constexpr fsize_t bitCount =
		std::is_same<t, uint8_t>::value ? 0x8 :
		(std::is_same<t, uint16_t>::value ? 0x10 :
			(std::is_same<t, uint32_t>::value ? 0x20 :
				0x40));

	static constexpr t maxPoolSizeMinus1 = (t)-1;

	//shuffle the numbers

	static constexpr t mask =
		(bitCount == 8) ? 0b10011101 :
		((bitCount == 0x10) ? 0b0010001101111111 :
			((bitCount == 0x20) ? 0b10000100011101011010001101010001 :
				0b1111010010011000101100100110110011100110110110011011101001001001));

	//a prime over (maxpoolsize / 2) and below maxpoolsize
	//it will make the number jump around

	//https://primes.utm.edu/curios/index.php?start=20&stop=24

	static constexpr t adder =
		(bitCount == 8) ? 0b11010011 :
		((bitCount == 0x10) ? 0b110001100001101 :
			((bitCount == 0x20) ? 0b11000000110011101111111111010001 :
				0b1101000001110101011001101010111011001000010011011100000101001001));



	static constexpr vectn<fsize_t, bitCount> bitShuffle =
		(bitCount == 8) ? vectn<fsize_t, bitCount>(vectn<fsize_t, 0x8>({
			(fsize_t)3,(fsize_t)7,(fsize_t)1,(fsize_t)5,(fsize_t)4,(fsize_t)2,(fsize_t)0,(fsize_t)6
			})) :
		((bitCount == 0x10) ? vectn<fsize_t, bitCount>(vectn<fsize_t, 0x10>({
			(fsize_t)4,(fsize_t)6,(fsize_t)1,(fsize_t)5,(fsize_t)11,(fsize_t)2,(fsize_t)7,(fsize_t)14,
			(fsize_t)3,(fsize_t)9,(fsize_t)15,(fsize_t)13,(fsize_t)12,(fsize_t)8,(fsize_t)10,(fsize_t)0
			})) :
		((bitCount == 0x20) ? vectn<fsize_t, bitCount>(vectn<fsize_t,0x20>({
			(fsize_t)21 ,(fsize_t)14,(fsize_t)13,(fsize_t)28,(fsize_t)9,(fsize_t)18,(fsize_t)8,(fsize_t)19,
			(fsize_t)29,(fsize_t)17,(fsize_t)4,(fsize_t)1,(fsize_t)6,(fsize_t)31,(fsize_t)3,(fsize_t)23,
			(fsize_t)0,(fsize_t)12,(fsize_t)10,(fsize_t)25,(fsize_t)16,(fsize_t)15,(fsize_t)22,(fsize_t)30,
			(fsize_t)2,(fsize_t)11,(fsize_t)5,(fsize_t)20,(fsize_t)27,(fsize_t)26,(fsize_t)7,(fsize_t)24,
			})) :
		vectn<fsize_t, bitCount>(vectn<fsize_t,0x40>({
			(fsize_t)35,(fsize_t)28,(fsize_t)1 ,(fsize_t)16,(fsize_t)52,(fsize_t)32,(fsize_t)10,(fsize_t)55,
			(fsize_t)31,(fsize_t)42,(fsize_t)12,(fsize_t)25,(fsize_t)45,(fsize_t)33,(fsize_t)29,(fsize_t)46,
			(fsize_t)54,(fsize_t)26,(fsize_t)47,(fsize_t)11,(fsize_t)30,(fsize_t)61,(fsize_t)56,(fsize_t)63,
			(fsize_t)41,(fsize_t)39,(fsize_t)20,(fsize_t)3 ,(fsize_t)34,(fsize_t)44,(fsize_t)6 ,(fsize_t)15,
			(fsize_t)19,(fsize_t)22,(fsize_t)59,(fsize_t)5 ,(fsize_t)21,(fsize_t)2 ,(fsize_t)53,(fsize_t)57,
			(fsize_t)9 ,(fsize_t)62,(fsize_t)13,(fsize_t)60,(fsize_t)27,(fsize_t)8 ,(fsize_t)58,(fsize_t)48,
			(fsize_t)36,(fsize_t)0 ,(fsize_t)23,(fsize_t)50,(fsize_t)18,(fsize_t)38,(fsize_t)40,(fsize_t)51,
			(fsize_t)14,(fsize_t)4 ,(fsize_t)49,(fsize_t)37,(fsize_t)7 ,(fsize_t)17,(fsize_t)43,(fsize_t)24,
			}))));

	constexpr t segregate(const t& x, vectn<int, bitCount> shifts)
	{
		//constexpr vectn<int, 8> addition = vectn<int, 8>({ (int)0, (int)1, (int)2, (int)3, (int)4, (int)5, (int)6, (int)7 });
		//vectn<int, 8> correctedShifts = shifts - addition;


		t shiftingInput = x;

		t result = 0;

		for (int i = 0; i < bitCount; i++)
		{
			//separate the current bit and shift it
			result += (shiftingInput & 1) << shifts[i];
			shiftingInput = shiftingInput >> 1;
		}
		return result;
	}

	constexpr t bitShift(const t& x)
	{
		t result = x;
		//correct for 32 bits
		result ^= result << 13;
		result ^= result >> 17;
		result ^= result << 5;
		return result;
	}

	t counter = 0;
	shuffleRng(t seed = 0) : counter(seed) {}

	constexpr t next()
	{
		t result = counter;

		/*t result = 0;


		result = (result + (adder * counter)) & maxPoolSizeMinus1;


		result = segregate(result, bitShuffle);

		result ^= mask;

		result = (result -(adder * counter)) & maxPoolSizeMinus1;
		*/

		result = bitShift(result);

		++counter;

		return result;
	}

};

struct gameForm final :public form
{
	gameForm();
	virtual void render(cveci2& position, const texture& renderTarget) final;
	virtual void keyDown(cvk& keycode) final;
	virtual void layout(crectanglei2& newRect) final;
	virtual void mouseDown(cveci2& position, cvk& button) final;
	virtual ~gameForm() override;
};

struct randomBrush : brush<byte, veci2>
{
	virtual byte getValue(const veci2& pos) const final;
};

