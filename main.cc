/*
 * @Description: 
 * @Author: zy
 * @Date: 2024-07-24 16:49:05
 * @LastEditTime: 2024-07-26 15:23:58
 * @LastEditors: zy
 */

#include <bits/stdc++.h>

#include "log.h"
#include "copy.h"

#define MODULE_NAME "copy_file"
#define LOG_ROOT "/home/cidi/vscodeWorkspace/temp/copy_file/log/"


int main() {

  CLogger::Get().Init(LOG_ROOT, MODULE_NAME);

  std::cout << "1-video 2-log 3-Dir enter:" ;
  int index = 0;
  std::cin >> index;

  std::string src, dst, dateValue;
  src = dst = dateValue = "";
  while(index){
    std::cout << "src: " ;
    std::cin >> src;
    std::cout << "dst: " ;
    std::cin >> dst;
    std::cout << "dateValue: " ;
    std::cin >> dateValue;
    MyCopy::get_instance()->Handle(index, src, dst, dateValue);
  }

  return 0;
}