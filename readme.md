# BLE HADM Sensing with on-device training

This project demonstrates the capabilities of wireless sensing using the IQ samples from the BLE 

## Usage

### IMPORTANT NOTE
However one may build this app one slight modification to the sdk is necessary to run this app successfully:

The following variables must be defined in the gecko_sdk_`<some version number>`\app\bluetooth\common\abr_cs_parser\abr_cs_parser.c

```sh
extern float initiator_i[];
extern float initiator_q[];
extern float reflector_i[];
extern float reflector_q[];
extern int done;
```

### Building and Flashing


Demo can be imported using [Simplicity Studio](https://www.silabs.com/developers/simplicity-studio) and it will handle building and flashing. Alternative is to do it all manually:

To generate project definitions, clone this repository and then use the [slc-cli](https://www.silabs.com/documents/public/user-guides/ug520-software-project-generation-configuration-with-slc-cli.pdf) command:


```sh
slc generate -s <path-to-gecko-sdk> -p ml_mcu_ble_hadm.slcp -d ml_mcu_ble_hadm --with <board-name>
```

`<board-name>` in this case would be `brd4198a`.

Then use make to build the project:

```sh
cd ml_mcu_ble_hadm
make -f ml_mcu_ble_hadm.Makefile -j4
```

Final step is to flash the board using [Simplicity Commander](https://www.silabs.com/documents/public/user-guides/ug162-simplicity-commander-reference-guide.pdf):

Binary can be found at: build/debug/ml_mcu_ble_hadm.s37

### Running
After startup it will scan for a device running the "Soc - ABR Reflector" sample application. When found, the initiator will create a connection between them and will start the data collection for the sensing task.



## Troubleshooting

This sample application does need a bootloader. Updating the software is only possible via re-flashing the code.
Use the bootloader-apploader-workspace that can be found in the example. 

## Resources

[Bluetooth Documentation](https://docs.silabs.com/bluetooth/latest/)

[UG103.14: Bluetooth LE Fundamentals](https://www.silabs.com/documents/public/user-guides/ug103-14-fundamentals-ble.pdf)

[QSG169: Bluetooth SDK v3.x Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf)

[UG434: Silicon Labs Bluetooth ® C Application Developer's Guide for SDK v3.x](https://www.silabs.com/documents/public/user-guides/ug434-bluetooth-c-soc-dev-guide-sdk-v3x.pdf)

[Bluetooth Training](https://www.silabs.com/support/training/bluetooth)

