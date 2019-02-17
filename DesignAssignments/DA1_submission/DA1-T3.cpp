#include <iostream>
using namespace std;

int main(){
int x = 65535 ; // 0xFFFF
int y = 170;  //0xAA
int answer=x*y;

cout<<answer<<endl; //11140950 == 0xA9FF56
return 0;
}
