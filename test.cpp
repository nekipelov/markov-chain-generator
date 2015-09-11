#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_TextGenerator

#include <string>
#include <list>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include "textgenerator.h"


BOOST_AUTO_TEST_CASE(test_generate)
{
    TextGenerator generator(2);

    generator.learn("hello world test1 test2");

    BOOST_CHECK_EQUAL(generator.prefixSize(), 2);
    BOOST_CHECK_EQUAL(generator.chainSize(), 4);
    BOOST_CHECK_EQUAL(generator.generate("", 1), "HELLO");
    BOOST_CHECK_EQUAL(generator.generate("", 2), "HELLO WORLD");
    BOOST_CHECK_EQUAL(generator.generate("", 3), "HELLO WORLD TEST1");
    BOOST_CHECK_EQUAL(generator.generate("HELLO", 1), "HELLO WORLD");
    BOOST_CHECK_EQUAL(generator.generate("HELLO WORLD", 1), "HELLO WORLD TEST1");
    BOOST_CHECK_EQUAL(generator.generate("WORLD TEST1", 1), "WORLD TEST1 TEST2");
}

BOOST_AUTO_TEST_CASE(test_multi_learn)
{
    TextGenerator generator(3);
    std::list<std::string> variants = {"TEST TEST1 TEST2", "TEST TEST3 TEST4"};

    generator.learn("test test1 test2");
    generator.learn("test test3 test4");

    BOOST_CHECK_EQUAL(generator.prefixSize(), 3);
    BOOST_CHECK_EQUAL(generator.chainSize(), 4);
    BOOST_CHECK(std::find(variants.begin(), variants.end(), generator.generate("test", 2)) !=
            variants.end());
    BOOST_CHECK(std::find(variants.begin(), variants.end(), generator.generate("test", 2)) !=
            variants.end());
}
