#include <string>
#include <iostream>

#include <stdlib.h>

#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "textgenerator.h"

// Часть, отвечающая за обучение, outputFileName - имя файла, в который будут
// сохранены данные для генерации текстов, prefixSize - порядок цепи.
static int run(const std::string &outputFileName, size_t prefixSize)
{
    TextGenerator markovChain(prefixSize);


    {
        // Читаем со стандартного входа
        std::cin >> std::noskipws;

        std::istream_iterator<char> it(std::cin);
        std::istream_iterator<char> end;
        std::string content(it, end);

        // И обучаем
        markovChain.learn(content);
    }

    BOOST_LOG_TRIVIAL(trace) << "Save data to '" << outputFileName << "'";
    if( !markovChain.store(outputFileName) )
    {
        BOOST_LOG_TRIVIAL(error) << "Fail to save file '" << outputFileName
                                 << "': " << markovChain.errorString();

        return EXIT_FAILURE;
    }
    else
    {
        BOOST_LOG_TRIVIAL(trace) << "Saved.";
        return EXIT_SUCCESS;
    }
}

int main(int argc, char **argv)
{
    namespace po = boost::program_options;
    std::string outputFileName;
    size_t prefixSize = 0;

    po::options_description description("Options");
    description.add_options()
        ("help", "produce help message")
        ("prefix-size", po::value(&prefixSize), "size of prefix")
        ("output", po::value(&outputFileName), "output file name")
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
    else if( vm.count("output") == 0 )
    {
        std::cerr << "missing argument 'output'\n";
        std::cerr << description << "\n";
        return EXIT_FAILURE;
    }
    else if( vm.count("prefix-size") == 0 )
    {
        std::cerr << "missing argument 'prefix-size'\n";
        std::cerr << description << "\n";
        return EXIT_FAILURE;
    }
    else if( prefixSize == 0 )
    {
        std::cerr << "prefix-length must be > 0\n";
        return EXIT_FAILURE;
    }

    boost::log::add_console_log(std::cerr);

    return run(outputFileName, prefixSize);
}
