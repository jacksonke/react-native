// Copyright 2004-present Facebook. All Rights Reserved.

#pragma once

#include <memory>
#include <cxxreact/JSBigString.h>
#include <cxxreact/JSModulesUnbundle.h>


namespace facebook {
namespace react {

class JniTeresaJSModulesUnbundle : public JSModulesUnbundle {
  /**
   * This implementation reads modules as single file from the assets of an apk.
   */
public:
  JniTeresaJSModulesUnbundle() = default;
  JniTeresaJSModulesUnbundle(const std::string& entryFile);
  JniTeresaJSModulesUnbundle(JniTeresaJSModulesUnbundle&& other) = delete;
  JniTeresaJSModulesUnbundle& operator= (JSModulesUnbundle&& other) = delete;

  std::unique_ptr<const JSBigString> getStartupCode();

  static bool isUnbundle(
    const std::string& dirName);
  virtual Module getModule(uint32_t moduleId) const override;
private:
  std::string m_moduleDirectory;
  std::unique_ptr<const JSBigFileString> m_startupCode;
};

}
}
