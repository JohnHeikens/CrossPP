#pragma once
//https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments
#define GetMacroOverLoad(_1,_2,_3,NAME,...) NAME


//add a member to your struct with [expression] = other variable&
#define addMemberName(memberName, expression)																		\
	constexpr t& memberName()																						\
	{																												\
		return expression;																							\
	}																												\
	constexpr const t& memberName() const																			\
	{																												\
		return expression;																							\
	}																												\

#define addTypeSizes(structName)																					\
typedef structName##n<1> structName##1;																				\
typedef structName##n<2> structName##2;																				\
typedef structName##n<3> structName##3;																				\
typedef structName##n<4> structName##4;

#define addTemplateSize(shortenedTemplateName, size)																\
template<typename t>																								\
using shortenedTemplateName##t##size = shortenedTemplateName##tn<t, size>;

#define addTemplateSizes(shortenedTemplateName)																		\
addTemplateSize(shortenedTemplateName, 1)																			\
addTemplateSize(shortenedTemplateName, 2)																			\
addTemplateSize(shortenedTemplateName, 3)																			\
addTemplateSize(shortenedTemplateName, 4)

#define addTemplateType(shortenedTemplateName, typeTemplateName, typeName)											\
template<size_t axisCount>																							\
using shortenedTemplateName##typeTemplateName##n = shortenedTemplateName##tn<typeName, axisCount>;					\
																													\
addTypeSizes(shortenedTemplateName##typeTemplateName)																\
																													\
template<size_t axisCount>																							\
using c##shortenedTemplateName##typeTemplateName##n = const shortenedTemplateName##typeTemplateName##n<axisCount>;	\
																													\
addTypeSizes(c##shortenedTemplateName##typeTemplateName)

#define addTemplateTypes(shortenedTemplateName)																		\
template<typename t, size_t axisCount>																				\
using c##shortenedTemplateName##tn = const shortenedTemplateName##tn<t, axisCount>;									\
addTemplateSizes(shortenedTemplateName)																				\
addTemplateSizes(c##shortenedTemplateName)																			\
addTemplateType(shortenedTemplateName, , fp)																		\
addTemplateType(shortenedTemplateName, i, int)																		\
addTemplateType(shortenedTemplateName, l, long)																		\
addTemplateType(shortenedTemplateName, b, bool)

//https://stackoverflow.com/questions/18290523/is-a-default-move-constructor-equivalent-to-a-member-wise-move-constructor
#define addDefaultConstructors(structName, constructorType)																			\
constructorType structName(const structName& other) = default;															\
constructorType structName(structName&& other) = default;																\
constructorType structName& operator=(structName && other) = default;													\
constructorType structName& operator=(const structName & other) = default;


//newexpression should be a macro that creates a new instance with the same size as the argument
#define addOperator(o, newExpression, structType, functionType)														\
template<typename t2>																								\
functionType typename std::enable_if<std::is_arithmetic_v<t2>, structType>										\
::type operator o(const t2& b) const {																				\
	newExpression((*this))																							\
	for (auto it : std::views::zip(result, (*this)))																			\
	{																												\
		std::get<0>(it) = std::get<1>(it) o b;																				\
	}																												\
	return result;																									\
}																													\
functionType structType operator o(const structType& b) const														\
{																													\
	newExpression(b)																								\
	for (auto it : std::views::zip(result, (*this), b))																			\
	{																												\
		std::get<0>(it) = std::get<1>(it) o std::get<2>(it);																	\
	}																												\
	return result;																									\
 }																													\
template<typename t2>																								\
functionType friend typename std::enable_if<std::is_arithmetic_v<t2>, structType>								\
::type operator o(const t2& a, const structType& b)																	\
 { 																													\
	newExpression(b)																								\
	for (auto it : std::views::zip(result, b))																					\
	{																												\
		std::get<0>(it) = a o std::get<1>(it);																				\
	}																												\
	return result;																									\
}																													\
functionType void operator o##= (const structType& b)																\
{																													\
	for (auto it : zip((*this), b))																					\
	{																												\
		std::get<0>(it) o##= std::get<1>(it);																				\
	}																												\
}																													\
template<typename t2>																								\
functionType typename std::enable_if<std::is_arithmetic_v<t2>, void>											\
::type operator o##=(const t2& b)																					\
{ 																													\
	for (t& val : (*this))																							\
	{																												\
		val o##= b;																									\
	}																												\
}

#define addOperators(newExpression, structType, functionType)														\
addOperator(+, newExpression, structType, functionType)																\
addOperator(-, newExpression, structType, functionType)																\
addOperator(*, newExpression, structType, functionType)																\
addOperator(/, newExpression, structType, functionType)									

#define addAssignmentOperator(structName, functionType) \
functionType structName& operator=(structName copy)\
	{\
	this->swap(copy);\
	return *this;\
	}

