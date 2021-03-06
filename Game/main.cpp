#include "stdafx.h"

static IBaseManager* BaseManager = nullptr;


void main_internal(int argumentCount, char* arguments[])
{
	// 1. Initialize engine and some improtant managers
	BaseManager = InitializeEngine(Utils::ArgumentsToVector(argumentCount, arguments), "");

	try
	{
		Application app(*BaseManager, ::GetModuleHandle(NULL));

		CNativeWindowFactory nativeWindowFactory(&app);

		std::unique_ptr<INativeWindow> nativeWindow = nativeWindowFactory.CreateWindowInstance(
			L"Zenon Engine",
			BaseManager->GetManager<ISettings>()->GetGroup("Video")->GetSettingT<glm::vec2>("WindowSize")->Get().x,
			BaseManager->GetManager<ISettings>()->GetGroup("Video")->GetSettingT<glm::vec2>("WindowSize")->Get().y
		);

		IRenderDevice& renderDevice = app.CreateRenderDevice(RenderDeviceType::RenderDeviceType_DirectX11);

		std::shared_ptr<IFontsManager> fontsManager = std::make_shared<FontsManager>(renderDevice, *BaseManager);
		BaseManager->AddManager<IFontsManager>(fontsManager);

		const auto& firstRenderWindow = renderDevice.GetObjectsFactory().CreateRenderWindow(*nativeWindow, false);
		app.AddRenderWindow(firstRenderWindow);

		BaseManager->GetManager<ILoader>()->Start();


		std::shared_ptr<IScene> scene = BaseManager->GetManager<IObjectsFactory>()->GetClassFactoryCast<IScenesFactory>()->CreateScene(cSceneDefault);
		scene->SetRenderWindow(firstRenderWindow);
		scene->ConnectEvents(std::dynamic_pointer_cast<IRenderWindowEvents>(firstRenderWindow));
		scene->Initialize();

		app.Run();

	}
	catch (const CznRenderException& e)
	{
		Log::Fatal("RenderError: %s", e.MessageCStr());
	}
	catch (const CException& e)
	{
		Log::Fatal("EngienError: %s", e.MessageCStr());
	}
}


int main(int argumentCount, char* arguments[])
{
#ifdef _DEBUG
	_CrtMemState _ms;
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

	//_CrtSetBreakAlloc(158);
#endif

	main_internal(argumentCount, arguments);		

	if (BaseManager)
	{
		BaseManager->RemoveAllManagers();
		delete BaseManager;
	}

#ifdef _DEBUG
	_CrtMemDumpAllObjectsSince(&_ms);
#endif
	return 0;
}
