
#include <iostream>
#include <regex>
#include <unordered_map>
#include <stack>
#include <deque>

struct Tag
{
    std::string name;
    std::unordered_map<std::string, std::string> attris;
    std::unordered_map<std::string, Tag*> childs;
};

// tag nodes traversal for checking
static void traverse(const Tag* node, int level)
{
    if (node == nullptr)
        return;

    for (int i=0; i<level; ++i) std::cout << " ";
    std::cout << node->name << std::endl;
    for (const auto& item : node->childs)
    {
        traverse(item.second, level+1);
    }
}

int main()
{
    int n, q;
    std::string line;

    std::cin >> n >> q;
    // ignore the left-over newline character
    std::cin.ignore(1, '\n');

    std::stack<Tag*> stags;

    Tag* root = new Tag();
    root->name = "Dummy Root";

    std::regex rgx_m_tagname("<([a-zA-Z0-9]+)[>| ]{1}.*");
    std::regex rgx_s_attris(R"(([a-zA-Z0-9\_]+) = \"([a-zA-Z0-9\$\,\.\!\%_]+)\")");
    std::regex rgx_m_endtag("</([a-zA-Z0-9]+)>");
    std::smatch matchResult;

    while (n > 0)
    {
        std::getline(std::cin, line);

        if (std::cin.fail())
            break;

        // handle tag
        if (std::regex_match(line, matchResult, rgx_m_tagname))
        {
            Tag* tagnode = new Tag();
            tagnode->name = matchResult[1];

            // connect to parent if any in the stack
            if (stags.size() >= 1)
            {
                Tag* parent = stags.top();
                parent->childs[matchResult[1]] = tagnode;
            }
            // otherwise add to top level; root
            else
            {
                root->childs[matchResult[1]] = tagnode;
            }
            stags.push(tagnode);
        
            // handle attributes of tag
            auto searchBegin = line.cbegin();
            while (std::regex_search(searchBegin, line.cend(), matchResult, rgx_s_attris))
            {
                searchBegin = matchResult.suffix().first;
                tagnode->attris[matchResult[1]] = matchResult[2];
            }
        }
        else if (std::regex_match(line, matchResult, rgx_m_endtag))
        {
            stags.pop();
        }

        --n;
    }

    // for debugging to check hierarchy of Tag nodes at this point
#if 0
    traverse(root, 0);
    return 0;
#endif

    std::regex rgx_s_tagchain(R"(([a-zA-Z0-9]+)\.)");
    std::regex rgx_s_attrib(R"(([a-zA-Z0-9]+)~([a-zA-Z0-9_]+)$)");

    while (q > 0)
    {
        std::getline(std::cin, line);
        if (std::cin.fail())
            break;

        Tag* tagnode = root;
        std::deque<std::string> qchain;

        auto searchBegin = line.cbegin();
        while (std::regex_search(searchBegin, line.cend(), matchResult, rgx_s_tagchain))
        {
            searchBegin = matchResult.suffix().first;

            qchain.push_back(matchResult[1]);
        }
        if (std::regex_search(searchBegin, line.cend(), matchResult, rgx_s_attrib))
        {
            qchain.push_back(matchResult[1]);
            qchain.push_back(matchResult[2]);
        }
        int qsize = qchain.size();
        bool clean=true;
        for (int i=0; i<qsize-1; ++i)
        {
            auto tagname = qchain.front();
            qchain.pop_front();

            auto search = tagnode->childs.find(tagname);
            if (search != tagnode->childs.end())
                // FOUND tag
                tagnode = tagnode->childs[tagname];
            else
            {
                // NOT FOUND
                std::cout << "Not Found!\n";
                clean = false;
                break;
            }
        }
        if (clean)
        {
            auto attribname = qchain.front();
            qchain.pop_front();
            auto search = tagnode->attris.find(attribname);
            if (search != tagnode->attris.end())
                // FOUND attrib
                std::cout << tagnode->attris[attribname] << '\n';
            else
                std::cout << "Not Found!\n";
        }

        --q;
    }

    std::cout.flush();
    
    return 0;
}
