#pragma once
#include <unordered_set>
#include <string>

template<class T>
class Group {
	T key;
	std::unordered_set<std::string> set;

public:
	Group<T> (T key_, std::unordered_set<std::string> set_):key(key_), set(set_){}
	T getKey() const {
		return key;
	}
	 bool operator<(Group<T>& other) {
		return set.size() > other.set.size();
	}
	std::unordered_set<std::string>::const_iterator find(const std::string& name)const {
		return set.find(name);
	}
	void insert(const std::string& name) {
		set.insert(name);
	}
	size_t size() const {
		return set.size();
	}
	std::unordered_set<std::string>::const_iterator end()const {
		return set.end();
	}

	const std::unordered_set<std::string>& getnames() const {
		return set;
	}

};

