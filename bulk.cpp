#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <ctime>
#include <string>



class IbaseClass
{
public:
    using type_to_handle = struct {
        const std::string s;
        const std::time_t t;
    };

    virtual void handle(const type_to_handle &ht) = 0;
};


class saver : public IbaseClass
{
public:
    void handle(const type_to_handle &ht) override
    {
        static std::string filename = "bulk" + std::to_string(ht.t) + ".log";

        std::fstream fs;
        fs.open (filename, std::fstream::in | std::fstream::out | std::fstream::app);
        fs << ht.s;
        fs.close();
    }
};

class printer : public IbaseClass
{
    void handle(const type_to_handle &ht) override
    {
        std::cout << ht.s << std::endl;
    }
};


class bulk
{
    const size_t bulk_size;
    std::vector<std::string> vs;
    std::list<IbaseClass *> lHandler;
    size_t brace_cnt;
    std::time_t *time_first_chunk;

public:
    bulk(size_t size) : bulk_size(size), time_first_chunk(0)
    {
        vs.reserve(bulk_size);
    }

    void addHandler(IbaseClass &handler)
    {
        lHandler.push_back(&handler);
    }

    void flush(void)
    {
        if (vs.size() == 0)
            return;
        bool first = true;
        std::string s("bulk: ");
        for (const auto &si : vs) {
            if (!first)
                s += ", ";
            else
                first = false;
            s += si;
        }
        std::cout << std::endl;

        for (const auto &h : lHandler){
            IbaseClass::type_to_handle ht = {s, *time_first_chunk};
            h->handle(ht);
        }

        vs.clear();
    }

    bool is_full(void)
    {
        return vs.size() >= bulk_size;
    }
    bool is_empty(void)
    {
        return vs.size() == 0;
    }

    void add(std::string &&s)
    {
        static std::time_t time_now = std::time(0);
        time_first_chunk = &time_now;
        std::cout << "dbg_: time now " << time_now << std::endl;
        vs.push_back(s);
    }

    friend std::istream& operator>>(std::istream&, bulk&);
};

std::istream& operator>>(std::istream& is, bulk& this_)
{
    std::string s;
    std::getline(is, s);

    if (s == "{")
    {
        if (!this_.is_empty() && (this_.brace_cnt == 0))
            this_.flush();
        ++this_.brace_cnt;
        return is;
    }
    else if (s == "}")
    {
        if (this_.brace_cnt > 0)
        {
            --this_.brace_cnt;
            if (this_.brace_cnt == 0)
            {
                this_.flush();
                return is;
            }
        }
    }
    else
        this_.add(std::move(s));

    if (this_.is_full() && !this_.brace_cnt)
    {
        this_.flush();
    }
    return is;
}



int main(int argc, char ** argv)
{
    printer printerHandler;
    saver saverHandler;

    if (argc != 2)
    {
        std::cout << "ERROR: incorrect argument number" << std::endl;
        return -1;
    }

    const size_t j = atoi(argv[1]);
    std::cout << j << std::endl;

    class bulk b{j};
    b.addHandler(printerHandler);
    b.addHandler(saverHandler);

    std::cout << "program time start:" << std::time(0) << std::endl;
    while (1)
    {
        std::cin >> b;
    }

    return 0;
}

