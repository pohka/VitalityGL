#include "meta.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cctype>
#include <oak/core/resources.h>

using namespace oak;

MetaDataList Meta::globalMetaList;
MetaData Meta::config;

void Meta::loadConfig()
{
  //parse config.m
  std::ifstream ifs("../config.m");
  std::string content(
    (std::istreambuf_iterator<char>(ifs)),
    (std::istreambuf_iterator<char>())
  );
  MetaDataList list;
  parseContent("config.m", content, list);
  list.findMetaData("config")->copy(config);
}

void Meta::load()
{
  loadConfig();

  
  std::cout << "----META----" << std::endl;

  std::string fullPath, path;

  std::string projectName = VarString::toString(config.getKV("project"));

  if (projectName.size() == 0)
  {
    std::cout << "CONFIG ERROR" << std::endl;
    return;
  }

  Resources::rootPath = "../projects/" + projectName + "/resources/";

  std::string root = std::filesystem::current_path().parent_path().generic_string() + "/projects/" + projectName + "/";
  std::cout << "root path: " << root << std::endl;

  std::unordered_map<std::string, std::string> files;

  for (const auto & entry : std::filesystem::recursive_directory_iterator(root))
  {

    if (".m" == entry.path().extension())
    {
      std::cout << "found:" << entry.path().filename() << std::endl;
      fullPath = entry.path().generic_string();

      path = fullPath.substr(root.size());

      std::cout << "path:" << path << std::endl;

      std::ifstream ifs(root + path);
      std::string content(
        (std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>())
      );

      files.insert_or_assign(path, content);
    }
  }

  std::cout << "------------" << std::endl;

  parseFiles(files);

  //std::cout << std::endl << "results:" << std::endl;
  //std::vector<MetaData>& list = globalMetaList.getList();
  //for (unsigned int i = 0; i < list.size(); i++)
  //{
  //  std::cout << "selector:" << list[i].selector << std::endl;
  //  for (MetaKV kv : list[i].kvs)
  //  {
  //    std::cout << kv.key << ":" << kv.val << std::endl;
  //  }
  //  std::cout << "==========" << std::endl;
  //}

}

void Meta::parseFiles(std::unordered_map<std::string, std::string>& files)
{
  for (auto it = files.begin(); it != files.end(); it++)
  {
    parseContent(it->first, it->second, globalMetaList);
  }
}


void Meta::parseContent(const std::string& fileName, std::string& content, MetaDataList& out)
{
  std::cout << "content: " << std::endl << content << std::endl;

  MetaData* metaData = nullptr;
  char state = STATE_BEFORE_SELECTOR;

  bool isSpace = false;
  std::string key, val;
  unsigned int lineNum = 1;

  for (char& ch : content) 
  {
    if (ch == '\n')
    {
      if (state == STATE_BEFORE_KEY && key.size() && key.size() > 0)
      {
        //std::cout << "kv:" << key << ":" << val << std::endl;
        metaData->addKV(key, val, VAR_TYPE_STRING);
        //VarString* var = new VarString(val);
        //MetaKV kv = MetaKV(key, var);
        //metaData->kvs.push_back({ key, var });
      }

      if (state > STATE_BEFORE_KEY)
      {
        state = STATE_BEFORE_KEY;
        std::cout << "'" << fileName << "' parsing error line:" << lineNum << std::endl;
      }

      key.clear();
      val.clear();

      lineNum++;
    }

    isSpace = isspace(ch);

    if (state == STATE_BEFORE_SELECTOR)
    {
      if (!isSpace)
      {
        state = STATE_SELECTOR;
        metaData = new MetaData();
        metaData->selector = ch;
      }
    }
    else if (state == STATE_SELECTOR)
    {
      if (!isSpace)
      {
        if (ch == '{')
        {
          if (metaData->selector.size() == 0)
          {
            std::cout << "'" << fileName << "' parsing error line:" << lineNum << "| selector has no name" << std::endl;
          }
          state = STATE_BEFORE_KEY;
          key = "";
        }
        else
        {
          metaData->selector += ch;
        }
      }
    }
    else if (state == STATE_BEFORE_KEY)
    {
      if (ch == '}')
      {
        out.add(metaData);
        state = STATE_BEFORE_SELECTOR;
      }
      else if (!isSpace)
      {
        key = ch;
        state = STATE_KEY;
      }
    }
    else if (state == STATE_KEY)
    {
      if (ch == ':')
      {
        state = STATE_BEFORE_VAL;
      }
      else if (!isSpace)
      {
        key += ch;
      }
    }
    else if (state == STATE_BEFORE_VAL)
    {
      if (ch == '"')
      {
        state = STATE_VAL;
        val = "";
      }
    }
    else if (state == STATE_VAL)
    {
      if (ch == '"')
      {
        state = STATE_BEFORE_KEY;
      }
      else
      {
        val += ch;
      }
    }
  }
}

std::string Meta::getConfigVal(const std::string& key)
{
  return "";// todo: config.getKV(key);
}