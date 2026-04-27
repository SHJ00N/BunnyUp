#include "framework.h"
#include "main.h"

#include <System.h>

INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	HRESULT hr = S_OK;

	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// Create system instance
	Engine::System::CreateInstance();

	// Initialize system
	hr = Engine::System::GetInstance().Initialize();

	// Run the system
	if(SUCCEEDED(hr))
	{
		hr = Engine::System::GetInstance().Run();
	}

	// Destroy system instance
	Engine::System::GetInstance().Shutdown();
	Engine::System::DestroyInstance();

	return hr;
}