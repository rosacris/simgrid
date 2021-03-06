#ifndef BASIC_TASK_HPP
#define BASIC_TASK_HPP

#include <MsgTask.hpp>
using namespace SimGrid::Msg;

class BasicTask : public Task
{
	MSG_DECLARE_DYNAMIC(BasicTask);
public:
	
	// Default constructor.
	BasicTask() {}
	
	// Destructor
	virtual ~BasicTask()
	throw (MsgException) {}
	BasicTask(const char* name, double computeDuration, double messageSize)
	throw (InvalidArgumentException, NullPointerException)
	:Task(name, computeDuration, messageSize){}

	/*virtual const BasicTask& operator = (const BasicTask& rTask) {
		Task::operator=(rTask);
		return *this;
	}*/
};

typedef BasicTask* BasicTaskPtr;


#endif // !BASIC_TASK_HPP
