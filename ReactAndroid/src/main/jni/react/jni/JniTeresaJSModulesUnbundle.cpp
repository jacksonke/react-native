// Copyright 2004-present Facebook. All Rights Reserved.

#include "JniTeresaJSModulesUnbundle.h"

#include <cstdint>
#include <fb/assert.h>
#include <libgen.h>
#include <memory>
#include <sstream>
#include <sys/endian.h>
#include <utility>
#include <fstream>

using magic_number_t = uint32_t;
const magic_number_t MAGIC_FILE_HEADER = 0xFB0BD1E5;
const std::string MAGIC_FILE_NAME = "TERESAUNBUNDLE";

namespace facebook {
namespace react {

static std::string jsModulesDir(const std::string& entryFile) {
  std::string dir = dirname(entryFile.c_str());

  // android's asset manager does not work with paths that start with a dot
  return dir == "." ? "js-modules/" : dir + "/js-modules/";
}

// static asset_ptr openAsset(
//     AAssetManager *manager,
//     const std::string& fileName,
//     int mode = AASSET_MODE_STREAMING) {
//   return asset_ptr(
//     AAssetManager_open(manager, fileName.c_str(), mode),
//     AAsset_close);
// }

JniTeresaJSModulesUnbundle::JniTeresaJSModulesUnbundle(const std::string& entryFile) :
  m_moduleDirectory(jsModulesDir(entryFile)),
  m_startupCode(JSBigFileString::fromPath(entryFile)){
    // m_startupCode = std::move(JSBigFileString::fromPath(entryFile));
}

bool JniTeresaJSModulesUnbundle::isUnbundle(
    const std::string& entryName) {

  magic_number_t fileHeader = 0;
  auto magicFileName = jsModulesDir(entryName) + MAGIC_FILE_NAME;

  std::ifstream magic_stream(magicFileName, std::ios_base::in);
  if (!magic_stream) {
    return false;
  }

  magic_stream.read(reinterpret_cast<char *>(&fileHeader), sizeof(fileHeader));
  magic_stream.close();

  return fileHeader == htole32(MAGIC_FILE_HEADER);
}

std::unique_ptr<const JSBigString> JniTeresaJSModulesUnbundle::getStartupCode() {
  CHECK(m_startupCode) << "startup code for a RAM Bundle can only be retrieved once";
  return std::move(m_startupCode);
}


JSModulesUnbundle::Module JniTeresaJSModulesUnbundle::getModule(uint32_t moduleId) const {
  // can be nullptr for default constructor.
  FBASSERTMSGF(!m_moduleDirectory.empty(), "Unbundle has not been initialized with entryfile");

  std::ostringstream sourceUrlBuilder;
  sourceUrlBuilder << moduleId << ".js";
  auto sourceUrl = sourceUrlBuilder.str();

  auto fileName = m_moduleDirectory + sourceUrl;

  // todo by jacksonke 这里需要先确保文件是存在的
  std::unique_ptr<const JSBigFileString> codeUPtr = JSBigFileString::fromPath(fileName);
  return {sourceUrl, std::string(codeUPtr.get()->c_str())};


  // auto asset = openAsset(m_assetManager, fileName, AASSET_MODE_BUFFER);

  // const char *buffer = nullptr;
  // if (asset != nullptr) {
  //   buffer = static_cast<const char *>(AAsset_getBuffer(asset.get()));
  // }
  // if (buffer == nullptr) {
  //   throw ModuleNotFound("Module not found: " + sourceUrl);
  // }
  // return {sourceUrl, std::string(buffer, AAsset_getLength(asset.get()))};
}

}
}
