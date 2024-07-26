/*
 * @Description: 
 * @Author: zy
 * @Date: 2024-07-24 17:13:52
 * @LastEditTime: 2024-07-26 15:31:53
 * @LastEditors: zy
 */
#include "copy.h"
#include "log.h"
#include "disk.h"
#include <dirent.h>

#include <bits/stdc++.h>

#define COPY_ROOT_NAME "zycopy"

//本地存储
#define LOCAL_LOG_PATH ""
#define LOCAL_RECORD_PATH "/tmp/record"

#define MY_COPY_ROOT_HEAD "/tmp/record"
// usb挂载点
#define USB_PATH "/tmp/record_dest"

MyCopy MyCopy::instance;

int MyCopy::Handle(int cmd, std::string& src, std::string& dst, std::string dateValue){
  int rc = -1;
  switch(cmd){
    case 1:
      rc = RecordDateCopy(src, dst, dateValue);
      break;
    case 2:
      rc = LogDateCopy(src, dst, dateValue);
      break;
    case 3:
      rc = DirDateCopy(dst, src);
    default:
      break;
  }
  return rc;
}

int MyCopy::RecordDateCopy(std::string& src, std::string& dst, std::string dateValue)
{
  AINFO << __func__ << " enter" << std::endl;
  return DateCopy(src, dst, dateValue, 0);
}

int MyCopy::LogDateCopy(std::string& src, std::string& dst, std::string dateValue)
{
  AINFO << __func__ << " enter" << std::endl;
  return DateCopy(src, dst, dateValue, 1);
}

int MyCopy::DateCopy(std::string& src, std::string& dst, std::string dateValue, int type)
{
  int ret = 0;
  AINFO << __func__ << " enter" << std::endl;
  if (is_copying) {
    AINFO << " is copying\n";
    ret = 0;
    std::cout << "上一个拷贝任务正在进行中" << std::endl;
  }
  else {  
    is_copying = true;
    copy_task.type = type;
    copy_task.ix_from.clear();
    copy_task.ix_to.clear();
    copy_task.ex_from.clear();
    copy_task.ex_to.clear();
    
    std::thread copyThread(&MyCopy::CopyHandler,this, dateValue);
    copyThread.detach();

    ret = 0;
    AINFO << "任务添加成功，开始拷贝" ;
    std::cout <<  "任务添加成功，开始拷贝" << std::endl;;
    }  

  return ret;
}



void* MyCopy::CopyHandler(std::string dateValue)
{
  AINFO << __func__ << " enter\n";
  int rc = 1;
  int type = copy_task.type;
  copy_task.state = 0;

  size_t size = dateValue.length() + 1;
  char value[100] ;  
  memcpy(value, dateValue.c_str(), size);
  
  std::vector<std::string> name_list;
  char* token = strtok(value,"|");

  while(token != nullptr) {
    name_list.push_back(token);
    token = strtok(nullptr, "|");
  }
  
  std::string usb_path = USB_PATH;

  std::string root_path = usb_path + "/" + COPY_ROOT_NAME;

  bool flag = true;

  if(access(root_path.c_str(), F_OK) != 0) {
    if(makeDir(root_path)) flag = true;
    else flag = false;
  }

  if(flag) 
    rc = RealCopy(type, rc, usb_path, name_list);
  else
    rc = 2;

  copy_task.state = rc;
  is_copying = false;
  AINFO << __func__ << " exit state " << rc;
  return NULL;
}

int MyCopy::RealCopy(int type, int &rc, std::string &usb_path, std::vector<std::string>& name_list)
{
  system("touch /tmp/copying_file");
  copy_task.percent = 0;
  copy_task.total_size = 0L;
  copy_task.start_ts = time(NULL);

  std::string ex_camera, ex_log, ix_bag, ix_log;
  ex_camera = ex_log = ix_bag = ix_log = "";

  std::string path = "";
  long size, free_s;
  size = free_s = 0L;
  
  bool usrdisk_rec = false;
  bool rec;

  //record
  if(0 == type) {

    path = LOCAL_RECORD_PATH ;

    AppendRecordCopyFromPath(path + "/camera/full", false, name_list, true);

  }
  //log
  else if(1 == type) {

  }

  for(int i =0;i<copy_task.ex_from.size();i++){
    AppendCopyToPath(copy_task.ex_from[i], false, usb_path);
  }

#if 1
  for (int i = 0; i < copy_task.ex_from.size(); i++)
    AINFO << "ex_from: i " << i << " v " <<copy_task.ex_from[i]<<"\n";
  for (int i = 0; i < copy_task.ex_to.size(); i++)
    AINFO << "ex_to: i " << i << " v " << copy_task.ex_to[i]<<"\n";
  for (int i = 0; i < copy_task.ix_from.size(); i++)
    AINFO << "ix_from: i " << i << " v " <<copy_task.ix_from[i]<<"\n";
  for (int i = 0; i < copy_task.ix_to.size(); i++)
    AINFO << "ix_to: i " << i << " v " << copy_task.ix_to[i]<<"\n";
#endif

  if (copy_task.ix_from.size() < 1 && copy_task.ex_from.size() < 1) {
    copy_task.state = rc = 4;
  }

  AINFO << "Copying...";
  int percent = 0;
  unsigned int exSize = copy_task.ex_to.size();
  unsigned int ixSize = copy_task.ix_to.size();

  for(int i =0; i<exSize; i++) {
    if (0 == type ) {
      if (copy_task.ex_from[i].find(".avi", 48) == std::string::npos && 
          copy_task.ex_from[i].find(".mp4", 48) == std::string::npos) {
        continue;
      }
    }


    ExecuteCopyCommand(copy_task.ex_to[i], copy_task.ex_from[i]);
    
    percent = (i + 1) * 100 / (16 * ixSize + exSize);
    if(percent >= 100)
      copy_task.percent = 99;
    else
      copy_task.percent = percent;
    
    if(copy_task.ex_to.size() <= i+1 || copy_task.ex_to[i] != copy_task.ex_to[i+1]) {
          //AINFO
          AINFO << "-------------test-------------" ;
          std::string cmdString = "sync ";
          std::string rtnString;
          cmdString += copy_task.ex_to[i];
          
          call_cmd(cmdString.data(), rtnString, 1);
        }
  }


  AINFO << "Copy and sync done, code " << rc;
  copy_task.percent = 100;
  remove("/tmp/copying_file");
  return rc;

}

void MyCopy::AppendRecordCopyFromPath(std::string xx, bool is_internal, std::vector<std::string> &name_list, bool is_video)
{
  if (2 != name_list.size()) return;

  // 20230718212500|20230718212700
  std::string start_time = name_list[0];
  std::string end_time = name_list[1];
  if (start_time.length() < 11) return;
  std::string date_str = start_time.substr(0, 8);
  int st_day = std::stoi(start_time.substr(6, 2));
  int st_hour = std::stoi(start_time.substr(8, 2));
  int st_min = std::stoi(start_time.substr(10, 2));
  int et_day = std::stoi(end_time.substr(6, 2));
  int et_hour = std::stoi(end_time.substr(8, 2));
  int et_min = std::stoi(end_time.substr(10, 2));
  int st_count = st_hour * 60 + st_min;
  int et_count = et_hour * 60 + et_min;

  std::vector<std::string> path_list;
  path_list.clear();

  std::string data_path = xx + "/" + date_str + "/";
  if (is_video) {
    data_path = xx + "/" + date_str + "/6mm/";
    path_list.push_back(data_path);
    data_path = xx + "/" + date_str + "/.res/";
    path_list.push_back(data_path);
  }
  else {
    path_list.push_back(data_path);
  }

  for(int i = 0; i < path_list.size(); i++) {
    struct dirent **namelist = NULL;
    char buf[32] = {0};

    int n = scandir(path_list[i].c_str(), &namelist, NULL, alphasort);
    if (n < 0) {
      perror("scandir");
      AERROR << __func__ << " scandir() " << path_list[i];
      continue;
    }
    else {
      while (n--) {
        //printf("%s\n", namelist[n]->d_name);
        if ('.' == namelist[n]->d_name[0]) continue;

        // bag have over day, so check day
        memset(buf, 0x0, sizeof(buf));
        if (is_video)
          strncpy(buf, namelist[n]->d_name + 6, 2);
        else
          strncpy(buf, namelist[n]->d_name + 15, 2);
        int cur_day = atoi(buf);
        if (cur_day < st_day | cur_day > et_day) continue;

        // hour
        memset(buf, 0x0, sizeof(buf));
        if (is_video)
          strncpy(buf, namelist[n]->d_name + 8, 2);
        else
          strncpy(buf, namelist[n]->d_name + 18, 2);
        int cur_hour = atoi(buf);
        // minute
        memset(buf, 0x0, sizeof(buf));
        if (is_video)
          strncpy(buf, namelist[n]->d_name + 10, 2);
        else
          strncpy(buf, namelist[n]->d_name + 21, 2);
        int cur_min = atoi(buf);
        int cur_count = cur_hour * 60 + cur_min;

        if (cur_count < st_count | cur_count > et_count) continue;

        std::string full_path = path_list[i] + namelist[n]->d_name;
        // AINFO << "path_list: " << path_list[i];
        // AINFO << "full_path: " << full_path;

        if (is_internal) copy_task.ix_from.push_back(full_path);
        else copy_task.ex_from.push_back(full_path);
        
        copy_task.total_size += GetFolderSize(full_path);

        free(namelist[n]);
      }
      free(namelist);
    }
  }

}

void MyCopy::AppendCopyToPath(std::string xx, bool is_internal,
    std::string &usb_path)
{
  AppendCopyToPath_COMMON(xx, is_internal, usb_path,MY_COPY_ROOT_HEAD);
}

//xx /tmp/record/camera/20221216/6mm/20221216074845221.
bool MyCopy::AppendCopyToPath_COMMON(std::string xx, bool is_internal,
    std::string &usb_path, std::string media_root)
{
  bool ret = false;
  int pos = xx.find(media_root);
  if (pos >= 0) {
    std::string parted_path;
    parted_path = xx.substr(pos + strlen(media_root.data()));

    if ('/' == parted_path.back()) parted_path.erase(parted_path.back());
    pos = parted_path.find_last_of("/");
    if (pos) {
      parted_path = parted_path.substr(0, pos);
  //  record_nvme/camera/full/20221216/6mm
      std::string root_name = "userdisk/";
      if (is_internal) root_name = "internaldisk/";

      std::string dst_path = usb_path + "/" + COPY_ROOT_NAME + "/" +
        root_name + parted_path;

      if (is_internal) copy_task.ix_to.push_back(dst_path);
      else copy_task.ex_to.push_back(dst_path);
      ret = true;
    }
  }
  return ret;
}

//dst src
void MyCopy::ExecuteCopyCommand(std::string xx, std::string yy)
{
  bool is_allow = false;
  if (access(xx.c_str(), F_OK) != 0)
  is_allow = makeDir(xx.c_str());
  else is_allow = true;
  if (is_allow) {
    std::string rtnString;
    std::string cmd = "rsync -a " + yy + " " + xx;
    AINFO << __func__ << " will execute cmd: " << cmd;
    call_cmd(cmd, rtnString, 1);
  }
}

int MyCopy::DirDateCopy(std::string& src, std::string& dst)
{
  std::cout <<  "开始拷贝目录文件" << std::endl;;

  AINFO << __func__ << " enter" << std::endl;
  ExecuteCopyCommand(src, dst);
  return 1;
}