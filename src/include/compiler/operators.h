#pragma once
#include "function.h"

struct operatorIndex : public calculation
{
	inline void calculate() const override
	{
		cint index = *(int*)inputs[1]->var;
		switch (inputs[0]->type)
		{
		case tbool:
			result->var = ((bool*)inputs[0]->var) + index;
			break;
		case tint:
			result->var = ((int*)inputs[0]->var) + index;
			break;
		case tfp:
			result->var = ((fp*)inputs[0]->var) + index;
			break;
		}
	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({ 
			std::vector<types>({tbool,tint}),
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tint}),
			}));
		result = new variable(inputs[0]->type, true);
	}
};

//operators
struct operatorOr : public calculation
{
	inline void calculate() const override
	{
		*(bool*)result->var = *(bool*)inputs[0]->var || *(bool*)inputs[1]->var;
	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tbool,tbool})
			}));
		result = new variable(tbool, false);
	}
};
struct operatorAnd : public calculation
{
	inline void calculate() const override
	{
		*(bool*)result->var = *(bool*)inputs[0]->var && *(bool*)inputs[1]->var;
	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({
	std::vector<types>({tbool,tbool})
			}));
		result = new variable(tbool, false);
	}
};
struct operatorIf : public calculation 
{
	inline void execute() const override
	{
		this->calculations[0]->execute();
		if (*(bool*)inputs[0]->var)
		{
			if (calculations[1]) 
			{
				calculations[1]->execute();
			}
			variable::copy(inputs[1], result);
		}
		else
		{
			if (calculations[2])
			{
				calculations[2]->execute();
			}
			variable::copy(inputs[2], result);
		}
	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({
		std::vector<types>({tbool,tbool, tbool}),
			std::vector<types>({tbool,tint, tint}),
			std::vector<types>({tbool,tfp, tfp})
			}));
		result = new variable(inputs[1]->type, false);
	}
};
struct operatorCopy : public calculation
{
	inline void calculate() const override
	{
		switch (inputs[0]->type)
		{
		case tbool:
			*(bool*)result->var = *(bool*)inputs[0]->var = *(bool*)inputs[1]->var;
			break;
		case tint:
			*(int*)result->var = *(int*)inputs[0]->var = *(int*)inputs[1]->var;
			break;
		case tfp:
			*(fp*)result->var = *(fp*)inputs[0]->var = *(fp*)inputs[1]->var;
			break;
		}
	}
	inline virtual void setresulttype()
	{
		if (!inputs[0]->modifyable)
		{
			ShowCompilerError(errortypes::mustbemodifyable, inputs[0]->name);
		}
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tbool,tbool}),
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tfp}),
			}));
		result = new variable(inputs[0]->type, false);
	}
};
struct operatorPlusCopy : public calculation
{
	inline void calculate() const override
	{
		switch (inputs[0]->type)
		{
		case tint:
			*(int*)result->var = *(int*)inputs[0]->var += *(int*)inputs[1]->var;
			break;
		case tfp:
			*(fp*)result->var = *(fp*)inputs[0]->var += *(fp*)inputs[1]->var;
			break;
		}
	}
	inline virtual void setresulttype()
	{
		if (!inputs[0]->modifyable)
		{
			ShowCompilerError(errortypes::mustbemodifyable, inputs[0]->name);
		}
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tfp}),
			}));
		result = new variable(inputs[0]->type, false);
	}
};
struct operatorMinCopy : public calculation
{
	inline void calculate() const override
	{
		switch (inputs[0]->type)
		{
		case tint:
			*(int*)result->var = *(int*)inputs[0]->var -= *(int*)inputs[1]->var;
			break;
		case tfp:
			*(fp*)result->var = *(fp*)inputs[0]->var -= *(fp*)inputs[1]->var;
			break;
		}
	}
	inline virtual void setresulttype()
	{
		if (!inputs[0]->modifyable)
		{
			ShowCompilerError(errortypes::mustbemodifyable, inputs[0]->name);
		}
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tfp}),
			}));
		result = new variable(inputs[0]->type, false);
	}
};

struct operatorNotIs : public calculation
{
	inline void calculate() const override
	{
		switch (inputs[0]->type)
		{
		case tbool:
			*(bool*)result->var = *(bool*)inputs[0]->var != *(bool*)inputs[1]->var;
			break;
		case tint:
			*(bool*)result->var = *(int*)inputs[0]->var != *(int*)inputs[1]->var;
			break;
		case tfp:
			*(bool*)result->var = *(fp*)inputs[0]->var != *(fp*)inputs[1]->var;
			break;
		}
	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tbool,tbool}),
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tfp}),
			}));
		result = new variable(tbool, false);
	}
};
struct operatorIs : public calculation
{
	inline void calculate() const override
	{
		switch (inputs[0]->type)
		{
			case tbool:
			{
				*(bool*)result->var = *(bool*)inputs[0]->var == *(bool*)inputs[1]->var;
				break;
			}
			case tint:
			{
				*(bool*)result->var = *(int*)inputs[0]->var == *(int*)inputs[1]->var;
				break;
			}
			case tfp:
			{
				*(bool*)result->var = *(fp*)inputs[0]->var == *(fp*)inputs[1]->var;
				break;
			}
		}
	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tbool,tbool}),
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tfp}),
			}));
		result = new variable(tbool, false);
	}
};
struct operatorLessIs : public calculation
{
	inline void calculate() const override
	{
		switch (inputs[0]->type)
		{
		case tint:
			*(bool*)result->var = *(int*)inputs[0]->var <= *(int*)inputs[1]->var;
			break;
		case tfp:
			*(bool*)result->var = *(fp*)inputs[0]->var <= *(fp*)inputs[1]->var;
			break;
		}
	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tfp}),
			}));
		result = new variable(tbool, false);
	}
};
struct operatorMoreIs : public calculation
{
	inline void calculate() const override
	{
		switch (inputs[0]->type)
		{
		case tint:
			*(bool*)result->var = *(int*)inputs[0]->var >= *(int*)inputs[1]->var;
			break;
		case tfp:
			*(bool*)result->var = *(fp*)inputs[0]->var >= *(fp*)inputs[1]->var;
			break;
		}
	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tfp}),
			}));
		result = new variable(tbool, false);
	}
};
struct operatorLess : public calculation
{
	inline void calculate() const override
	{
		switch (inputs[0]->type)
		{
		case tint:
			*(bool*)result->var = *(int*)inputs[0]->var < *(int*)inputs[1]->var;
			break;
		case tfp:
			*(bool*)result->var = *(fp*)inputs[0]->var < *(fp*)inputs[1]->var;
			break;
		}
	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tfp}),
			}));
		result = new variable(tbool, false);
	}
};
struct operatorMore : public calculation
{
	inline void calculate() const override
	{
		switch (inputs[0]->type)
		{
		case tint:
			*(bool*)result->var = *(int*)inputs[0]->var > *(int*)inputs[1]->var;
			break;
		case tfp:
			*(bool*)result->var = *(fp*)inputs[0]->var > *(fp*)inputs[1]->var;
			break;
		}
	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tfp}),
			}));
		result = new variable(tbool, false);
	}
};
struct operatorMin : public calculation
{
	inline void calculate() const override
	{
		switch (result->type)
		{
		case tint:
			*(int*)result->var = *(int*)inputs[0]->var - * (int*)inputs[1]->var;
			break;
		case tfp:
			*(fp*)result->var = *(fp*)inputs[0]->var - * (fp*)inputs[1]->var;
			break;
		}
	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tfp}),
			}));
		result = new variable(inputs[0]->type, false);
	}
};
struct operatorPlus : public calculation
{
	inline void calculate() const override
	{
		switch (result->type)
		{
		case tint:
			*(int*)result->var = *(int*)inputs[0]->var + *(int*)inputs[1]->var;
			break;
		case tfp:
			*(fp*)result->var = *(fp*)inputs[0]->var + *(fp*)inputs[1]->var;
			break;
		}
	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tfp}),
			}));
		result = new variable(inputs[0]->type, false);
	}
};
struct operatorDiv : public calculation
{
	inline void calculate() const override
	{
		switch (result->type)
		{
		case tint:
			*(int*)result->var = *(int*)inputs[0]->var / *(int*)inputs[1]->var;
			break;
		case tfp:
			*(fp*)result->var = *(fp*)inputs[0]->var / *(fp*)inputs[1]->var;
			break;

		}
	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tfp}),
			}));
		result = new variable(inputs[0]->type, false);
	}
};
struct operatorMult : public calculation
{
	inline void calculate() const override
	{
		switch (result->type)
		{
		case tint:
			*(int*)result->var = *(int*)inputs[0]->var * *(int*)inputs[1]->var;
			break;
		case tfp:
			*(fp*)result->var = *(fp*)inputs[0]->var * *(fp*)inputs[1]->var;
			break;
		}
	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tfp}),
			}));
		result = new variable(inputs[0]->type, false);
	}
};
struct operatorDivCopy : public calculation
{
	inline void calculate() const override
	{
		switch (result->type)
		{
		case tint:
			*(int*)result->var = *(int*)inputs[0]->var /= *(int*)inputs[1]->var;
			break;
		case tfp:
			*(fp*)result->var = *(fp*)inputs[0]->var /= *(fp*)inputs[1]->var;
			break;

		}
	}
	inline virtual void setresulttype()
	{
		if (!inputs[0]->modifyable)
		{
			ShowCompilerError(errortypes::mustbemodifyable, inputs[0]->name);
		}
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tfp}),
			}));
		result = new variable(inputs[0]->type, false);
	}
};
struct operatorMultCopy : public calculation
{
	inline void calculate() const override
	{
		switch (result->type)
		{
		case tint:
			*(int*)result->var = *(int*)inputs[0]->var *= *(int*)inputs[1]->var;
			break;
		case tfp:
			*(fp*)result->var = *(fp*)inputs[0]->var *= *(fp*)inputs[1]->var;
			break;
		}
	}
	inline virtual void setresulttype()
	{
		if (!inputs[0]->modifyable)
		{
			ShowCompilerError(errortypes::mustbemodifyable, inputs[0]->name);
		}
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tfp}),
			}));
		result = new variable(inputs[0]->type, false);
	}
};
struct operatorNot : public calculation
{
	inline void calculate() const override
	{
		*(bool*)result->var = !*(bool*)inputs[0]->var;
	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tbool,tbool}),
			}));
		result = new variable(tbool, false);
	}
};
struct operatorInc : public calculation
{
	inline void calculate() const override
	{
		switch (inputs[0]->type)
		{
		case tint:
			*(int*)result->var = (*(int*)inputs[0]->var)++;
			break;
		case tfp:
			*(fp*)inputs[0]->var = (*(fp*)inputs[0]->var)++;
			//*(fp*)result->var = ((*(fp*)inputs[0]->var)++);
			break;
		}
	}
	inline virtual void setresulttype()
	{
		if (!inputs[0]->modifyable)
		{
			ShowCompilerError(errortypes::mustbemodifyable, inputs[0]->name);
		}
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tint}),
			std::vector<types>({tfp}),
			}));
		result = new variable(inputs[0]->type, false);
	}
};
struct operatorDec : public calculation
{
	inline void calculate() const override
	{
		switch (inputs[0]->type)
		{
		case tint:
			*(int*)inputs[0]->var = *(int*)inputs[0]->var = *(int*)inputs[0]->var - 1;
			break;
		case tfp:
			*(fp*)inputs[0]->var = *(fp*)inputs[0]->var = *(fp*)inputs[0]->var - 1;
			break;
		}
	}
	inline virtual void setresulttype()
	{
		if (!inputs[0]->modifyable)
		{
			ShowCompilerError(errortypes::mustbemodifyable, inputs[0]->name);
		}
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tint}),
			std::vector<types>({tfp}),
			}));
		result = new variable(inputs[0]->type, false);
	}
};
struct operatorRem : public calculation
{
	inline void calculate() const override
	{
		switch (result->type)
		{
		case tint:
			*(int*)result->var = *(int*)inputs[0]->var % *(int*)inputs[1]->var;
			break;
		}
	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tint,tint}),
			}));
		result = new variable(tint, false);
	}
};
