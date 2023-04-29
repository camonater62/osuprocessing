#include <string>
#include <vector>
#include <fstream>

namespace osu
{
    std::vector<std::string> split(std::string s, std::string delim)
    {
        std::vector<std::string> res;
        size_t pos = 0;
        std::string token;
        while ((pos = s.find(delim)) != std::string::npos)
        {
            token = s.substr(0, pos);
            res.push_back(token);
            s.erase(0, pos + delim.length());
        }
        res.push_back(s);
        return res;
    }

    class HitObject
    {
    public:
        int time;
        bool hit;

        std::string toString() const
        {
            return std::to_string(time) + " " + std::to_string(hit);
        }
    };

    std::ostream &operator<<(std::ostream &os, const HitObject &obj)
    {
        os << obj.toString();
        return os;
    }

    class Song
    {
    public:
        Song::Song(std::string osufile)
        {
            processOsuFile(osufile);
            time = 0.0f;
        }

    private:
        int leadin;
        float time;
        std::vector<HitObject> hitObjects;

    public:
        void Song::Update(float fElapsedTime)
        {
            time += fElapsedTime;
        }

        int Song::TimeMillis() const
        {
            return int(time * 1000.0f);
        }

        int Song::LeadIn() const
        {
            return leadin;
        }

        const std::vector<HitObject> &
        Song::GetHitObjects() const
        {
            return hitObjects;
        }

    private:
        void Song::processOsuFile(std::string osufile)
        {
            std::ifstream osu(osufile);

            std::hash<std::string> hasher;
            srand(hasher(osufile));

            const auto GeneralFunc = [&](std::string line)
            {
                if (line.rfind("AudioLeadIn") == 0)
                {
                    leadin = stoi(line.substr(line.find(':') + 1));
                }
            };

            const auto HitObjFunc = [&](std::string line)
            {
                std::vector<std::string> args = split(line, ",");
                int time = stoi(args[2]) + leadin;
                HitObject obj;
                obj.time = time;
                obj.hit = false;
                hitObjects.push_back(obj);
            };

            std::string line = "";
            std::string section = "";
            while (getline(osu, line))
            {
                if (line.length() < 1)
                    continue;

                if (line[0] == '[')
                {
                    section = line.substr(1, line.find_last_of(']') - 1);
                }
                else if (section == "General")
                {
                    GeneralFunc(line);
                }
                else if (section == "HitObjects")
                {
                    HitObjFunc(line);
                }
            }
        }
    };

    std::ostream &operator<<(std::ostream &os, const Song &song)
    {
        os << "LeadIn: " << song.LeadIn() << std::endl;
        os << "Time: " << song.TimeMillis() << std::endl;
        os << "HitObjects: " << std::endl;
        for (const auto &obj : song.GetHitObjects())
        {
            os << obj << std::endl;
        }
        return os;
    }
}