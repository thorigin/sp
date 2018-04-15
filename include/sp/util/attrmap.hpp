/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_UTIL_ATTRMAP_HPP
#define	SP_UTIL_ATTRMAP_HPP

#include <unordered_map>
#include <utility>
#include <string>

SP_UTIL_NAMESPACE_BEGIN

// \internal Implementation details
namespace details {

struct attribute_value {
    virtual ~attribute_value() {}
};

/**
 * \brief Typed Attribute value holder object
 *
 * A polymorphic type extending the basic attribute
 *
 */
template<typename T>
struct typed_attribute_value : attribute_value {

    typed_attribute_value(const T& value) : value(value) {}
    typed_attribute_value(T&& value) : value(std::forward<T>(value)) {}

    typed_attribute_value(const typed_attribute_value&) = delete;
    typed_attribute_value(typed_attribute_value&&) = delete;
    typed_attribute_value& operator=(const typed_attribute_value&) = delete;
    typed_attribute_value& operator=(typed_attribute_value&&) = delete;

    inline T& get() {
        return value;
    }

    inline T get() const {
        return value;
    }

    inline void set(T&& value) {
        this->value = std::forward<T>(value);
    }

    inline void set(const T& value) {
        this->value = value;
    }

private:
    T value;
};

}

/**
 * \brief Attribute Map
 *
 * Simple data structure that helps with variable typed values of a map
 */
template<typename Key_type = std::string>
struct attributes_map {

    using key_type = Key_type;

    attributes_map() : attrs(0) {
    }

    virtual ~attributes_map() {
        for (const auto& pair : this->attrs) {
            delete pair.second;
        }
    }

    template<typename T>
    bool set(const key_type& key, const T& value) {
        auto it = this->attrs.find(key);
        if (it == attrs.end()) {
            this->attrs[key] = new details::typed_attribute_value<T>(std::forward(value));
            return true;
        }
        details::typed_attribute_value<T>* ptr = dynamic_cast<details::typed_attribute_value<T>*> ((*it).second);
        if (ptr) {
            ptr->set(value);
            return true;
        }
        return false;
    }

    template<typename T>
    bool set(const key_type& key, T&& value) {
        auto it = this->attrs.find(key);
        if (it == attrs.end()) {
            this->attrs[key] = new details::typed_attribute_value<T>(std::forward<T>(value));
            return true;
        }
        details::typed_attribute_value<T>* ptr = dynamic_cast<details::typed_attribute_value<T>*> ((*it).second);
        if (ptr) {
            ptr->set(std::forward<T>(value));
            return true;
        }
        return false;
    }

    /**
     * \brief Get value from map by key
     *
     * If key does not exist, a nullptr is returned, otherwise
     * if type specified is wrong, this call will simply return a
     * nullptr value
     * @param key
     * @return
     */
    template<typename T>
    T* get(const key_type& key) const {
        auto res = this->attrs.find(key);
        if (res != attrs.end()) {
            auto ptr = dynamic_cast<details::typed_attribute_value<T>*> ((*res).second);
            if (ptr) {
                return &ptr->get();
            } else {
                return nullptr;
            }
        } else {
            return nullptr;
        }
    }

    template<typename T>
    T* getOrDefault(const key_type& key, T& def) const {
        auto res = get(key);
        if (res) {
            return res;
        } else {
            return &def;
        }
    }

private:
    std::unordered_map<key_type, details::attribute_value*> attrs;

};

SP_UTIL_NAMESPACE_END

#endif /* SP_UTIL_ATTRMAP_HPP */