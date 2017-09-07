using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AmtPtpDevice.Settings.DataObjects
{
    public struct PtpUserModeConfReport
    {
        public byte ReportId;
        public byte PressureQualificationLevel;
        public byte SingleContactSizeQualificationLevel;
        public byte MultipleContactSizeQualificationLevel;
    }
}
