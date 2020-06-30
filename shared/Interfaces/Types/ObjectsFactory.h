#pragma once

ZN_INTERFACE ZN_API IObjectCreationArgs
{
	virtual ~IObjectCreationArgs() {}
};

ZN_INTERFACE ZN_API IObjectClassCreator
{
	virtual ~IObjectClassCreator() {}

	virtual size_t GetSupportedClassCount() const = 0;
	virtual ObjectClassType GetSupportedClassKey(size_t Index) const = 0;
	virtual std::shared_ptr<IObject> CreateObject(size_t Index, const IObjectCreationArgs* ObjectCreationArgs) = 0;
};

ZN_INTERFACE ZN_API __declspec(uuid("5455FD09-C8F2-4E6C-855A-C1E5E7377F3F")) IObjectClassFactory
{
	virtual ~IObjectClassFactory() {}

	virtual std::shared_ptr<IObjectClassCreator> GetClassCreator(ObjectClassType ObjectClassKey) const = 0;
	virtual void AddClassCreator(std::shared_ptr<IObjectClassCreator> Creator) = 0;
	virtual void RemoveClassCreator(std::shared_ptr<IObjectClassCreator> Creator) = 0;

	virtual ObjectFactoryType GetType() const = 0;
	virtual Object::Guid GenerateGuid(ObjectClassType ObjectClassKey) = 0;
	virtual std::shared_ptr<IObject> CreateObject(ObjectClassType ObjectClassKey, const IObjectCreationArgs* ObjectCreationArgs) = 0;
	virtual std::shared_ptr<IObject> LoadObject(ObjectClassType ObjectClassKey, std::shared_ptr<IByteBuffer> Bytes) = 0;
	virtual std::shared_ptr<IByteBuffer> SaveObject(std::shared_ptr<IObject> Object) = 0;
};

ZN_INTERFACE ZN_API __declspec(uuid("F2E660BC-4074-48D2-9786-67041B41E97E")) IObjectsFactory
	: public IManager
{
	virtual ~IObjectsFactory() {}

	virtual std::shared_ptr<IObjectClassFactory> GetClassFactory(ObjectFactoryType ObjectFactoryKey) const = 0;
	virtual void AddClassFactory(std::shared_ptr<IObjectClassFactory> Creator) = 0;
	virtual void RemoveClassFactory(std::shared_ptr<IObjectClassFactory> Creator) = 0;
	virtual std::shared_ptr<IObject> CreateObject(ObjectFactoryType ObjectFactoryKey, ObjectClassType ObjectClassKey, const IObjectCreationArgs* ObjectCreationArgs) = 0;
	virtual std::shared_ptr<IObject> LoadObject(std::shared_ptr<IByteBuffer> Bytes) = 0;
	virtual std::shared_ptr<IByteBuffer> SaveObject(std::shared_ptr<IObject> Object) = 0;

	template<class T>
	inline std::shared_ptr<T> GetClassFactoryCast(ObjectFactoryType ObjectFactoryKey) const
	{
		return std::dynamic_pointer_cast<T>(GetClassFactory(ObjectFactoryKey));
	}
};