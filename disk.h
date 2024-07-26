/*
 * @Description: 
 * @Author: zy
 * @Date: 2024-07-26 09:23:43
 * @LastEditTime: 2024-07-26 13:46:17
 * @LastEditors: zy
 */
#pragma once
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <unistd.h>

bool makeDir(const std::string &sPathName)
{
  bool flag = true;
  if( access(sPathName.c_str(), F_OK) != 0 ) {
    char DirName[256];
    strcpy(DirName, sPathName.c_str());

    int i, len = strlen(DirName);
    if(DirName[len-1]!='/') strcat(DirName, "/");

    len = strlen(DirName);

    for(i=1; i<len; i++) {
      if(flag && DirName[i]=='/') {
        DirName[i] = 0;
        if((access(DirName, F_OK)!=0) && (mkdir(DirName, 0755))==-1) {
          perror ("mkdir error");
          flag = false;
        }
        DirName[i]= '/';
      }
    }
  }
  return flag;
}
//0-r 1-w
int call_cmd(std::string cmd, std::string& out_buffer, int type)
{
  FILE *fp = nullptr;
  char result[1024] = {0};
  int rc = -1;

  if(cmd.length() < 2) return -1;

  if(0 == type) {
    fp = popen(cmd.c_str(), "r");
  } else if (1==type){
    fp = popen(cmd.c_str(), "w");
  } else {
    return -1;
  }

  if(!fp) return -2;

//读数据
  fread(result, 1, 1024, fp);
  rc = strlen(result);
  if(rc > 0) out_buffer = result;

//写数据 暂未实现

  pclose(fp);
  return rc;
}

uint64_t GetFolderSize(std::string path)
{
  std::string cmdString, rtnString;
  
  cmdString = "du --max-depth=0 " + path + " | awk '{print $1}'";

  if(call_cmd(cmdString, rtnString, 0) >= 0) {
    return atol(rtnString.c_str());
  }
  
  return -1;
}