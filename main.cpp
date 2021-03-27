#ifdef RELEASE
#pragma GCC optimzie(2)
#pragma GCC optimize(3,"Ofast","inline")
#endif
#include <bits/stdc++.h>
#include "Agent/Agent.hpp"

using namespace std;

int main() {
    auto ai = new Agent;
    ai->Run();
    delete ai;
    return 0;
}