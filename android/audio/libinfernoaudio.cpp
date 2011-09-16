// arm-eabi-gcc -c -o libinfernoaudio.o -I/home/joel/android/system/core/include -I/home/joel/android/hardware/libhardware/include -I/home/joel/android/hardware/ril/include -I/home/joel/android/dalvik/libnativehelper/include -I/home/joel/android/frameworks/base/include -I/home/joel/android/external/skia/include -I/home/joel/android/out/target/product/generic/obj/include -I/home/joel/android/bionic/libc/arch-arm/include -I/home/joel/android/bionic/libc/include -I/home/joel/android/bionic/libstdc++/include -I/home/joel/android/bionic/libc/kernel/common -I/home/joel/android/bionic/libc/kernel/arch-arm -I/home/joel/android/bionic/libm/include -I/home/joel/android/bionic/libm/include/arch/arm -I/home/joel/android/bionic/libthread_db/include -I/home/joel/android/bionic/libm/arm -I/home/joel/android/bionic/libm -I/home/joel/android/out/target/product/generic/obj/SHARED_LIBRARIES/libm_intermediates -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__ -DANDROID -DSK_RELEASE -DNDEBUG -include /home/joel/android/system/core/include/arch/linux-arm/AndroidConfig.h -UDEBUG -g -march=armv5te -mtune=xscale -msoft-float -mthumb-interwork -fpic -fno-exceptions -ffunction-sections -funwind-tables -fstack-protector -fmessage-length=0 -Bdynamic -Wl,-T,/home/joel/android/build/core/armelf.x -Wl,-dynamic-linker,/system/bin/linker -Wl,--gc-sections -Wl,-z,nocopyreloc -Wl,-rpath-link=/home/joel/android/out/target/product/crespo/obj/lib -L/home/joel/android/out/target/product/crespo/obj/lib -nostdlib /home/joel/android/out/target/product/crespo/obj/lib/crtend_android.o /home/joel/android/out/target/product/crespo/obj/lib/crtbegin_dynamic.o /home/joel/android/prebuilt/linux-x86/toolchain/arm-eabi-4.2.1/lib/gcc/arm-eabi/4.2.1/interwork/libgcc.a -lc -lm libinfernoaudio.cpp /home/joel/android/prebuilt/linux-x86/toolchain/arm-eabi-4.2.1/lib/gcc/arm-eabi/4.2.1/interwork/libgcc.a -laudioflinger

//arm-eabi-g++ -shared -Wl,-soname,libinfernoaudio.so -o libinfernoaudio.so -I/home/joel/android/system/core/include -I/home/joel/android/hardware/libhardware/include -I/home/joel/android/hardware/ril/include -I/home/joel/android/dalvik/libnativehelper/include -I/home/joel/android/frameworks/base/include -I/home/joel/android/external/skia/include -I/home/joel/android/out/target/product/generic/obj/include -I/home/joel/android/bionic/libc/arch-arm/include -I/home/joel/android/bionic/libc/include -I/home/joel/android/bionic/libstdc++/include -I/home/joel/android/bionic/libc/kernel/common -I/home/joel/android/bionic/libc/kernel/arch-arm -I/home/joel/android/bionic/libm/include -I/home/joel/android/bionic/libm/include/arch/arm -I/home/joel/android/bionic/libthread_db/include -I/home/joel/android/bionic/libm/arm -I/home/joel/android/bionic/libm -I/home/joel/android/out/target/product/generic/obj/SHARED_LIBRARIES/libm_intermediates -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__ -DANDROID -DSK_RELEASE -DNDEBUG -include /home/joel/android/system/core/include/arch/linux-arm/AndroidConfig.h -UDEBUG -g -march=armv5te -mtune=xscale -msoft-float -mthumb-interwork -fpic -fno-exceptions -ffunction-sections -funwind-tables -fstack-protector -fmessage-length=0 -Bdynamic -Wl,-T,/home/joel/android/build/core/armelf.x -Wl,-dynamic-linker,/system/bin/linker -Wl,--gc-sections -Wl,-z,nocopyreloc -Wl,-rpath-link=/home/joel/android/out/target/product/crespo/obj/lib -L/home/joel/android/out/target/product/crespo/obj/lib -nostdlib /home/joel/android/out/target/product/crespo/obj/lib/crtend_android.o /home/joel/android/out/target/product/crespo/obj/lib/crtbegin_dynamic.o /home/joel/android/prebuilt/linux-x86/toolchain/arm-eabi-4.2.1/lib/gcc/arm-eabi/4.2.1/interwork/libgcc.a -lc -lm libinfernoaudio.o /home/joel/android/prebuilt/linux-x86/toolchain/arm-eabi-4.2.1/lib/gcc/arm-eabi/4.2.1/interwork/libgcc.a -lmedia -lcutils -lutils

// Thin C++ layer to enable devphone.c to call AudioFlinger methods

#include <binder/Parcel.h>
#include <stdlib.h>
#include <media/AudioSystem.h>
#include <media/AudioTrack.h>
#include <utils/String8.h>

using namespace android;

extern "C" void af_setMode(int mode);
extern "C" void af_setVoiceVolume(float volume);
extern "C" void af_setParameters(char *keysvalues);

void af_setMode(int mode)
{
	AudioSystem::setMode(mode);
}

void af_setVoiceVolume(float volume)
{
	AudioSystem::setVoiceVolume(volume);
}

void af_setParameters(char *keysvalues)
{
	// setParameters(int ioHandle, const String8 &keyValuePairs)
	// if ioHandle is 1, we can get to the audio out stream
	AudioSystem::setParameters(1, String8(keysvalues));
}

int main(void)
{
}
