#include "taskList.h"
#include "type/types.h"
void taskList::execute()
{
	for (int i = 0; i < tasks.size(); i++)
	{
		if (i == executingTaskIndex)
		{
			if (tasks[i]->continueExecuting())
			{
				tasks[i]->updateTask();
				return;
			}
			else
			{
				tasks[i]->resetTask();
			}
		}
		else if (tasks[i]->shouldExecute())
		{
			if (executingTaskIndex != -1)
			{
				//higher priority
				tasks[executingTaskIndex]->resetTask();
			}
			executingTaskIndex = i;
			tasks[i]->startExecuting();
			return;
		}
	}
}

void taskList::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"executing task index"), executingTaskIndex);
	if (s.push<nbtDataTag::tagCompound>(std::wstring(L"tasks")))
	{
		//won't mess up that much when tasks are shuffeled
		for (int i = 0; i < tasks.size(); i++)
		{
			if (s.push<nbtDataTag::tagCompound>(getClassName(*tasks[i])))
			{
				tasks[i]->serializeValue(s);
				s.pop();
			}
		}
		s.pop();
	}
}

taskList::~taskList()
{
	for (taskAI* const& task : tasks)
	{
		delete task;
	}
}