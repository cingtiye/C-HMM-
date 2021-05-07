#include <cstdlib>
#include <memory>
#include <string>

namespace transchinese{
std::wstring s2ws(const std::string& str) {
  if (str.empty()) {
    return L"";
  }
  unsigned len = str.size() + 1;
  setlocale(LC_CTYPE, "en_US.UTF-8");
  std::unique_ptr<wchar_t[]> p(new wchar_t[len]);
  mbstowcs(p.get(), str.c_str(), len);
  std::wstring w_str(p.get());
  return w_str;
}

std::string ws2s(const std::wstring& w_str) {
    if (w_str.empty()) {
      return "";
    }
    unsigned len = w_str.size() * 4 + 1;
    setlocale(LC_CTYPE, "en_US.UTF-8");
    std::unique_ptr<char[]> p(new char[len]);
    wcstombs(p.get(), w_str.c_str(), len);
    std::string str(p.get());
    return str;
}
}

// int main(){
// 	string input = "今天你在武汉！！";
// 	string temp = "在";

//     wstring _s = s2ws(input);
//     cout<<_s.size()<<endl;
//     cout<<ws2s(_s.substr(0,1))<<endl;  
//     cout<<ws2s(_s)<<endl;
//     CHAR2INDEX[ws2s(_s.substr(0,1))] = 1;
//     cout<<CHAR2INDEX[ws2s(_s.substr(0,1))]<<endl;
//     return 0;
// }