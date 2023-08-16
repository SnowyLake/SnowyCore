#pragma once
namespace Snowy
{
template<typename T>
class TSingleton
{
public:
	static T& GetInstance()
	{
		static T instance;
		return instance;
	}
	TSingleton(const TSingleton&) = delete;
	TSingleton(TSingleton&&) = delete;
	TSingleton& operator=(const TSingleton&) = delete;
	TSingleton& operator=(TSingleton&&) = delete;
protected:
	TSingleton() = default;
	~TSingleton() = default;
};
}
