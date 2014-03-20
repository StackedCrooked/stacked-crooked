set -e
(svn co http://llvm.org/svn/llvm-project/llvm/tags/RELEASE_34 llvm)
(cd llvm/tools && svn co http://llvm.org/svn/llvm-project/cfe/tags/RELEASE_34 clang)
(cd llvm/tools/clang/tools && svn co http://llvm.org/svn/llvm-project/clang-tools-extra/tags/RELEASE_34 extra)
(cd llvm/projects && svn co http://llvm.org/svn/llvm-project/compiler-rt/tags/RELEASE_34 compiler-rt)
