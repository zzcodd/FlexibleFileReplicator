/*
 * @Description: 
 * @Author: zy
 * @Date: 2024-07-24 16:58:14
 * @LastEditTime: 2024-07-26 15:16:27
 * @LastEditors: zy
 */
#pragma once 

#include <bits/stdc++.h>

typedef struct {
  int type;
  time_t start_ts;
  std::vector<std::string> ex_from;
  std::vector<std::string> ex_to;
  std::vector<std::string> ix_from;
  std::vector<std::string> ix_to;
  int state;
  long total_size;
  int percent;
} CopyTask;

typedef struct {
  std::string name;
  std::string type;
} DateListItem;

class MyCopy {

public:
// 执行录像还是日志的逻辑 0-video 1-log
  int Handle(int cmd, std::string& src, std::string& dst, std::string dateValue); 

private:

  int RecordDateCopy(std::string& src, std::string& dst, std::string dateValue);
  
  int LogDateCopy(std::string& src, std::string& dst, std::string dateValue);

  int DateCopy(std::string& src, std::string& dst, std::string dateValue, int type); //0-video 1-log

  bool is_copying = false;

  void* CopyHandler(std::string dateValue);

  int RealCopy(int type, int &rc, std::string &usb_path, std::vector<std::string>& name_list);

  void AppendRecordCopyFromPath(std::string xx, bool is_internal, std::vector<std::string> &name_list, bool is_video);
  
  void AppendCopyToPath(std::string xx, bool is_internal, std::string &usb_path);

  bool AppendCopyToPath_COMMON(std::string xx, bool is_internal,std::string &usb_path, std::string media_root);

  void ExecuteCopyCommand(std::string xx, std::string yy);

  int DirDateCopy(std::string& src, std::string& dst);

public:
  static MyCopy* get_instance(){
    return &instance;
  }  

private:
  static MyCopy instance;
  MyCopy(){}
  ~MyCopy(){}
  MyCopy(const MyCopy&) = delete;
  MyCopy operator=(const MyCopy&) =delete;

  CopyTask copy_task;
};
