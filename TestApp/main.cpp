#include "stdafx.h"

// Additional (TEST)
#include "GameState_World.h"

// Additional (Windows)
#include <ctime>

int main(int argumentCount, char* arguments[])
{
#ifdef _DEBUG 
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif
	{
		std::shared_ptr<IBaseManager> BaseManager = std::make_shared<CBaseManager>();

		std::shared_ptr<IznPluginsManager> pluginsManager = std::make_shared<CznPluginsManager>(BaseManager);
		//pluginsManager->RegisterPlugin("znPlugin.dll");
		pluginsManager->RegisterPlugin("znEngine.dll");
		pluginsManager->RegisterPlugin("znRenderDX11.dll");
		pluginsManager->RegisterPlugin("znRenderOpenGL.dll");

		GetManager<IFilesManager>(BaseManager)->RegisterFilesStorage(std::make_shared<CLocalFilesStorage>("D:\\_programming\\ZenonEngine\\gamedata\\"));
		GetManager<IFilesManager>(BaseManager)->RegisterFilesStorage(std::make_shared<CLibraryResourceFileStotage>(GetModuleHandle(NULL)));

        HMODULE m_HINSTANCE = ::GetModuleHandle(NULL);

        CWindowObject* windowObject = new CWindowObject();
        windowObject->RegisterWindowClass(m_HINSTANCE);
        windowObject->CreateWindowInstance(1280, 1024);

		//--

		Application app(BaseManager);

        std::shared_ptr<IRenderDevice> renderDevice = app.CreateRenderDevice(RenderDeviceType::RenderDeviceType_DirectX);
        std::shared_ptr<IRenderWindow> renderWindow = app.CreateRenderWindow(windowObject, true);

        std::shared_ptr<IFontsManager> fontsManager = std::make_shared<FontsManager>(BaseManager);
        AddManager<IFontsManager>(BaseManager, fontsManager);

		app.AddGameState(GameStatesNames::GAME_STATE_WORLD, std::make_shared<CGameState_World>(&app));
		app.SetGameState(GameStatesNames::GAME_STATE_WORLD);
		app.Run();
	}

	return 0;
}
