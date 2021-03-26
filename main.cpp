#ifdef RELEASE
#pragma GCC optimize(2)
#pragma GCC optimize(3, "Ofast", "inline")
#endif
#include "Agent/Agent.hpp"
#include <bits/stdc++.h>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    auto ai = new Agent;
    ai->Run();
    delete ai;
    return 0;
}