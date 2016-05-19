# tn40x_raw
This is modified version of tehuti networks tn40xx 10G NIC (used in DLINK DXE810S) chip which allow raw access to NIC input and outputs efficiently. the modified driver allows network application to bypass linux networking stack and send/receive packets to NIC directly via character device.
for each nic devices a character device (named eth0 to ethN) will be created. network applications could access raw network by openning raw access device and write ethernet frames by and structure defined in tn40xx.h
an exmaple test application provided wich test raw RX/TX functionality of this driver
