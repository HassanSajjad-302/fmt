#include "Configure.hpp"

void configurationSpecification(Configuration& configuration) {
  DSC<CppSourceTarget>& std = configuration.GetCppStaticDSC("std");
  std.getSourceTarget()
      .MODULE_FILES("stl.cpp")
      .assignStandardIncludesToHUIncludes();

  DSC<CppSourceTarget>& fmt = configuration.GetCppStaticDSC("fmt");
  fmt.getSourceTarget()
      .MODULE_DIRECTORIES("src", ".*cc")
      .PUBLIC_HU_INCLUDES("include/");

  DSC<CppSourceTarget>& app =
      configuration.GetCppExeDSC("app").PRIVATE_LIBRARIES(&fmt, &std);
  app.getSourceTarget()
      .MODULE_FILES("main.cpp");

  configuration.setModuleScope(app.getSourceTargetPointer());
}

void buildSpecification() {
  Configuration& debug = GetConfiguration("Debug");
  debug.ASSIGN(ConfigType::DEBUG);
  Configuration& release = GetConfiguration("Release");
  release.ASSIGN(LTO::ON);

  configurationSpecification(debug);
  configurationSpecification(release);
}

#ifdef EXE
int main(int argc, char** argv) {
  try {
    initializeCache(getBuildSystemModeFromArguments(argc, argv));
    buildSpecification();
    configureOrBuild();
  } catch (std::exception& ec) {
    string str(ec.what());
    if (!str.empty()) {
      printErrorMessage(str);
    }
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
#else
extern "C" EXPORT int func2(BSMode bsMode_) {
  try {
    initializeCache(bsMode_);
    buildSpecification();
    configureOrBuild();
  } catch (std::exception& ec) {
    string str(ec.what());
    if (!str.empty()) {
      printErrorMessage(str);
    }
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
#endif