#pragma once
#include <set>
#include"Group.h"
#include <map>
#include <memory>
#include <vector>
#include<iostream>

template<class T>
class Groups {
	std::vector<Group<T>> groups;
	size_t size , maximal_size;
	std::shared_ptr<Group<T>> biggest_group;

public:	

	Groups<T>(size_t maximal_size_):size(0),maximal_size(maximal_size_){
		groups.reserve(maximal_size);		
	}

	void insert(const T key, const std::string name) {
		size++;

		if (maximal_size < size) {
			size--;
			return;
		}
		for (auto& group : groups) {
			if (group.getKey() == key) {
				group.insert(name);
				if (biggest_group) {
					if (biggest_group->size() < group.size()) {
						biggest_group = std::make_shared<Group<T>>(group);
						return;
					}
				}
				biggest_group = std::make_shared<Group<T>>(group);
				return;								
			}
		}
		groups.push_back(Group<T>(key, std::unordered_set<std::string>{name}));
		biggest_group = std::make_shared<Group<T>>(groups[groups.size() - 1]);
	}

	const Group<T>& getBiggestGroup() const {
		return (*biggest_group);
	}

	bool hasConsensus()const {
		return (biggest_group->size() > double(size / 2));
	}

	bool existsInBiggest(const std::string& name) const {
		return biggest_group->find(name) != biggest_group->end();
	}


};

