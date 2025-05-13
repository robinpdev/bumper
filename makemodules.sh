unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     make module;;
    Darwin*)    make macmodule;;
    MINGW*)     echo "coming soonr";;
    MSYS_NT*)   echo "tf u doin!";;
    *)          machine="UNKNOWN:${unameOut}"
esac