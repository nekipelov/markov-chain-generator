#include <memory>
#include <vector>
#include <iostream>

#include <boost/noncopyable.hpp>
#include <boost/format.hpp>

#include <stdio.h>

#include "utils.h"
#include "downloader.h"

namespace
{

// Обертка над FILE, чтобы обеспечить RAII
class FileWrapper : boost::noncopyable
{
public:
    explicit FileWrapper(FILE *fp)
        : m_fp(fp)
    {
    }

    ~FileWrapper()
    {
        close();
    }

    FILE *get()
    {
        return m_fp;
    }

    int close()
    {
        int status = 0;

        if( m_fp )
        {
            status = pclose(m_fp);
            m_fp = nullptr;
        }

        return status;
    }

private:
    FILE *m_fp;
};

}

bool Downloader::download(const std::string &url, std::string &content)
{
    std::string commandLine = "curl --fail --silent --show-error \"";

    m_error = "no error";

    commandLine += url;
    commandLine += "\" 2>&1";

    // Запускаем
    FileWrapper fp(popen(commandLine.c_str(), "r"));

    if( !fp.get() )
    {
        m_error = systemError("popen fail");
        return false;
    }

    // Читаем вывод
    std::string tmpContent;

    while( !feof(fp.get()) )
    {
        char buf[1024];
        size_t bytes = fread(buf, sizeof(*buf), sizeof(buf), fp.get());

        tmpContent.insert(tmpContent.end(), buf, buf + bytes);
    }

    // Анализируем код возврата.
    int statusCode = fp.close();
    statusCode = WEXITSTATUS(statusCode);

    if( statusCode == EXIT_SUCCESS )
    {
        std::swap(tmpContent, content);
        return true;
    }
    else
    {
        m_error = str(boost::format("Abnormal exit code %1%. Message: %2%")
                      % statusCode % tmpContent);
        return false;
    }
}

std::string Downloader::errorString() const
{
    return m_error;
}
