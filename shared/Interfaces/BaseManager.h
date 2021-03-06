#pragma once

// FORWARD BEGIN
ZN_INTERFACE IRenderDevice;
ZN_INTERFACE IApplication;
ZN_INTERFACE IObjectsFactory;
// FORWARD END

ZN_INTERFACE ZN_API __declspec(uuid("BDC2768B-055D-42EA-ABE3-CF17CD21178D")) IManager
{
	virtual ~IManager() = 0 {};
};



// FORWARD BEGIN
class CFont;
// FORWARD END

ZN_INTERFACE ZN_API
	__declspec(uuid("1427E242-CCB8-4AEC-ABC8-17DE58A96B05"))
	IFontsManager : public IManager
{
	virtual ~IFontsManager() {};

	virtual std::shared_ptr<CFont> GetMainFont() const = 0;
	virtual std::shared_ptr<CFont> Add(IRenderDevice& RenderDevice, const std::string& _fontFileName, uint32 _fontSize) = 0;
	virtual bool Exists(const std::string& name) const = 0;
	virtual void Delete(const std::string& name) = 0;
	virtual void Delete(std::shared_ptr<CFont> item) = 0;
};

//--

ZN_INTERFACE ZN_API __declspec(uuid("BB9FD479-C7AD-4F57-837B-E299A04AF171")) IBaseManager
{
	virtual ~IBaseManager() {};

	virtual IManager* AddManager(GUID Type, const std::shared_ptr<IManager>& Manager) = 0;
	virtual void RemoveManager(GUID Type) = 0;
	virtual IManager* GetManager(GUID Type) const = 0;
	virtual void RemoveAllManagers() = 0;

	virtual const IApplication& GetApplication() const = 0;

	// Templates

	template<typename T>
	inline T* AddManager(const std::shared_ptr<IManager>& _manager)
	{
		return dynamic_cast<T*>(AddManager(__uuidof(T), _manager));
	}

	template<typename T>
	inline void RemoveManager()
	{
		RemoveManager(__uuidof(T));
	}

	template<typename T>
	inline T* GetManager() const
	{
		IManager* manager = GetManager(__uuidof(T));
		if (manager == nullptr)
			_ASSERT_EXPR(false, L"Manager not found.");

		return dynamic_cast<T*>(manager);
	}
};


ZN_INTERFACE ZN_API IBaseManagerInternal
{
	virtual ~IBaseManagerInternal() {}

	virtual void SetApplicationInternal(const IApplication* Application) = 0;
};


ZN_INTERFACE ZN_API IBaseManagerHolder
{
	virtual ~IBaseManagerHolder() {}

	virtual IBaseManager& GetBaseManager() const = 0;
};