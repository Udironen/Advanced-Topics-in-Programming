#pragma once 

template<typename T>
class NamedType {
protected:
	T val;
public:
	constexpr explicit NamedType(const T& t): val(t) {}
	
	operator const T&() const {
		return val;
    }
    
    const T* operator->() const {
        return &val;
    }
    
	auto operator<=>(const NamedType<T>&) const = default;
};


