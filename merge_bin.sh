esptool.py --chip ESP32 merge_bin -o dynamic_indicator.bin --flash_mode dio --flash_size 4MB 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin
