#include <ecmascript/json/json.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <tuple>
#include <vector>

void help(std::vector<std::string> args)
{
    std::cout << "Usage: " << args[0] << " [command] [arguments]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  json: <file>\n";
    exit(1);
}

void json(std::vector<std::string> args)
{
    auto source = std::string{};
    for (int i = 2; i < args.size(); i++)
    {
        if (args[i] == "-e")
        {
            if (i + 1 == args.size())
                throw std::runtime_error("Missing argument -e");
            source = args[++i];
        }
        else if (args[i] == "-f")
        {
            if (i + 1 == args.size())
                throw std::runtime_error("Missing argument to -f");
            auto file = std::ifstream{};
            file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try
            {
                file.open(args[++i]);
                source = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
            }
            catch (std::system_error &e)
            {
                throw std::runtime_error(e.code().message());
            }
        }
        else
            throw std::runtime_error("Unknown argument");
    }

    auto json = parse_json(source);
    std::cout << json.to_string() << std::endl;
}

int main(int argc, char *argv[])
{
    try
    {
        using namespace std::string_literals;

        auto args = std::vector<std::string>(argv, argv + argc);
        if (argc < 2)
            args.push_back("help");

        auto commands = std::map{std::pair{"help"s, &help}, std::pair{"json"s, &json}};

        if (commands.contains(args[1]))
            commands[args[1]](args);
        else
            throw std::runtime_error("No such command: "s + argv[1]);
    }
    catch (const std::exception &err)
    {
        std::cerr << err.what() << std::endl;
        return -1;
    }
    return 0;
}