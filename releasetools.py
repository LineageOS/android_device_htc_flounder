import common
import struct

def FullOTA_InstallEnd(info):
  try:
    firmware_img = info.input_zip.read("RADIO/firmware.zip")
  except KeyError:
    print "no firmware.zip in target_files; skipping install"
  else:
    info.script.Print("Writing firmware.zip...")
    common.ZipWriteStr(info.output_zip, "firmware.zip", firmware_img)
    info.script.AppendExtra(
        'package_extract_file("firmware.zip", "/dev/block/platform/sdhci-tegra.3/by-name/OTA");')


def IncrementalOTA_InstallEnd(info):
  try:
    source_firmware_img = info.source_zip.read("RADIO/firmware.zip")
  except KeyError:
    print "no firmware.zip in source_files; skipping install"
  else:
    try:
      target_firmware_img = info.target_zip.read("RADIO/firmware.zip")
    except KeyError:
      print "no firmware.zip in target_files; skipping install"
    else:
      if source_firmware_img == target_firmware_img:
        return
      info.script.Print("Writing firmware.zip...")
      common.ZipWriteStr(info.output_zip, "firmware.zip", target_firmware_img)
      info.script.AppendExtra(
          'package_extract_file("firmware.zip", "/dev/block/platform/sdhci-tegra.3/by-name/OTA");')
