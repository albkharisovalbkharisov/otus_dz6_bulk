#include <iostream>
#include <vector>
#include <list>

class IbaseClass
{
public:
    virtual void handle(const std::string &s) = 0;
};


class saver : public IbaseClass
{
    public:
    void handle(const std::string &s) override
    {
        std::cout << "tipa save " << s << std::endl;
    }
};

class printer : public IbaseClass
{
    void handle(const std::string &s) override
    {
        std::cout << s << std::endl;
    }
};


class bulk
{
    const size_t bulk_size;
    std::vector<std::string> vs;
    std::list<IbaseClass *> lHandler;
    size_t brace_cnt;

public:
    bulk(size_t size) : bulk_size(size)
    {
        vs.reserve(bulk_size);
    }

    void addHandler(IbaseClass &handler)
    {
        lHandler.push_back(&handler);
    }

    void flush(void)
    {
        std::string s("bulk:");
        for (const auto &si : vs)
            s += " " + si;
        std::cout << std::endl;

        for (const auto &h : lHandler)
            h->handle(s);

        vs.clear();
    }

    bool is_full(void)
    {
        return vs.size() >= bulk_size;
    }

    void add(std::string &&s)
    {
        vs.push_back(s);
    }

    friend std::istream& operator>>(std::istream&, bulk&);
};

std::istream& operator>>(std::istream& is, bulk& this_)
{
    std::string s;
    is << s;

    if (s == "{")
    {
        ++brace_cnt;
        b.flush();
        return is;
    }
    else if (s == "}")
    {
        if (brace_cnt > 0)
        {
            --brace_cnt;
            if (brace_cnt == 0)
            {
                b.flush();
                return is;
            }
        }
    }
    else
        b.add(std::move(s));

    if (b.is_full() && !brace_cnt)
    {
        b.flush();
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

    int brace_cnt = 0;
    while (1)
    {
        std::string s = "";
        std::cin >> b;
    }

    return 0;
}

