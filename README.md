# Windows Precision Touchpad Implementation for Apple MacBook family/Magic Trackpad 2

[![Build Status](https://ligstd.visualstudio.com/_apis/public/build/definitions/7694e0d0-94e3-4fd2-b39a-ecd261e1ba2e/22/badge)](https://ligstd.visualstudio.com/Apple%20PTP%20Trackpad/)

**Warning: The project is under development, with some features incomplete yet.**

This project aims to implement Windows Precision Touchpad Protocol for Apple MacBook family/Magic Trackpad 2 on Windows 10.

## See it in action (YouTube video)

[![Preview of Windows Precision Touchpad for MacBook Pro](https://img.youtube.com/vi/7dFqtcDArUg/0.jpg)](https://www.youtube.com/watch?v=7dFqtcDArUg)

## For developers

It is a user-mode driver, using UMDF Framework v2.15. Windows 10 Driver Development Kit (at least Creators Update version) is required for development and testing.

## Device support

- [x] Apple Magic Trackpad 2
- [x] MacBook, MacBook Pro 2011 (MacBook8 and more)
- [x] MacBook Pro with Retina Display (MacBookPro 10,1, MacBookPro10,2 & MacBookPro11,1)
- [x] MacBook Air 2013, 2014(?), 2015 (MacBookAir6,2 & 7,2 and more)
- [ ] _(Work in progress)_ MacBook Pro with Retina Display (2014, 2015)
- [ ] MacBook Pro 2015, 2016 _SPI devices will be considered later_

## Roadmap

- [x] Touchpad Device Power Management
- [x] Touchpad Protocol Stack Implementation
- [x] Touchpad Raw Input Output (in Driver Trace)
- [x] HID Protocol Implementation (PTP Touch)
- [x] HID Protocol Implementation (Configuration)
- [x] Device Test (Magic Trackpad 2)
- [x] Refine input experience (tip switch detection and pressure)
- [x] Refine input experience (gesture experience)
- [ ] More Models (I need donation)

## Acknowledgements

- [magictrackpad2-dkms](https://github.com/robbi5/magictrackpad2-dkms)

## License

[GPLv2](LICENSE.md)

## Donation

- [PayPal](https://www.paypal.com/paypalme/imbushuo)
- Chase QuickPay: ben.imbushuo#internal.ligstd.com