#include "memorymanagement.h"
#include "errors.h"
#include <vector>
#pragma once

//does not check l-values
inline void checkarguments(std::vector< variable*> arguments, std::vector<std::vector<types>> overloads)
{
	csize_t& size = arguments.size();
	for (int j = 0; j < overloads.size(); j++)
	{
		auto overload = overloads[j];
		if(overload.size() == size)
		{ 
			//check argument types
			for (int i = 0; i < size; i++)
			{
				if (arguments[i]->type != overload[i])
				{
					goto next;
				}
			}
			return;//fits
		}
	next:;
	}
	std::wstring text = std::wstring(L"arguments: (");
	for (size_t i = 0; i < size;i++) 
	{
		text += varnames[arguments[i]->type];
		text += std::wstring(L", ");
	}
	text += std::wstring(L")");
	ShowCompilerError(errortypes::noavailableoverload, text);
}

//no templates, because of lists
struct action
{
	inline virtual void execute() const
	{

	}
};
struct calculation : public action //to make a layered calculation possible
{
//public:
	variable* result = nullptr;
	std::vector<action*> calculations;
	std::vector<variable*> inputs;
	inline virtual void execute() const override
	{
		for (action* a : calculations) 
		{
			a->execute();
		}
		calculate();
	}
	inline virtual void calculate() const 
	{

	}
	inline virtual void setresulttype()
	{
		checkarguments(inputs, std::vector<std::vector<types>>({
			std::vector<types>({tbool,tbool}),
			std::vector<types>({tint,tint}),
			std::vector<types>({tfp,tfp}),
		}));
	}
};
struct function : public action
{
	std::wstring name = std::wstring(L"");
	std::vector<variable*> arguments = std::vector<variable*>();
	variable* result = nullptr;
	inline virtual void execute() const override
	{

	}
};
struct functioncall : public calculation 
{
	function* f = nullptr;
	inline virtual void execute() const override
	{
		for (action* a : calculations) //first calculate parameters
		{
			a->execute();
		}
		//copy arguments
		for (int i = 0; i < inputs.size(); i++)
		{
			variable::copy(inputs[i], f->arguments[i]);
		}
		f->execute();
		//copy result, because the same function can be called multiple times in a equation
		variable::copy(f->result, result);
	}
};
struct internalfunction : public function
{
	std::vector<action*> actions;
	inline virtual void execute() const override
	{
		for (action* a : actions)
		{
			a->execute();
		}
	}
};

struct loop : public action
{
	variable* checkresult = nullptr;
	std::vector<action*> actions;
	inline virtual void execute() const override
	{
		while(*(bool*)checkresult->var)
		{
			for (action* a : actions)
			{
				a->execute();
			}
		}
	}
};
struct ifstatement : public action
{
	variable* checkresult = nullptr;
	std::vector<action*> actionswhentrue;
	std::vector<action*> actionswhenfalse;
	inline virtual void execute() const override
	{
		if (*(bool*)checkresult->var)
		{
			for (action* a : actionswhentrue)
			{
				a->execute();
			}
		}
		else 
		{
			for (action* a : actionswhenfalse)
			{
				a->execute();
			}
		}
	}
};

//vars used by operators have to be the same variable type
template<typename t>
struct addition: action
{
	variable* left;
	variable* right;
	variable* result;

	// Inherited via action
	inline virtual void execute() const override
	{
		*((t*)result->var) = *((t*)left->var) + *((t*)right->var);
	}
};
