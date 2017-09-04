# Windows Precision Touchpad Implementation for Apple MacBook family/Magic Trackpad 2

**Warning: The project is under development, with some features incomplete yet. Currently it only works on Apple Magic Trackpad 2 with minor issues. A duly signed, working driver will be released in future. Stay tuned.**

This project aims to implement Windows Precision Touchpad Protocol for Apple MacBook family/Magic Trackpad 2 on Windows 10.

## For Developers

It is a user-mode driver, using UMDF Framework v2.15. Windows 10 Driver Development Kit (at least Creators Update version) is required for development and testing. 

## Roadmap

- [x] Touchpad Device Power Management
- [x] Touchpad Protocol Stack Implementation
- [x] Touchpad Raw Input Output (in Driver Trace)
- [x] HID Protocol Implementation (PTP Touch)
- [x] HID Protocol Implementation (Configuration)
- [x] Device Test (Magic Trackpad 2)
- [ ] Refine input experience (tip switch detection and pressure)
- [ ] Refine input experience (gesture experience)
- [ ] More Models (I need donation)

## Acknowledgements

- [magictrackpad2-dkms](https://github.com/robbi5/magictrackpad2-dkms)

## License

[GPLv2](LICENSE.md)

## Donation

- [PayPal](https://www.paypal.com/paypalme/imbushuo)
- Chase QuickPay: ben.imbushuo#internal.ligstd.com