#include <oak/meta/meta_data.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <oak/debug.h>
#include <oak/meta/prefab_validator.h>

using namespace oak;

std::map<uchar, nlohmann::json> MetaData::dataMap;
std::string MetaData::projectPath = "";


void MetaData::load(uchar key, const std::string& fileName)
{
  std::string fullPath = projectPath + fileName;
  nlohmann::json data;
  std::ifstream i(fullPath);
  if (i.fail())
  {
    LOG_ERROR << "failed to load metadata: " << fullPath;
  }
  i >> data;
  i.close();

  if (data != nullptr)
  {
    LOG << "Meta data loaded: " << fullPath;
    dataMap[key] = data;
  }
  else
  {
    LOG_WARNING << "error loading meta data file: " << fullPath;
  }
}

const nlohmann::json& MetaData::getData(uchar key)
{
  return dataMap[key];
}

