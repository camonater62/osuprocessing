#include <iostream>

#include "../osu.hpp"

using namespace std;

int main()
{
    osu::Beatmap beatmap("XXXTENTACION - SAD! (Chromoxx) [Depression].osu");
    cout << beatmap << endl;
    return 0;
}