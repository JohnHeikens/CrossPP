#pragma once
#include "area.h"
#include "math/mathfunctions.h"
extern area* standardArea;

void GenerateStandardArea();

struct frand : public function
{
	frand()
	{
		name = std::wstring(L"rand");
		arguments = std::vector<variable*>();
		result = new variable(tint, false);
	}
	virtual void execute() const override;

};
struct frandfp : public function
{
	frandfp()
	{
		name = std::wstring(L"randFp");
		arguments = std::vector<variable*>();
		result = new variable(tfp, false);
	}
	virtual void execute() const override;

};
struct fsin : public function
{
	fsin()
	{
		name = std::wstring(L"sin");
		arguments = std::vector<variable*>();
		arguments.push_back(new variable(tfp, false));
		result = new variable(tfp, false);
	}
	virtual void execute() const override;
};
struct fcos : public function
{
	fcos()
	{
		name = std::wstring(L"cos");
		arguments = std::vector<variable*>();
		arguments.push_back(new variable(tfp, false));
		result = new variable(tfp,false);
	}
	virtual void execute() const override;
};
struct fgettime : public function
{
	fgettime()
	{
		name = std::wstring(L"gettime");
		arguments = std::vector<variable*>();
		result = new variable(tfp, false);
	}
	virtual void execute() const override;
};
struct ftofp : public function
{
	ftofp()
	{
		name = std::wstring(L"tofp");
		arguments = std::vector<variable*>();
		arguments.push_back(new variable(tint, false));//int to convert
		result = new variable(tfp, false);
	}
	virtual void execute() const override;

};
struct ftoint : public function
{
	ftoint()
	{
		name = std::wstring(L"toint");
		arguments = std::vector<variable*>();
		arguments.push_back(new variable(tfp, false));//fp to convert
		result = new variable(tint, false);
	}
	virtual void execute() const override;

};

