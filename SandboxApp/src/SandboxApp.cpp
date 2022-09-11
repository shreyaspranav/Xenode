#include <Xenode.h>

class SandboxApp : public Xen::GameApplication
{
public:
	SandboxApp(){}
	~SandboxApp(){}
	
};

Xen::GameApplication* Xen::CreateApplication() { return new SandboxApp(); }