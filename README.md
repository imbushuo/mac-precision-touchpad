# Windows Precision Touchpad Implementation for Apple MacBook family/Magic Trackpad 2

[![Build Status](https://ligstd.visualstudio.com/_apis/public/build/definitions/7694e0d0-94e3-4fd2-b39a-ecd261e1ba2e/22/badge)](https://ptp-release.imbushuo.net/)

**Warning: The project is under development, with some features incomplete yet.** See projects page for known issues and WIP(work-in-progress) items before opening a new issue.

This project aims to implement Windows Precision Touchpad Protocol for Apple MacBook family/Magic Trackpad 2 on Windows 10.

Both USB and SPI trackpads are supported.

## See it in action (YouTube video)

[![Preview of Windows Precision Touchpad for MacBook Pro](https://img.youtube.com/vi/7dFqtcDArUg/0.jpg)](https://www.youtube.com/watch?v=7dFqtcDArUg)

## Help needed!

Please open an issue for model support. In the issue, attach your MacBook model identifier (e.g. MacBook9,1) and HID device ID (you can view it in System Report in macOS, or Properties Window of the "Apple SPI Trackpad" or "Apple USB Trackpad" in Device Manager of Windows). Here is an example in macOS:

![image_2018-07-15_22-59-15.png](https://i.loli.net/2018/07/15/5b4b616019799.png)

If you like my work, please consider buying me a coffee. Thank you for your support!

<a href="https://www.buymeacoffee.com/imbushuo" target="_blank"><img src="https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png" alt="Buy Me A Coffee" style="height: auto !important;width: auto !important;" ></a>

Or [PayPal](https://www.paypal.com/paypalme/imbushuo)

## USB Installation

1. Go to the release tab in Github and download the newest version
2. Open the zip and navigate to `Drivers\x64\ReleaseSigned`
3. Install `AmtPtpDevice.cer` certificate to your machine's root authority store
4. Go to `AmtPtpDevice` directory
5. Right click `AmtPtpDevice.inf` and install it 

You don't have to enable manual trust, as recent builds are cross-signed by DigiCert.
If you want to try latest changes, you can click "Build Status" badge and download CI builds (tagged with commit ID and machine architecture) manually.

## SPI Installation

(Coming soon)

**Note: SPI version needs at least Windows 10, version 1709 to work.**

## For developers

- SPI version is kernel-mode driver, using KMDF Framework v1.23. Windows 10 Driver Development Kit Version 1803 is required for development and testing.
- USB version is a user-mode driver, using UMDF Framework v2.15. Windows 10 Driver Development Kit Version 1803 is required for development and testing.

## Device support

There is a bring-up issue for certain MacBook Pro and MacBook Air. I am looking into it. If you have such device with Windows installed, please contact me so I can look into the issue remotely (as I don't have many devices).

- [x] Non-Retina MacBook 
- [x] Some non-Retina MacBook Pro (2011 and 2012)
- [x] MacBook Pro with Retina Display (MacBookPro 10,1, MacBookPro10,2 & MacBookPro11,1)
- [x] All recent MacBook Air (Please report if your model is not supported)
- [x] Apple Magic Trackpad 2 (Not stable)
- [x] MacBook Pro with Retina Display (2013, 2014, 2015, 13-inch & 15-inch)
- [x] New MacBook (12-inch)
- [x] _(Work in progress)_ MacBook Pro 2015, 2016, 2017 (**I need the device ID for 2016 model**)
- [ ] _(Needs triage)_ T2-based devices: MacBook Air 2018, MacBook Pro 2017/2018

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
- [ ] Signal Processing Algorithm (Papers, papers!)
- [ ] KM Driver EV & WHQL

## Acknowledgements

People familiar with me know that I don't use Apple MacBook (so development work of this driver occurs on a Surface Pro 4). People probably think it doesn't make sense that I started this project. The motivation behind this project origin from complaints from my girlfriend. Hearing about frequent complains about Dell XPS 13's touchpad (though it passed Precision Touchpad certification), I decided to start this project, so by the time she switches to MacBook Pro, she will have excellent touchpad experience out-of-box, even on Windows. This driver is made for you, but also for everyone.

I would like to thank projects like [magictrackpad2-dkms](https://github.com/robbi5/magictrackpad2-dkms) and [macbook12-spi-driver](https://github.com/cb22/macbook12-spi-driver) that inspire me. I would also like to thank my friends for providing me immense help on devices and testing.
 
## License

- USB driver is licensed under [GPLv2](LICENSE-GPL.md).
- SPI driver is licensed under [MIT](LICENSE-MIT.md).

