//
// Created by kkeiper1103 on 5/27/2023.
//

#ifndef PREDATION_IFACTORY_H
#define PREDATION_IFACTORY_H

#include <vector>
#include <memory>
#include <map>
#include <unordered_map>

template <typename T>
class Factory {
public:
    using Ptr = std::shared_ptr<T>;

    explicit Factory() = default;
    virtual ~Factory() = default;

    virtual Ptr Get(const std::string& filename) = 0;

protected:
    std::unordered_map<std::string, Ptr> items;
};

#endif //PREDATION_IFACTORY_H
