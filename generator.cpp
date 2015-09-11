#include <string>
#include <iostream>
#include <sstream>

#include <stdlib.h>

#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "textgenerator.h"

// Часть, отвечающая за генерацию текстов, inputFileName - имя файла, в котором
// сохранены результаты обучения.
static int run(const std::string &inputFileName)
{
    TextGenerator markovChain;

    srand(time(0));

    BOOST_LOG_TRIVIAL(trace) << "Load data from '" << inputFileName << "'";

    if( markovChain.load(inputFileName) )
    {
        BOOST_LOG_TRIVIAL(trace) << "done";

        for (std::string line; std::getline(std::cin, line);)
        {
            if( line.empty() )
            {
                continue;
            }

            std::stringstream ss(line);
            size_t textSize;
            std::string startText;

            ss >> textSize;
            std::getline(ss, startText);

            std::cout << markovChain.generate(startText, textSize) << "\n";
        }

        return EXIT_SUCCESS;
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Fail to load file '" << inputFileName
                                 << "': " << markovChain.errorString();

        return EXIT_FAILURE;

    }
}

int main(int argc, char **argv)
{
    namespace po = boost::program_options;
    std::string inputFileName;

    po::options_description description("Options");
    description.add_options()
        ("help", "produce help message")
        ("input", po::value(&inputFileName), "input file name")
    ;

    po::variables_map vm;

    try
    {
        po::store(po::parse_command_line(argc, argv, description), vm);
        po::notify(vm);
    }
    catch(const po::error &e)
    {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    if( vm.count("help") )
    {
        std::cout << description << "\n";
        return EXIT_SUCCESS;
    }

    else if( vm.count("input") == 0 )
    {
        std::cerr << "missing argument 'input'\n";
        std::cerr << description << "\n";
        return EXIT_FAILURE;
    }

    boost::log::add_console_log(std::cerr);

    return run(inputFileName);
}
