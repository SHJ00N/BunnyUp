#pragma once

#include <EngineSystem.h>

namespace Game
{
    template<class T>
    class Singleton : public Engine::Component
    {
    public:
        static T& GetInstance()
        {
            assert(m_instance && "Singleton instance is not created yet!");
            return *m_instance;
        }

    protected:
        Singleton() = default;
        ~Singleton() = default;

        void OnAwake() override
        {
            assert(m_instance == nullptr);
            m_instance = static_cast<T*>(this);
        }

        void OnDestroy() override
        {
            if (m_instance == this)
                m_instance = nullptr;
        }

    private:
        static T* m_instance;
    };

    // block scope to avoid linker error
    template <class T>
    inline T* Singleton<T>::m_instance = nullptr;
}