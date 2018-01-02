# Windows Precision Touchpad Implementation for Apple MacBook family/Magic Trackpad 2

[![Build Status](https://ligstd.visualstudio.com/_apis/public/build/definitions/7694e0d0-94e3-4fd2-b39a-ecd261e1ba2e/22/badge)](https://ligstd.visualstudio.com/Apple%20PTP%20Trackpad/)

**Warning: The project is under development, with some features incomplete yet. Currently it only works on Apple Magic Trackpad 2 with minor issues. A duly signed, working driver will be released in future. Stay tuned.**

This project aims to implement Windows Precision Touchpad Protocol for Apple MacBook family/Magic Trackpad 2 on Windows 10.

## For Developers

It is a user-mode driver, using UMDF Framework v2.15. Windows 10 Driver Development Kit (at least Creators Update version) is required for development and testing.

## Device Support

- [x] Apple Magic Trackpad 2
- [x] MacBook Pro with Retina Display (MacBookPro10,2 & MacBookPro11,1)
- [ ] _(Work in progress)_ MacBook Pro with Retina Display (2013, 2014, 2015)
- [ ] MacBook Air (2014, 2015)
- [ ] MacBook Pro 2016

## Roadmap

- [x] Touchpad Device Power Management
- [x] Touchpad Protocol Stack Implementation
- [x] Touchpad Raw Input Output (in Driver Trace)
- [x] HID Protocol Implementation (PTP Touch)
- [x] HID Protocol Implementation (Configuration)
- [x] Device Test (Magic Trackpad 2)
- [x] Refine input experience (tip switch detection and pressure)
- [ ] Refine input experience (gesture experience)
- [ ] More Models (I need donation)

## Acknowledgements

- [magictrackpad2-dkms](https://github.com/robbi5/magictrackpad2-dkms)

## License

[GPLv2](LICENSE.md)

## Donation

- [PayPal](https://www.paypal.com/paypalme/imbushuo)
- Chase QuickPay: ben.imbushuo#internal.ligstd.com