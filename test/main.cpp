#include <iostream>

#include "../memphis.hpp"
#include "../osu.hpp"

using namespace std;

int main()
{
    osu::Beatmap beatmap(memphis);
    cout << beatmap << endl;
    for (auto &hitObject : beatmap.HitObjects())
    {
        cout << hitObject << endl;
    }
    return 0;
}