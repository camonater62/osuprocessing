#include <iostream>

#include "../memphis.hpp"
#include "../osu.hpp"

using namespace std;

int main()
{
    osu::Beatmap beatmap(memphis);
    cout << beatmap << endl;
    return 0;
}