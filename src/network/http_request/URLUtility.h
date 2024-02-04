#ifndef URL_UTILITY_H
#define URL_UTILITY_H

class URLUtility {
public:
    static int convertHex(char ch){
    if(ch >= 'A' && ch <= 'F') return ch -'A' + 10;
    if(ch >= 'a' && ch <= 'f') return ch -'a' + 10;
    return ch;
}
};
// int URLUtility::convertHex(char ch)
#endif // URL_UTILITY_H
