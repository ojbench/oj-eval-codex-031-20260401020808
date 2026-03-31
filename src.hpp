// Implementation of pylist per assignment requirements
#ifndef PYLIST_H
#define PYLIST_H

#include <iostream>
#include <vector>
#include <memory>
#include <unordered_set>
#include <cstddef>

class pylist {
private:
    enum class Kind { INT, LIST };

    Kind kind_;
    int ivalue_ = 0;
    std::shared_ptr<std::vector<pylist>> list_;

    // Ensure this pylist is a list; if currently int, convert to empty list
    void ensure_list() {
        if (kind_ != Kind::LIST) {
            list_ = std::make_shared<std::vector<pylist>>();
            kind_ = Kind::LIST;
        }
    }

    // Printing helper with cycle detection on current recursion stack
    static void print_impl(std::ostream &os, const pylist &ls,
                           std::unordered_set<const void *> &stack) {
        if (ls.kind_ == Kind::INT) {
            os << ls.ivalue_;
            return;
        }

        const void *id = ls.list_.get();
        if (!id) {
            os << "[]";
            return;
        }
        if (stack.find(id) != stack.end()) {
            os << "[...]";
            return;
        }
        stack.insert(id);
        os << '[';
        for (std::size_t i = 0; i < ls.list_->size(); ++i) {
            if (i) os << ", ";
            print_impl(os, (*ls.list_)[i], stack);
        }
        os << ']';
        stack.erase(id);
    }

public:
    // Constructors
    pylist() : kind_(Kind::LIST), list_(std::make_shared<std::vector<pylist>>()) {}
    pylist(int v) : kind_(Kind::INT), ivalue_(v), list_(nullptr) {}

    // Default copy/move semantics: share underlying list via shared_ptr
    pylist(const pylist &) = default;
    pylist(pylist &&) noexcept = default;
    pylist &operator=(const pylist &) = default;
    pylist &operator=(pylist &&) noexcept = default;

    // Assign from int: turn into int kind
    pylist &operator=(int v) {
        kind_ = Kind::INT;
        ivalue_ = v;
        list_.reset();
        return *this;
    }

    // Append operations (O(1))
    void append(const pylist &x) {
        ensure_list();
        list_->push_back(x);
    }
    void append(int v) {
        ensure_list();
        list_->emplace_back(pylist(v));
    }

    // Pop last element and return it (O(1))
    pylist pop() {
        ensure_list();
        if (list_->empty()) {
            // Undefined by spec; return empty list
            return pylist();
        }
        pylist ret = std::move(list_->back());
        list_->pop_back();
        return ret;
    }

    // Indexing (O(1))
    pylist &operator[](std::size_t i) {
        ensure_list();
        return (*list_)[i];
    }
    const pylist &operator[](std::size_t i) const {
        // For completeness if ever used
        return (*list_)[i];
    }

    // Implicit conversion to int for arithmetic and comparisons when holding int
    operator int() const {
        return ivalue_;
    }

    // Output operator
    friend std::ostream &operator<<(std::ostream &os, const pylist &ls) {
        if (ls.kind_ == Kind::INT) {
            os << ls.ivalue_;
            return os;
        }
        std::unordered_set<const void *> stack;
        print_impl(os, ls, stack);
        return os;
    }
};

#endif // PYLIST_H
