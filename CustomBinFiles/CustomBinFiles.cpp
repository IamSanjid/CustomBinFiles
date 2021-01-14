/*
*  (LEN_1B)(Name S)(Name)(Searches)(Search B S)(Size)(Search B)(+++)(Size)(Execution CODE For each Searches)(+++)
*    char   uint32  char  uint32     uint32    uint32 raw bytes      uint32        raw bytes
*/

#include <iostream>
#include <fstream>
#include "bin.h"
#include "buffer.h"

int main(int argc, char *argv[])
{
    if (argc == 3)
    {
        const char* path = argv[2];
        bool read = argv[1] == "r";
        bin* b = {};
        if (read)
        {
            std::cout << "Reading..." << std::endl;
            if (read_bin(path, &b))
            {
                std::cout << "Read from '" << path << std::endl;
                std::cout << "Contents:\n";
                std::cout << "Name: " << b->name << "\n";
                std::cout << "Total Seaches: " << b->se_Len << "\n";
                std::cout << "Total Execution Codes: " << b->ex_c_len << "\n";
                std::cout << "Searches:\n";
                for (int i = 0; i < b->se_Len; i++)
                {
                    std::cout << "\t bytes len: " << b->searches[i].len << "\n";
                }
                std::cout << "Execution Codes:\n";
                for (int i = 0; i < b->ex_c_len; i++)
                {
                    std::cout << "\t bytes len: " << b->ex_codes[i].len << "\n";
                }
                std::cout << std::endl;
                free_bin(b);
            }
        }
        else
        {
            init_bin(&b, 4, "Test");
            b->se_Len = 2;
            b->ex_c_len = 2;
            b->searches = new bin_bytes[2];
            b->ex_codes = new bin_bytes[2];

            unsigned char* bb = new unsigned char[4]{ 84, 69, 83, 84 };
            b->searches[0].bytes = bb;
            b->searches[0].len = 4;

            b->searches[1].bytes = bb;
            b->searches[1].len = 4;

            b->ex_codes[0].bytes = bb;
            b->ex_codes[0].len = 4;

            b->ex_codes[1].bytes = bb;
            b->ex_codes[1].len = 4;

            std::cout << "Writing..." << std::endl;
            write_bin(path, b);
            std::cout << "Wrote to '" << path << "'." << std::endl;

            free(b->name);
            delete[] b->searches;
            delete[] b->ex_codes;
            free(b);
        }
    }
    
}