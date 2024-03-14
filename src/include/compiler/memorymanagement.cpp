#include "memorymanagement.h"

variable::variable()
{

}

variable::variable(types type, bool modifyable, std::wstring name)
{
	this->type = type;
	this->modifyable = modifyable;
	this->name = name;
	switch (type)
	{
	case tvoid:
		break;
	case tbool:
		this->var = new bool();
		break;
	case tint:
		this->var = new int();
		break;
	case tfp:
		this->var = new fp();
		break;
	}
}

variable::variable(types type, void* var, bool modifyable, std::wstring name)
{
	this->name = name;
	this->var = var;
	this->modifyable = modifyable;
	this->type = type;
}
