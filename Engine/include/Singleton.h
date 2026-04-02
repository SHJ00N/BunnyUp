#pragma once

#include <memory>
#include <assert.h>

namespace Engine
{
	template <class T>
	class Singleton
	{
	public:
		// get instance
		static T& GetInstance() 
		{ 
			assert(m_instance != nullptr);
			return *m_instance; 
		}

		// create instance
		static void CreateInstance()
		{
			assert(m_instance == nullptr);
			m_instance = std::make_unique<T>();
		}

		// destroy instance
		static void DestroyInstance()
		{
			assert(m_instance != nullptr);
			m_instance.reset();
		}

	protected:
		Singleton() = default;
		~Singleton() = default;

	private:
		static std::unique_ptr<T> m_instance;
	};

	// block scope to avoid linker error
	template <class T>
	inline std::unique_ptr<T> Singleton<T>::m_instance = nullptr;
}