#pragma once

#include "common/quad/QuadBatch.h"

#include <map>
#include <memory>

namespace chr
{
    namespace gl
    {
        template<typename T> class QuadBatchMap
        {
        public:
            std::map<T*, std::unique_ptr<QuadBatch>> map;
            
            QuadBatchMap() = default;
            QuadBatchMap(const QuadBatchMap &other) = delete; // QuadBatchMap CAN'T BE COPIED (I.E. BECAUSE OF THE map OF unique_ptr)
            
            void clear()
            {
                for (auto &it : map)
                {
                    it.second->clear();
                }
            }
            
            void pack()
            {
                for (auto it = map.begin(); it != map.end();)
                {
                    if (it->second->empty())
                    {
                        it = map.erase(it);
                    }
                    else
                    {
                        it->second->pack();
                        ++it;
                    }
                }
            }

            QuadBatch* getBatch(T *texture)
            {
                auto it = map.find(texture);
                
                if (it == map.end())
                {
                    auto batch = new QuadBatch;
                    map.emplace(texture, std::unique_ptr<QuadBatch>(batch));
                    
                    return batch;
                }
                else
                {
                    return it->second.get();
                }
            }
            
            void flush(const ShaderProgram &shader, Buffer<GLushort> &indexBuffer, bool useColor = false) const
            {
                for (auto &it : map)
                {
                    it.first->bind(); // RELOADS THE TEXTURE, IF NECESSARY
                    it.second->flush(shader, indexBuffer, useColor);
                    it.first->unbind();
                }
            }
        };
    }
}
