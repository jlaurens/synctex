/**
 * Reader_0.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */


#include <iostream>
#include <memory>

#include <functional>
#include <iostream>

class Multiplier {
public:
    int coefficient = 10;
    int operator()(int x) const {
        return x * coefficient;
    }
};

enum class Status {
    Failed,
    EndOfData,
    Done
};

int callFunction(std::function<int(int)> func, int i) {
    return func(i);
}

class Provider {
public:
    Status operator()(void *ptr, int len, int &read) {
        read = len;
        return Status::Failed;
    }
};

int callProfider(std::function<Status(void *, int, int &)> prvdr, int i) {
    int read = 0;
    auto status = prvdr(nullptr, 2*i+1, read);
    return read;
}

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("POC");

TEST_CASE("[POC]") {
    CHECK(1==1);
}

TEST_CASE("[POC]") {
    Multiplier multiplier;
    CHECK(callFunction(multiplier, 10)==100);
    multiplier.coefficient = 20;
    CHECK(callFunction(multiplier, 10)==200);
}

TEST_CASE("[POC]") {
    Provider p;
    CHECK(callProfider(p, 0)==1);
}

TEST_SUITE_END();


