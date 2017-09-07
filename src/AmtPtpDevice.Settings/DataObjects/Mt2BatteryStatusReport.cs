using System.Runtime.InteropServices;

namespace AmtPtpDevice.Settings.DataObjects
{
    public struct Mt2BatteryStatusReport
    {
        public byte ReportId;
        public byte BatteryFlags;
        public byte ChargeStatus;
    }
}
