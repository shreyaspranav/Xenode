#include <Xenode.h>

class SandboxApp : public Xen::GameApplication
{
public:
	SandboxApp(){
		XEN_INIT_LOGGER
		XEN_APP_LOG_WARN("efsdjklakl")
	}
	~SandboxApp(){}
	
};

Xen::GameApplication* Xen::CreateApplication() { return new SandboxApp(); }