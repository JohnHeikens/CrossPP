#include "compiler/standardarea.h"
#include "math/random/random.h"
#include "timemath.h"
area* standardArea;
void frand::execute() const
{
	*(int*)result->var = rand();
}

void frandfp::execute() const
{
	*(fp*)result->var = randFp();
}

void ftofp::execute() const
{
	*(fp*)result->var = *(int*)arguments[0]->var;
}

void ftoint::execute() const
{
	*(int*)result->var = (int)*(fp*)arguments[0]->var;
}

void GenerateStandardArea()
{
	standardArea = new area();
	standardArea->functions.push_back(new frand());
	standardArea->functions.push_back(new frandfp());
	standardArea->functions.push_back(new ftofp());
	standardArea->functions.push_back(new ftoint());
	standardArea->functions.push_back(new fsin());
	standardArea->functions.push_back(new fcos());
	standardArea->functions.push_back(new fgettime());
	standardArea->variables.push_back(new variable(tfp, (void*)new fp(math::PI), std::wstring(L"pi")));
	standardArea->variables.push_back(new variable(tfp, (void*)new fp(math::goldenRatio), std::wstring(L"goldenratio")));
}

void fsin::execute() const
{
	*(fp*)result->var = sin(*(fp*)arguments[0]->var);
}

void fgettime::execute() const
{
	*(fp*)result->var = (fp)GetSecondsSinceApplicationBoot();
}

void fcos::execute() const
{
	*(fp*)result->var = cos(*(fp*)arguments[0]->var);
}
