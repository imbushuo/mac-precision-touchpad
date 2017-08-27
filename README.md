# Windows Precision Touchpad Implementation for Apple MacBook family/Magic Trackpad 2

**Warning: The project is under development, with major features incomplete yet. Hence, do not ask questions such as "why this is not working."  A duly signed, working driver will be released in future. Stay tuned.**

This project aims to implement Windows Precision Touchpad Protocol for Apple MacBook family/Magic Trackpad 2 on Windows 10.

## For Developers

It is a user-mode driver, using UMDF Framework v2.15. Windows 10 Driver Development Kit (at least Creators Update version) is required for development and testing. 

## Roadmap

- [x] Touchpad Device Power Management
- [x] Touchpad Protocol Stack Implementation
- [x] Touchpad Raw Input Output (in Driver Trace)
- [ ] HID Protocol Implementation
- [ ] Device Test

## Acknowledgements

- [magictrackpad2-dkms](https://github.com/robbi5/magictrackpad2-dkms)

## License

GPLv2