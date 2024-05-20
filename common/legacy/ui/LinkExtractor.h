#pragma once

#include "chr/utils/Utils.h"

namespace chr
{
    struct ExtractedLink
    {
        int offset;
        int length;
        std::string url;
        
        ExtractedLink(int offset, int length, const std::string &url)
        :
        offset(offset),
        length(length),
        url(url)
        {}
    };
    
    class LinkExtractor
    {
    public:
        static std::pair<std::vector<ExtractedLink>, std::u16string> extract(const std::u16string &input)
        {
            std::vector<ExtractedLink> links;
            std::u16string output;
            
            size_t pos = 0;
            
            while (true)
            {
                int pos1 = input.find(u"<a href=\"", pos);
                
                if (pos1 != std::u16string::npos)
                {
                    int pos2 = pos1 + 9;
                    int pos3 = input.find(u"\">", pos2);
                    
                    int pos4 = pos3 + 2;
                    int pos5 = input.find(u"</a>", pos4);
                    
                    output.append(input, pos, pos1 - pos);
                    links.emplace_back(output.size(), pos5 - pos4, utils::to<std::string>(input.substr(pos2, pos3 - pos2)));
                    output.append(input, pos4, pos5 - pos4);
                    
                    pos = pos5 + 4;
                }
                else
                {
                    output.append(input.begin() + pos, input.end());
                    break;
                }
            }
            
            return make_pair(links, output);
        }
    };
}
