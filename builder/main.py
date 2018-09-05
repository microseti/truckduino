# Copyright 2014-present PlatformIO <contact@platformio.org>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from os.path import basename, isfile, join
from SCons.Script import (COMMAND_LINE_TARGETS, AlwaysBuild, Builder, Default,
                          DefaultEnvironment)
import os
import platform

platformx = platform

env = DefaultEnvironment()

platform = env.PioPlatform()
board = env.BoardConfig()
FRAMEWORK_DIR = join(platform.get_package_dir(
    "framework-arduinoststm32"), "STM32F1")

env.Replace(
    AR="arm-none-eabi-ar",
    AS="arm-none-eabi-as",
    CC="arm-none-eabi-gcc",
    CXX="arm-none-eabi-g++",
    OBJCOPY="arm-none-eabi-objcopy",
    RANLIB="arm-none-eabi-ranlib",
    SIZETOOL="arm-none-eabi-size",

    ARFLAGS=["rcs"],

    ASFLAGS=["-x", "assembler-with-cpp"],

    CCFLAGS=[
        "-g",   # include debugging info (so errors include line numbers)
        "-Os",  # optimize for size
        "-ffunction-sections",  # place each function in its own section
        #~ "-fdata-sections",
        "-Wall",
        "-mthumb",
        "-mthumb-interwork",
        "-nostdlib"
    ],

    CXXFLAGS=[
        "-fno-rtti",
        "-fno-exceptions"
    ],

    CPPDEFINES=[
        ("F_CPU", "$BOARD_F_CPU")
    ],

    LINKFLAGS=[
        "-Os",
        #~ "-Wl,--gc-sections,--relax",
        "-Wl,--gc-sections",
        "-mthumb",
        "-mthumb-interwork",
        "-nostartfiles",
        "-nostdlib"
    ],

    LIBS=["c", "gcc", "m", "stdc++", "nosys"],

    UPLOADER="st-flash",
    UPLOADERFLAGS=[
        "write",        # write in flash
        "$SOURCES",     # firmware path to flash
        "0x08000000"    # flash start adress
    ],
    UPLOADCMD='$UPLOADER $UPLOADERFLAGS',

    SIZEPRINTCMD='$SIZETOOL -B -d $SOURCES',

    PROGNAME="firmware",
    PROGSUFFIX=".elf"
)

if "BOARD" in env:
    env.Append(
        CCFLAGS=[
            "-mcpu=%s" % env.BoardConfig().get("build.cpu")
        ],
        CPPDEFINES=[
            env.BoardConfig().get("build.variant", "").upper()
        ],
        LINKFLAGS=[
            "-mcpu=%s" % env.BoardConfig().get("build.cpu")
        ]
    )

env.Append(
    ASFLAGS=env.get("CCFLAGS", [])[:],

    BUILDERS=dict(
        ElfToBin=Builder(
            action=env.VerboseAction(" ".join([
                "$OBJCOPY",
                "-O",
                "binary",
                "$SOURCES",
                "$TARGET"
            ]), "Building $TARGET"),
            suffix=".bin"
        ),
        ElfToHex=Builder(
            action=env.VerboseAction(" ".join([
                "$OBJCOPY",
                "-O",
                "ihex",
                "-R",
                ".eeprom",
                "$SOURCES",
                "$TARGET"
            ]), "Building $TARGET"),
            suffix=".hex"
        )
    )
)

if env.subst("$UPLOAD_PROTOCOL") == "gdb":
    if not isfile(join(env.subst("$PROJECT_DIR"), "upload.gdb")):
        env.Exit(
            "Error: You are using GDB as firmware uploader. "
            "Please specify upload commands in upload.gdb "
            "file in project directory!"
        )
    env.Replace(
        UPLOADER="arm-none-eabi-gdb",
        UPLOADERFLAGS=[
            join("$BUILD_DIR", "firmware.elf"),
            "-batch",
            "-x",
            join("$PROJECT_DIR", "upload.gdb")
        ],

        UPLOADCMD='$UPLOADER $UPLOADERFLAGS'
    )

elif env.subst("$UPLOAD_PROTOCOL") in ("serial", "dfu") \
        and "arduino" in env.subst("$PIOFRAMEWORK"):
    if platformx.system() == "Windows":
        _upload_tool = FRAMEWORK_DIR+"/../../tool-stm32duino/stm32flash.exe"
        _boot_cmd = "start mode $UPLOAD_PORT: dtr=on & echo hello > $UPLOAD_PORT & $UPLOADER $UPLOADERFLAGS"
    elif platformx.system() == "Linux":
        _upload_tool = FRAMEWORK_DIR+"/../../tool-stm32duino/stm32flash/stm32flash"
        _boot_cmd = "echo 123456789012345678901234567890123456789012345678901234567890 > $UPLOAD_PORT && $UPLOADER $UPLOADERFLAGS"
    _upload_flags = ["{upload.altID}", "{upload.usbID}"]
    _upload_src = "-w $PROJECT_DIR/$SOURCES"
    if env.subst("$UPLOAD_PROTOCOL") == "dfu":
        _upload_tool = "maple_upload"
        _usbids = env.BoardConfig().get("build.hwids")
        _upload_flags = [env.BoardConfig().get("upload.boot_version", 2),
                         "%s:%s" % (_usbids[0][0][2:], _usbids[0][1][2:])]
# 500000
# 576000
# 921600
# 1000000
# 1500000
# 2000000
#~ 0.33
    env.Replace(
        UPLOADER=_upload_tool,
        #~ UPLOADERFLAGS="-g 0x8000000 -b 576000 -R -i dtr,-dtr,dtr:-dtr " + _upload_src + " $UPLOAD_PORT",
        UPLOADERFLAGS="-g 0x8000000 -b 921600 -R -i dtr:-dtr " + _upload_src + " $UPLOAD_PORT",
        UPLOADCMD=(_boot_cmd))

#
# Target: Build executable and linkable firmware
#

target_elf = None
if "nobuild" in COMMAND_LINE_TARGETS:
    target_firm = join("$BUILD_DIR", "firmware.bin")
else:
    target_elf = env.BuildProgram()
    target_firm = env.ElfToBin(join("$BUILD_DIR", "firmware"), target_elf)

AlwaysBuild(env.Alias("nobuild", target_firm))
target_buildprog = env.Alias("buildprog", target_firm, target_firm)

#
# Target: Print binary size
#

target_size = env.Alias(
    "size", target_elf,
    env.VerboseAction("$SIZEPRINTCMD", "Calculating size $SOURCE"))
AlwaysBuild(target_size)

#
# Target: Upload by default .bin file
#

if "arduino" in env.subst("$PIOFRAMEWORK"):
    def BeforeUpload(target, source, env):
        env.AutodetectUploadPort()
        #~ env.Replace(UPLOAD_PORT=basename(env.subst("$UPLOAD_PORT")))
    target_upload = env.Alias(
        "upload", target_firm,
        [env.VerboseAction(BeforeUpload,
                           "Looking for upload disk..."),
         env.VerboseAction("$UPLOADCMD", "Uploading $SOURCE")])
else:
    target_upload = env.Alias(
        "upload", target_firm,
        env.VerboseAction("$UPLOADCMD", "Uploading $SOURCE"))
AlwaysBuild(target_upload)

#
# Default targets
#

Default([target_buildprog, target_size])
