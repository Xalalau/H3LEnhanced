# Rascunho - Xala



# Noble

sudo dpkg --add-architecture i386
sudo apt-get -qq update
sudo apt install -y linux-libc-dev gcc-14-multilib g++-14-multilib libcurl4-openssl-dev:i386 libpthread-stubs0-dev:i386 linux-libc-dev:i386

BASE_DIR="~/Stuff/Hu3-Life/H3LEnhanced"



COMPILER_VERSION_C=gcc-14
COMPILER_VERSION_CXX=g++-14
DEPENDENCIES_DIR="${BASE_DIR}/dependencies"



# Preparar as dependências, dar build e falsear o diretório common do Steam para o Travis
DEPS_DIR="${BASE_DIR}/deps"
STEAMCOMMON_DIR="${BASE_DIR}/fake_steamcommon"
mkdir -p "${DEPS_DIR}" && mkdir -p "${STEAMCOMMON_DIR}" && mkdir -p "${STEAMCOMMON_DIR}/Half-Life" && cd "${STEAMCOMMON_DIR}/Half-Life"

# Make symlinks to required libraries located in the Half-Life directory
ln -s ${BASE_DIR}/lib/public/vgui.so libvgui.so
ln -s ${BASE_DIR}/lib/public/libtier0.so libtier0.so
ln -s ${BASE_DIR}/lib/public/libvstdlib.so libvstdlib.so

cd ${DEPS_DIR}

# Pegar o CMake correto e criar symlinks para que ele possa usar o VGUI corretamente
CMAKE_URL="http://www.cmake.org/files/v3.11/cmake-3.11.4-Linux-x86_64.tar.gz"
mkdir cmake && wget --no-check-certificate --quiet -O - ${CMAKE_URL} | tar --strip-components=1 -xz -C cmake
export PATH=${DEPS_DIR}/cmake/bin:${PATH}
COMPILER_SETTINGS="-DCMAKE_CXX_COMPILER=${COMPILER_VERSION_CXX} -DCMAKE_C_COMPILER=${COMPILER_VERSION_C}"





# Mover o diretório de build
cd ${BASE_DIR}

# Usar o arquivo de toolchain para forçar o ambiente de 32 bits
COMPILER_SETTINGS="${COMPILER_SETTINGS} -DCMAKE_TOOLCHAIN_FILE=${BASE_DIR}/cmake/Linux_32bit_toolchain.cmake"

bash ${BASE_DIR}/travis_install_dependencies.sh ${DEPENDENCIES_DIR} "${COMPILER_SETTINGS}"

mkdir build && cd build



# Executar os comandos de cmake e make
# Usar a biblioteca vgui local
# Nota original: TODO: SDL should be found by CMake at all times, this needs fixing - Solokiller
cmake ${COMPILER_SETTINGS} -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -m32 -fno-strict-overflow -w" -DCMAKE_C_FLAGS="${CMAKE_C_FLAGS} -m32 -fno-strict-overflow -w" \
-DSTEAMCOMMON=${STEAMCOMMON_DIR} ${BUILD_RULES} \
-Dvgui_DIR="${BASE_DIR}/lib/public"

make

