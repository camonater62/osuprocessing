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

    enum class HitObjectType
    {
        CIRCLE = 0,
        SLIDER = 1,
        SPINNER = 3
    };

    class HitObject
    {
    public:
        HitObject(HitObjectType type, int time, int length)
        {
            this->type = type;
            this->time = time;
            this->length = length;
        }

    private:
        HitObjectType type;
        int time;
        int length;

    public:
        HitObjectType HitObject::Type() const
        {
            return type;
        }

        int HitObject::Time() const
        {
            return time;
        }

        int HitObject::Length() const
        {
            return length;
        }

        std::string toString() const
        {
            std::string type = "";
            switch (this->type)
            {
            case HitObjectType::CIRCLE:
                type = "CIRCLE";
                break;
            case HitObjectType::SLIDER:
                type = "SLIDER";
                break;
            case HitObjectType::SPINNER:
                type = "SPINNER";
                break;
            }

            return "Type: " + type + ", Time: " + std::to_string(time) + ", Length: " + std::to_string(length);
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
            leadin = 0;
            processOsuFile(osufile);
        }

    private:
        int leadin;
        std::vector<HitObject> hitObjects;

    public:
        int Song::LeadIn() const
        {
            return leadin;
        }

        int Song::EndTime() const
        {
            const HitObject &last = hitObjects.back();
            return last.Time() + last.Length();
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

            const auto GeneralFunc = [&](std::string line)
            {
                if (line.rfind("AudioLeadIn") == 0)
                {
                    leadin = stoi(line.substr(line.find(':') + 1));
                }
            };

            float sliderMultiplier = 1.0f;
            const auto DifficultyFunc = [&](std::string line)
            {
                if (line.rfind("SliderMultiplier") == 0)
                {
                    sliderMultiplier = stof(line.substr(line.find(':') + 1));
                }
            };

            float sliderVelocity = 1.0f;
            float beatLength = 1.0f;
            const auto TimingPointFunc = [&](std::string line)
            {
                std::vector<std::string> args = split(line, ",");

                float bl = stof(args[1]);
                int uninherited = stoi(args[6]);

                if (uninherited == 0)
                {
                    sliderVelocity = -100.0f / beatLength;
                }
                else
                {
                    beatLength = bl;
                }
            };

            const auto HitObjFunc = [&](std::string line)
            {
                std::vector<std::string> args = split(line, ",");

                int x = stoi(args[0]);
                int y = stoi(args[1]);

                int time = stoi(args[2]) + leadin;
                int noteLength = 0;

                int type = stoi(args[3]);
                HitObjectType hitType = HitObjectType::CIRCLE;
                if (type & 0b00000010)
                {
                    hitType = HitObjectType::SLIDER;
                    int length = stoi(args[7]);
                    int slides = stoi(args[6]);
                    noteLength = length / (sliderMultiplier * 100 * sliderVelocity) * beatLength * slides;
                }
                else if (type & 0b00001000)
                {
                    hitType = HitObjectType::SPINNER;
                    noteLength = stoi(args[5]) - time;
                }

                hitObjects.emplace_back(hitType, time, noteLength);
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
        os << "HitObjects: " << std::endl;
        for (const auto &obj : song.GetHitObjects())
        {
            os << obj << std::endl;
        }
        return os;
    }
}