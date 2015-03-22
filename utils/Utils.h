//
// Created by luckybug on 22.03.15.
//

#ifndef _WEBSOCKETSERVER_UTILS_H_
#define _WEBSOCKETSERVER_UTILS_H_

#include <vector>
#include <string>

namespace Utils{
    namespace Crypt{
        std::vector<unsigned char> calcSHA1(const std::string & str);
        std::vector<unsigned char> encryptBase64(const void * data, size_t size);
    }// namespace Crypt
}// namespace Utils


#endif //_WEBSOCKETSERVER_UTILS_H_
