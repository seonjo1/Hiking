#include "Context.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Context* context;
	bool result;

	// Create the system object.
	context = new Context();

	// Initialize and run the system object.
	result = context->Initialize();
	if (result)
	{
		context->Run();
	}

	// Shutdown and release the system object.
	context->Shutdown();
	delete context;
	context = 0;

	return 0;
}
