#pragma once
#include "array/fastlist.h"

template<typename eventArgsType, typename returnType = void>
struct eventHandler
{
	//https://stackoverflow.com/questions/4295432/typedef-function-pointer
	//functionType will be the name of the typedef
	typedef returnType(*functionType)(const eventArgsType& eventArgs);

	fastList <functionType> eventList = fastList<functionType>();

	constexpr void invoke(const eventArgsType& eventArgs)
	{
		if (!std::is_same<returnType, void>::value)
		{
			throw "functions with return types should be overloaded";
		}

		for (functionType f : eventList)
		{
			f(eventArgs);
		}
	}

	constexpr void hook(functionType f)
	{
		eventList.push_back(f);
		eventList.update();
	}


};