#include "stdafx.h"

// General
#include "PluginsManager.h"



typedef IznPlugin*(GetPluginFuncProc)(IBaseManager* BaseManager);


CznPluginsManager::CznPluginsManager(IBaseManager* BaseManager)
	: m_BaseManager(BaseManager)
{}

CznPluginsManager::~CznPluginsManager()
{
	for (const auto& pluginPair : m_Plugins)
	{
		FreeLibrary(pluginPair.first);
	}
}



//
// IznPluginsManager
//
bool CznPluginsManager::AddPlugin(const std::string& PluginDLLName)
{
	try
	{
		//if (m_Plugins.find(PluginDLLName) != m_Plugins.end())
		//{
		//	throw CPluginException("Already registered.");
		//}

		char modulePath[MAX_PATH];
		::GetModuleFileNameA(NULL, modulePath, MAX_PATH);

		std::string modulePathStr(modulePath);
		modulePathStr = modulePathStr.substr(0, modulePathStr.find_last_of("\\") + 1);

		HMODULE pluginDLL = LoadLibraryA(PluginDLLName.c_str());
		if (pluginDLL == NULL)
		{
			DWORD lastError = ::GetLastError();
			throw CPluginException_NotAPlguin("File not found.");
		}

		void* getPluginProcNative = GetProcAddress(pluginDLL, "GetPlugin");
		if (getPluginProcNative == NULL)
		{
			FreeLibrary(pluginDLL);
			return false;
		}

		GetPluginFuncProc* getPluginProc = (GetPluginFuncProc*)getPluginProcNative;

		std::shared_ptr<IznPlugin> pluginObject(getPluginProc(m_BaseManager));
		if (pluginObject == nullptr)
		{
			FreeLibrary(pluginDLL);
			throw CPluginException("Error while create plugin object.");
		}

		m_Plugins.push_back(std::make_pair(pluginDLL, pluginObject));

		// Notify all listeners about new plguin
		for (const auto& listener : m_PluginsEventsListener)
			listener->OnPluginAdded(pluginObject);
	}
	catch (const CPluginException_NotAPlguin& e)
	{
		Log::Warn(("Plugin[" + PluginDLLName + "]: " + e.what()).c_str());
		return false;
	}
	catch (const CPluginException& e)
	{
		throw std::exception(("Plugin[" + PluginDLLName + "]: " + e.what()).c_str());
	}

	Log::Green("Plugin[%s]: Successfully added.", PluginDLLName.c_str());

	return true;
}

void CznPluginsManager::RemovePlugin(std::shared_ptr<IznPlugin> Plugin)
{
}

void CznPluginsManager::InitializeAllPlugins()
{
	for (const auto& pluginPair : m_Plugins)
	{
		const std::shared_ptr<IznPlugin>& plugin = pluginPair.second;

		if (!plugin->Initialize())
			throw CPluginException("Error while initialize.");

		// Notify all listeners about initialize plguin
		for (const auto& listener : m_PluginsEventsListener)
			listener->OnPluginInitialized(plugin);
	}
}

void CznPluginsManager::AddPluginEventListener(std::shared_ptr<IznPluginsEventListener> PluginEventListener)
{
	m_PluginsEventsListener.push_back(PluginEventListener);
}

void CznPluginsManager::RemovePluginEventListener(std::shared_ptr<IznPluginsEventListener> PluginEventListener)
{

}
