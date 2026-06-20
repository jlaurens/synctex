/**
 * Reader_0.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include <ostream>

namespace zoo {
    class Animal {
        friend std::ostream& operator<<(std::ostream& os, const Animal& a) {
            a.print(os);  // virtual dispatch — unaffected by namespaces
            return os;
        }
    protected:
        virtual void print(std::ostream& os) const { os << "ANIMAL"; }

        public:
        virtual ~Animal() = default;
    };

    class Dog : public Animal {
        protected:
            void print(std::ostream& os) const override { os << "DOG"; }  // found at runtime
        };

    // operator<< for shared_ptr<Animal> — same namespace, ADL finds it
    std::ostream& operator<<(std::ostream& os,
                             const std::shared_ptr<Animal>& ptr) {
        if (ptr)
            os << "Ptr to:" << *ptr;          // delegates to Animal's operator
        else
            os << "<null Animal>";
        return os;
    }
    // operator<< for shared_ptr<Animal> — same namespace, ADL finds it
    std::ostream& operator<<(std::ostream& os,
                             const std::shared_ptr<Dog>& ptr) {
        if (ptr)
            os << "Ptr to:" << *ptr;          // delegates to Animal's operator
        else
            os << "<null Dog>";
        return os;
    }
};

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("POC2");

TEST_CASE("[POC]") {
    CHECK(1==1);
}

TEST_CASE("[POC]") {
    zoo::Animal animal;
    zoo::Dog dog;
    std::cout << animal << std::endl;
    std::cout << dog << std::endl;
}

TEST_CASE("[POC*]") {
    auto animal_p = std::shared_ptr<zoo::Animal>( new zoo::Animal());
    auto dog_p = std::shared_ptr<zoo::Dog>( new zoo::Dog());
    std::cout << animal_p << std::endl;
    std::cout << dog_p << std::endl;
}

TEST_SUITE_END();
