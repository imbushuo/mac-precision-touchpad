# Windows Precision Touchpad Implementation for Apple MacBook family/Magic Trackpad 2

[![Build Status](https://ligstd.visualstudio.com/_apis/public/build/definitions/7694e0d0-94e3-4fd2-b39a-ecd261e1ba2e/22/badge)](https://ligstd.visualstudio.com/Apple%20PTP%20Trackpad/_build?definitionId=22)

This project implements Windows Precision Touchpad Protocol for Apple MacBook family/Magic Trackpad 2 on Windows 10. Both USB and SPI trackpads are supported.

## Future Plans and feature tracking

Use the [Azure DevOps Board](https://ligstd.visualstudio.com/Apple%20PTP%20Trackpad/_workitems/) to track feature and device support plans. Bug reports should go to Github issues.

## See it in action (YouTube video)

[![Watch the video](https://img.youtube.com/vi/-GWlfw7omdo/hqdefault.jpg)](https://youtu.be/-GWlfw7omdo)

## T2 devices

I got my T2 MacBook Pro :) Work is in progress. Eventually all USB devices will use the km driver (not only T2).

If you like my work, please consider buying me a coffee. Thank you for your support!

<a href="https://www.buymeacoffee.com/imbushuo" target="_blank"><img src="https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png" alt="Buy Me A Coffee" style="height: auto !important;width: auto !important;" ></a>

Or [PayPal](https://www.paypal.com/paypalme/imbushuo)

## A better installation guide

https://www.reddit.com/r/bootcamp/comments/c4alv2/how_to_install_mac_precision_touchpad_better_than/ - thank you Azmat

## USB-based model Installation

1. Go to the release tab in Github and download the newest version
2. Open the zip and navigate to `Drivers\x64\ReleaseSigned`
3. Go to `AmtPtpDevice` directory
4. Right click `AmtPtpDevice.inf` and install it
5. Confirm driver publisher

## SPI-based model Installation

1. Go to the release tab in Github and download the newest version
2. Open the zip and navigate to `Drivers\x64\ReleaseSigned`
3. Go to `AmtPtpDeviceSpiKm` directory
4. Right click `AmtPtpDeviceSpiKm.inf` and install it
5. Confirm driver publisher

## Apple T2-based model Installation

0. **Disable Secure Boot as temporary mitigation**: Boot with Command + R, select Startup Security Tools and disable Secure Boot (No Boot Security)
1. Go to the release tab in Github and download the newest version
2. Open the zip and navigate to `Drivers\x64\ReleaseSigned`
3. Go to `AmtPtpDeviceUsbKm` directory
4. Right click `AmtPtpDeviceUsbKm.inf` and install it
5. Confirm driver publisher

## For developers

- SPI version is kernel-mode driver, using KMDF Framework v1.23. Windows 10 Driver Development Kit Version 1803 is required for development and testing.
- USB version is a user-mode driver, using UMDF Framework v2.15. Windows 10 Driver Development Kit Version 1803 is required for development and testing.

**Note: I plan to target a higher version of KMDF and drop UMDF later this year.**

## Device support

There is a bring-up issue for certain MacBook Pro and MacBook Air. I am looking into it. If you have such device with Windows installed, please contact me so I can look into the issue remotely (as I don't have many devices).

- [x] Non-Retina MacBook 
- [x] Some non-Retina MacBook Pro (2011 and 2012)
- [x] MacBook Pro with Retina Display (MacBookPro 10,1, MacBookPro10,2 & MacBookPro11,1)
- [x] All recent MacBook Air (Please report if your model is not supported)
- [x] Apple Magic Trackpad 2 (Not stable)
- [x] MacBook Pro with Retina Display (2013, 2014, 2015, 13-inch & 15-inch)
- [x] New MacBook (12-inch)
- [x] MacBook Pro 2015, 2016, 2017
- [ ] _(Work in progress)_ T2-based devices: MacBook Air 2018, MacBook Pro 2017/2018/2019

## Roadmap

- [x] Touchpad Device Power Management
- [x] Touchpad Protocol Stack Implementation
- [x] Touchpad Raw Input Output (in Driver Trace)
- [x] HID Protocol Implementation (PTP Touch)
- [x] HID Protocol Implementation (Configuration)
- [x] Device Test (Magic Trackpad 2)
- [x] Refine input experience (tip switch detection and pressure)
- [x] Refine input experience (gesture experience)
- [x] More Models (I need donation)
- [x] SPI
- [x] KM Driver cross-cert

## Acknowledgements

People familiar with me know that I don't use Apple MacBook (so development work of this driver occurs on a Surface Pro 4). People probably think it doesn't make sense that I started this project. The motivation behind this project origin from complaints from my girlfriend. Hearing about frequent complains about Dell XPS 13's touchpad (though it passed Precision Touchpad certification), I decided to start this project, so by the time she switches to MacBook Pro, she will have excellent touchpad experience out-of-box, even on Windows. This driver is made for you, but also for everyone.

I would like to thank projects like [magictrackpad2-dkms](https://github.com/robbi5/magictrackpad2-dkms) and [macbook12-spi-driver](https://github.com/cb22/macbook12-spi-driver) that inspire me. I would also like to thank my friends for providing me immense help on devices and testing.
 
## License

- USB driver is licensed under [GPLv2](LICENSE-GPL.md).
- SPI driver is licensed under [MIT](LICENSE-MIT.md).

