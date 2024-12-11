import json
import struct

from bluepy3.btle import DefaultDelegate, Scanner, Peripheral, UUID, BTLEException, BTLEInternalError
from time import sleep
from datetime import datetime

JSON_FILE = "data_pipe.json"

servUUID = UUID(0x185A)
# Characteristics format:
#   {
#       <mac_addr>: [
#           {
#               "handle": ...,
#               "uuid": ...,
#               "peripheral": ...,
#               "value": ...,
#           },
#           {
#               "handle": ...,
#               "uuid": ...,
#               "peripheral": ...,
#               "value": ...,
#           }
#       ],
#       <mac_addr>: {...}
#   }
ble_devices_characteristics = dict()

charac_uuids = {
        "19b10001e8f2537e4f6cd104768a1214": "air_temp",
        "ff7b36c646fd4577af827bc30c876221": "air_hum",
        "6041f41ecfee4a0f96e0ce7ba222482e": "soil_moisture",
        }

class ScannerDelegate(DefaultDelegate): 
        def __init__(self):
            DefaultDelegate.__init__(self)

        def handleDiscovery(self, dev, isNewDev, _):
            if dev.addr not in ble_devices_characteristics and dev.getValueText(0xff) == "010203040506":
                print("Connecting...")
                try:
                    con = Peripheral(dev)
                    print("Connected !")
                except:
                    print("Failed to connect")
                    return

                try:
                    con.getServices()
                    serv = con.getServiceByUUID(servUUID)
                except:
                    print("### ERROR: Can't find service... ###")
                    return

                for newCharac in serv.getCharacteristics():
                    if con.addr not in ble_devices_characteristics:
                        ble_devices_characteristics[con.addr] = list()

                    ble_devices_characteristics[con.addr].append(dict())
                    charac = ble_devices_characteristics[con.addr][-1]
                    charac["uuid"] = newCharac.uuid
                    charac["handle"] = newCharac.getHandle()
                    charac["peripheral"] = newCharac.peripheral
                    charac["value"] = newCharac.read()


def readCharacteristics():
    data_file = open(JSON_FILE, mode="w+")

    # Change json format for the output
    #   {
    #       <mac_addr>: {
    #           "addr": ...,
    #           "air_temp": ...,
    #           "air_hum": ...,
    #           "soil_mositure": ...,
    #           "timestamp": ...
    #       },
    #       <mac_addr>: {...},
    #   }
    data = dict()
    for (device_addr, device_characteristics) in ble_devices_characteristics.items():
        data[device_addr] = dict()
        data[device_addr]["timestamp"] = datetime.now().replace(microsecond=0).isoformat()

        for charac in device_characteristics:
            str_uuid = ''.join(f"{b:#0{4}x}"[2:] for b in charac["uuid"].binVal)
            if str_uuid in charac_uuids:
                try:
                    charac["value"] = charac["peripheral"].readCharacteristic(charac["handle"])
                except Exception as err:
                    charac["peripheral"].connect(device_addr)
                    print(f"### ERROR: Reading from device failed ###\n### {err=} ###")
                data[device_addr][charac_uuids[str_uuid]] = struct.unpack('f', charac["value"])[0]

    print(f"\nin ble.py: {data=}")

    json.dump(data, fp=data_file)


def reconnectToDevices():
    for (device_addr, device) in ble_devices_characteristics.items():
        try:
            device[0]["peripheral"].connect(device_addr)
        except Exception as err:
            print(f"### ERROR: Can't reconnect after scan ###\n### {err=} ###")


scannerDelegate = ScannerDelegate()
scanner = Scanner().withDelegate(scannerDelegate)

if __name__ == "__main__":
    i = 5
    while True:
        if i == 5:
            i = 0
            scanner.clear()
            scanner.start()
            scanner.process(3)
            try:
                scanner.stop()
            except:
                pass
            reconnectToDevices()
        else:
            sleep(3)
            readCharacteristics()

        i += 1
