#include <iostream>
#include <stdio.h>

using std::string;
using std::cout;
using std::endl;

static string LogPath = "../../log/20220809";

int main() {
    char sst[] = "abcabc";

    cout << "hello logger" << endl;
    FILE *logger;
    logger = fopen(LogPath.c_str(), "a");
    FILE *fp;
    char str[] = "This is runoob.com";
    fp = fopen( "file.txt" , "w" );
    fwrite(str, sizeof(str), 1, fp );
    fclose(fp);
    return 0;
}

