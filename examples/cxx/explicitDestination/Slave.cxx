#include "Slave.hpp"
#include "FinalizeTask.hpp"
#include "BasicTask.hpp"

#include <MsgHost.hpp>
#include <HostNotFoundException.hpp>

#include <Msg.hpp>
using namespace std;

MSG_IMPLEMENT_DYNAMIC(Slave, Process)

int Slave::main(int argc, char** argv)
{
	info("Hello");
	
	Task* receivedTask;
	BasicTask* basicTask;
	
	while(true) 
	{ 
		receivedTask = Task::receive(Host::currentHost().getName());	
	
		if(receivedTask->isInstanceOf("FinalizeTask")) 
		{
			delete receivedTask;
			break;
		}
	
		basicTask = reinterpret_cast<BasicTask*>(receivedTask);
	
		info(TEXT_("Received \"") + TEXT_(basicTask->getName()) + TEXT_("\" "));
	
		info(TEXT_("Processing \"") + TEXT_(basicTask->getName()) + TEXT_("\" "));
		basicTask->execute();
		info(TEXT_("\"") + TEXT_(basicTask->getName()) + TEXT_("\" done "));
	
		delete basicTask;
	}
		
	info("Received Finalize. I'm done. See you!");
	
	delete this;
	return 0;
}
