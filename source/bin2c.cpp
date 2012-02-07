#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

int main( int argc, char** argv )
{
    if( argc != 4 )
    {
        std::cerr
        << "Usage: " << argv[0] << " <binaryfile> <arrayname> <outputfile> \n";
        return(-1);
    }

    //std::cerr << "binaryfile: " << argv[1] << "\n";
    //std::cerr << "arrayname : " << argv[2] << "\n";
    //std::cerr << "outputfile: " << argv[3] << "\n";

    std::fstream    infile( argv[1], std::ios::in|std::ios::binary );

    if( infile.is_open() )
    {
        std::fstream outfile( argv[3], std::ios::out );

        if( outfile.is_open() )
        {
            infile.seekg( 0, std::ios::end );
            size_t length_of_file=infile.tellg();
            infile.seekg( 0, std::ios::beg );

            outfile
            << "// ==========================================================\n"
            << "// auto-generated with bin2c, (C) 2010 S.Fendt, released     \n"
            << "// under LGPL v2.0 or later. See artwork-folder for source!  \n"
            << "// ==========================================================\n"
            << "\n"
            << "unsigned long " << argv[2] << "_size = "
                                << length_of_file << ";\n"
            << "unsigned char " << argv[2] << "[] = \n" //"[" << length_of_file << "] = \n"
            << "{\n";

            int count=0;
            unsigned int value = infile.get();
            while( !infile.eof() )
            {
                outfile << "0x"
                        << std::hex
                        << std::setw(2)
                        << std::setfill('0')
                        << (value&0xff);

                value = infile.get();

                if( !infile.eof() )
                    outfile << ", ";

                if( count == 15 )
                {
                    outfile << "\n";
                }
                count++;
                count %= 16;
            }

            outfile << " };\n";

            outfile.close();
            infile.close();
        }
        else
        {
            std::cerr << "Cannot open outfile. Sorry.\n";
            return( -1 );
        }
    }
    else
    {
        std::cerr << "Cannot open infile. Sorry.\n";
        return( -1 );
    }

    return(0);
}
