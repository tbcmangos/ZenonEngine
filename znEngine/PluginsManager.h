#pragma once

class CPluginException : public std::exception
{
public:
	CPluginException(const std::string& Message)
		: std::exception(Message.c_str())
	{}
	virtual ~CPluginException()
	{}
};


class CPluginException_NotAPlguin : public CPluginException
{
public:
	CPluginException_NotAPlguin(const std::string& Message)
		: CPluginException(Message)
	{}
	virtual ~CPluginException_NotAPlguin()
	{}
};


class ZN_API CznPluginsManager 
	: public IznPluginsManager
{
public:
	CznPluginsManager(IBaseManager& BaseManager);
	virtual ~CznPluginsManager();

	// IznPluginsManager
	bool											AddPlugin(const std::string& PluginDLLName);
	void											RemovePlugin(const std::string& PluginDLLName);
	void											InitializeAllPlugins();
	void                                            AddPluginEventListener(std::shared_ptr<IznPluginsEventListener> PluginEventListener);
	void                                            RemovePluginEventListener(std::shared_ptr<IznPluginsEventListener> PluginEventListener);                               

private:
	struct SPluginDescription
	{
		std::string Path;
		HMODULE HModule;
		std::shared_ptr<IznPlugin> Plugin;
	};

private:
	IBaseManager&                                   m_BaseManager;
	std::vector<SPluginDescription> m_Plugins;
	std::vector<std::shared_ptr<IznPluginsEventListener>> m_PluginsEventsListener;
};

