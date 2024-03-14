#include "compiler.h"
#include "array/wstring.h"

void compiler::initialize()
{
	GenerateStandardArea();
}

//area can contain external functions and variables that you want to be able to call in the code
area* compiler::Compile(std::wstring code, area* a)
{
	//split in functions
	//first, remove all comments
	std::vector<std::wstring> lines = GetCodeLines(code);
	area* localarea = Compile(lines, a);
	return localarea;

}

area* compiler::Compile(std::vector<std::wstring> lines, area* parent)
{
	area* cur = new area();
	cur->Parent = parent;
	for (size_t i = 0; i < lines.size(); i++)
	{
		std::wstring line = lines[i];
		if (line.length() > 2) //smallest line of code: x++
		{
			wstringContainer words = split_string(line, std::wstring(L" "));
			csize_t& space1index = line.find(' ');
			csize_t& space2index = line.find(' ', space1index + 1);

			bool isconst = compareSubstring(words[0], std::wstring(L"const"));//const or constexpr, does not matter
			size_t typeindex = isconst ? 1 : 0;
			types vartype = GetType(words[typeindex]);
			bool isvar = vartype >= 0;
			if (isvar) //declaration
			{
				if (i + 1 < lines.size() && lines[i + 1] == std::wstring(L"{"))//function or array
				{
					//seek braces
					csize_t& brace1 = i + 1;
					csize_t& brace2 = FindLine(lines, brace1 + 1, std::wstring(L"}"), wstringContainer{ std::wstring(L"{"),std::wstring(L"}") });
					if (brace2 < 0)
					{
						ShowCompilerError(errortypes::noclosingbrace, line);
					}
					//arguments
					csize_t& index1 = line.find('(');
					csize_t& index2 = find(line, index1 + 1, std::wstring(L")"), skip);
					if (index2 >= 0) {
						//function that returns vartype
						internalfunction* f = new internalfunction();
						f->name = line.substr(space1index + 1, index1 - space1index - 1);
						std::wstring argumentspace = line.substr(index1 + 1, index2 - index1 - 1);
						if (argumentspace.length())
						{
							wstringContainer arguments = split_string(argumentspace, std::wstring(L","));
							for (size_t i = 0; i < arguments.size(); i++)
							{
								trim(arguments[i]);
								size_t spaceindex = arguments[i].find(' ');
								variable* v = new variable(GetType(trim_copy(arguments[i].substr(0, spaceindex))), 
									true,//is modifyable, because the arguments will be copied
									trim_copy(arguments[i].substr(spaceindex + 1, arguments[i].size() - spaceindex - 1)));
								f->arguments.push_back(v);
							}
						}
						area* WithArguments = new area();
						WithArguments->Parent = cur;
						WithArguments->variables = f->arguments;
						//copy lines into a new array
						wstringContainer functionlines = wstringContainer{ &lines[brace1 + 1], &lines[brace2] };

						area* a = Compile(functionlines, WithArguments);
						f->actions = a->actions;
						cur->functions.push_back(f);
						delete a;//the area is not necessary anymore
					}
					else //array
					{
						//get name
						csize_t& bracket0 = line.find('[') + 1;
						csize_t& bracket1 = line.find(']');
						std::wstring namewithbrackets = words[typeindex + 1];
						variable* v = new variable();
						v->name = namewithbrackets.substr(0, namewithbrackets.find('['));
						long count;
						convertToLong(line.substr(bracket0, bracket1 - bracket0), count, 10);
						v->type = vartype;
						std::wstring values = lines[i + 2];
						wstringContainer valcontainer = split_string(values, std::wstring(L","));
						if (valcontainer.size() != count) 
						{
							ShowCompilerError(errortypes::notallindexesinitialized,namewithbrackets);
						}
						switch (vartype)
						{
						case tbool:
							v->var = (void*)new bool[count];
							for (size_t i = 0; i < count; i++)
							{
								trim(valcontainer[i]);
								((bool*)v->var)[i] = valcontainer[i] == std::wstring(L"true");
							}
							break;
						case tint:
							v->var = (void*)new int[count];
							for (size_t i = 0; i < count; i++)
							{
								trim(valcontainer[i]);
								long l;
								convertToLong(valcontainer[i], l, 10);
								((int*)v->var)[i] = (int)l;
							}
							break;
						case tfp:
							v->var = (void*)new fp[count];
							for (size_t i = 0; i < count; i++)
							{
								trim(valcontainer[i]);
								fp d;
								convertToDouble(valcontainer[i], d);
								((fp*)v->var)[i] = d;
							}
							break;
						}
						cur->variables.push_back(v);
					}
					i = brace2;//warp to end of function
					continue;
				}
				else
				{//variable
					std::wstring namepart = line.substr(isconst ? space2index : space1index);
					wstringContainer names = split_string(namepart, std::wstring(L","), skip);
					for (size_t i = 0; i < names.size(); i++)
					{
						trim(names[i]);
						std::wstring declaration = names[i];
						if (declaration.length() > 0) {
							variable* Declared = new variable(vartype, true);
							cur->variables.push_back(Declared);

							csize_t& equalsindex = declaration.find('=');
							if (equalsindex > 0)//minimum chars: a=
							{
								//instant initialization
								Declared->name = trim_copy(declaration.substr(0, equalsindex));//will be trimmed later
								calculation* c = cur->GetCalculation(declaration, NULL);
								if (c) {
									cur->actions.push_back(c);
								}
							}
							else
							{
								Declared->name = trim_copy(names[i]);
							}
						}
					}
				}
			}
			else if (compareSubstring(line, std::wstring(L"for")))
			{
				//for loop
				loop* l = new loop();

				csize_t& index1 = line.find('(') + 1;
				csize_t& index2 = find(line, index1, std::wstring(L")"), skip);
				std::wstring betweenparentheses = line.substr(index1, index2 - index1);
				//split line
				wstringContainer parenthesescontainer = split_string(betweenparentheses, std::wstring(L";"));
				//seek brackets
				csize_t& bracket1 = i + 1;
				csize_t& bracket2 = FindLine(lines, bracket1 + 1, std::wstring(L"}"), wstringContainer{ std::wstring(L"{"), std::wstring(L"}") });
				//between the brackets
				wstringContainer bracketscontainer = wstringContainer{ &lines[bracket1 + 1],&lines[bracket2] };
				bracketscontainer.push_back(parenthesescontainer[2]);//at the end of each iteration

				//do this once
				area* abegin = Compile(wstringContainer{ parenthesescontainer[0] }, cur);
				cur->actions.insert(cur->actions.end(), abegin->actions.begin(), abegin->actions.end());
				cur->variables.insert(cur->variables.end(), abegin->variables.begin(), abegin->variables.end());

				area* a = Compile(bracketscontainer, cur);
				l->actions = a->actions;//copy actions
				l->checkresult = nullptr;
				if (action* check = a->GetCalculation(parenthesescontainer[1], &l->checkresult))
				{
					l->actions.push_back(check);//at the end of each iteration and
					cur->actions.push_back(check);//before the iteration starts
				}
				cur->actions.push_back(l);//insert loop
				i = bracket2;//warp to end of statement
				continue;
			}
			else if (compareSubstring(line, std::wstring(L"if")))
			{
				//if statement
				ifstatement* s = new ifstatement();

				csize_t& index1 = line.find('(') + 1;
				csize_t& index2 = find(line, index1, std::wstring(L")"), skip);
				std::wstring betweenparentheses = line.substr(index1, index2 - index1);
				if (action* check = cur->GetCalculation(betweenparentheses, &s->checkresult))
				{
					cur->actions.push_back(check);//before the statement is called, the variables will be checked
				}

				//seek brackets
				csize_t& truebracket1 = i + 1;
				csize_t& truebracket2 = FindLine(lines, truebracket1 + 1, std::wstring(L"}"), wstringContainer{ std::wstring(L"{"), std::wstring(L"}") });
				wstringContainer bracketscontainer = wstringContainer{ &lines[truebracket1 + 1],&lines[truebracket2] };

				area* truea = Compile(bracketscontainer, cur);

				s->actionswhentrue = truea->actions;
				//check for else
				if (compareSubstring(lines[truebracket2 + 1], std::wstring(L"else")))
				{
					csize_t& falsebracket1 = truebracket2 + 2;//skip the 'else' sentence
					csize_t& falsebracket2 = FindLine(lines, falsebracket1 + 1, std::wstring(L"}"), wstringContainer{ std::wstring(L"{"), std::wstring(L"}") });
					wstringContainer falsebracketscontainer = wstringContainer{ &lines[falsebracket1 + 1],&lines[falsebracket2] };

					area* falsea = Compile(falsebracketscontainer, cur);

					s->actionswhenfalse = falsea->actions;
					i = falsebracket2;//warp to end of statement
				}
				else
				{
					i = truebracket2;//warp to end of statement
				}
				cur->actions.push_back(s);//insert statement
				continue;
			}
			else
			{
				calculation* c = cur->GetCalculation(line, NULL);//return variable will not be used
				cur->actions.push_back(c);
			}
		}
		else if (line.length())
		{
			ShowCompilerError(errortypes::linetooshort, line);
		}
	}
	return cur;
}

std::wstring RemoveComments(std::wstring str) 
{
	size_t lastindex = 0;
	while (true)
	{
		csize_t& newindex = str.find(std::wstring(L"//"), lastindex);
		if (newindex == std::wstring::npos)
		{
			return str;
		}
		else
		{
			csize_t& newlineindex = str.find(L'\n', newindex + 1);
			str = str.substr(0, newindex) + str.substr(newlineindex + 1);//remove the '\n' too
			lastindex = newindex;//jump just over the begin of the cut
		}
	}
}
std::vector<std::wstring> compiler::GetCodeLines(std::wstring code)
{
	//remove comments
	code = RemoveComments(code);
	code = replace(code, std::wstring(L"\n"), std::wstring(L""));
	code = replace(code, std::wstring(L"\r"), std::wstring(L""));//sort of \n
	code = replace(code, std::wstring(L"\t"), std::wstring(L" "));
	code = replace(code, std::wstring(L"}"), std::wstring(L";};"));
	code = replace(code, std::wstring(L"{"), std::wstring(L";{;"));
	convertToLowerCase(code);
	//std::transform(code.begin(), code.end(), code.begin(), ::tolower);
	while (code.find(std::wstring(L";;")) != std::wstring::npos)
	{
		code = replace(code, std::wstring(L";;"), std::wstring(L";"));
	}
	while (code.find(std::wstring(L"  ")) != std::wstring::npos)
	{
		code = replace(code, std::wstring(L"  "), std::wstring(L" "));
	}

	std::vector<std::wstring> lines = split_string(code, L';', skip);
	for (size_t i = 0; i < lines.size(); i++) 
	{
		//lines can be empty strings
		trim(lines[i]);
	}
	return lines;
}
