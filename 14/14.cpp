// https://adventofcode.com/2019/day/14

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <assert.h>

//#define VERBOSE

struct Chemical
{
    int64_t amount;
    std::string name;
};

struct Reaction
{
    std::vector<Chemical> inputs;
    Chemical output;
};

using Reactions = std::vector<Reaction>;
using Stock = std::unordered_map<std::string, int64_t>;

Chemical ReadChemical(std::istream& input)
{
    Chemical chemical = {};
    input >> chemical.amount >> chemical.name;
    return chemical;
}

void ReadInput(Reactions& output, std::istream& input)
{
    std::string line;
    while (std::getline(input, line))
    {
        size_t splitpos = line.find(" => ");
        if (splitpos == std::string::npos)
        {
            continue;
        }
        std::string line_inputs = line.substr(0, splitpos);
        std::string line_output = line.substr(splitpos + 4);

        output.emplace_back();
        Reaction& reaction = output.back();

        // get reaction inputs
        std::istringstream line_inputs_stream(line_inputs);
        std::string line_input;
        while (std::getline(line_inputs_stream, line_input, ','))
        {
            std::istringstream line_input_stream(line_input);
            reaction.inputs.push_back(ReadChemical(line_input_stream));
        }

        // get reaction output
        std::istringstream line_outputs_stream(line_output);
        reaction.output = ReadChemical(line_outputs_stream);
    }
}

void AddRequirement(Stock& inventory, const Chemical& chemical, const Reactions& reactions)
{
    const int64_t need = chemical.amount - inventory[chemical.name];
    if (need > 0)
    {
#ifdef VERBOSE
        std::cout << "need " << need << " more " << chemical.name << " (need " << chemical.amount << ", have " << inventory[chemical.name] << ")" << std::endl;
#endif

        // look for a reaction with the chemical as output
        Reactions::const_iterator iter = std::find_if(reactions.begin(), reactions.end(), [&chemical](const Reaction& r) { return r.output.name == chemical.name; });
        if (iter != reactions.end())
        {
            const Reaction& reaction = *iter;

            // how many times to run the reaction
            const int64_t repeat = (need + reaction.output.amount - 1) / reaction.output.amount;

            // how much the reaction will make
            const int64_t make = reaction.output.amount * repeat;

#ifdef VERBOSE
            // list input requirements
            std::cout << "make " << repeat << " x " << reaction.output.amount << " = " << make << " " << chemical.name << std::endl;
            for (const Chemical& input : reaction.inputs)
            {
                std::cout << "\trequire " << repeat << " x " << input.amount << " = " << input.amount * repeat << " " << input.name << std::endl;
            }
#endif

            // store leftover output in inventory
            const int64_t store = make - need;
#ifdef VERBOSE
            if (store > 0)
            {
                std::cout << "store " << store << " " << chemical.name << std::endl;
            }
#endif
            inventory[chemical.name] = store;

            // add input requirements
            for (const Chemical& input : reaction.inputs)
            {
                AddRequirement(inventory, { input.amount * repeat, input.name }, reactions);
            }
        }
        else
        {
            // no reaction makes the chemical so assume it's a source ("ORE")
            // and we can "make" as much as we need
#ifdef VERBOSE
            std::cout << "collect " << need << " " << chemical.name << std::endl;
#endif
            inventory[chemical.name] -= chemical.amount;
        }
    }
    else
    {
        // use from inventory
#ifdef VERBOSE
        std::cout << "use " << chemical.amount << " " << chemical.name << std::endl;
#endif
        inventory[chemical.name] -= chemical.amount;
    }
}

void Part1(const Reactions& reactions)
{
    std::unordered_map<std::string, int64_t> inventory;
    AddRequirement(inventory, { 1, "FUEL" }, reactions);

    std::cout << "Part 1: ore " << -inventory["ORE"] << std::endl;

#ifdef VERBOSE
    std::cout << "leftover inventory:" << std::endl;
    for (const std::pair<std::string, int64_t>& item : inventory)
    {
        if (item.second > 0)
        {
            std::cout << "\t" << item.second << " " << item.first << std::endl;
        }
    }
#endif
}

void Part2(const Reactions& reactions)
{
    constexpr int64_t starting_ore = 1000000000000;

    // binary search
    int64_t range_low = 1, range_high = starting_ore;
    while (range_low <= range_high)
    {
        std::unordered_map<std::string, int64_t> inventory;
        inventory["ORE"] = starting_ore;
        int64_t fuel = (range_low + range_high) / 2;
        AddRequirement(inventory, { fuel, "FUEL" }, reactions);

        if (inventory["ORE"] < 0)
        {
#ifdef VERBOSE
            std::cout << fuel << " FAIL" << std::endl;
#endif
            range_high = fuel - 1;
        }
        else
        {
#ifdef VERBOSE
            std::cout << fuel << " PASS" << std::endl;
#endif
            range_low = fuel + 1;
        }
    }

    std::cout << "Part 2: fuel " << range_high;

#ifdef VERBOSE
    std::cout << "leftover inventory:" << std::endl;
    for (const std::pair<std::string, int64_t>& item : inventory)
    {
        if (item.second > 0)
        {
            std::cout << "\t" << item.second << " " << item.first << std::endl;
        }
    }
#endif
}

int main()
{
    Reactions reactions;
    ReadInput(reactions, std::cin);

    Part1(reactions);
    Part2(reactions);

    return 0;
}
