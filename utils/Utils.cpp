//
// Created by luckybug on 22.03.15.
//

#include <boost/uuid/sha1.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include "Utils.h"


namespace Utils{
    namespace Crypt{
        std::vector<unsigned char> calcSHA1(const std::string & str)
        {
            boost::uuids::detail::sha1 sha1;

            sha1.process_bytes(str.c_str(), str.size());

            unsigned int digest[5];
            sha1.get_digest(digest);

            unsigned char hash[20];
            for(int i = 0; i < 5; ++i)
            {
                const unsigned char* tmp = reinterpret_cast<unsigned char*>(digest);
                hash[i*4] = tmp[i*4+3];
                hash[i*4+1] = tmp[i*4+2];
                hash[i*4+2] = tmp[i*4+1];
                hash[i*4+3] = tmp[i*4];
            }

            return std::vector<unsigned char>(hash, hash+20);
        }

        std::vector<unsigned char> encryptBase64(const void * data, size_t size)
        {
            using namespace boost::archive::iterators;
            typedef insert_linebreaks<base64_from_binary<transform_width<const unsigned char *,6,8>>, 72> base64_text;

            std::vector<unsigned char> res;
            std::copy(
                    base64_text((char*)data),
                    base64_text((char*)data + size),
                    std::back_inserter(res)
            );
            res.push_back('=');

            return res;
        }
    }// namespace Crypt
}// namespace Utils