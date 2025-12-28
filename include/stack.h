#pragma once

#include <vector>
#include <stdexcept>

namespace ds {

    template <typename T, template<typename...> class TContainer = std::vector>
    class Stack {
    public:
        Stack() = default;
        Stack(const Stack& other) : m_data(other.m_data) {
        }
        Stack(Stack&& other) noexcept : m_data(std::move(other.m_data)) {
        }
        Stack& operator=(const Stack& other) {
            if (this != &other) {
                m_data = other.m_data;
            }
            return *this;
        }
        Stack& operator=(Stack&& other) noexcept {
            if (this != &other) {
                m_data = std::move(other.m_data);
            }
            return *this;
        }

        void push(const T& value) {
            m_data.push_back(value);
        }

        void push(T&& value) {
            m_data.push_back(std::move(value));
        }

        void pop() {
            if (m_data.empty()) {
                throw std::out_of_range("Stack::pop on empty stack");
            }
            m_data.pop_back();
        }

        T& top() {
            if (m_data.empty()) {
                throw std::out_of_range("Stack::top on empty stack");
            }
            return m_data.back();
        }

        const T& top() const {
            if (m_data.empty()) {
                throw std::out_of_range("Stack::top on empty stack");
            }
            return m_data.back();
        }

        bool empty() const noexcept {
            return m_data.empty();
        }

        std::size_t size() const noexcept {
            return m_data.size();
        }

    private:
        TContainer<T> m_data;
    };

}

