// clang++ -std=c++17 -o main main.cpp
//
// FAILURE:
// clang++ -std=c++17 -static -o main main.cpp
/*
(base) Perso@MBProJL vibe_17_06_26 % clang++ -std=c++17 -static -v -o main main.cpp
Apple clang version 21.0.0 (clang-2100.0.123.102)
Target: arm64-apple-darwin25.5.0
Thread model: posix
InstalledDir: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin
ignoring nonexistent directory "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../include/c++/v1"
 "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang" -cc1 -triple arm64-apple-macosx26.0.0 -Wundef-prefix=TARGET_OS_ -Wdeprecated-objc-isa-usage -Werror=deprecated-objc-isa-usage -Werror=implicit-function-declaration -emit-obj -dumpdir main- -disable-free -clear-ast-before-backend -disable-llvm-verifier -discard-value-names -main-file-name main.cpp -static-define -mrelocation-model static -mframe-pointer=non-leaf -fno-strict-return -ffp-contract=on -fno-rounding-math -funwind-tables=1 -fobjc-msgsend-selector-stubs -fno-sized-deallocation -target-sdk-version=26.5 -fvisibility-inlines-hidden-static-local-var -fno-modulemap-allow-subdirectory-search -fdefine-target-os-macros -fno-assume-unique-vtables -fno-modulemap-allow-subdirectory-search -enable-tlsdesc -target-cpu apple-m1 -target-feature +v8.5a -target-feature +aes -target-feature +altnzcv -target-feature +ccdp -target-feature +ccpp -target-feature +complxnum -target-feature +crc -target-feature +dotprod -target-feature +flagm -target-feature +fp-armv8 -target-feature +fp16fml -target-feature +fptoint -target-feature +fullfp16 -target-feature +jsconv -target-feature +lse -target-feature +neon -target-feature +pauth -target-feature +perfmon -target-feature +predres -target-feature +ras -target-feature +rcpc -target-feature +rdm -target-feature +sb -target-feature +sha2 -target-feature +sha3 -target-feature +specrestrict -target-feature +ssbs -target-abi darwinpcs -debugger-tuning=lldb -fdebug-compilation-dir=/Volumes/Users/GitHub/jlaurens/synctex/cpp/sandbox/vibe_17_06_26 -target-linker-version 1266.8 -v -fcoverage-compilation-dir=/Volumes/Users/GitHub/jlaurens/synctex/cpp/sandbox/vibe_17_06_26 -resource-dir /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/21 -isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk -I/usr/local/include -internal-isystem /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1 -internal-isystem /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/local/include -internal-isystem /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/21/include -internal-externc-isystem /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include -internal-externc-isystem /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include -internal-iframework /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks -internal-iframework /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/SubFrameworks -internal-iframework /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/Library/Frameworks -Wno-elaborated-enum-base -std=c++17 -fdeprecated-macro -ferror-limit 19 -fmessage-length=157 -stack-protector 1 -fstack-check -mdarwin-stkchk-strong-link -fblocks -fencode-extended-block-signature -fregister-global-dtors-with-atexit -fgnuc-version=4.2.1 -fskip-odr-check-in-gmf -fcxx-exceptions -fexceptions -fmax-type-align=16 -fcommon -fcolor-diagnostics -clang-vendor-feature=+disableNonDependentMemberExprInCurrentInstantiation -fno-odr-hash-protocols -clang-vendor-feature=+enableAggressiveVLAFolding -clang-vendor-feature=+revert09abecef7bbf -clang-vendor-feature=+thisNoAlignAttr -clang-vendor-feature=+thisNoNullAttr -clang-vendor-feature=+disableAtImportPrivateFrameworkInImplementationError -Wno-error=allocator-wrappers -fdwarf2-cfi-asm -o /var/folders/hj/78q1fxz969z95rtwxxs0gbrm0000gq/T/main-2233e8.o -x c++ main.cpp
clang -cc1 version 21.0.0 (clang-2100.0.123.102) default target arm64-apple-darwin25.5.0
ignoring nonexistent directory "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/local/include"
ignoring nonexistent directory "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/Library/Frameworks"
#include "..." search starts here:
#include <...> search starts here:
 /usr/local/include
 /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1
 /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/21/include
 /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include
 /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include
 /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks (framework directory)
 /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/SubFrameworks (framework directory)
End of search list.
 "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ld" -demangle -lto_library /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/libLTO.dylib -no_deduplicate -static -arch arm64 -platform_version macos 26.0.0 26.5 -syslibroot /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk -mllvm -enable-linkonceodr-outlining -o main -lcrt0.o -L/usr/local/lib /var/folders/hj/78q1fxz969z95rtwxxs0gbrm0000gq/T/main-2233e8.o -lc++
ld: library 'crt0.o' not found
clang++: error: linker command failed with exit code 1 (use -v to see invocation)
*/
//
// -lstdc++


#include <iostream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <chemin>" << std::endl;
        return 1;
    }

    std::string chemin = argv[1];

    if (!fs::exists(chemin)) {
        std::cerr << "Erreur: Le chemin '" << chemin << "' n'existe pas." << std::endl;
        return 1;
    }

    if (fs::is_directory(chemin)) {
        std::cout << "Dossier :" << std::endl;
        std::cout << fs::canonical(chemin) << std::endl;
        std::cout << "Contenu :" << std::endl;
        for (const auto& entry : fs::directory_iterator(chemin)) {
            std::cout << "- " << entry.path().filename() << std::endl;
        }
    } else {
        std::cout << "Fichier :" << std::endl;
        std::cout << fs::canonical(chemin) << std::endl;
        std::cout << "Taille: "
                  << fs::file_size(chemin) << " octets." << std::endl;
    }

    return 0;
}
