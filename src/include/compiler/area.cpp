#include "area.h"
#include "array/wstring.h"

//if result == NULL then user does not store the result in a variable
calculation* area::GetCalculation(std::wstring equation, variable** result)
{

	calculation* c;
	trim(equation);
	if (equation.size())
	{
		if (result)
		{
			*result = GetVariable(equation);
			if (*result)
			{
				return nullptr;
			}
		}
		//calculation needed
		size_t i;
		size_t length = 1;
		i = find(equation, 0, std::wstring(L"="), skip);
		if (i != std::wstring::npos && equation[i + 1] != L'=' && std::wstring(L"-+/*<>").find(equation[i - 1]) == std::wstring::npos)
		{
			c = new operatorCopy(); length = 1; goto check;
		}
		i = find(equation, 0, std::wstring(L"+="), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorPlusCopy(); length = 2; goto check;
		}
		i = find(equation, 0, std::wstring(L"-="), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorMinCopy(); length = 2; goto check;
		}
		i = find(equation, 0, std::wstring(L"*="), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorMultCopy(); length = 2; goto check;
		}
		i = find(equation, 0, std::wstring(L"/="), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorDivCopy(); length = 2; goto check;
		}
		i = find(equation, 0, std::wstring(L"?"), skip);
		if (i != std::wstring::npos)
		{

		}
		if (i != std::wstring::npos)
		{
			csize_t& j = find(equation, i + 1, std::wstring(L":"), skip);
			//if
			c = new operatorIf();
			c->inputs = std::vector<variable*>(3);
			c->calculations.push_back(GetCalculation(equation.substr(0, i), &c->inputs[0]));
			c->calculations.push_back(GetCalculation(equation.substr(i + 1, j - i - 1), &c->inputs[1]));
			c->calculations.push_back(GetCalculation(equation.substr(j + 1), &c->inputs[2]));
			c->setresulttype();
			if (result)
			{
				*result = c->result;
				(*result)->name = equation;
			}
			return c;
		}

		i = find(equation, 0, std::wstring(L"||"), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorOr();
			length = 2; goto check;
		}
		i = find(equation, 0, std::wstring(L"&&"), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorAnd();
			length = 2; goto check;
		}
		i = find(equation, 0, std::wstring(L"!="), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorNotIs();
			length = 2; goto check;
		}
		i = find(equation, 0, std::wstring(L"=="), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorIs();
			length = 2; goto check;
		}
		i = find(equation, 0, std::wstring(L"<="), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorLessIs();
			length = 2; goto check;
		}
		i = find(equation, 0, std::wstring(L">="), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorMoreIs();
			length = 2; goto check;
		}
		i = find(equation, 0, std::wstring(L"<"), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorLess();
			length = 1; goto check;
		}
		i = find(equation, 0, std::wstring(L">"), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorMore();
			length = 1; goto check;
		}
		i = find(equation, 0, std::wstring(L"-"), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorMin();
			length = 1; goto check;
		}
		i = find(equation, 0, std::wstring(L"+"), skip);
		if (i != std::wstring::npos && equation[i + 1] != L'+')
		{
			c = new operatorPlus();
			length = 1; goto check;
		}
		i = find(equation, 0, std::wstring(L"/"), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorDiv();
			length = 1; goto check;
		}
		i = find(equation, 0, std::wstring(L"*"), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorMult();
			length = 1; goto check;
		}
		i = find(equation, 0, std::wstring(L"%"), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorRem(); length = 1; goto check;
		}
		i = find(equation, 0, std::wstring(L"!"), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorNot();
			length = 1; goto check;
		}
		i = find(equation, 0, std::wstring(L"++"), skip);
		if (i != std::wstring::npos)
		{
			c = new operatorInc();
			length = 2; goto check;
		}
		i = find(equation, 0, std::wstring(L"["), skip);
		if (i != std::wstring::npos)
		{
			csize_t& j = find(equation, i + 1, std::wstring(L"]"), skip);
			if (j == std::wstring::npos)
			{
				ShowCompilerError(errortypes::noclosingsquarebracket, equation);
			}
			if (j < equation.size() - 1)
			{
				ShowCompilerError(errortypes::typedafterclosingbracket, equation);
			}
			std::wstring betweenbraces = equation.substr(i + 1, j - i - 1);
			std::wstring arrayname = equation.substr(0, i);
			variable* vArray = this->FindVariableName(arrayname);
			c = new operatorIndex();
			c->inputs.push_back(vArray);
			c->inputs.push_back(nullptr);
			if (calculation* indexcalc = GetCalculation(betweenbraces, &c->inputs[1]))
			{
				c->calculations.push_back(indexcalc);
			}
			c->setresulttype();
			if (result)
			{
				*result = c->result;
			}
			return c;
		}
		//because all operators are evaluated, it can only be "(a)", "a(b, c, d...)"
		i = find(equation, 0, std::wstring(L"("), skip);
		if (i != std::wstring::npos)
		{
			csize_t& j = find(equation, i + 1, std::wstring(L")"), skip);
			if (j == std::wstring::npos)
			{
				ShowCompilerError(errortypes::noclosingbrace, equation);
			}
			if (j < equation.size() - 1)
			{
				ShowCompilerError(errortypes::typedafterclosingbracket, equation);
			}
			std::wstring betweenbraces = equation.substr(i + 1, j - i - 1);
			if (i == 0)//no functions, just specified to be calculated first
			{
				return GetCalculation(betweenbraces, result);
			}
			else
			{
				std::wstring functionname = equation.substr(0, i);
				function* f = this->FindFunctionName(functionname);
				if (!f)ShowCompilerError(errortypes::couldnotfindfunction, equation);
				functioncall* fcall = new functioncall();
				fcall->f = f;
				fcall->inputs = std::vector<variable*>(f->arguments.size());
				fcall->result = new variable(f->result->type, false);
				if (betweenbraces.length())
				{
					wstringContainer arguments = split_string(betweenbraces, std::wstring(L","), skip);
					//c->calculations = std::vector<action*>();
					for (size_t k = 0; k < arguments.size(); k++)
					{
						calculation* cur = GetCalculation(arguments[k], &fcall->inputs[k]);
						if (cur) {
							fcall->calculations.push_back(cur);
						}
					}
				}
				std::vector<types> overload = std::vector<types>();
				for (variable* var : fcall->f->arguments)
				{
					overload.push_back(var->type);
				}
				checkarguments(fcall->inputs, std::vector<std::vector<types>>({ overload }));
				if (result)
				{
					*result = fcall->result;
				}
				return fcall;
			}
		}
		ShowCompilerError(errortypes::nooperator, equation);
		throw 0;
	check:
		std::wstring str1 = trim_copy(equation.substr(0, i));
		if (str1.size()) {
			c->inputs.push_back(nullptr);
			calculation* c1 = GetCalculation(str1, &c->inputs[0]);
			if (c1)
			{
				c->calculations.push_back(c1);
			}
		}
		std::wstring str2 = trim_copy(equation.substr(i + length));
		if (str2.size())
		{
			c->inputs.push_back(nullptr);
			calculation* c2 = GetCalculation(str2, &c->inputs[c->inputs.size() - 1]);
			if (c2)
			{
				c->calculations.push_back(c2);
			}
		}
		c->setresulttype();
		if (result)
		{
			*result = c->result;
			(*result)->name = equation;
		}
		return c;

		//c->inputs.push_back(var1);
	}
	else
	{
		ShowCompilerError(errortypes::calclengthzero, equation);
	}
	//else error
	return nullptr;
}
variable* area::FindVariableName(std::wstring name)
{
	//local scope
	for (variable* var : variables)
	{
		if (var->name == name)
		{
			return var;
		}
	}
	//seek in higher scope
	if (this->Parent)
	{
		return Parent->FindVariableName(name);
	}
	else
	{
		return nullptr;
	}
}
function* area::FindFunctionName(std::wstring name)
{
	//local scope
	for (function* func : functions)
	{
		if (func->name == name)
		{
			return func;
		}
	}
	//seek in higher scope
	if (this->Parent)
	{
		return Parent->FindFunctionName(name);
	}
	else
	{
		return nullptr;
	}
}

variable* area::GetVariable(std::wstring name)
{
	variable* var = FindVariableName(name);
	if (var)return var;

	letter* endptr = nullptr;
	fp d = wcstof(&name[0], &endptr);
	if (*endptr == 0) //successful
	{
		variable* container = new variable();
		if (name.find('.') != std::wstring::npos) {
			container->type = tfp;
			container->var = (void*)new fp(d);
		}
		else
		{
			container->type = tint;
			container->var = (void*)new int((int)d);
		}
		return container;
	}

	if (name == std::wstring(L"true"))
	{
		variable* container = new variable();
		container->type = tbool;
		container->var = (void*)new bool(true);
		return container;
	}
	if (name == std::wstring(L"false"))
	{
		variable* container = new variable();
		container->type = tbool;
		container->var = (void*)new bool(false);
		return container;
	}
	return nullptr;
}

void area::ExecuteAllActions()
{
	for (action* a : actions)
	{
		a->execute();
	}
}
