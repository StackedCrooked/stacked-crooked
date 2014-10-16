set -e
svn_tag=RELEASE_342
(svn co -q http://llvm.org/svn/llvm-project/llvm/tags/${svn_tag}/final llvm)
(cd llvm/tools && svn co -q http://llvm.org/svn/llvm-project/cfe/tags/${svn_tag}/final clang)
(cd llvm/tools/clang/tools && svn co -q http://llvm.org/svn/llvm-project/clang-tools-extra/tags/${svn_tag}/final extra)
(cd llvm/projects && svn co -q http://llvm.org/svn/llvm-project/compiler-rt/tags/${svn_tag}/final compiler-rt)
