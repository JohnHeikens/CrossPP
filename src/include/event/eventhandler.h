#pragma once
#include "array/fastlist.h"
#include <functional>

template<typename eventArgsType, typename returnType = void>
struct eventHandler
{
	//https://stackoverflow.com/questions/4295432/typedef-function-pointer
	//functionType will be the name of the typedef
    typedef std::function<returnType(const eventArgsType&)> functionType;
	//typedef returnType(*functionType)(const eventArgsType& eventArgs);
//
	fastList <functionType> eventList = fastList<functionType>();

    //std::function<returnType(eventArgsType)> functionPtr;

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
    //member functions
    template<typename memberFunctionType, typename structType>
    constexpr void hook(memberFunctionType memberFunction, structType* structToBindTo)
    {
        functionType f = std::bind(memberFunction, structToBindTo, std::placeholders::_1);
        eventList.push_back(f);
        eventList.update();
    }
	constexpr void hook(functionType f)
	{
		eventList.push_back(f);
		eventList.update();
	}


};