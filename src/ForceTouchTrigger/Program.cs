
using HidSharp;
using System;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;

namespace ForceTouchTrigger
{
    [StructLayout(LayoutKind.Explicit)]
    struct HapticFeedback
    {
        [FieldOffset(0)]
        public ushort Magic;
        [FieldOffset(2)]
        public uint Strength;
        [FieldOffset(6)]
        public int X;
        [FieldOffset(10)]
        public int Y;
    }

    class Program
    {
        static void Main(string[] args)
        {
            // Force Touch Device: USB\VID_05AC&PID_027D&REV_0928&MI_03 or 0265 / mi_02
            // But I don't know if I have a better way to deal with multi collections
            var hidDeviceList = DeviceList.Local.GetHidDevices(0x05ac, 0x0265).Where(i => i.DevicePath.Contains("mi_02"));
            if (!hidDeviceList.Any())
            {
                hidDeviceList = DeviceList.Local.GetHidDevices(0x05ac, 0x027d).Where(i => i.DevicePath.Contains("mi_03"));
            }

            if (!hidDeviceList.Any()) return;
            var dev = hidDeviceList.FirstOrDefault();
            if (dev.TryOpen(out DeviceStream stream))
            {
                for (int i = 0; i < 3; i++)
                {
                    var feedback = new HapticFeedback
                    {
                        Magic = 0x153,
                        // 0x26c00 - 0x26cff
                        Strength = 0x26c50,
                        X = i,
                        Y = 0
                    };

                    int size = Marshal.SizeOf(feedback);
                    byte[] arr = new byte[size];

                    IntPtr ptr = Marshal.AllocHGlobal(size);
                    Marshal.StructureToPtr(feedback, ptr, true);
                    Marshal.Copy(ptr, arr, 0, size);
                    Marshal.FreeHGlobal(ptr);

                    stream.Write(arr, 0, size);

                    Thread.Sleep(TimeSpan.FromSeconds(0.15));
                }
            }
        }
    }
}
