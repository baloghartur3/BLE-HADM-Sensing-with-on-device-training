# BLE HADM Sensing with on-device training

This project demonstrates the capabilities of wireless sensing using the IQ samples from the BLE 

## Usage

### IMPORTANT NOTE
However one may build this app one slight modification to the sdk is necessary to run this app successfully:
The IQ samples have to be retrieved somehow from the HADM stack. Currently there is no API to support this so here is a suggested working solution:

The following changes must be made in the gecko_sdk_`<some version number>`\app\bluetooth\common\abr_cs_parser\abr_cs_parser.c

These variables must defined: 

```c
extern float initiator_i[];
extern float initiator_q[];
extern float reflector_i[];
extern float reflector_q[];
extern int done;
```
These changes should start after line 675
```c
meas->rtp_data.rtl_input.q_samples_d2 = meas->rtp_data.reflector_q_samples;
//MODIFICATION START
int j=0;
for(int i=0; i<40;i++){
    initiator_i[j]=meas->rtp_data.initiator_i_samples[i];
    initiator_q[j]=meas->rtp_data.initiator_q_samples[i];
    reflector_i[j]=meas->rtp_data.reflector_i_samples[i];
    reflector_q[j]=meas->rtp_data.reflector_q_samples[i];
    if (i!=0 && i!=12 && i!=39)
      j++;
}
done=1;
//MODIFICATION END
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

## Training without collecting
I have provided an additional dataset to this repo to eliminate the need for data collection in the form of a .h file, which contains samples of a person being present or absent in a certain test area. This way there is no need for 2 devices, one can experiment with this dataset and try various models.

## Troubleshooting

This sample application does need a bootloader. Updating the software is only possible via re-flashing the code.
Use the bootloader-apploader-workspace that can be found in the example. 

## Resources

[Bluetooth Documentation](https://docs.silabs.com/bluetooth/latest/)

[UG103.14: Bluetooth LE Fundamentals](https://www.silabs.com/documents/public/user-guides/ug103-14-fundamentals-ble.pdf)

[QSG169: Bluetooth SDK v3.x Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf)

[UG434: Silicon Labs Bluetooth ® C Application Developer's Guide for SDK v3.x](https://www.silabs.com/documents/public/user-guides/ug434-bluetooth-c-soc-dev-guide-sdk-v3x.pdf)

[Bluetooth Training](https://www.silabs.com/support/training/bluetooth)

