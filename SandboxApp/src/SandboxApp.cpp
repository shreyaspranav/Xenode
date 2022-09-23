#include <Xenode.h>

class SandboxApp : public Xen::GameApplication
{
public:
	SandboxApp()
	{

	}
	~SandboxApp()
	{

	}

private:

};

Xen::GameApplication* Xen::CreateApplication() { return new SandboxApp(); }