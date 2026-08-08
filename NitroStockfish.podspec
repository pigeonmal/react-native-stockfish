require "json"

package = JSON.parse(File.read(File.join(__dir__, "package.json")))

Pod::Spec.new do |s|
  s.name         = "NitroStockfish"
  s.version      = package["version"]
  s.summary      = package["description"]
  s.homepage     = package["homepage"]
  s.license      = { :type => package["license"], :file => "third-party/stockfish/Copying.txt" }
  s.authors      = package["author"]

  s.platforms    = { :ios => min_ios_version_supported, :visionos => 1.0 }
  s.source       = { :git => "https://github.com/pigeonmal/react-native-stockfish.git", :tag => "v#{s.version}" }

  s.source_files = [
    "ios/**/*.{swift}",
    "ios/**/*.{m,mm}",
    "cpp/**/*.{hpp,cpp}",
    "third-party/stockfish/src/**/*.cpp",
    "third-party/stockfish/src/**/*.h",
  ]
  s.exclude_files = "third-party/stockfish/src/main.cpp"

  s.pod_target_xcconfig = {
    "CLANG_CXX_LANGUAGE_STANDARD" => "c++20",
    "CLANG_CXX_LIBRARY" => "libc++",
    "GCC_PREPROCESSOR_DEFINITIONS" => "$(inherited) NDEBUG USE_PTHREADS NO_PREFETCH",
    # incbin resolves its binary at assembler time, not through C++ include
    # paths. Give Apple's integrated assembler a package-relative search root
    # so simulator and device builds embed the same NNUE network.
    "OTHER_CPLUSPLUSFLAGS" => "$(inherited) -Wa,-I$(PODS_TARGET_SRCROOT)/third-party/stockfish/src",
  }

  load 'nitrogen/generated/ios/NitroStockfish+autolinking.rb'
  add_nitrogen_files(s)

  s.dependency 'React-jsi'
  s.dependency 'React-callinvoker'
  install_modules_dependencies(s)
end
