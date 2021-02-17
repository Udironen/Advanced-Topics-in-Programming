#pragma once 

#include <ranges>
#include <algorithm>
#include <string>
#include <string_view>
#include <unordered_set>
#include <boost/algorithm/string.hpp>

class Restrictions {
    std::unordered_set<std::string> restrictions;
public:
    Restrictions(const std::string_view r) {
        auto parts = r | std::views::split(',');
        std::ranges::for_each(parts, [this](auto part) {
            std::string str{std::string_view(&*part.begin(), std::ranges::distance(part))};
            boost::trim(str);
            if(!str.empty()) restrictions.insert(str);
        });
    }
    bool contains(const std::string& r) const {
        return restrictions.contains(r);
    }

    /**
     * @brief Get the Restrictions object
     *        allows calling on its result:
     * 
     * @return const std::ranges::sized_range 
     *           size()
     *           begin() and end(), thus allowing also range-based-for
     *           BUT it doesn't expose the exact type that is hold inside the class
     */
    const std::ranges::sized_range auto getRestrictions() const {
        return restrictions;
    }
};
