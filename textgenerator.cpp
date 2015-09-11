#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <deque>

#include <stdint.h>

#include <boost/locale.hpp>
#include <boost/functional/hash.hpp>
#include <boost/crc.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>

#include "utils.h"
#include "textgenerator.h"

namespace
{
// Вспомогательный класс для построения префиксов заданного размера.
class Prefix
{
public:
    explicit Prefix(size_t length_)
        : length(length_)
    {
    }

    Prefix(const Prefix &) = default;

    Prefix(Prefix && other)
        : length(std::move(other.length)), words(std::move(other.words))
    {
    }

    void add(std::string s)
    {
        if( words.size() >= length )
        {
            words.erase(words.begin());
        }

        words.push_back(std::move(s));
    }

    std::string toString() const
    {
        std::string result;

        for(const auto &word: words)
        {
            result += word;
            result += " ";
        }

        return result;
    }

    uint64_t key() const
    {
        // Ключем выступает контрольная сумма слов. Тесты показали, что на наборе книг в 400 Мб
        // crc32 дает колизии, а crc64 нет. Магическое число 0x42f0e1eba9ea3693 взято
        // от сюда: http://reveng.sourceforge.net/crc-catalogue/17plus.htm#crc.cat-bits.64
        boost::crc_optimal<64, 0x42f0e1eba9ea3693, 0, 0, false, false> result;
        const char separator[] = {' '};

        for(const auto &word: words)
        {
            result.process_bytes(word.data(), word.size());
            result.process_bytes(separator, sizeof(separator));
        }

        return  result.checksum();
    }

private:
    size_t length;
    std::list<std::string> words;
};

template<typename Archive, typename Map>
void serialize(Archive &ar, const Map &map)
{
    size_t size = map.size();

    ar << size;

    for(const auto &pair: map)
    {
        ar << pair.first << pair.second;
    }
}

template<typename Archive, typename Map>
void deserialize(Archive &ar, Map &map)
{
    Map result;

    size_t size = 0;
    ar >> size;

    for(size_t i = 0; i != size; ++i)
    {
        typename Map::key_type key;
        typename Map::mapped_type value;

        ar >> key >> value;

        result[std::move(key)] = std::move(value);
    }

    std::swap(result, map);
}
}

class TextGeneratorImpl
{
public:
    TextGeneratorImpl(size_t prefixSize_, const std::string &localeName)
        : prefixSize(prefixSize_), locale(boost::locale::generator()(localeName))
    {
    }

    size_t prefixSize;
    std::locale locale;
    std::string errorString;
    std::unordered_map<uint64_t, std::multiset<std::string>> markovChain;
};


TextGenerator::TextGenerator(size_t prefixSize_, const std::string &localeName)
    : pimpl(new TextGeneratorImpl(prefixSize_, localeName))
{
}

TextGenerator::TextGenerator(TextGenerator &&other)
    : pimpl(std::move(other.pimpl))
{
}

TextGenerator::~TextGenerator()
{
}

void TextGenerator::learn(const std::string &text)
{
    using namespace boost::locale::boundary;

    std::string upperString = boost::locale::to_upper(text, pimpl->locale);

    // Разбиваем текст на слова
    ssegment_index segments(word, upperString.begin(), upperString.end(),
                            word_any, pimpl->locale);

    Prefix prefix(pimpl->prefixSize);

    for(std::string word: segments)
    {
        pimpl->markovChain[prefix.key()].insert(word);
        prefix.add(std::move(word));
    }
}

std::string TextGenerator::generate(const std::string &prefixString, size_t textSize) const
{
    using namespace boost::locale::boundary;

    std::string upperString = boost::locale::to_upper(prefixString, pimpl->locale);

    // Разбиваем текст на слова для того, чтобы получить префикс (ключ).
    ssegment_index segments(word, upperString.begin(), upperString.end(),
                            word_any, pimpl->locale);

    Prefix prefix(pimpl->prefixSize);
    std::string result;

    for(const std::string &word: segments)
    {
        result += word;
        result += ' ';

        prefix.add(word);
    }

    if( textSize == 0 )
    {
        return result;
    }

    size_t count = 0;

    // Проходим по цепи, добавляя слова
    for(auto it = pimpl->markovChain.find(prefix.key());
        it != pimpl->markovChain.end();
        it = pimpl->markovChain.find(prefix.key()))
    {
        size_t index = (rand() % it->second.size());
        auto wordIt = it->second.begin();

        std::advance(wordIt, index);
        const std::string &word = *wordIt;

        result += word;
        result += ' ';

        prefix.add(word);

        if( ++count >= textSize )
        {
            break;
        }
    }

    // Вырежем вставленный пробел
    if( !result.empty() )
    {
        result.resize(result.size() - 1);
    }

    return result;
}

bool TextGenerator::store(const std::string &fileName) const
{
    std::ofstream stream(fileName.c_str());

    pimpl->errorString.clear();

    if( stream )
    {
        try
        {
            boost::archive::binary_oarchive archive(stream);

            archive & pimpl->prefixSize;
            serialize(archive, pimpl->markovChain);

            return true;
        }
        catch(const std::exception &e)
        {
            pimpl->errorString = e.what();

            return false;
        }
    }
    else
    {
        std::stringstream ss;

        ss << "Can't open file '" << fileName << "'";
        pimpl->errorString = systemError(ss.str());

        return false;
    }
}

bool TextGenerator::load(const std::string &fileName)
{
    std::ifstream stream(fileName.c_str());

    pimpl->errorString.clear();

    if( stream )
    {
        try
        {
            boost::archive::binary_iarchive archive(stream);

            archive & pimpl->prefixSize;
            deserialize(archive, pimpl->markovChain);

            return true;
        }
        catch(const std::exception &e)
        {
            pimpl->errorString = e.what();

            return false;
        }
    }
    else
    {
        std::stringstream ss;

        ss << "Can't open file '" << fileName << "'";
        pimpl->errorString = systemError(ss.str());

        return false;
    }
}

std::string TextGenerator::errorString() const
{
    return pimpl->errorString;
}

size_t TextGenerator::prefixSize() const
{
    return pimpl->prefixSize;
}

size_t TextGenerator::chainSize() const
{
    return pimpl->markovChain.size();
}

